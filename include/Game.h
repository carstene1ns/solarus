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
#ifndef SOLARUS_GAME_H
#define SOLARUS_GAME_H

#include "Common.h"
#include "Transition.h"
#include "GameCommands.h"
#include "Savegame.h"
#include "DialogBox.h"
#include <lua.hpp>

/**
 * \brief Represents the game currently running.
 *
 * The game shows the current map and handles all game elements.
 */
class Game {

  private:

    // main objects
    MainLoop& main_loop;       /**< the main loop object */
    Savegame* savegame;        /**< the game data saved */
    Hero* hero;

    // current game state (elements currently shown)
    bool pause_allowed;        /**< indicates that the player is allowed to use the pause command */
    bool paused;               /**< indicates that the game is paused */
    DialogBox dialog_box;      /**< The dialog box manager. */
    bool showing_game_over;    /**< Whether a game-over sequence is currently active. */
    bool started;              /**< true if this game is running, false if it is not yet started or being closed. */
    bool restarting;           /**< true if the game will be restarted */

    // controls
    GameCommands* commands;    /**< this object receives the keyboard and joypad events */
    KeysEffect* keys_effect;   /**< current effect associated to the main game keys
                                * (represented on the HUD by the action icon, the objects icons, etc.) */

    // map
    Map* current_map;          /**< the map currently displayed */
    Map* next_map;             /**< the map where the hero is going to; if not NULL, it means that the hero
                                * is changing from current_map to next_map */
    Surface* previous_map_surface;  /**< a copy of the previous map surface for transition effects that display two maps */

    Transition::Style transition_style; /**< the transition style between the current map and the next one */
    Transition* transition;             /**< the transition currently shown, or NULL if no transition is playing */

    // world (i.e. the current set of maps)
    bool crystal_state;        /**< indicates that a crystal has been enabled (i.e. the orange blocks are raised) */

    // update functions
    void update_keys_effect();
    void update_transitions();
    void update_gameover_sequence();
    void notify_map_changed();

  public:

    // creation and destruction
    Game(MainLoop& main_loop, Savegame* savegame);
    ~Game();

    void start();
    void stop();
    void restart();

    // global objects
    MainLoop& get_main_loop();
    LuaContext& get_lua_context();
    Hero& get_hero();
    const Rectangle& get_hero_xy();
    GameCommands& get_commands();
    KeysEffect& get_keys_effect();
    Savegame& get_savegame();
    Equipment& get_equipment();

    // functions called by the main loop
    bool notify_input(InputEvent &event);
    void update();
    void draw(Surface& dst_surface);

    // game controls
    void notify_command_pressed(GameCommands::Command command);
    void notify_command_released(GameCommands::Command command);

    // map
    bool has_current_map();
    Map& get_current_map();
    void set_current_map(const std::string& map_id, const std::string& destination_name,
        Transition::Style transition_style);

    // world
    bool get_crystal_state();
    void change_crystal_state();

    // current game state
    bool is_paused();
    bool is_dialog_enabled();
    bool is_playing_transition();
    bool is_showing_game_over();
    bool is_suspended(); // true if at least one of the three functions above returns true

    // pause
    bool can_pause();
    bool can_unpause();
    bool is_pause_allowed();
    void set_pause_allowed(bool pause_allowed);
    void set_paused(bool paused);

    // dialogs
    void start_dialog(const std::string& dialog_id,
        int info_ref = LUA_REFNIL, int callback_ref = LUA_REFNIL);
    void stop_dialog(int status_ref = LUA_REFNIL);

    // game over
    void start_game_over();
    void stop_game_over();
};

#endif

