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
#include "aura/core/window.hpp"
#include <memory>

namespace aura {
namespace md {

    class MDDialog : public MaterialWidget
    {
    public:
        MDDialog()
        {
            // Scrim (dark transparent background)
            m_scrim_color = std::make_shared<SetColor>();
            m_scrim = std::make_shared<Rectangle>();

            // Dialog surface container
            m_surface_color = std::make_shared<SetColor>();
            m_surface = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 312.0f, 200.0f, 28.0f); // Standard M3 Dialog

            this->canvas->before->add(m_scrim_color);
            this->canvas->before->add(m_scrim);
            
            this->canvas->before->add(m_surface_color);
            this->canvas->before->add(m_surface);

            m_content_layout = std::make_shared<BoxLayout>();
            m_content_layout->orientation.set("vertical");
            m_content_layout->padding.set(24.0f);
            m_content_layout->spacing.set(16.0f);
            this->add_widget(m_content_layout);

            m_headline = std::make_shared<MDText>();
            m_headline->set_text("Dialog Title");
            m_headline->size_hint_y.set(-1.0f);
            m_headline->height.set(32.0f);
            m_content_layout->add_widget(m_headline);

            // Container for user content
            m_user_content = std::make_shared<BoxLayout>();
            m_user_content->orientation.set("vertical");
            m_content_layout->add_widget(m_user_content);

            // Bottom action row
            m_action_row = std::make_shared<BoxLayout>();
            m_action_row->orientation.set("horizontal");
            m_action_row->size_hint_y.set(-1.0f);
            m_action_row->height.set(40.0f);
            m_action_row->spacing.set(8.0f);
            
            // Push actions to right
            auto spacer = std::make_shared<Widget>();
            m_action_row->add_widget(spacer);

            m_content_layout->add_widget(m_action_row);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            // Dismiss dialog if scrim is clicked
            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& e) {
                if (m_is_open) {
                    auto touch = std::any_cast<std::shared_ptr<TouchEvent>>(e);
                    // Check if click is inside the actual surface
                    float sx = m_surface->x;
                    float sy = m_surface->y;
                    float sw = m_surface->width;
                    float sh = m_surface->height;
                    
                    if (touch->x >= sx && touch->x <= sx + sw && touch->y >= sy && touch->y <= sy + sh) {
                        return false; // Let it pass to children
                    } else {
                        // Clicked on scrim
                        close();
                        return true; // Consume event
                    }
                }
                return false;
            });

            // Dialogs start hidden
            width.set(0.0f);
            height.set(0.0f);
            m_is_open = false;
            
            on_theme_changed();
        }

        void set_title(const std::string& title)
        {
            m_headline->set_text(title);
        }

        void add_content(std::shared_ptr<Widget> widget)
        {
            m_user_content->add_widget(widget);
        }

        void add_action(std::shared_ptr<Widget> button)
        {
            m_action_row->add_widget(button);
        }

        void open()
        {
            if (m_is_open) return;
            m_is_open = true;

            // Bind to Window size to cover the entire screen
            auto& window = Window::get_instance();
            float ww = window.width.get_value();
            float wh = window.height.get_value();
            
            pos.set(std::vector<float>{0.0f, 0.0f});
            width.set(ww);
            height.set(wh);
            
            // To act as an overlay, attach to root if not already
            if (this->parent.lock() == nullptr) {
                window.add_widget(shared_from_this());
            }

            update_geometry();
        }

        void close()
        {
            if (!m_is_open) return;
            m_is_open = false;
            
            // Optional: Remove from parent
            if (auto p = this->parent.lock()) {
                p->remove_widget(shared_from_this());
            }
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& scrim = colors.scrim;
            m_scrim_color->r = scrim.r; m_scrim_color->g = scrim.g; m_scrim_color->b = scrim.b; m_scrim_color->a = 0.32f; // Standard M3 Scrim opacity

            const auto& surface = colors.surface; // Dialogs are usually elevated
            m_surface_color->r = surface.r; m_surface_color->g = surface.g; m_surface_color->b = surface.b; m_surface_color->a = surface.a;

            const auto& on_surf = colors.on_surface;
            m_headline->set_text_color(on_surf);
        }

        void update_geometry()
        {
            if (!m_is_open) return;

            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_scrim->x = x; m_scrim->y = y;
            m_scrim->width = w; m_scrim->height = h;

            // Compute dialog surface size (min width 280, max 560)
            float surf_w = std::min(w - 48.0f, 312.0f); // Give 24dp padding on edges minimum
            // Approximate height from content
            float surf_h = m_content_layout->height.get_value() + 48.0f; // Padding
            
            if (surf_h < 150.0f) surf_h = 150.0f;

            m_surface->width = surf_w;
            m_surface->height = surf_h;
            m_surface->x = x + (w - surf_w) / 2.0f;
            m_surface->y = y + (h - surf_h) / 2.0f;

            m_content_layout->pos.set(std::vector<float>{m_surface->x, m_surface->y});
            m_content_layout->width.set(m_surface->width);
            m_content_layout->height.set(m_surface->height);
        }

    private:
        bool m_is_open{false};

        std::shared_ptr<SetColor> m_scrim_color;
        std::shared_ptr<Rectangle> m_scrim;

        std::shared_ptr<SetColor> m_surface_color;
        std::shared_ptr<RoundedRectangle> m_surface;

        std::shared_ptr<BoxLayout> m_content_layout;
        std::shared_ptr<MDText> m_headline;
        std::shared_ptr<BoxLayout> m_user_content;
        std::shared_ptr<BoxLayout> m_action_row;
    };

} // namespace md
} // namespace aura
