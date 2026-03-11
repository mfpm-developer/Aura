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

    enum class ButtonVariant {
        Elevated,
        Filled,
        FilledTonal,
        Outlined,
        Text
    };

    class MDButton : public MaterialWidget
    {
    public:
        MDButton(ButtonVariant variant = ButtonVariant::Filled)
            : m_variant(variant)
        {
            m_bg_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 1.0f);
            m_bg_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 100.0f, 20.0f); // Fully rounded

            m_outline_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.0f);
            m_outline = std::make_shared<Line>();
            m_outline->width_val = 1.0f;

            m_state_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 0.0f);
            m_state_rect = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 100.0f, 20.0f);

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg_rect);
            
            this->canvas->before->add(m_outline_color);
            this->canvas->before->add(m_outline);
            
            this->canvas->after->add(m_state_color);
            this->canvas->after->add(m_state_rect);

            m_label = std::make_shared<MDText>();
            m_label->set_text("Button");
            
            this->add_widget(m_label);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            on_theme_changed();
        }

        StringProperty text{"text", "Button"};

        void set_text(const std::string& t) {
            text.set(t);
            if (m_label) {
                m_label->set_text(t);
                update_geometry();
            }
        }
        
        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "text") { set_text(value); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            aura::Color bg, fg, state_c, outline;

            switch (m_variant)
            {
                case ButtonVariant::Filled:
                    bg.r = colors.primary.r; bg.g = colors.primary.g; bg.b = colors.primary.b; bg.a = colors.primary.a;
                    fg.r = colors.on_primary.r; fg.g = colors.on_primary.g; fg.b = colors.on_primary.b; fg.a = colors.on_primary.a;
                    state_c.r = colors.on_primary.r; state_c.g = colors.on_primary.g; state_c.b = colors.on_primary.b; state_c.a = colors.on_primary.a;
                    outline.a = 0.0f;
                    break;
                case ButtonVariant::FilledTonal:
                    bg.r = colors.secondary_container.r; bg.g = colors.secondary_container.g; bg.b = colors.secondary_container.b; bg.a = colors.secondary_container.a;
                    fg.r = colors.on_secondary_container.r; fg.g = colors.on_secondary_container.g; fg.b = colors.on_secondary_container.b; fg.a = colors.on_secondary_container.a;
                    state_c.r = colors.on_secondary_container.r; state_c.g = colors.on_secondary_container.g; state_c.b = colors.on_secondary_container.b; state_c.a = colors.on_secondary_container.a;
                    outline.a = 0.0f;
                    break;
                case ButtonVariant::Outlined:
                    bg.r = colors.surface.r; bg.g = colors.surface.g; bg.b = colors.surface.b;
                    bg.a = 0.0f;
                    fg.r = colors.primary.r; fg.g = colors.primary.g; fg.b = colors.primary.b; fg.a = colors.primary.a;
                    state_c.r = colors.primary.r; state_c.g = colors.primary.g; state_c.b = colors.primary.b;
                    outline.r = colors.outline.r; outline.g = colors.outline.g; outline.b = colors.outline.b; outline.a = colors.outline.a;
                    break;
                case ButtonVariant::Text:
                    bg.r = colors.surface.r; bg.g = colors.surface.g; bg.b = colors.surface.b;
                    bg.a = 0.0f;
                    fg.r = colors.primary.r; fg.g = colors.primary.g; fg.b = colors.primary.b; fg.a = colors.primary.a;
                    state_c.r = colors.primary.r; state_c.g = colors.primary.g; state_c.b = colors.primary.b;
                    outline.a = 0.0f;
                    break;
                case ButtonVariant::Elevated:
                    bg.r = colors.surface_variant.r; bg.g = colors.surface_variant.g; bg.b = colors.surface_variant.b; bg.a = colors.surface_variant.a;
                    fg.r = colors.primary.r; fg.g = colors.primary.g; fg.b = colors.primary.b; fg.a = colors.primary.a;
                    state_c.r = colors.primary.r; state_c.g = colors.primary.g; state_c.b = colors.primary.b;
                    outline.a = 0.0f;
                    break;
            }

            m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;
            m_outline_color->r = outline.r; m_outline_color->g = outline.g; m_outline_color->b = outline.b; m_outline_color->a = outline.a;
            m_state_color->r = state_c.r; m_state_color->g = state_c.g; m_state_color->b = state_c.b;
            
            m_label->set_text_color(fg);
            
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
        void update_geometry() {
            std::cout << "MDButton update_geometry: " << std::any_cast<float>(x.get()) << ", " << std::any_cast<float>(y.get()) << " | " << std::any_cast<float>(width.get()) << "x" << std::any_cast<float>(height.get()) << std::endl;
            
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];
            float w = width.get_value();
            float h = height.get_value();

            m_bg_rect->x = x;
            m_bg_rect->y = y;
            m_bg_rect->width = w;
            m_bg_rect->height = h;

            m_state_rect->x = x;
            m_state_rect->y = y;
            m_state_rect->width = w;
            m_state_rect->height = h;

            if (m_outline_color->a > 0.0f) {
                m_outline->rectangle = {x, y, w, h};
            }

            m_label->pos.set(std::vector<float>{x, y});
            m_label->width.set(w);
            m_label->height.set(h);
        }

        ButtonVariant m_variant;
        std::shared_ptr<MDText> m_label;
        
        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg_rect;
        
        std::shared_ptr<SetColor> m_outline_color;
        std::shared_ptr<Line> m_outline;
        
        std::shared_ptr<SetColor> m_state_color;
        std::shared_ptr<RoundedRectangle> m_state_rect;
    };

} // namespace md
} // namespace aura
