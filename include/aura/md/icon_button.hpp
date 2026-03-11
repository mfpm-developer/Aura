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
#include "aura/md/text.hpp"

namespace aura {
namespace md {

    class MDIconButton : public MaterialWidget
    {
    public:
        MDIconButton()
        {
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Ellipse>();

            this->canvas->before->add(m_state_color);
            this->canvas->before->add(m_state_layer);

            m_icon_label = std::make_shared<MDText>();
            m_icon_label->set_text("star"); // Placeholder for Material Symbols
            this->add_widget(m_icon_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                 // Forward release to any bindings on the icon button itself if needed
                return false;
            });

            width.set(40.0f);
            height.set(40.0f);
            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);

            on_theme_changed();
        }

        StringProperty icon{"icon", "star"};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "icon") { set_icon(value); return true; }
            return MaterialWidget::set_property(name, value);
        }

        void set_icon(const std::string& val) {
            icon.set(val);
            if (m_icon_label) m_icon_label->set_text(val);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& on_surf_var = colors.on_surface_variant;
            
            m_state_color->r = on_surf_var.r; 
            m_state_color->g = on_surf_var.g; 
            m_state_color->b = on_surf_var.b;
            
            if (m_icon_label) m_icon_label->set_text_color(on_surf_var);

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

            m_state_layer->width = std::min(width.get_value(), height.get_value());
            m_state_layer->height = m_state_layer->width;
            m_state_layer->x = cx - m_state_layer->width / 2.0f;
            m_state_layer->y = cy - m_state_layer->height / 2.0f;

            if (m_icon_label) {
                m_icon_label->pos.set(std::vector<float>{pos.get_value()[0], pos.get_value()[1]});
                m_icon_label->width.set(width.get_value());
                m_icon_label->height.set(height.get_value());
            }
        }

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Ellipse> m_state_layer;
        
        std::shared_ptr<MDText> m_icon_label;
    };

} // namespace md
} // namespace aura
