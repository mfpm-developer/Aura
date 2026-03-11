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
#include <algorithm>
#include "aura/input/touch_event.hpp"

namespace aura {
namespace md {

    class MDSlider : public MaterialWidget
    {
    public:
        MDSlider()
        {
            // Active Track (left of thumb)
            m_active_track_color = std::make_shared<SetColor>();
            m_active_track = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 10.0f, 16.0f, 8.0f); // M3: 16dp height

            // Inactive Track (right of thumb)
            m_inactive_track_color = std::make_shared<SetColor>();
            m_inactive_track = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 10.0f, 16.0f, 8.0f);

            // Thumb
            m_thumb_color = std::make_shared<SetColor>();
            m_thumb = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 20.0f, 20.0f, 10.0f); // M3 Continuous circle

            // State layer
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Ellipse>(0.0f, 0.0f, 40.0f, 40.0f);

            this->canvas->before->add(m_inactive_track_color);
            this->canvas->before->add(m_inactive_track);

            this->canvas->before->add(m_active_track_color);
            this->canvas->before->add(m_active_track);
            
            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_layer);
            
            this->canvas->after->add(m_thumb_color);
            this->canvas->after->add(m_thumb);

            auto update_val = [this](EventDispatcher*, const std::any&) {
                this->update_geometry();
                return false;
            };

            min.bind("on_min", update_val);
            max.bind("on_max", update_val);
            
            value.bind("on_value", [this, update_val](EventDispatcher* sender, const std::any& val) {
                update_val(sender, val);
                this->dispatch("on_value", val);
                return false;
            });

            this->bind("on_pos", update_val);
            this->bind("on_size", update_val);

            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& ev) {
                auto motion = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                if (motion && this->collide_point(motion->x, motion->y)) {
                    // Start in pressed state natively from MaterialWidget base 
                    // But explicitly demand we consume this event to prevent parent ScrollView capturing it
                    // if they are trying to steal.
                    return true; 
                }
                return false;
            });

            // M3 slider interaction drags
            this->bind("on_touch_move", [this](EventDispatcher*, const std::any& ev) {
                if (get_state() == State::Pressed)
                {
                    auto motion = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                    if (motion && motion->grab_current == this)
                    {
                        float mx = motion->x;
                        float percent = (mx - this->pos.get_value()[0]) / this->width.get_value();
                        percent = std::clamp(percent, 0.0f, 1.0f);
                        
                        float v = this->min.get_value() + percent * (this->max.get_value() - this->min.get_value());
                        this->value.set(v);
                        return true;
                    }
                }
                return false;
            });

            this->bind("on_touch_up", [this](EventDispatcher*, const std::any& ev) {
                auto motion = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                if (motion && motion->grab_current == this) {
                    motion->ungrab(this);
                    return true;
                }
                return false;
            });

            height.set(48.0f);

            on_theme_changed();
        }

        NumericProperty<float> value{"value", 50.0f};
        NumericProperty<float> min{"min", 0.0f};
        NumericProperty<float> max{"max", 100.0f};

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            const auto& primary = colors.primary;
            m_active_track_color->r = primary.r; m_active_track_color->g = primary.g; m_active_track_color->b = primary.b; m_active_track_color->a = primary.a;
            m_thumb_color->r = primary.r; m_thumb_color->g = primary.g; m_thumb_color->b = primary.b; m_thumb_color->a = primary.a;

            const auto& surface_var = colors.surface_variant;
            m_inactive_track_color->r = surface_var.r; m_inactive_track_color->g = surface_var.g; m_inactive_track_color->b = surface_var.b; m_inactive_track_color->a = surface_var.a;

            const auto& state_c = colors.primary;
            m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;

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
            float px = pos.get_value()[0];
            float py = pos.get_value()[1];
            float cy = py + h / 2.0f;
            
            float span = max.get_value() - min.get_value();
            float percent = 0.0f;
            if (span > 0) percent = (value.get_value() - min.get_value()) / span;
            percent = std::clamp(percent, 0.0f, 1.0f);

            float track_h = 16.0f; // M3 default track height
            float thumb_x = px + (w * percent);

            m_active_track->x = px;
            m_active_track->y = cy - (track_h / 2.0f);
            m_active_track->width = std::max(track_h, w * percent); // Ensure width > radius
            m_active_track->height = track_h;

            m_inactive_track->x = px; // Typically starts from left as well but under active
            m_inactive_track->y = cy - (track_h / 2.0f);
            m_inactive_track->width = w;
            m_inactive_track->height = track_h;

            m_thumb->x = thumb_x - 10.0f;
            m_thumb->y = cy - 10.0f;

            m_state_layer->x = thumb_x - 20.0f;
            m_state_layer->y = cy - 20.0f;
        }

        std::shared_ptr<SetColor> m_active_track_color;
        std::shared_ptr<RoundedRectangle> m_active_track;

        std::shared_ptr<SetColor> m_inactive_track_color;
        std::shared_ptr<RoundedRectangle> m_inactive_track;

        std::shared_ptr<SetColor> m_thumb_color;
        std::shared_ptr<RoundedRectangle> m_thumb;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Ellipse> m_state_layer;
    };

} // namespace md
} // namespace aura
