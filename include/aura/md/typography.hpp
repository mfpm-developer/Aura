/*
 * Copyright (C) 2026 <Your Name or Organization>
 *
 * This file is part of Aura.
 *
 * Aura is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aura is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aura.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>

namespace aura {
namespace md {

    // Represents a single text style in the M3 Typography Scale
    struct TextStyle
    {
        std::string font_family{"Roboto"};
        int font_size{14};
        float line_height{20.0f};
        float letter_spacing{0.25f}; // Tracking
        std::string font_weight{"Regular"};
    };

    // The full M3 Typography Scale
    struct Typography
    {
        TextStyle display_large   = {"Roboto", 57, 64.0f, -0.25f, "Regular"};
        TextStyle display_medium  = {"Roboto", 45, 52.0f,  0.0f,  "Regular"};
        TextStyle display_small   = {"Roboto", 36, 44.0f,  0.0f,  "Regular"};

        TextStyle headline_large  = {"Roboto", 32, 40.0f,  0.0f,  "Regular"};
        TextStyle headline_medium = {"Roboto", 28, 36.0f,  0.0f,  "Regular"};
        TextStyle headline_small  = {"Roboto", 24, 32.0f,  0.0f,  "Regular"};

        TextStyle title_large     = {"Roboto", 22, 28.0f,  0.0f,  "Regular"};
        TextStyle title_medium    = {"Roboto", 16, 24.0f,  0.15f, "Medium"};
        TextStyle title_small     = {"Roboto", 14, 20.0f,  0.1f,  "Medium"};

        TextStyle label_large     = {"Roboto", 14, 20.0f,  0.1f,  "Medium"};
        TextStyle label_medium    = {"Roboto", 12, 16.0f,  0.5f,  "Medium"};
        TextStyle label_small     = {"Roboto", 11, 16.0f,  0.5f,  "Medium"};

        TextStyle body_large      = {"Roboto", 16, 24.0f,  0.5f,  "Regular"};
        TextStyle body_medium     = {"Roboto", 14, 20.0f,  0.25f, "Regular"};
        TextStyle body_small      = {"Roboto", 12, 16.0f,  0.4f,  "Regular"};
    };

} // namespace md
} // namespace aura
