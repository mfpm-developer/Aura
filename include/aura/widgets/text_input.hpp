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
#include "aura/properties/string_property.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/graphics/text_instruction.hpp"

namespace aura
{
    /**
     * @class TextInput
     * @brief A text editing widget.
     */
    class TextInput : public Widget
    {
    public:
        TextInput();
        virtual ~TextInput();

        StringProperty text{"text", ""};
        NumericProperty<float> padding_x{"padding_x", 6.0f};
        NumericProperty<float> padding_y{"padding_y", 6.0f};
        NumericProperty<bool> focus{"focus", false};

        bool set_property(const std::string& name, const std::string& value) override;
        bool on_touch_down(std::shared_ptr<TouchEvent> touch) override;

    private:
        void update_graphics();
        void bind_window_events();
        void unbind_window_events();

        bool handle_key_down(EventDispatcher* dispatcher, const std::any& key_event);
        bool handle_text_input(EventDispatcher* dispatcher, const std::any& text_str);

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg_rect;
        
        std::shared_ptr<SetColor> m_cursor_color;
        std::shared_ptr<Rectangle> m_cursor_rect;

        std::shared_ptr<SetColor> m_text_color;
        std::shared_ptr<TextInstruction> m_text_render;

        int m_cursor_index = 0;
        bool m_is_window_bound = false;
    };
}
