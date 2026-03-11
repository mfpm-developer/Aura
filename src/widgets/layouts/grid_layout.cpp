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
#include "aura/widgets/layouts/grid_layout.hpp"
#include <algorithm>
#include <iostream>

namespace aura
{
    GridLayout::GridLayout()
    {
        auto trigger_layout = [this](EventDispatcher*, const std::any&) {
            this->do_layout();
            return false;
        };

        cols.bind("on_cols", trigger_layout);
        rows.bind("on_rows", trigger_layout);
        spacing.bind("on_spacing", trigger_layout);
        padding.bind("on_padding", trigger_layout);
        width.bind("on_width", trigger_layout);
        height.bind("on_height", trigger_layout);
    }

    void GridLayout::do_layout()
    {
        if (m_children.empty()) return;

        int c = cols.get_value();
        int r = rows.get_value();

        if (c <= 0 && r <= 0)
        {
            // Fallback to placing everything in 1 row if undefined
            c = m_children.size();
            r = 1;
        }
        else if (c > 0 && r <= 0)
        {
            r = (m_children.size() + c - 1) / c; // Ceiling division
        }
        else if (r > 0 && c <= 0)
        {
            c = (m_children.size() + r - 1) / r;
        }

        if (c == 0 || r == 0) return;

        float space = spacing.get_value();
        float pad = padding.get_value();

        float inner_width = width.get_value() - (2 * pad) - (space * (c - 1));
        float inner_height = height.get_value() - (2 * pad) - (space * (r - 1));

        float cell_width = inner_width / c;
        float cell_height = inner_height / r;

        int current_col = 0;
        int current_row = 0;

        for (auto& child : m_children)
        {
            child->width.set(cell_width);
            child->height.set(cell_height);

            float cx = x.get_value() + pad + current_col * (cell_width + space);
            float cy = y.get_value() + pad + current_row * (cell_height + space);

            child->x.set(cx);
            child->y.set(cy);

            current_col++;
            if (current_col >= c)
            {
                current_col = 0;
                current_row++;
            }
        }
    }

    bool GridLayout::set_property(const std::string& name, const std::string& value)
    {
        if (Layout::set_property(name, value)) return true;
        
        try {
            if (name == "cols") { cols.set_value(std::stoi(value)); return true; }
            if (name == "rows") { rows.set_value(std::stoi(value)); return true; }
            if (name == "spacing") { spacing.set_value(std::stof(value)); return true; }
            if (name == "padding") { padding.set_value(std::stof(value)); return true; }
        } catch (...) { return false; }
        return false;
    }
}
