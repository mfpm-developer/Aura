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
#include "aura/md/divider.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/properties/string_property.hpp"
#include <memory>
#include <vector>

namespace aura {
namespace md {

    // A single interactable item in an MDList
    class MDListItem : public MaterialWidget
    {
    public:
        MDListItem()
        {
            m_state_color = std::make_shared<SetColor>();
            m_state_layer = std::make_shared<Rectangle>();

            this->canvas->before->add(m_state_color);
            this->canvas->before->add(m_state_layer);

            m_headline = std::make_shared<MDText>();
            m_headline->set_text("List Item");
            this->add_widget(m_headline);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            size_hint_y.set(-1.0f);
            height.set(56.0f); // Default one-line height
            size_hint_x.set(1.0f);

            on_theme_changed();
        }

        StringProperty headline{"headline", "List Item"};
        StringProperty supporting_text{"supporting_text", ""};

        void set_headline(const std::string& text) {
            headline.set(text);
            m_headline->set_text(text);
        }

        void set_supporting_text(const std::string& text) {
            supporting_text.set(text);
            if (!m_supporting && !text.empty()) {
                m_supporting = std::make_shared<MDText>();
                this->add_widget(m_supporting);
                height.set(72.0f); // Switch to two-line height
                on_theme_changed();
                update_geometry();
            }
            if (m_supporting) {
                m_supporting->set_text(text);
            }
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "headline") { set_headline(value); return true; }
            if (name == "supporting_text") { set_supporting_text(value); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& on_surf = colors.on_surface;
            const auto& on_surf_var = colors.on_surface_variant;
            
            m_state_color->r = on_surf.r; 
            m_state_color->g = on_surf.g; 
            m_state_color->b = on_surf.b;

            if (m_headline) m_headline->set_text_color(on_surf);
            if (m_supporting) m_supporting->set_text_color(on_surf_var);

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

            if (m_supporting) {
                // Two-line layout
                m_headline->pos.set(std::vector<float>{x + 16.0f, y + 16.0f});
                m_headline->width.set(w - 32.0f);
                m_headline->height.set(24.0f);

                m_supporting->pos.set(std::vector<float>{x + 16.0f, y + 40.0f});
                m_supporting->width.set(w - 32.0f);
                m_supporting->height.set(20.0f);
            } else {
                // One-line layout, vertically centered
                m_headline->pos.set(std::vector<float>{x + 16.0f, y + (h - 24.0f) / 2.0f});
                m_headline->width.set(w - 32.0f);
                m_headline->height.set(24.0f);
            }
        }

        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<Rectangle> m_state_layer;
        
        std::shared_ptr<MDText> m_headline;
        std::shared_ptr<MDText> m_supporting;
    };


    // A vertical container specifically for MDListItems
    class MDList : public BoxLayout
    {
    public:
        void add_widget(std::shared_ptr<Widget> child) override
        {
            BoxLayout::add_widget(child);
            recalculate_height();
            child->bind("on_size", [this](EventDispatcher*, const std::any&) {
                this->recalculate_height();
                return false;
            });
        }

        MDList()
        {
            orientation.set("vertical");
            size_hint_y.set(-1.0f); // Often lists resize based on children or are inside ScrollViews
            
            // Background is technically surface but often just transparent
            m_bg_color = std::make_shared<SetColor>();
            m_bg_rect = std::make_shared<Rectangle>();

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg_rect);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });

            on_theme_changed();
        }

        void add_item(const std::string& headline, const std::string& supporting_text = "")
        {
            auto item = std::make_shared<MDListItem>();
            item->set_headline(headline);
            if (!supporting_text.empty()) item->set_supporting_text(supporting_text);
            
            this->add_widget(item);
            recalculate_height();
        }

        void add_divider()
        {
            auto div = std::make_shared<MDDivider>();
            this->add_widget(div);
            recalculate_height();
        }
        protected:
        void on_theme_changed()
        {
            const auto& theme = Theme::get();
            const auto& c = theme.colors().surface;
            
            m_bg_color->r = c.r; 
            m_bg_color->g = c.g; 
            m_bg_color->b = c.b; 
            m_bg_color->a = c.a;
        }

    private:
        void recalculate_height()
        {
            float total_h = padding.get_value() * 2.0f;
            int child_count = 0;
            for (const auto& child : m_children) {
                total_h += child->height.get_value();
                child_count++;
            }
            if (child_count > 1) {
                total_h += spacing.get_value() * (child_count - 1);
            }
            height.set(total_h);
        }

        void update_geometry()
        {
            m_bg_rect->x = pos.get_value()[0];
            m_bg_rect->y = pos.get_value()[1];
            m_bg_rect->width = width.get_value();
            m_bg_rect->height = height.get_value();
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<Rectangle> m_bg_rect;
    };

} // namespace md
} // namespace aura
