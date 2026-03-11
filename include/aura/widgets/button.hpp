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
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/graphics/text_instruction.hpp"
#include <iostream>

namespace aura
{
    /**
     * @class Button
     * @brief A simple button widget that reacts to touch.
     */
    class Button : public ButtonBehavior
    {
    public:
        Button()
        {
            // Set up our graphics representation
            m_bg_color = std::make_shared<SetColor>(0.2f, 0.2f, 0.2f, 1.0f);
            m_rect = std::make_shared<RoundedRectangle>(x.get_value(), y.get_value(), width.get_value(), height.get_value(), 5.0f);
            
            m_text_color = std::make_shared<SetColor>(1.0f, 1.0f, 1.0f, 1.0f);
            m_text_render = std::make_shared<TextInstruction>(text.get_value(), x.get_value(), y.get_value());

            canvas->add(m_bg_color);
            canvas->add(m_rect);
            canvas->add(m_text_color);
            canvas->add(m_text_render);

            // Bind position and size to update the rectangle
            auto update_rect = [this](EventDispatcher*, const std::any&) {
                m_rect->x = this->x.get_value();
                m_rect->y = this->y.get_value();
                m_rect->width = this->width.get_value();
                m_rect->height = this->height.get_value();
                m_rect->radius = 5.0f;

                // Center text
                m_text_render->x = this->x.get_value() + (this->width.get_value() - m_text_render->get_width()) / 2.0f;
                m_text_render->y = this->y.get_value() + (this->height.get_value() - m_text_render->get_height()) / 2.0f;
                return false;
            };

            auto update_text = [this, update_rect](EventDispatcher*, const std::any& val) {
                m_text_render->set_text(std::any_cast<std::string>(val));
                update_rect(nullptr, std::any()); // Re-center now that width/height might have changed
                return false;
            };

            text.bind("on_text", update_text);

            x.bind("on_x", update_rect);
            y.bind("on_y", update_rect);
            width.bind("on_width", update_rect);
            height.bind("on_height", update_rect);

            // Bind to state change to alter color
            state.bind("on_state", [this](EventDispatcher*, const std::any& value) {
                std::string current_state = std::any_cast<std::string>(value);
                if (current_state == "down")
                {
                    m_bg_color->r = 0.2f; m_bg_color->g = 0.71f; m_bg_color->b = 0.898f; // Kivy holo blue tint
                }
                else
                {
                    m_bg_color->r = 0.2f; m_bg_color->g = 0.2f; m_bg_color->b = 0.2f;
                }
                return false;
            });
        }

        virtual ~Button() = default;

        StringProperty text{"text", ""};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (Widget::set_property(name, value)) return true;
            try {
                if (name == "text") { text.set(value); return true; }
            } catch (...) { return false; }
            return false;
        }

    private:
        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_rect;
        std::shared_ptr<SetColor> m_text_color;
        std::shared_ptr<TextInstruction> m_text_render;
    };
}
