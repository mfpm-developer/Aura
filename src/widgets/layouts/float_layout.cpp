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
#include "aura/widgets/layouts/float_layout.hpp"
#include <sstream>
#include <map>
#include <vector>

namespace aura
{
    // Helper function to parse pos_hint
    static std::map<std::string, float> parse_pos_hint(const std::string& hint_str)
    {
        std::map<std::string, float> hints;
        std::stringstream ss(hint_str);
        std::string token;
        while (std::getline(ss, token, ';'))
        {
            size_t colon = token.find(':');
            if (colon != std::string::npos)
            {
                std::string key = token.substr(0, colon);
                std::string val_str = token.substr(colon + 1);
                
                // trim
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                try { hints[key] = std::stof(val_str); } catch (...) {}
            }
        }
        return hints;
    }

    void FloatLayout::do_layout()
    {
        float lw = width.get_value();
        float lh = height.get_value();
        float lx = x.get_value();
        float ly = y.get_value();

        for (auto& child : m_children)
        {
            // Apply size_hint
            float sx = child->size_hint_x.get_value();
            float sy = child->size_hint_y.get_value();

            float cw = (sx >= 0.0f) ? (lw * sx) : child->width.get_value();
            float ch = (sy >= 0.0f) ? (lh * sy) : child->height.get_value();

            child->width.set(cw);
            child->height.set(ch);

            // Apply pos_hint
            std::string hint_str = child->pos_hint.get_value();
            auto hints = parse_pos_hint(hint_str);

            float cx = child->x.get_value(); // default fallback
            float cy = child->y.get_value(); // default fallback

            if (hints.count("x")) cx = lx + lw * hints["x"];
            else if (hints.count("right")) cx = lx + lw * hints["right"] - cw;
            else if (hints.count("center_x")) cx = lx + lw * hints["center_x"] - cw / 2.0f;

            if (hints.count("y")) cy = ly + lh * hints["y"];
            else if (hints.count("top")) cy = ly + lh * hints["top"] - ch;
            else if (hints.count("center_y")) cy = ly + lh * hints["center_y"] - ch / 2.0f;

            child->x.set(cx);
            child->y.set(cy);
        }
    }

    bool FloatLayout::set_property(const std::string& name, const std::string& value)
    {
        if (Layout::set_property(name, value)) return true;
        return false; // FloatLayout doesn't have specific properties beyond base yet
    }
}
