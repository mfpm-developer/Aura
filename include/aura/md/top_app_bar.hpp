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

    // Center-aligned M3 Top App Bar
    class MDTopAppBar : public MaterialWidget
    {
    public:
        MDTopAppBar()
        {
            m_bg_color = std::make_shared<SetColor>();
            m_bg = std::make_shared<Rectangle>(0.0f, 0.0f, 800.0f, 64.0f); // Default 64dp Mediumish

            // Shadows for scrolled states
            m_shadow_color = std::make_shared<SetColor>();
            m_shadow = std::make_shared<Rectangle>(); 

            this->canvas->before->add(m_shadow_color);
            this->canvas->before->add(m_shadow);
            
            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg);

            m_title = std::make_shared<MDText>();
            m_title->set_text("Title");
            this->add_widget(m_title);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });

            height.set(64.0f);
            size_hint_y.set(-1.0f);
            size_hint_x.set(1.0f);
            
            on_theme_changed();
        }
        
        StringProperty title{"title", "Title"};
        NumericProperty<bool> scrolled{"scrolled", false}; // Triggers elevation in M3

        void set_title(const std::string& t) {
            title.set(t);
            m_title->set_text(t);
        }

        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "title") { set_title(value); return true; }
            if (name == "scrolled") { 
                scrolled.set(value == "True" || value == "1"); 
                on_theme_changed(); 
                update_layout();
                return true; 
            }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            // When scrolled M3 typically transitions from surface to surface_container
            // We use surface variant to show the distinction visually right now
            if (scrolled.get_value()) {
                const auto& bg = colors.surface_variant;
                m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;
                m_shadow_color->a = 0.12f; // Lifted shadow
            } else {
                const auto& bg = colors.surface;
                m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;
                m_shadow_color->a = 0.0f; // flat
            }
            
            const auto& shadow = colors.shadow;
            m_shadow_color->r = shadow.r; m_shadow_color->g = shadow.g; m_shadow_color->b = shadow.b; 

            const auto& txt = colors.on_surface;
            m_title->set_text_color(txt);
        }

    private:
        void update_layout()
        {
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_bg->x = x; m_bg->y = y;
            m_bg->width = w; m_bg->height = h;

            // Downward shadow 
            m_shadow->x = x; m_shadow->y = y + h;
            m_shadow->width = w; m_shadow->height = 6.0f;

            // Center align title (M3 Center-Aligned Top App Bar)
            m_title->pos.set(std::vector<float>{x, y});
            m_title->width.set(w);
            m_title->height.set(h);
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<Rectangle> m_bg;

        std::shared_ptr<SetColor> m_shadow_color;
        std::shared_ptr<Rectangle> m_shadow;

        std::shared_ptr<MDText> m_title;
    };

} // namespace md
} // namespace aura
