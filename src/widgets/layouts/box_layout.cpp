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
#include "aura/widgets/layouts/box_layout.hpp"

namespace aura
{
    void BoxLayout::do_layout()
    {
        if (m_children.empty()) return;

        bool is_horizontal = (orientation.get_value() == "horizontal");
        
        float pad = padding.get_value();
        float spc = spacing.get_value();

        float cx = x.get_value() + pad;
        float cy = y.get_value() + pad;
        
        float available_w = std::max(0.0f, width.get_value() - 2 * pad - (m_children.size() - 1) * spc);
        float available_h = std::max(0.0f, height.get_value() - 2 * pad - (m_children.size() - 1) * spc);

        float total_hint = 0.0f;
        for (auto& child : m_children) {
            if (is_horizontal) {
                if (child->size_hint_x.get_value() < 0.0f) {
                    available_w = std::max(0.0f, available_w - child->width.get_value());
                } else {
                    total_hint += child->size_hint_x.get_value();
                }
            } else {
                if (child->size_hint_y.get_value() < 0.0f) {
                    available_h = std::max(0.0f, available_h - child->height.get_value());
                } else {
                    total_hint += child->size_hint_y.get_value();
                }
            }
        }

        if (total_hint < 1.0f && total_hint > 0.0f) total_hint = 1.0f;

        for (auto& child : m_children)
        {
            if (is_horizontal)
            {
                float cw = child->width.get_value();
                if (child->size_hint_x.get_value() >= 0.0f) {
                    cw = total_hint > 0 ? available_w * (child->size_hint_x.get_value() / total_hint) : 0;
                }
                
                float ch = child->height.get_value();
                if (child->size_hint_y.get_value() >= 0.0f) {
                    float base_h = std::max(0.0f, height.get_value() - 2 * pad);
                    ch = base_h * child->size_hint_y.get_value();
                }

                child->width.set(cw);
                child->height.set(ch);
                child->x.set(cx);
                child->y.set(cy); 
                
                cx += cw + spc;
            }
            else // vertical
            {
                float ch = child->height.get_value();
                if (child->size_hint_y.get_value() >= 0.0f) {
                    ch = total_hint > 0 ? available_h * (child->size_hint_y.get_value() / total_hint) : 0;
                }

                float cw = child->width.get_value();
                if (child->size_hint_x.get_value() >= 0.0f) {
                    float base_w = std::max(0.0f, width.get_value() - 2 * pad);
                    cw = base_w * child->size_hint_x.get_value();
                }

                child->width.set(cw);
                child->height.set(ch);
                child->x.set(cx);
                child->y.set(cy);
                
                cy += ch + spc;
            }
        }
    }
}
