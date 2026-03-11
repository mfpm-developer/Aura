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
#include <memory>
#include "aura/input/touch_event.hpp"
#include <vector>

namespace aura {
namespace md {

    class MDNavigationDrawer : public MaterialWidget
    {
    public:
        MDNavigationDrawer()
        {
            m_scrim_color = std::make_shared<SetColor>();
            m_scrim = std::make_shared<Rectangle>(0.0f, 0.0f, 800.0f, 600.0f);

            m_bg_color = std::make_shared<SetColor>();
            m_bg = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 360.0f, 600.0f, 16.0f); // M3 drawers have rounded right corners

            m_shadow_color = std::make_shared<SetColor>();
            m_shadow = std::make_shared<Rectangle>();

            this->canvas->before->add(m_scrim_color);
            this->canvas->before->add(m_scrim);

            this->canvas->before->add(m_shadow_color);
            this->canvas->before->add(m_shadow);

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });

            opened.bind("on_opened", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                this->update_layout(); // Animate open/close
                return false;
            });

            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& ev) {
                if (!opened.get_value()) return false;
                
                auto touch = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                if (touch && touch->x > width.get_value()) { // Clicked on scrim
                    opened.set(false);
                    return true;
                }
                return false;
            });

            width.set(360.0f); // M3 standard drawer max width on mobile, scaled up depending on screen
            size_hint_y.set(1.0f);

            on_theme_changed();
        }

        NumericProperty<bool> opened{"opened", false};

        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "opened") { opened.set(value == "True"); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& bg = colors.surface_variant;
            m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;

            const auto& scrim = colors.scrim;
            m_scrim_color->r = scrim.r; m_scrim_color->g = scrim.g; m_scrim_color->b = scrim.b; 
            m_scrim_color->a = opened.get_value() ? 0.32f : 0.0f; // M3 standard scrim opacity
            
            const auto& shadow = colors.shadow;
            m_shadow_color->r = shadow.r; m_shadow_color->g = shadow.g; m_shadow_color->b = shadow.b; 
        }

    private:
        void update_layout()
        {
            // Full screen scrim
            m_scrim->x = 0; m_scrim->y = 0;
            // Hack: assuming screen size bounds for scrim, in a real layout engine we'd anchor to root
            m_scrim->width = 10000.0f; m_scrim->height = 10000.0f; 

            float w = width.get_value();
            float h = height.get_value();
            
            // Drawer slides in from left
            float target_x = opened.get_value() ? 0.0f : -w;

            m_bg->x = target_x; 
            m_bg->y = pos.get_value()[1]; // usually 0
            m_bg->width = w; 
            m_bg->height = h;

            // Right side shadow
            m_shadow->x = target_x + w;
            m_shadow->y = pos.get_value()[1];
            m_shadow->width = 16.0f;
            m_shadow->height = h;

            // M3 Elevation Level 1 = 16dp radius right corners
            m_shadow_color->a = opened.get_value() ? 0.08f : 0.0f;

            // Update children positions based on offset
            for (auto child : children) {
                child->pos.set(std::vector<float>{target_x + child->pos.get_value()[0], child->pos.get_value()[1]});
            }
        }

        std::shared_ptr<SetColor> m_scrim_color;
        std::shared_ptr<Rectangle> m_scrim;

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg;

        std::shared_ptr<SetColor> m_shadow_color;
        std::shared_ptr<Rectangle> m_shadow;
    };

} // namespace md
} // namespace aura
