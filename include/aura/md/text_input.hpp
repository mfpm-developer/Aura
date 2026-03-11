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
#include "aura/properties/string_property.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/md/text.hpp"
#include "aura/input/touch_event.hpp"
#include "aura/core/window.hpp"
#include <memory>
#include <SDL3/SDL.h>

namespace aura {
namespace md {

    // Simple implementation of M3 Filled Text Field
    class MDTextInput : public MaterialWidget
    {
    public:
        MDTextInput()
        {
            m_bg_color = std::make_shared<SetColor>();
            m_bg = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 200.0f, 56.0f, 4.0f); // 4dp top corners

            // M3 active indicator (bottom line)
            m_indicator_color = std::make_shared<SetColor>();
            m_indicator = std::make_shared<Rectangle>(0.0f, 0.0f, 200.0f, 1.0f);

            // Text Rendering
            m_text_color = std::make_shared<SetColor>();
            m_text_render = std::make_shared<TextInstruction>("", 0, 0, "Roboto-Regular.ttf", 16);
            
            // Cursor
            m_cursor_color = std::make_shared<SetColor>();
            m_cursor = std::make_shared<Rectangle>(0.0f, 0.0f, 2.0f, 20.0f);

            // Floating Label
            m_label = std::make_shared<MDText>();
            m_label->set_text("Hint text");
            m_label->halign.set("left");
            m_label->valign.set("middle");
            this->add_widget(m_label);

            this->canvas->before->add(m_bg_color);
            this->canvas->before->add(m_bg);
            
            this->canvas->before->add(m_indicator_color);
            this->canvas->before->add(m_indicator);

            this->canvas->before->add(m_text_color);
            this->canvas->before->add(m_text_render);

            auto update_val = [this](EventDispatcher*, const std::any&) {
                this->update_geometry();
                return false;
            };

            text.bind("on_text", update_val);
            this->bind("on_pos", update_val);
            this->bind("on_size", update_val);
            
            focused.bind("on_focused", [this](EventDispatcher*, const std::any& val) {
                bool is_f = std::any_cast<bool>(val);
                if (is_f) {
                    SDL_StartTextInput(Window::get_instance().get_sdl_window());
                    this->canvas->after->add(this->m_cursor_color);
                    this->canvas->after->add(this->m_cursor);
                    this->bind_window_events();
                } else {
                    SDL_StopTextInput(Window::get_instance().get_sdl_window());
                    this->canvas->after->remove(this->m_cursor_color);
                    this->canvas->after->remove(this->m_cursor);
                }
                this->on_theme_changed(); // update colors based on focus
                this->update_geometry();  // animate label
                return false; 
            });

            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& ev) {
                auto touch = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                if (touch && collide_point(touch->x, touch->y)) {
                    focused.set(true);
                    return true;
                } else {
                    if (focused.get_value()) focused.set(false);
                }
                return false;
            });

            width.set(200.0f);
            height.set(56.0f); // M3 standard text field height

