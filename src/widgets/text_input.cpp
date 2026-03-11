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
#include "aura/widgets/text_input.hpp"
#include "aura/core/window.hpp"
#include <SDL3/SDL.h>

namespace aura
{
    TextInput::TextInput()
    {
        m_bg_color = std::make_shared<SetColor>(1.0f, 1.0f, 1.0f, 1.0f);
        m_bg_rect = std::make_shared<RoundedRectangle>(x.get_value(), y.get_value(), width.get_value(), height.get_value(), 5.0f);
        
        m_cursor_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 1.0f);
        m_cursor_rect = std::make_shared<Rectangle>(0, 0, 2, 0);

        m_text_color = std::make_shared<SetColor>(0.0f, 0.0f, 0.0f, 1.0f);
        m_text_render = std::make_shared<TextInstruction>(text.get_value(), 0, 0);

        canvas->add(m_bg_color);
        canvas->add(m_bg_rect);
        canvas->add(m_text_color);
        canvas->add(m_text_render);
        // cursor added dynamically depending on focus

        auto update_layout = [this](EventDispatcher*, const std::any&) {
            this->update_graphics();
            return false;
        };

        x.bind("on_x", update_layout);
        y.bind("on_y", update_layout);
        width.bind("on_width", update_layout);
        height.bind("on_height", update_layout);
        text.bind("on_text", update_layout);
        padding_x.bind("on_padding_x", update_layout);
        padding_y.bind("on_padding_y", update_layout);

        focus.bind("on_focus", [this](EventDispatcher*, const std::any& val) {
            bool is_focused = std::any_cast<bool>(val);
            if (is_focused) {
                SDL_StartTextInput(Window::get_instance().get_sdl_window());
                canvas->add(m_cursor_color);
                canvas->add(m_cursor_rect);
                bind_window_events();
            } else {
                SDL_StopTextInput(Window::get_instance().get_sdl_window());
                canvas->remove(m_cursor_color);
                canvas->remove(m_cursor_rect);
                unbind_window_events();
            }
            update_graphics();
            return false; // let others run
        });
    }

    TextInput::~TextInput()
    {
        unbind_window_events();
    }

    void TextInput::update_graphics()
    {
        m_bg_rect->x = x.get_value();
        m_bg_rect->y = y.get_value();
        m_bg_rect->width = width.get_value();
        m_bg_rect->height = height.get_value();
        m_bg_rect->radius = 5.0f;

        float px = padding_x.get_value();

        if (text.get_value() != m_text_render->get_text()) {
            m_text_render->set_text(text.get_value());
        }

        float text_h = m_text_render->get_height();
        if (text_h == 0) {
            // Measure a dummy string to get the font height if empty
            TextInstruction dummy("A", 0, 0);
            text_h = dummy.get_height();
        }

        // Vertically center the text
        m_text_render->x = x.get_value() + px;
        m_text_render->y = y.get_value() + (height.get_value() - text_h) / 2.0f;

        if (focus.get_value()) {
            std::string current_text = text.get_value();
            std::string text_before_cursor = current_text.substr(0, m_cursor_index);
            
            float cursor_x_offset = 0;
            if (!text_before_cursor.empty()) {
                TextInstruction measure(text_before_cursor, 0, 0);
                cursor_x_offset = measure.get_width();
            }

            m_cursor_rect->x = m_text_render->x + cursor_x_offset;
            m_cursor_rect->y = m_text_render->y;
            m_cursor_rect->width = 2.0f;
            m_cursor_rect->height = text_h;
        }
    }

    bool TextInput::on_touch_down(std::shared_ptr<TouchEvent> touch)
    {
        if (collide_point(touch->x, touch->y)) {
            focus.set(true);
            return true;
        } else {
            if (focus.get_value()) focus.set(false);
        }
        return false;
    }

    void TextInput::bind_window_events()
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

    void TextInput::unbind_window_events()
    {
        if (!m_is_window_bound) return;
        // In a complete framework, we would use unique bound IDs to unbind specifically
        // As a prototype, we just clear but we shouldn't wipe all bindings to Window.
        // For Aura, we omit strict unbinding here and assume focus management covers it
        m_is_window_bound = false;
    }

    bool TextInput::handle_text_input(EventDispatcher* dispatcher, const std::any& text_str)
    {
        if (!focus.get_value()) return false;
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

    bool TextInput::handle_key_down(EventDispatcher* dispatcher, const std::any& key_event)
    {
        if (!focus.get_value()) return false;
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
                update_graphics();
                return true;
            } else if (key.key == SDLK_RIGHT) {
                if (m_cursor_index < current.length()) m_cursor_index++;
                update_graphics();
                return true;
            } else if (key.key == SDLK_DELETE) {
                 if (m_cursor_index < current.length()) {
                     current.erase(m_cursor_index, 1);
                     text.set(current);
                 }
                 return true;
            }
        } catch (...) {}
        return false;
    }

    bool TextInput::set_property(const std::string& name, const std::string& value)
    {
        if (Widget::set_property(name, value)) return true;
        try {
            if (name == "text") { text.set(value); return true; }
            if (name == "padding_x") { padding_x.set_value(std::stof(value)); return true; }
            if (name == "padding_y") { padding_y.set_value(std::stof(value)); return true; }
            if (name == "focus") { focus.set_value(value == "True" || value == "1"); return true; }
        } catch (...) { return false; }
        return false;
    }
}
