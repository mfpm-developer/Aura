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

    class MDCheckbox : public MaterialWidget
    {
    public:
        MDCheckbox()
        {
            m_bg_color = std::make_shared<SetColor>();
            
            // Checkbox outline box
            m_box = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 18.0f, 18.0f, 2.0f); // M3 standard: 18x18, 2dp radius
            
            // State layer (hover/focus overlay, circular for M3 checkboxes)
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Ellipse>(0.0f, 0.0f, 40.0f, 40.0f);
            
            // Checkmark paths
            m_check_color = std::make_shared<SetColor>();
            m_check_line = std::make_shared<Line>();
            m_check_line->width_val = 2.0f;

            // Box outline line instruction (simulated stroke)
            m_inner_bg_color = std::make_shared<SetColor>();
            m_inner_box = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 14.0f, 14.0f, 1.0f); // 2dp stroke = 18-4

            this->canvas->before->add(m_state_color);
            this->canvas->before->add(m_state_layer);

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_box);
            
            this->canvas->before->add(m_inner_bg_color);
            this->canvas->before->add(m_inner_box);

            this->canvas->after->add(m_check_color);
            this->canvas->after->add(m_check_line);

            // Inherit toggle logic
            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                checked.set(!checked.get_value());
                return false;
            });

            checked.bind("on_checked", [this](EventDispatcher*, const std::any& val) {
                this->on_theme_changed(); // Re-apply colors based on checked state
                this->dispatch("on_checked", val);
                return false;
            });

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            width.set(40.0f); // Touch target size
            height.set(40.0f);
            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);

            on_theme_changed();
        }

        NumericProperty<bool> checked{"checked", false};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "checked") { checked.set(value == "True" || value == "1" || value == "true"); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            const auto& surf = colors.surface;
            m_inner_bg_color->r = surf.r; m_inner_bg_color->g = surf.g; m_inner_bg_color->b = surf.b; m_inner_bg_color->a = surf.a;

            if (checked.get_value())
            {
                // M3 Checked: Primary fill, OnPrimary checkmark, no outline (inner box hidden)
                const auto& bg = colors.primary;
                m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;
                m_inner_bg_color->a = 0.0f;
                
                const auto& fg = colors.on_primary;
                m_check_color->r = fg.r; m_check_color->g = fg.g; m_check_color->b = fg.b; m_check_color->a = fg.a;
                
                // M3 State layer
                const auto& state_c = colors.primary;
                m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            }
            else
            {
                // M3 Unchecked: Transparent inner fill, OnSurfaceVariant outer outline
                m_check_color->a = 0.0f;
                
                const auto& outline = colors.on_surface_variant;
                m_bg_color->r = outline.r; m_bg_color->g = outline.g; m_bg_color->b = outline.b; m_bg_color->a = outline.a;
                
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

            float box_size = 18.0f;
            float bx = cx - box_size / 2.0f;
            float by = cy - box_size / 2.0f;

            m_box->x = bx; m_box->y = by;
            m_box->width = box_size; m_box->height = box_size;

            m_inner_box->x = bx + 2.0f; m_inner_box->y = by + 2.0f;
            m_inner_box->width = box_size - 4.0f; m_inner_box->height = box_size - 4.0f;

            m_state_layer->x = cx - 20.0f;
            m_state_layer->y = cy - 20.0f;

            if (checked.get_value())
            {
                // Checkmark path (simplistic lines mapping the v shape)
                m_check_line->points = {
                    cx - 4.0f, cy + 1.0f,
                    cx - 1.0f, cy + 4.0f,
                    cx + 5.0f, cy - 4.0f
                };
            }
            else
            {
                m_check_line->points.clear();
            }
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_box;

        std::shared_ptr<SetColor> m_inner_bg_color;
        std::shared_ptr<RoundedRectangle> m_inner_box;

        std::shared_ptr<SetColor> m_check_color;
        std::shared_ptr<Line> m_check_line;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Ellipse> m_state_layer;
    };

} // namespace md
} // namespace aura
