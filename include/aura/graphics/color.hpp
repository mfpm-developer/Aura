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

#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>

namespace aura
{
    struct Color
    {
        float r{0.0f}, g{0.0f}, b{0.0f}, a{1.0f};

        Color() = default;
        Color(float red, float green, float blue, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {}

        // Construct from 0-255 integer values
        static Color from_rgba8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
        }

        // Construct from hex string like "#FF0000" or "#FF0000FF"
        static Color from_hex(const std::string& hex)
        {
            if (hex.empty() || hex[0] != '#') return Color();

            std::string sub = hex.substr(1);
            if (sub.length() == 6 || sub.length() == 8)
            {
                uint32_t val;
                std::stringstream ss;
                ss << std::hex << sub;
                ss >> val;

                if (sub.length() == 6)
                {
                    return from_rgba8((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF, 255);
                }
                else
                {
                    return from_rgba8((val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
                }
            }
            return Color();
        }
        
        Color with_alpha(float new_alpha) const
        {
            return {r, g, b, new_alpha};
        }

        bool operator==(const Color& other) const
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        bool operator!=(const Color& other) const
        {
            return !(*this == other);
        }
    };
}
