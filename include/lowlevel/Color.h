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
#ifndef SOLARUS_COLOR_H
#define SOLARUS_COLOR_H

#include "Common.h"
#include <SDL.h>

/**
 * \brief Represents a color.
 *
 * This module defines a type for the colors and provides some color related functions.
 * It encapsulates a library-dependent color.
 */
class Color {

  // low-level classes allowed to manipulate directly the internal SDL objects encapsulated
  friend class Surface;
  friend class TextSurface;

  private:

    static SDL_Surface* format_surface;	  /**< a dummy surface used to get a pixel format */
    uint32_t internal_value;              /**< the SDL 32-bit value representing this color */
    SDL_Color internal_color;             /**< the SDL color encapsulated */

    // some predefined colors
    static Color black;
    static Color white;
    static Color red;
    static Color green;
    static Color blue;
    static Color yellow;
    static Color magenta;
    static Color cyan;

    Color(uint32_t internal_value);
    uint32_t get_internal_value() const;
    SDL_Color* get_internal_color();

  public:

    static void initialize();
    static void quit();

    static Color& get_black();
    static Color& get_white();
    static Color& get_red();
    static Color& get_green();
    static Color& get_blue();
    static Color& get_yellow();
    static Color& get_magenta();
    static Color& get_cyan();

    Color();
    Color(const Color& other);
    Color(int r, int g, int b);

    void get_components(int& r, int& g, int& b) const;
};

/**
 * \brief Returns the black color.
 * \return the black color
 */
inline Color& Color::get_black() {
  return black;
}

/**
 * \brief Returns the white color.
 * \return the white color
 */
inline Color& Color::get_white() {
  return white;
}

/**
 * \brief Returns the red color.
 * \return the red color
 */
inline Color& Color::get_red() {
  return red;
}

/**
 * \brief Returns the green color.
 * \return the green color
 */
inline Color& Color::get_green() {
  return green;
}

/**
 * \brief Returns the blue color.
 * \return the blue color
 */
inline Color& Color::get_blue() {
  return blue;
}

/**
 * \brief Returns the yellow color.
 * \return the yellow color
 */
inline Color& Color::get_yellow() {
  return yellow;
}

/**
 * \brief Returns the magenta color.
 * \return the magenta color
 */
inline Color& Color::get_magenta() {
  return magenta;
}

/**
 * \brief Returns the cyan color.
 * \return the cyan color
 */
inline Color& Color::get_cyan() {
  return cyan;
}

#endif

