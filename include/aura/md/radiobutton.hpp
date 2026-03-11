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
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/context_instructions.hpp"

namespace aura {
namespace md {

    class MDRadioButton : public MaterialWidget
    {
    public:
        MDRadioButton()
        {
            m_outline_color = std::make_shared<SetColor>();
            m_bg_color = std::make_shared<SetColor>();
            
            // Radio outline (20dp standard)
            m_outline = std::make_shared<Ellipse>(0.0f, 0.0f, 20.0f, 20.0f);
            
            // Inner dot (10dp standard)
            m_dot_color = std::make_shared<SetColor>();
            m_dot = std::make_shared<Ellipse>(0.0f, 0.0f, 10.0f, 10.0f);
            
            // State layer (40dp)
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Ellipse>(0.0f, 0.0f, 40.0f, 40.0f);

            // M3 uses stroke for unselected radio, but aura Ellipse currently does filled. 
            // We simulate stroke by drawing an outer colored ellipse, then an inner transparent/surface ellipse.
            m_inner_bg_color = std::make_shared<SetColor>();
            m_inner_bg = std::make_shared<Ellipse>(0.0f, 0.0f, 16.0f, 16.0f); // 2dp stroke = 20-4 inner size

            this->canvas->before->add(m_state_color);
            this->canvas->before->add(m_state_layer);

            this->canvas->before->add(m_outline_color);
            this->canvas->before->add(m_outline);
            
            this->canvas->before->add(m_inner_bg_color);
            this->canvas->before->add(m_inner_bg);

            this->canvas->after->add(m_dot_color);
            this->canvas->after->add(m_dot);

            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                checked.set(true); // Radios don't uncheck themselves when clicked
                return false;
            });

            checked.bind("on_checked", [this](EventDispatcher*, const std::any& val) {
                this->on_theme_changed(); 
                this->dispatch("on_checked", val);
                return false;
            });

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            width.set(40.0f); 
            height.set(40.0f);
            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);

            on_theme_changed();
        }

        NumericProperty<bool> checked{"checked", false};

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            const auto& surf = colors.surface;
            m_inner_bg_color->r = surf.r; m_inner_bg_color->g = surf.g; m_inner_bg_color->b = surf.b; m_inner_bg_color->a = surf.a;

            if (checked.get_value())
            {
                // Selected: Primary color for outline and dot
                const auto& primary = colors.primary;
                m_outline_color->r = primary.r; m_outline_color->g = primary.g; m_outline_color->b = primary.b; m_outline_color->a = primary.a;
                m_dot_color->r = primary.r; m_dot_color->g = primary.g; m_dot_color->b = primary.b; m_dot_color->a = primary.a;
                
                const auto& state_c = colors.primary;
                m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            }
            else
            {
                // Unselected: on-surface-variant outline, transparent dot
                const auto& outline = colors.on_surface_variant;
                m_outline_color->r = outline.r; m_outline_color->g = outline.g; m_outline_color->b = outline.b; m_outline_color->a = outline.a;
                
                m_dot_color->a = 0.0f;
                
                const auto& state_c = colors.on_surface;
                m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            }

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
            float cx = pos.get_value()[0] + width.get_value() / 2.0f;
            float cy = pos.get_value()[1] + height.get_value() / 2.0f;

            m_outline->x = cx - 10.0f; m_outline->y = cy - 10.0f;
            m_outline->width = 20.0f; m_outline->height = 20.0f;

            m_inner_bg->x = cx - 8.0f; m_inner_bg->y = cy - 8.0f;
            m_inner_bg->width = 16.0f; m_inner_bg->height = 16.0f;

            m_dot->x = cx - 5.0f; m_dot->y = cy - 5.0f;
            m_dot->width = 10.0f; m_dot->height = 10.0f;

            m_state_layer->x = cx - 20.0f; m_state_layer->y = cy - 20.0f;
            m_state_layer->width = 40.0f; m_state_layer->height = 40.0f;
        }

        std::shared_ptr<SetColor> m_bg_color;
        
        std::shared_ptr<SetColor> m_outline_color;
        std::shared_ptr<Ellipse> m_outline;

        std::shared_ptr<SetColor> m_inner_bg_color;
        std::shared_ptr<Ellipse> m_inner_bg;

        std::shared_ptr<SetColor> m_dot_color;
        std::shared_ptr<Ellipse> m_dot;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Ellipse> m_state_layer;
    };

} // namespace md
} // namespace aura
