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
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/md/text.hpp"
#include "aura/properties/string_property.hpp"
#include <memory>

namespace aura {
namespace md {

    // MDChip representing a simple Assist or Filter chip
    class MDChip : public MaterialWidget
    {
    public:
        MDChip()
        {
            m_bg_color = std::make_shared<SetColor>();
            m_bg = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 32.0f, 8.0f);

            m_outline_color = std::make_shared<SetColor>();
            m_outline = std::make_shared<Line>();
            m_outline->width_val = 1.0f;

            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 32.0f, 8.0f);

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg);
            
            this->canvas->before->add(m_outline_color);
            this->canvas->before->add(m_outline);

            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_layer);

            m_label = std::make_shared<MDText>();
            m_label->set_text("Chip");
            this->add_widget(m_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            size_hint_y.set(-1.0f);
            size_hint_x.set(-1.0f); // Chips size to their content usually
            height.set(32.0f);
            width.set(80.0f);

            on_theme_changed();
        }

        StringProperty text{"text", "Chip"};

        void set_text(const std::string& t) {
            text.set(t);
            m_label->set_text(t);
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "text") { set_text(value); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            // Default style: Assist Chip (Transparent BG, Outlined)
            m_bg_color->a = 0.0f; // Transparent
            
            const auto& outline_c = colors.outline;
            m_outline_color->r = outline_c.r; m_outline_color->g = outline_c.g; m_outline_color->b = outline_c.b; m_outline_color->a = outline_c.a;

            const auto& on_surf = colors.on_surface;
            m_label->set_text_color(on_surf);
            m_state_color->r = on_surf.r; m_state_color->g = on_surf.g; m_state_color->b = on_surf.b;

            update_state_layer();
        }

        void update_state_layer()
        {
            m_state_color->a = Theme::get().get_state_layer_alpha(get_state());
        }

        void on_state_changed() override
        {
            update_state_layer();
        }

    private:
        void update_geometry()
        {
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_bg->x = x; m_bg->y = y;
            m_bg->width = w; m_bg->height = h;

            m_state_layer->x = x; m_state_layer->y = y;
            m_state_layer->width = w; m_state_layer->height = h;

            // Simple rectangle outline approximation for now
            m_outline->rectangle = {x, y, w, h};

            // Needs proper alignment/padding, usually 12-16dp on sides
            m_label->pos.set(std::vector<float>{x, y});
            m_label->width.set(w);
            m_label->height.set(h);
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg;

        std::shared_ptr<SetColor> m_outline_color;
        std::shared_ptr<Line> m_outline;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<RoundedRectangle> m_state_layer;

        std::shared_ptr<MDText> m_label;
    };

} // namespace md
} // namespace aura
