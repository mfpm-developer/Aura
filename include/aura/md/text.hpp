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

#include "aura/md/material_widget.hpp"
#include "aura/graphics/text_instruction.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/properties/string_property.hpp"
#include <memory>

namespace aura {
namespace md {

    // Helper text component mapped to M3 typography
    class MDText : public MaterialWidget
    {
    public:
        MDText()
        {
            m_color_inst = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 1.0f);
            m_text_inst = std::make_shared<aura::TextInstruction>("", 0, 0, "Roboto-Regular.ttf", 14);

            this->canvas->add(m_color_inst);
            this->canvas->add(m_text_inst);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            text.bind("on_text", [this](EventDispatcher*, const std::any& val) {
                m_text_inst->set_text(std::any_cast<std::string>(val));
                this->update_geometry();
                return false;
            });

            auto update_val = [this](EventDispatcher*, const std::any&) {
                this->update_geometry();
                return false;
            };
            halign.bind("on_halign", update_val);
            valign.bind("on_valign", update_val);
            
            on_theme_changed();
        }

        StringProperty text{"text", ""};
        StringProperty halign{"halign", "center"};
        StringProperty valign{"valign", "middle"};

        void set_text(const std::string& t) { text.set(t); }

        // Allows explicit override of text color, ignoring theme default if set
        void set_text_color(const aura::Color& c)
        {
            m_explicit_color = true;
            m_color_inst->r = c.r; m_color_inst->g = c.g; m_color_inst->b = c.b; m_color_inst->a = c.a;
        }

        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "text") { set_text(value); return true; }
            if (name == "halign") { halign.set(value); return true; }
            if (name == "valign") { valign.set(value); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            if (m_explicit_color) return;

            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            // Default maps to on_surface for standard body text
            const auto& c = colors.on_surface;
            m_color_inst->r = c.r; m_color_inst->g = c.g; m_color_inst->b = c.b; m_color_inst->a = c.a;
        }

        void update_geometry()
        {
            float w = width.get_value();
            float h = height.get_value();
            float txt_w = m_text_inst->get_width();
            float txt_h = m_text_inst->get_height();

            float tx = pos.get_value()[0];
            float ty = pos.get_value()[1];

            if (halign.get_value() == "left") {
                m_text_inst->x = tx;
            } else if (halign.get_value() == "right") {
                m_text_inst->x = tx + (w - txt_w);
            } else { // center
                m_text_inst->x = tx + (w - txt_w) / 2.0f;
            }

            if (valign.get_value() == "top") {
                m_text_inst->y = ty;
            } else if (valign.get_value() == "bottom") {
                m_text_inst->y = ty + (h - txt_h);
            } else { // middle
                m_text_inst->y = ty + (h - txt_h) / 2.0f;
            }
        }

    private:
        std::shared_ptr<SetColor> m_color_inst;
        std::shared_ptr<aura::TextInstruction> m_text_inst;
        bool m_explicit_color{false};
    };

} // namespace md
} // namespace aura
