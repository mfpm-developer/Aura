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

#include "aura/widgets/layouts/box_layout.hpp"
#include "aura/md/material_widget.hpp"
#include "aura/md/text.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/properties/string_property.hpp"
#include <memory>
#include <vector>

namespace aura {
namespace md {

    class MDTab : public MaterialWidget
    {
    public:
        MDTab()
        {
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Rectangle>();
            
            m_indicator_color = std::make_shared<SetColor>();
            m_indicator = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 0.0f, 0.0f, 3.0f); // 3dp top radius

            this->canvas->before->add(m_state_color);
            this->canvas->before->add(m_state_layer);
            
            this->canvas->after->add(m_indicator_color);
            this->canvas->after->add(m_indicator);

            m_label = std::make_shared<MDText>();
            m_label->set_text("Tab");
            this->add_widget(m_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            active.bind("on_active", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                this->update_geometry();
                return false;
            });

            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                dispatch("on_selected", std::any());
                return false;
            });

            size_hint_y.set(-1.0f);
            height.set(48.0f); // Default Primary Tab height

            on_theme_changed();
        }

        NumericProperty<bool> active{"active", false};
        StringProperty text{"text", "Tab"};

        void set_text(const std::string& t) {
            text.set(t);
            m_label->set_text(t);
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "text") { set_text(value); return true; }
            if (name == "active") { active.set(value == "true" || value == "1"); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            if (active.get_value())
            {
                const auto& pri = colors.primary;
                m_indicator_color->r = pri.r; m_indicator_color->g = pri.g; m_indicator_color->b = pri.b; m_indicator_color->a = pri.a;
                m_label->set_text_color(pri);

                m_state_color->r = pri.r; m_state_color->g = pri.g; m_state_color->b = pri.b;
            }
            else
            {
                m_indicator_color->a = 0.0f; // Hidden when inactive
                
                const auto& on_surf_v = colors.on_surface_variant;
                m_label->set_text_color(on_surf_v);

                const auto& on_surf = colors.on_surface;
                m_state_color->r = on_surf.r; m_state_color->g = on_surf.g; m_state_color->b = on_surf.b;
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
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_state_layer->x = x;
            m_state_layer->y = y;
            m_state_layer->width = w;
            m_state_layer->height = h;

            // Indicator at the bottom, 3dp height
            float ind_w = w * 0.5f; // Short width for primary tabs usually
            m_indicator->x = x + (w - ind_w) / 2.0f;
            m_indicator->y = y + h - 3.0f;
            m_indicator->width = ind_w;
            m_indicator->height = 3.0f;

            m_label->pos.set(std::vector<float>{x, y});
            m_label->width.set(w);
            m_label->height.set(h);
        }

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Rectangle> m_state_layer;
        
        std::shared_ptr<SetColor> m_indicator_color;
        std::shared_ptr<RoundedRectangle> m_indicator;

        std::shared_ptr<MDText> m_label;
    };


    class MDTabs : public BoxLayout
    {
    public:
        MDTabs()
        {
            orientation.set("horizontal");
            size_hint_y.set(-1.0f);
            height.set(48.0f);

            // Background line (surface variant divider)
            m_bg_line_color = std::make_shared<SetColor>();
            m_bg_line = std::make_shared<Rectangle>();

            this->canvas->before->add(m_bg_line_color);
            this->canvas->before->add(m_bg_line);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });

            on_theme_changed();
        }

        void add_tab(const std::string& text)
        {
            auto tab = std::make_shared<MDTab>();
            tab->set_text(text);
            
            size_t idx = m_tabs.size();
            m_tabs.push_back(tab);
            this->add_widget(tab);

            if (m_tabs.size() == 1) {
                tab->active.set(true);
                m_active_index = 0;
            }

            tab->bind("on_selected", [this, idx](EventDispatcher*, const std::any&) {
                this->set_active_index(idx);
                return false;
            });
            
            update_layout_sizes();
        }

    protected:
        void on_theme_changed()
        {
            const auto& theme = Theme::get();
            const auto& c = theme.colors().surface_variant;
            m_bg_line_color->r = c.r; 
            m_bg_line_color->g = c.g; 
            m_bg_line_color->b = c.b; 
            m_bg_line_color->a = c.a;
        }

    private:
        void set_active_index(size_t index)
        {
            if (index == m_active_index) return;
            if (m_active_index < m_tabs.size()) {
                m_tabs[m_active_index]->active.set(false);
            }
            m_active_index = index;
            m_tabs[m_active_index]->active.set(true);
        }

        void update_layout_sizes()
        {
            if (m_tabs.empty()) return;
            // Distribute tabs equally across width if horizontal
            // Note: BoxLayout handles this naturally if size_hint_x=1.0, 
            // but we can enforce it.
            for (auto& tab : m_tabs) {
                tab->size_hint_x.set(1.0f);
            }
        }

        void update_geometry()
        {
            // The thin line at the bottom of the tab row
            m_bg_line->x = pos.get_value()[0];
            m_bg_line->y = pos.get_value()[1] + height.get_value() - 1.0f;
            m_bg_line->width = width.get_value();
            m_bg_line->height = 1.0f;
        }

        std::shared_ptr<SetColor> m_bg_line_color;
        std::shared_ptr<Rectangle> m_bg_line;

        std::vector<std::shared_ptr<MDTab>> m_tabs;
        size_t m_active_index{0};
    };

} // namespace md
} // namespace aura
