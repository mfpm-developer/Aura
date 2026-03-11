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

    class MDSwitch : public MaterialWidget
    {
    public:
        MDSwitch()
        {
            m_track_color = std::make_shared<SetColor>();
            m_track = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 52.0f, 32.0f, 16.0f); // Standard M3 Switch track

            // Inner carved track
            m_inner_color = std::make_shared<SetColor>();
            m_inner_track = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 48.0f, 28.0f, 14.0f);

            m_thumb_color = std::make_shared<SetColor>();
            m_thumb = std::make_shared<Ellipse>(0.0f, 0.0f, 16.0f, 16.0f);

            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Ellipse>(0.0f, 0.0f, 40.0f, 40.0f);

            this->canvas->before->add(m_track_color);
            this->canvas->before->add(m_track);
            
            this->canvas->before->add(m_inner_color);
            this->canvas->before->add(m_inner_track);

            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_layer);
            
            this->canvas->after->add(m_thumb_color);
            this->canvas->after->add(m_thumb);

            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                active.set(!active.get_value());
                return false;
            });

            active.bind("on_active", [this](EventDispatcher*, const std::any& val) {
                this->on_theme_changed(); 
                this->update_geometry(); // Thumb moves
                this->dispatch("on_active", val);
                return false;
            });

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            width.set(52.0f);
            height.set(32.0f);
            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);

            on_theme_changed();
        }

        NumericProperty<bool> active{"active", false};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "active") { active.set(value == "True" || value == "1" || value == "true"); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            if (active.get_value())
            {
                // M3 Switch On
                const auto& bg = colors.primary;
                m_track_color->r = bg.r; m_track_color->g = bg.g; m_track_color->b = bg.b; m_track_color->a = bg.a;
                m_inner_color->a = 0.0f;
                
                const auto& thumb = colors.on_primary;
                m_thumb_color->r = thumb.r; m_thumb_color->g = thumb.g; m_thumb_color->b = thumb.b; m_thumb_color->a = thumb.a;
                
                const auto& state_c = colors.primary;
                m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            }
            else
            {
                // M3 Switch Off
                const auto& outline = colors.outline;
                m_track_color->r = outline.r; m_track_color->g = outline.g; m_track_color->b = outline.b; m_track_color->a = outline.a;
                
                const auto& bg = colors.surface_variant;
                m_inner_color->r = bg.r; m_inner_color->g = bg.g; m_inner_color->b = bg.b; m_inner_color->a = bg.a;
                
                const auto& thumb = colors.outline;
                m_thumb_color->r = thumb.r; m_thumb_color->g = thumb.g; m_thumb_color->b = thumb.b; m_thumb_color->a = thumb.a;
                
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
            update_geometry(); // Thumb size grows slightly on hover/press in M3
        }

    private:
        void update_geometry()
        {
            float cx = pos.get_value()[0] + width.get_value() / 2.0f;
            float cy = pos.get_value()[1] + height.get_value() / 2.0f;

            // Track is mostly fixed within bounds
            m_track->x = pos.get_value()[0]; m_track->y = pos.get_value()[1];
            m_track->width = 52.0f; m_track->height = 32.0f;
            
            m_inner_track->x = pos.get_value()[0] + 2.0f; m_inner_track->y = pos.get_value()[1] + 2.0f;
            m_inner_track->width = 48.0f; m_inner_track->height = 28.0f;

            // Thumb sizing (M3: Matures from 16dp unpressed to 24dp pressed, and 24dp when active)
            float thumb_size = 16.0f;
            if (active.get_value()) thumb_size = 24.0f;
            if (get_state() == State::Pressed) thumb_size = 28.0f;

            m_thumb->width = thumb_size;
            m_thumb->height = thumb_size;

            // Thumb positioning
            float thumb_y = cy - (thumb_size / 2.0f);
            float thumb_x = active.get_value() 
                ? (pos.get_value()[0] + 52.0f - 4.0f - thumb_size) // Right justified
                : (pos.get_value()[0] + 4.0f); // Left justified

            m_thumb->x = thumb_x;
            m_thumb->y = thumb_y;

            // Ensure state layer centers on thumb
            m_state_layer->x = thumb_x + (thumb_size / 2.0f) - 20.0f;
            m_state_layer->y = cy - 20.0f;
        }

        std::shared_ptr<SetColor> m_track_color;
        std::shared_ptr<RoundedRectangle> m_track;

        std::shared_ptr<SetColor> m_inner_color;
        std::shared_ptr<RoundedRectangle> m_inner_track;

        std::shared_ptr<SetColor> m_thumb_color;
        std::shared_ptr<Ellipse> m_thumb;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Ellipse> m_state_layer;
    };

} // namespace md
} // namespace aura
