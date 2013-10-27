/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "SpriteAnimationSet.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lua/LuaContext.h"

/**
 * \brief Loads the animations of a sprite from a file.
 * \param id Id of the sprite animation set to load
 * (name of a sprite definition file, without the ".dat" extension).
 */
SpriteAnimationSet::SpriteAnimationSet(const std::string& id):
  id(id) {

  load();
}

/**
 * \brief Destructor.
 */
SpriteAnimationSet::~SpriteAnimationSet() {

  // delete the animations
  std::map<std::string, SpriteAnimation*>::const_iterator it;

  for (it = animations.begin(); it != animations.end(); it++) {
    delete it->second;
  }
}

/**
 * \brief Attempts to load this animation set from its file.
 */
void SpriteAnimationSet::load() {

  Debug::check_assertion(animations.empty(),
      "Animation set already loaded");

  // Compute the file name.
  std::string file_name = std::string("sprites/") + id + ".dat";

  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  if (load_result != 0) {
    Debug::error(StringConcat() << "Failed to load sprite file '" << file_name
        << "': " << lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    lua_pushlightuserdata(l, this);
    lua_setfield(l, LUA_REGISTRYINDEX, "animation_set");
    lua_register(l, "animation", l_animation);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::error(StringConcat() << "Failed to load sprite file '" << file_name
          << "': " << lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }
  lua_close(l);
}

/**
 * \brief Function called by the Lua data file to define an animation.
 *
 * - Argument 1 (table): properties of the animation.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int SpriteAnimationSet::l_animation(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "animation_set");
  SpriteAnimationSet* animation_set = static_cast<SpriteAnimationSet*>(
      lua_touserdata(l, -1));
  lua_pop(l, 1);

  luaL_checktype(l, 1, LUA_TTABLE);


  std::string animation_name = LuaContext::check_string_field(l, 1, "name");
  std::string src_image = LuaContext::check_string_field(l, 1, "src_image");
  uint32_t frame_delay = uint32_t(LuaContext::opt_int_field(l, 1, "frame_delay", 0));
  int frame_to_loop_on = LuaContext::opt_int_field(l, 1, "frame_to_loop_on", -1);

  lua_settop(l, 1);
  lua_getfield(l, 1, "directions");
  if (lua_type(l, 2) != LUA_TTABLE) {
    LuaContext::arg_error(l, 1, StringConcat() <<
          "Bad field 'directions' (table expected, got " <<
          luaL_typename(l, -1) << ")");
  }

  // Traverse the directions table.
  std::vector<SpriteAnimationDirection*> directions;
  int i = 1;
  lua_rawgeti(l, -1, i);
  while (!lua_isnil(l, -1)) {
    ++i;

    if (lua_type(l, -1) != LUA_TTABLE) {
      LuaContext::arg_error(l, 1, StringConcat() <<
          "Bad field 'directions' (got " <<
          luaL_typename(l, -1) << " in the table)");
    }

    int x = LuaContext::check_int_field(l, -1, "x");
    int y = LuaContext::check_int_field(l, -1, "y");
    int frame_width = LuaContext::check_int_field(l, -1, "frame_width");
    int frame_height = LuaContext::check_int_field(l, -1, "frame_height");
    int origin_x = LuaContext::opt_int_field(l, -1, "origin_x", 0);
    int origin_y = LuaContext::opt_int_field(l, -1, "origin_y", 0);
    int num_frames = LuaContext::opt_int_field(l, -1, "num_frames", 1);
    int num_columns = LuaContext::opt_int_field(l, -1, "num_columns", num_frames);

    lua_pop(l, 1);
    lua_rawgeti(l, -1, i);

    animation_set->max_size.set_width(std::max(frame_width, animation_set->max_size.get_width()));
    animation_set->max_size.set_height(std::max(frame_height, animation_set->max_size.get_height()));

    int num_rows;
    if (num_frames % num_columns == 0) {
      num_rows = num_frames / num_columns;
    }
    else {
      num_rows = (num_frames / num_columns) + 1;
    }

    std::vector<Rectangle> positions_in_src;
    int j = 0;  // Frame number.
    for (int r = 0; r < num_rows && j < num_frames; ++r) {
      for (int c = 0; c < num_columns && j < num_frames; ++c) {

        Rectangle position_in_src(
            x + c * frame_width,
            y + r * frame_height,
            frame_width,
            frame_height);
        positions_in_src.push_back(position_in_src);
        ++j;
      }
    }

    directions.push_back(new SpriteAnimationDirection(
          positions_in_src,
          Rectangle(origin_x, origin_y)));
  }

  if (animation_set->animations.find(animation_name) != animation_set->animations.end()) {
    LuaContext::error(l, std::string("Duplicate animation '") + animation_name
        + "' in sprite '" + animation_set->id + "'");
  }

  animation_set->animations[animation_name] = new SpriteAnimation(
      src_image, directions, frame_delay, frame_to_loop_on);

  // Set the first animation as the default one.
  if (animation_set->animations.size() == 1) {
    animation_set->default_animation_name = animation_name;
  }

  return 0;
}

/**
 * \brief When the sprite is displayed on a map, sets the tileset.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * \param tileset The tileset.
 */
void SpriteAnimationSet::set_tileset(Tileset& tileset) {

  std::map<std::string, SpriteAnimation*>::const_iterator it;

  for (it = animations.begin(); it != animations.end(); it++) {
    it->second->set_tileset(tileset);
  }
}

/**
 * \brief Returns whether this animation set has an animation with the specified name.
 * \param animation_name an animation name
 * \return true if this animation exists
 */
bool SpriteAnimationSet::has_animation(
    const std::string& animation_name) const {
  return animations.find(animation_name) != animations.end();
}

/**
 * \brief Returns an animation.
 * \param animation_name name of the animation to get
 * \return the specified animation
 */
const SpriteAnimation* SpriteAnimationSet::get_animation(
    const std::string& animation_name) const {

  Debug::check_assertion(has_animation(animation_name), StringConcat() <<
      "No animation '" << animation_name << "' in animation set '" << id << "'");

  return animations.find(animation_name)->second; // the [] operator is not const in std::map
}

/**
 * \brief Returns an animation.
 * \param animation_name name of the animation to get
 * \return the specified animation
 */
SpriteAnimation* SpriteAnimationSet::get_animation(
    const std::string& animation_name) {

  Debug::check_assertion(has_animation(animation_name), StringConcat() <<
      "No animation '" << animation_name << "' in animation set '" << id << "'");

  return animations[animation_name];
}

/**
 * \brief Returns the name of the default animation, i.e. the first one.
 * \return the name of the default animation
 */
const std::string& SpriteAnimationSet::get_default_animation() const {
  return default_animation_name;
}

/**
 * \brief Enables the pixel-perfect collision detection for these animations.
 */
void SpriteAnimationSet::enable_pixel_collisions() {

  if (!are_pixel_collisions_enabled()) {

    std::map<std::string, SpriteAnimation*>::const_iterator it;

    for (it = animations.begin(); it != animations.end(); it++) {
      it->second->enable_pixel_collisions();
    }
  }
}

/**
 * \brief Returns whether the pixel-perfect collisions are enabled for these animations.
 * \return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimationSet::are_pixel_collisions_enabled() const {
  return animations.begin()->second->are_pixel_collisions_enabled();
}

/**
 * \brief Returns a rectangle big enough to contain any frame of this animation set.
 * \return The maximum size of a frame in this animation set.
 */
const Rectangle& SpriteAnimationSet::get_max_size() const {
  return max_size;
}

