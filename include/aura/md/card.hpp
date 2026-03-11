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
#include <memory>
#include <algorithm>

namespace aura {
namespace md {

    // Core Material surface with elevation shadow implementation
    class MDCard : public MaterialWidget
    {
    public:
        MDCard()
        {
            m_elevation = 1;
            m_radius = 12.0f;

            m_shadow_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.15f);
            m_shadow_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 100.0f, m_radius);

            m_surface_color = std::make_shared<SetColor>(1.0f, 1.0f, 1.0f, 1.0f);
            m_surface_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 100.0f, m_radius);

            m_state_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.0f);
            m_state_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 100.0f, m_radius);

            this->canvas->before->add(m_shadow_color);
            this->canvas->before->add(m_shadow_rect);
            
            this->canvas->before->add(m_surface_color);
            this->canvas->before->add(m_surface_rect);
            
            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_rect);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            on_theme_changed();
        }

        virtual ~MDCard() = default;

        int get_elevation() const { return m_elevation; }
        void set_elevation(int elevation) { m_elevation = elevation; update_geometry(); }

        float get_radius() const { return m_radius; }
        void set_radius(float radius) {
            m_radius = radius;
            m_shadow_rect->radius = radius;
            m_surface_rect->radius = radius;
            m_state_rect->radius = radius;
        }

        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "padding") { padding.set(std::stof(value)); return true; }
            return MaterialWidget::set_property(name, value);
        }

        NumericProperty<float> padding{"padding", 16.0f};

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& s = colors.surface;
            m_surface_color->r = s.r; m_surface_color->g = s.g; m_surface_color->b = s.b; m_surface_color->a = s.a;

            const auto& shadow = colors.shadow;
            m_shadow_color->r = shadow.r; m_shadow_color->g = shadow.g; m_shadow_color->b = shadow.b; 

            const auto& on_s = colors.on_surface;
            m_state_color->r = on_s.r; m_state_color->g = on_s.g; m_state_color->b = on_s.b;

            update_state_layer();
        }

        void on_state_changed() override
        {
            update_state_layer();
            
            if (get_state() == State::Pressed) {
                m_current_elevation = std::max(0, m_elevation - 1);
            } else if (get_state() == State::Hovered) {
                m_current_elevation = m_elevation + 1;
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

            m_surface_rect->x = x;
            m_surface_rect->y = y;
            m_surface_rect->width = w;
            m_surface_rect->height = h;

            m_state_rect->x = x;
            m_state_rect->y = y;
            m_state_rect->width = w;
            m_state_rect->height = h;

            float shadow_offset_y = m_current_elevation * 1.5f;

            m_shadow_rect->x = x;
            m_shadow_rect->y = y + shadow_offset_y;
            m_shadow_rect->width = w;
            m_shadow_rect->height = h;
            
            m_shadow_color->a = (m_current_elevation > 0) ? 0.15f : 0.0f;

            // Propagate bounds to children so they stay within the card physically
            float p = padding.get_value();
            for (auto& child : get_children()) {
                child->pos.set(std::vector<float>{x + p, y + p});
                child->size.set(std::vector<float>{std::max(0.0f, w - p * 2.0f), std::max(0.0f, h - p * 2.0f)});
                // To force internal properties without recursion lock
                child->width.set(std::max(0.0f, w - p * 2.0f));
                child->height.set(std::max(0.0f, h - p * 2.0f));
                child->x.set(x + p);
                child->y.set(y + p);
            }
        }

        void update_state_layer()
        {
            m_state_color->a = Theme::get().get_state_layer_alpha(get_state());
        }

        int m_elevation{1};
        int m_current_elevation{1};
        float m_radius{12.0f};

        std::shared_ptr<SetColor> m_shadow_color;
        std::shared_ptr<RoundedRectangle> m_shadow_rect;

        std::shared_ptr<SetColor> m_surface_color;
        std::shared_ptr<RoundedRectangle> m_surface_rect;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<RoundedRectangle> m_state_rect;
    };

} // namespace md
} // namespace aura
