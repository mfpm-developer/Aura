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
#include <memory>

namespace aura {
namespace md {

    class MDFAB : public MaterialWidget
    {
    public:
        MDFAB(bool extended = false)
            : m_is_extended(extended)
        {
            m_elevation = 3;
            m_radius = 16.0f; // Standard FAB corner radius

            m_shadow_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.15f);
            m_shadow_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 56.0f, 56.0f, m_radius);

            m_bg_color = std::make_shared<SetColor>(1.0f, 1.0f, 1.0f, 1.0f);
            m_bg_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 56.0f, 56.0f, m_radius);

            m_state_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.0f);
            m_state_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 56.0f, 56.0f, m_radius);

            this->canvas->before->add(m_shadow_color);
            this->canvas->before->add(m_shadow_rect);
            
            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg_rect);
            
            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_rect);

            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);
            width.set(m_is_extended ? 100.0f : 56.0f);
            height.set(56.0f); // Default standard FAB height

            m_label = std::make_shared<MDText>();
            if (m_is_extended)
            {
                m_label->set_text("Action");
            }
            else
            {
                m_label->set_text("+"); // Placeholder for an icon
            }
            this->add_widget(m_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            on_theme_changed();
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            // Primary Container is standard for FABs in M3
            const auto& bg = colors.primary_container;
            m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;

            const auto& shadow = colors.shadow;
            m_shadow_color->r = shadow.r; m_shadow_color->g = shadow.g; m_shadow_color->b = shadow.b; 

            const auto& on_bg = colors.on_primary_container;
            m_state_color->r = on_bg.r; m_state_color->g = on_bg.g; m_state_color->b = on_bg.b;

            if (m_label) m_label->set_text_color(on_bg);

            update_state_layer();
        }

        void on_state_changed() override
        {
            update_state_layer();
            if (get_state() == State::Pressed) {
                m_current_elevation = m_elevation - 1;
            } else if (get_state() == State::Hovered) {
                m_current_elevation = m_elevation + 1; // Elevates on hover
            } else {
                m_current_elevation = m_elevation;
            }
            update_geometry();
        }

    private:
        void update_geometry()
        {
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];
            float w = width.get_value();
            float h = height.get_value();

            m_bg_rect->x = x;
            m_bg_rect->y = y;
            m_bg_rect->width = w;
            m_bg_rect->height = h;

            m_state_rect->x = x;
            m_state_rect->y = y;
            m_state_rect->width = w;
            m_state_rect->height = h;

            float shadow_offset_y = m_current_elevation * 1.5f;

            m_shadow_rect->x = x;
            m_shadow_rect->y = y + shadow_offset_y;
            m_shadow_rect->width = w;
            m_shadow_rect->height = h;
            
            m_shadow_color->a = (m_current_elevation > 0) ? 0.20f : 0.0f;

            if (m_label) {
                m_label->pos.set(std::vector<float>{x, y});
                m_label->width.set(w);
                m_label->height.set(h);
            }
        }

        void update_state_layer()
        {
            m_state_color->a = Theme::get().get_state_layer_alpha(get_state());
        }

        bool m_is_extended{false};

        int m_elevation{3};
        int m_current_elevation{3};
        float m_radius{16.0f};

        std::shared_ptr<SetColor> m_shadow_color;
        std::shared_ptr<RoundedRectangle> m_shadow_rect;

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg_rect;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<RoundedRectangle> m_state_rect;
        
        std::shared_ptr<MDText> m_label;
    };

} // namespace md
} // namespace aura
