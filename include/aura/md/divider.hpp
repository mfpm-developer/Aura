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

#include "aura/widgets/widget.hpp"
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/md/theme.hpp"

namespace aura {
namespace md {

    class MDDivider : public Widget
    {
    public:
        MDDivider(const std::string& orientation_val = "horizontal")
        {
            m_color = std::make_shared<SetColor>();
            m_rect = std::make_shared<Rectangle>();

            this->canvas->before->add(m_color);
            this->canvas->before->add(m_rect);

            orientation.set(orientation_val);
            setup_dimensions();

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            
            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });
            
            orientation.bind("on_orientation", [this](EventDispatcher*, const std::any&) {
                this->setup_dimensions();
                this->update_geometry();
                return false;
            });

            on_theme_changed();
        }

        StringProperty orientation{"orientation", "horizontal"};

    protected:
        void on_theme_changed()
        {
            const auto& theme = Theme::get();
            const auto& c = theme.colors().outline_variant;
            
            m_color->r = c.r; 
            m_color->g = c.g; 
            m_color->b = c.b; 
            m_color->a = c.a;
        }

    private:
        void setup_dimensions()
        {
            if (orientation.get_value() == "horizontal") {
                size_hint_y.set(-1.0f);
                height.set(1.0f);
                size_hint_x.set(1.0f); // Expand horizontally
            } else {
                size_hint_x.set(-1.0f);
                width.set(1.0f);
                size_hint_y.set(1.0f); // Expand vertically
            }
        }

        void update_geometry()
        {
            m_rect->x = pos.get_value()[0];
            m_rect->y = pos.get_value()[1];
            m_rect->width = width.get_value();
            m_rect->height = height.get_value();
        }

        std::shared_ptr<SetColor> m_color;
        std::shared_ptr<Rectangle> m_rect;
    };

} // namespace md
} // namespace aura
