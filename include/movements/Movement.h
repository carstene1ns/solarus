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
#ifndef SOLARUS_MOVEMENT_H
#define SOLARUS_MOVEMENT_H

#include "Common.h"
#include "lua/ExportableToLua.h"
#include "lowlevel/Rectangle.h"

/**
 * \brief Abstract class for representing a movement.
 *
 * This is the parent class of all kinds of movement.
 * An instance of Movement can be applied to a map entity during the game,
 * to a drawable object or to some arbitrary coordinates.
 *
 * TODO: generalize this and allow to apply a movement to any Lua object that
 * has a set_xy method()?
 */
class Movement: public ExportableToLua {

  private:

    // Object to move (can be an entity, a drawable or a point).
    MapEntity* entity;                           /**< The entity controlled by this movement. */
    Drawable* drawable;                          /**< The drawable controlled by this movement. */
    Rectangle xy;                                /**< Coordinates of the point controlled by this movement. */

    uint32_t last_move_date;                     /**< Date of the last x or y move. */
    bool finished;                               /**< true if is_finished() returns true. */

    // suspended
    bool suspended;                              /**< Indicates whether the movement is suspended. */
    uint32_t when_suspended;                     /**< Indicates when the movement was suspended. */

    // obstacles (only when the movement is applied to an entity)
    Rectangle last_collision_box_on_obstacle;    /**< Copy of the entity's bounding box of the last call
                                                  * to test_collision_with_map() returning true. */

    bool default_ignore_obstacles;               /**< Indicates that this movement normally ignores obstacles. */
    bool current_ignore_obstacles;               /**< Indicates that this movement currently ignores obstacles. */

    LuaContext* lua_context;                     /**< The Solarus Lua API (NULL means no callbacks for this movement).
                                                  * TODO move this to ExportableToLua */
    int finished_callback_ref;                   /**< Lua ref to a function to call when this movement finishes. */

  protected:

    Movement(bool ignore_obstacles = false);

    // suspended
    uint32_t get_when_suspended();

    // obstacles (only when the movement is applied to an entity)
    void set_default_ignore_obstacles(bool ignore_obstacles);

  public:

    virtual ~Movement();

    // object controlled
    MapEntity* get_entity() const;
    void set_entity(MapEntity* entity);
    Drawable* get_drawable() const;
    void set_drawable(Drawable* drawable);
    virtual void notify_object_controlled();

    // update
    virtual void update(); // called repeatedly
    bool is_suspended();
    virtual void set_suspended(bool suspended);

    // position
    int get_x();
    int get_y();
    const Rectangle get_xy();
    void set_x(int x);
    void set_y(int y);
    void set_xy(int x, int y);
    void set_xy(const Rectangle &xy);
    void translate_x(int dx);
    void translate_y(int dy);
    void translate_xy(int dx, int dy);
    void translate_xy(const Rectangle& dxy);
    virtual void notify_position_changed(); // called whenever x or y is changed
    virtual void notify_obstacle_reached();
    virtual void notify_movement_changed();
    virtual void notify_movement_finished();

    // movement
    bool is_stopped();
    virtual bool is_started();
    virtual void stop();
    virtual bool is_finished();

    // obstacles
    bool test_collision_with_obstacles(int dx, int dy);
    bool test_collision_with_obstacles(const Rectangle& dxy);
    const Rectangle& get_last_collision_box_on_obstacle();
    bool are_obstacles_ignored();
    void set_ignore_obstacles(bool ignore_obstacles);
    void restore_default_ignore_obstacles();

    // displaying moving objects
    virtual int get_displayed_direction4();
    virtual const Rectangle get_displayed_xy();

    // Lua
    LuaContext* get_lua_context() const;
    void set_lua_context(LuaContext* lua_context);
    int get_finished_callback() const;
    void set_finished_callback(int finished_callback_ref);
    virtual const std::string& get_lua_type_name() const;
};

#endif

