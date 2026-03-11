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
#include "aura/md/list.hpp"
#include "aura/core/window.hpp"
#include <memory>
#include <vector>

namespace aura {
namespace md {

    class MDMenu : public MaterialWidget
    {
    public:
        MDMenu()
        {
            // Scrim (invisible background to catch clicks outside the menu)
            m_scrim_color = std::make_shared<SetColor>();
            m_scrim = std::make_shared<Rectangle>();
            
            // Menu surface container
            m_surface_color = std::make_shared<SetColor>();
            m_surface = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 112.0f, 100.0f, 8.0f); // 8dp standard menu radius

            this->canvas->before->add(m_scrim_color);
            this->canvas->before->add(m_scrim);
            
            this->canvas->before->add(m_surface_color);
            this->canvas->before->add(m_surface);

            m_list = std::make_shared<MDList>();
            m_list->padding.set(8.0f); // M3 menus have 8dp vertical padding
            this->add_widget(m_list);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            // Dismiss menu if scrim is clicked
            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& e) {
                if (m_is_open) {
                    auto touch = std::any_cast<std::shared_ptr<TouchEvent>>(e);
                    
                    float sx = m_surface->x;
                    float sy = m_surface->y;
                    float sw = m_surface->width;
                    float sh = m_surface->height;
                    
                    if (touch->x >= sx && touch->x <= sx + sw && touch->y >= sy && touch->y <= sy + sh) {
                        return false; // Clicked inside the menu list
                    } else {
                        // Clicked outside menu (on invisible scrim)
                        close();
                        return true; // Consume event so underlying widgets don't trigger
                    }
                }
                return false;
            });

            m_is_open = false;
            width.set(0.0f);
            height.set(0.0f);

            on_theme_changed();
        }

        void add_item(const std::string& text)
        {
            auto item = std::make_shared<MDListItem>();
            item->set_headline(text);
            item->height.set(48.0f); // Menus usually use denser items
            
            // Close menu and dispatch selected event when item is clicked
            item->bind("on_release", [this, text](EventDispatcher*, const std::any&) {
                this->dispatch("on_selected", text);
                this->close();
                return false;
            });

            m_list->add_widget(item);
        }

        void open(float anchor_x, float anchor_y)
        {
            if (m_is_open) return;
            m_is_open = true;

            m_anchor_x = anchor_x;
            m_anchor_y = anchor_y;

            auto& window = Window::get_instance();
            float ww = window.width.get_value();
            float wh = window.height.get_value();
            
            pos.set(std::vector<float>{0.0f, 0.0f});
            width.set(ww);
            height.set(wh);
            
            if (this->parent.lock() == nullptr) {
                window.add_widget(shared_from_this());
            }

            update_geometry();
        }

        void close()
        {
            if (!m_is_open) return;
            m_is_open = false;
            
            if (auto p = this->parent.lock()) {
                p->remove_widget(shared_from_this());
            }
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            m_scrim_color->a = 0.0f; // Invisible scrim to capture clicks!

            const auto& surface = colors.surface_variant; // Elevated M3 menu
            m_surface_color->r = surface.r; m_surface_color->g = surface.g; m_surface_color->b = surface.b; m_surface_color->a = surface.a;
        }

        void update_geometry()
        {
            if (!m_is_open) return;

            float w = width.get_value();
            float h = height.get_value();

            m_scrim->x = pos.get_value()[0]; m_scrim->y = pos.get_value()[1];
            m_scrim->width = w; m_scrim->height = h;

            float surf_w = 280.0f; // Standard width approx
            float surf_h = m_list->height.get_value();
            if (surf_h < 48.0f) surf_h = 48.0f;

            // Spawn at anchor point, but prevent overflowing screen
            float final_x = m_anchor_x;
            float final_y = m_anchor_y;

            if (final_x + surf_w > w) final_x = w - surf_w - 16.0f; // Shift left
            if (final_y + surf_h > h) final_y = m_anchor_y - surf_h; // Spawn upwards if clipping bottom
            
            m_surface->width = surf_w;
            m_surface->height = surf_h;
            m_surface->x = final_x;
            m_surface->y = final_y;

            m_list->pos.set(std::vector<float>{m_surface->x, m_surface->y});
            m_list->width.set(m_surface->width);
            m_list->height.set(m_surface->height);
        }

    private:
        bool m_is_open{false};
        float m_anchor_x{0.0f};
        float m_anchor_y{0.0f};

        std::shared_ptr<SetColor> m_scrim_color;
        std::shared_ptr<Rectangle> m_scrim;

        std::shared_ptr<SetColor> m_surface_color;
        std::shared_ptr<RoundedRectangle> m_surface;

        std::shared_ptr<MDList> m_list;
    };

} // namespace md
} // namespace aura
