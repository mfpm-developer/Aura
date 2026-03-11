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
#include <memory>
#include <vector>

namespace aura {
namespace md {

    // Simple implementation of a single Navigation Bar Item
    class MDNavigationItem : public MaterialWidget
    {
    public:
        MDNavigationItem()
        {
            // Indicator pill (M3 Active state background)
            m_pill_color = std::make_shared<SetColor>();
            m_pill = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 64.0f, 32.0f, 16.0f); // Standard M3 64x32 pill

            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Rectangle>();

            this->canvas->before->add(m_pill_color);
            this->canvas->before->add(m_pill);

            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_layer);
            
            m_label = std::make_shared<MDText>();
            m_label->set_text("Tab");
            this->add_widget(m_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            active.bind("on_active", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });

            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                dispatch("on_selected", std::any());
                return false;
            });

            on_theme_changed();
        }

        NumericProperty<bool> active{"active", false};
        StringProperty text{"text", "Tab"};

        void set_text(const std::string& t) {
            text.set(t);
            m_label->set_text(t);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            if (active.get_value())
            {
                const auto& bg = colors.secondary_container;
                m_pill_color->r = bg.r; m_pill_color->g = bg.g; m_pill_color->b = bg.b; m_pill_color->a = bg.a;
                
                const auto& on_bg = colors.on_secondary_container;
                m_label->set_text_color(on_bg);

                const auto& state_c = on_bg;
                m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            }
            else
            {
                m_pill_color->a = 0.0f; // Transparent when inactive

                const auto& on_surf = colors.on_surface_variant;
                m_label->set_text_color(on_surf);

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

            m_pill->x = cx - 32.0f;
            m_pill->y = pos.get_value()[1] + 12.0f; // Pill sits near top
            
            m_state_layer->x = pos.get_value()[0];
            m_state_layer->y = pos.get_value()[1];
            m_state_layer->width = width.get_value();
            m_state_layer->height = height.get_value();

            m_label->pos.set(std::vector<float>{pos.get_value()[0], pos.get_value()[1] + 44.0f});
            m_label->width.set(width.get_value());
            m_label->height.set(20.0f);
        }

        std::shared_ptr<SetColor> m_pill_color;
        std::shared_ptr<RoundedRectangle> m_pill;

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Rectangle> m_state_layer;

        std::shared_ptr<MDText> m_label;
    };

    // M3 Bottom Navigation Bar
    class MDNavigationBar : public MaterialWidget
    {
    public:
        MDNavigationBar()
        {
            m_bg_color = std::make_shared<SetColor>();
            m_bg = std::make_shared<Rectangle>(0.0f, 0.0f, 800.0f, 80.0f); // Default height 80dp M3

            // Optional M3 Elevation (Level 2 default for NavBar)
            m_shadow_color = std::make_shared<SetColor>();
            m_shadow = std::make_shared<Rectangle>(); 

            this->canvas->before->add(m_shadow_color);
            this->canvas->before->add(m_shadow);
            
            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_layout(); return false; });

            height.set(80.0f);
            size_hint_y.set(-1.0f);

            on_theme_changed();
        }

        void add_item(const std::string& text)
        {
            auto item = std::make_shared<MDNavigationItem>();
            item->set_text(text);
            
            size_t idx = m_items.size();
            m_items.push_back(item);
            this->add_widget(item);

            if (m_items.size() == 1) {
                item->active.set(true); // First item active by default
                m_active_index = 0;
            }

            item->bind("on_selected", [this, idx](EventDispatcher*, const std::any&) {
                this->set_active_index(idx);
                return false;
            });

            update_layout();
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& bg = colors.surface_variant; // M3 default is often surface container, we map to surface_variant for now
            m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;

            const auto& shadow = colors.shadow;
            m_shadow_color->r = shadow.r; m_shadow_color->g = shadow.g; m_shadow_color->b = shadow.b; m_shadow_color->a = 0.08f;
        }

    private:
        void set_active_index(size_t index)
        {
            if (index == m_active_index) return;
            
            if (m_active_index < m_items.size()) {
                m_items[m_active_index]->active.set(false);
            }
            m_active_index = index;
            m_items[m_active_index]->active.set(true);
        }

        void update_layout()
        {
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_bg->x = x; m_bg->y = y;
            m_bg->width = w; m_bg->height = h;

            // Upward shadow
            m_shadow->x = x; m_shadow->y = y - 4.0f;
            m_shadow->width = w; m_shadow->height = 4.0f;

            if (m_items.empty()) return;

            float item_width = w / m_items.size();
            for (size_t i = 0; i < m_items.size(); ++i)
            {
                m_items[i]->pos.set(std::vector<float>{x + (i * item_width), y});
                m_items[i]->width.set(item_width);
                m_items[i]->height.set(h);
            }
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<Rectangle> m_bg;

        std::shared_ptr<SetColor> m_shadow_color;
        std::shared_ptr<Rectangle> m_shadow;

        std::vector<std::shared_ptr<MDNavigationItem>> m_items;
        size_t m_active_index{0};
    };

} // namespace md
} // namespace aura
