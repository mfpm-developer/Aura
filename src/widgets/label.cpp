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
#include "aura/widgets/label.hpp"
#include "aura/graphics/context_instructions.hpp" // If we want background colors, currently omitted for standard labels

namespace aura
{
    Label::Label()
    {
        m_text_color = std::make_shared<SetColor>(1.0f, 1.0f, 1.0f, 1.0f);
        m_text_instruction = std::make_shared<TextInstruction>(
            text.get_value(), 
            x.get_value(), 
            y.get_value(), 
            font_name.get_value(), 
            font_size.get_value()
        );

        canvas->add(m_text_color);
        canvas->add(m_text_instruction);

        auto trigger_update = [this](EventDispatcher*, const std::any&) {
            this->update_text_instruction();
            return false;
        };

        auto trigger_align = [this](EventDispatcher*, const std::any&) {
            this->update_alignment();
            return false;
        };

        text.bind("on_text", trigger_update);
        font_size.bind("on_font_size", trigger_update);
        font_name.bind("on_font_name", trigger_update);

        x.bind("on_x", trigger_align);
        y.bind("on_y", trigger_align);
        width.bind("on_width", trigger_align);
        height.bind("on_height", trigger_align);
        halign.bind("on_halign", trigger_align);
        valign.bind("on_valign", trigger_align);
    }

    bool Label::set_property(const std::string& name, const std::string& value)
    {
        if (Widget::set_property(name, value)) return true;
        try {
            if (name == "text") { text.set(value); return true; }
            if (name == "font_size") { font_size.set_value(std::stoi(value)); return true; }
            if (name == "font_name") { font_name.set(value); return true; }
            if (name == "halign") { halign.set(value); return true; }
            if (name == "valign") { valign.set(value); return true; }
        } catch (...) { return false; }
        return false;
    }

    void Label::update_text_instruction()
    {
        // Not all of these setters might exist on TextInstruction right away, 
        // we might need to recreate it if font path changes in a real implementation
        m_text_instruction->set_text(text.get_value());
        m_text_instruction->set_font_size(font_size.get_value());
        
        update_alignment();
    }

    void Label::update_alignment()
    {
        float txt_w = m_text_instruction->get_width();
        float txt_h = m_text_instruction->get_height();

        float new_x = x.get_value();
        float new_y = y.get_value();

        std::string h = halign.get_value();
        if (h == "center") new_x += (width.get_value() - txt_w) / 2.0f;
        else if (h == "right") new_x += (width.get_value() - txt_w);
        // else "left" is just x

        std::string v = valign.get_value();
        if (v == "center") new_y += (height.get_value() - txt_h) / 2.0f;
        else if (v == "top") new_y += (height.get_value() - txt_h);
        // else "bottom" is just y

        m_text_instruction->x = new_x;
        m_text_instruction->y = new_y;
    }
}
