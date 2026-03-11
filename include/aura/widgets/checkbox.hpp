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

#include "aura/widgets/behaviors/button_behavior.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"

namespace aura
{
    /**
     * @class CheckBox
     * @brief A simple boolean toggle widget.
     */
    class CheckBox : public ButtonBehavior
    {
    public:
        CheckBox()
        {
            m_bg_color = std::make_shared<SetColor>(0.8f, 0.8f, 0.8f, 1.0f); // Light gray unchecked outline
            m_outline_line = std::make_shared<Line>();
            m_outline_line->width_val = 2.0f;
            
            m_check_color = std::make_shared<SetColor>(0.2f, 0.71f, 0.898f, 1.0f); // Holo Blue when checked
            m_check_rect = std::make_shared<Ellipse>(x.get_value(), y.get_value(), 0, 0); // hidden initially

            canvas->add(m_bg_color);
            canvas->add(m_outline_line);
            canvas->add(m_check_color);
            canvas->add(m_check_rect);

            auto update_graphics = [this](EventDispatcher*, const std::any&) {
                this->update_check();
                return false;
            };

            x.bind("on_x", update_graphics);
            y.bind("on_y", update_graphics);
            width.bind("on_width", update_graphics);
            height.bind("on_height", update_graphics);
            active.bind("on_active", update_graphics);
            state.bind("on_state", update_graphics);
            
            // Toggle active when state goes down
            state.bind("on_state", [this](EventDispatcher*, const std::any& val) {
                if (std::any_cast<std::string>(val) == "down") {
                    this->active.set(!this->active.get_value());
                }
                return false;
            });
            
            // Set some default sizing different from normal widgets
            width.set(30.0f);
            height.set(30.0f);
            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);
        }

        virtual ~CheckBox() = default;

        NumericProperty<bool> active{"active", false};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (Widget::set_property(name, value)) return true;
            try {
                if (name == "active") { active.set(value == "True" || value == "1" || value == "true"); return true; }
            } catch (...) { return false; }
            return false;
        }

    private:
        void update_check()
        {
            float vx = x.get_value();
            float vy = y.get_value();
            float vw = width.get_value();
            float vh = height.get_value();
            float size = std::min(vw, vh); // Keep it square/circular visually
            float cx = vx + (vw - size) / 2.0f;
            float cy = vy + (vh - size) / 2.0f;

            m_outline_line->rectangle = {cx, cy, size, size};

            if (active.get_value()) {
                m_check_rect->x = cx + size * 0.25f;
                m_check_rect->y = cy + size * 0.25f;
                m_check_rect->width = size * 0.5f;
                m_check_rect->height = size * 0.5f;
                
                // Active color for the outline too
                m_bg_color->r = 0.2f; m_bg_color->g = 0.71f; m_bg_color->b = 0.898f;
            } else {
                m_check_rect->width = 0.0f;
                m_check_rect->height = 0.0f;
                
                // Inactive color
                m_bg_color->r = 0.6f; m_bg_color->g = 0.6f; m_bg_color->b = 0.6f;
            }
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<Line> m_outline_line;
        std::shared_ptr<SetColor> m_check_color;
        std::shared_ptr<Ellipse> m_check_rect;
    };
}