            on_theme_changed();
        }

        StringProperty text{"text", ""};
        StringProperty hint_text{"hint_text", "Hint text"};
        NumericProperty<bool> focused{"focused", false};

        void set_text(const std::string& t) { text.set(t); }

        bool set_property(const std::string& name, const std::string& value) override {
            if (name == "text") { set_text(value); return true; }
            if (name == "hint_text") { hint_text.set(value); return true; }
            if (name == "focused") { focused.set(value == "True"); return true; }
            return MaterialWidget::set_property(name, value);
        }

    protected:
        void on_theme_changed() override
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();

            const auto& bg = colors.surface_variant;
            m_bg_color->r = bg.r; m_bg_color->g = bg.g; m_bg_color->b = bg.b; m_bg_color->a = bg.a;

            const auto& txt = colors.on_surface;
            m_text_color->r = txt.r; m_text_color->g = txt.g; m_text_color->b = txt.b; m_text_color->a = txt.a;

            if (focused.get_value())
            {
                const auto& primary = colors.primary;
                m_indicator_color->r = primary.r; m_indicator_color->g = primary.g; m_indicator_color->b = primary.b; m_indicator_color->a = primary.a;
                m_cursor_color->r = primary.r; m_cursor_color->g = primary.g; m_cursor_color->b = primary.b; m_cursor_color->a = primary.a;
                
                m_label->set_text_color(primary); // Float label colored primary
            }
            else
            {
                const auto& outline = colors.on_surface_variant; // Unfocused line
                m_indicator_color->r = outline.r; m_indicator_color->g = outline.g; m_indicator_color->b = outline.b; m_indicator_color->a = outline.a;
                
                m_label->set_text_color(outline);
            }
        }

    private:
        void update_geometry()
        {
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];
            float w = width.get_value();
            float h = height.get_value();

            m_bg->x = x; m_bg->y = y;
            m_bg->width = w; m_bg->height = h;
            
            // Only round top corners (M3 spec) - hack using another rect to cover bottom if needed, 
            // but for now rounded rectangle draws all 4. 
            m_bg->radius = 4.0f; 

            // Active indicator line
            m_indicator->x = x;
            m_indicator->width = w;
            m_indicator->height = focused.get_value() ? 2.0f : 1.0f;
            m_indicator->y = y + h - m_indicator->height;

            // Float Label Animation Logic
            bool has_text = !text.get_value().empty();
            bool is_float = focused.get_value() || has_text;

            if (is_float) {
                // Label floats up and shrinks
                m_label->pos.set(std::vector<float>{x + 16.0f, y + 8.0f});
                m_label->width.set(w - 32.0f);
                m_label->height.set(16.0f);

                // If user actually typed something, we hide the "hint" text
                // by clearing its content or making it invisible.
                if (has_text) {
                    if (m_label->text.get_value() != "") m_label->set_text("");
                } else {
                    if (m_label->text.get_value() != hint_text.get_value()) m_label->set_text(hint_text.get_value());
                }
            } else {
                // Label centered vertically inside component
                m_label->pos.set(std::vector<float>{x + 16.0f, y});
                m_label->width.set(w - 32.0f);
                m_label->height.set(h);
                if (m_label->text.get_value() != hint_text.get_value()) m_label->set_text(hint_text.get_value());
            }

            if (m_text_render->get_text() != text.get_value()) {
                m_text_render->set_text(text.get_value());
            }

            m_text_render->x = x + 16.0f;
            m_text_render->y = y + 24.0f;

            if (focused.get_value()) {
                std::string current_text = text.get_value();
                std::string text_before_cursor = current_text.substr(0, m_cursor_index);
                float cursor_x_offset = 0;
                if (!text_before_cursor.empty()) {
                    TextInstruction measure(text_before_cursor, 0, 0, "Roboto-Regular.ttf", 16);
                    cursor_x_offset = measure.get_width();
                }

                m_cursor->x = m_text_render->x + cursor_x_offset;
                m_cursor->y = m_text_render->y;
                m_cursor->height = 20.0f; // Approx height for 16pt font
            }
        }

        void bind_window_events()
        {
            if (m_is_window_bound) return;
            Window::get_instance().bind("on_text_input", [this](EventDispatcher* d, const std::any& a){
                return this->handle_text_input(d, a);
            });
            Window::get_instance().bind("on_key_down", [this](EventDispatcher* d, const std::any& a){
                return this->handle_key_down(d, a);
            });
            m_is_window_bound = true;
        }

        bool handle_text_input(EventDispatcher*, const std::any& text_str)
        {
            if (!focused.get_value()) return false;
            try {
                std::string t = std::any_cast<std::string>(text_str);
                std::string current = text.get_value();
                current.insert(m_cursor_index, t);
                m_cursor_index += t.length();
                text.set(current);
                return true;
            } catch (...) {}
            return false;
        }

        bool handle_key_down(EventDispatcher*, const std::any& key_event)
        {
            if (!focused.get_value()) return false;
            try {
                SDL_KeyboardEvent key = std::any_cast<SDL_KeyboardEvent>(key_event);
                std::string current = text.get_value();

                if (key.key == SDLK_BACKSPACE) {
                    if (m_cursor_index > 0 && !current.empty()) {
                        current.erase(m_cursor_index - 1, 1);
                        m_cursor_index--;
                        text.set(current);
                    }
                    return true;
                } else if (key.key == SDLK_LEFT) {
                    if (m_cursor_index > 0) m_cursor_index--;
                    update_geometry();
                    return true;
                } else if (key.key == SDLK_RIGHT) {
                    if (m_cursor_index < current.length()) m_cursor_index++;
                    update_geometry();
                    return true;
                }
            } catch (...) {}
            return false;
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg;

        std::shared_ptr<SetColor> m_indicator_color;
        std::shared_ptr<Rectangle> m_indicator;

        std::shared_ptr<SetColor> m_text_color;
        std::shared_ptr<TextInstruction> m_text_render;

        std::shared_ptr<SetColor> m_cursor_color;
        std::shared_ptr<Rectangle> m_cursor;

        std::shared_ptr<MDText> m_label;
        
        int m_cursor_index{0};
        bool m_is_window_bound{false};
    };

} // namespace md
} // namespace aura
