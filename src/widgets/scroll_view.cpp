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
#include "aura/widgets/scroll_view.hpp"
#include "aura/input/touch_event.hpp"
#include "aura/core/window.hpp"
#include <algorithm>
#include <iostream>

namespace aura
{
    void ScissorInstruction::apply()
    {
        SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
        if (renderer)
        {
            auto t = Window::get_instance().get_transform();
            SDL_Rect rect;
            rect.x = static_cast<int>(m_x);
            rect.y = static_cast<int>(m_y);
            rect.w = static_cast<int>(m_w * t.sx);
            rect.h = static_cast<int>(m_h * t.sy);
            SDL_SetRenderClipRect(renderer, &rect);
        }
    }

    void ScissorDisableInstruction::apply()
    {
        SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
        if (renderer)
        {
            SDL_SetRenderClipRect(renderer, nullptr);
        }
    }

    ScrollView::ScrollView()
    {
        m_clip_start = std::make_shared<ScissorInstruction>(x.get_value(), y.get_value(), width.get_value(), height.get_value());
        m_clip_end = std::make_shared<ScissorDisableInstruction>();

        canvas->before->add(m_clip_start);
        canvas->after->add(m_clip_end);

        auto trigger_layout = [this](EventDispatcher*, const std::any&) {
            this->update_clip_rect();
            this->update_child_pos();
            return false;
        };

        x.bind("on_x", trigger_layout);
        y.bind("on_y", trigger_layout);
        width.bind("on_width", trigger_layout);
        height.bind("on_height", trigger_layout);
        scroll_x.bind("on_scroll_x", trigger_layout);
        scroll_y.bind("on_scroll_y", trigger_layout);
    }

    void ScrollView::add_widget(std::shared_ptr<Widget> widget)
    {
        if (m_children.size() >= 1) {
            std::cerr << "ScrollView only accepts one child! Subsequent children ignored." << std::endl;
            return;
        }
        
        Layout::add_widget(widget);
        update_child_pos();
    }

    void ScrollView::do_layout()
    {
        update_child_pos();
    }

    void ScrollView::update_clip_rect()
    {
        m_clip_start->set_rect(x.get_value(), y.get_value(), width.get_value(), height.get_value());
    }

    void ScrollView::update_child_pos()
    {
        if (m_children.empty()) return;
        auto child = m_children.front();

        float vx = x.get_value();
        float vy = y.get_value();
        float vw = width.get_value();
        float vh = height.get_value();

        if (child->size_hint_x.get_value() >= 0.0f) {
            child->width.set(vw * child->size_hint_x.get_value());
        }
        if (child->size_hint_y.get_value() >= 0.0f) {
            child->height.set(vh * child->size_hint_y.get_value());
        }

        float cw = child->width.get_value();
        float ch = child->height.get_value();

        float diff_w = std::max(0.0f, cw - vw);
        float diff_h = std::max(0.0f, ch - vh);

        float sx = scroll_x.get_value();
        float sy = scroll_y.get_value();

        // SDL top-down mode: sy=1.0 puts child top edge at viewport top
        float target_x = vx - (sx * diff_w);
        float target_y = vy - ((1.0f - sy) * diff_h);

        child->x.set(target_x);
        child->y.set(target_y);
    }

    bool ScrollView::on_touch_down(std::shared_ptr<TouchEvent> touch)
    {
        if (collide_point(touch->x, touch->y)) {
            // Give children a chance to intercept first (like a button)
            Layout::on_touch_down(touch);

            // We grab for potential scrolling
            touch->grab(this);
            m_last_touch_x = touch->x;
            m_last_touch_y = touch->y;
            m_touch_down_x = touch->x;
            m_touch_down_y = touch->y;
            m_is_scrolling = false;
            return true;
        }
        return false;
    }

    bool ScrollView::on_touch_move(std::shared_ptr<TouchEvent> touch)
    {
        // If another widget is actively dragging (grab_current is not us), we should not steal.
        // We only process scroll physics if we are the primary grabber OR if nobody has exclusively claimed it yet.
        if (touch->grab_current == this) {
            float dx = touch->x - m_last_touch_x;
            float dy = touch->y - m_last_touch_y;
            m_last_touch_x = touch->x;
            m_last_touch_y = touch->y;

            if (!m_is_scrolling) {
                float total_dx = std::abs(touch->x - m_touch_down_x);
                float total_dy = std::abs(touch->y - m_touch_down_y);
                if (total_dx > 10.0f || total_dy > 10.0f) {
                    
                    // NEW CHECK: Are we fighting with an active child control (like a Slider?)
                    // If a child specifically claimed grab_current BEFORE returning to the window event loop,
                    // we should respect their drag and relinquish our own background grab.
                    bool child_is_dragging = false;
                    for (auto* g : touch->get_grabbers()) {
                        // If another grabber exists that isn't us, assume they want the drag.
                        if (g != this) {
                            child_is_dragging = true;
                            break;
                        }
                    }

                    if (child_is_dragging) {
                        // Surrender the scroll attempt, let the child have it natively.
                        touch->ungrab(this);
                        return false; 
                    }

                    m_is_scrolling = true;
                    // Steal touch from passive children (e.g. Buttons being hovered)
                    auto grabbers = touch->get_grabbers();
                    for (auto* g : grabbers) {
                        if (g != this) {
                            touch->ungrab(g);
                            float old_x = touch->x;
                            float old_y = touch->y;
                            touch->x = -9999.0f; // Force un-hover
                            touch->y = -9999.0f;
                            touch->grab_current = g;
                            g->on_touch_up(touch);
                            touch->x = old_x;
                            touch->y = old_y;
                        }
                    }
                    touch->grab_current = this; // Restore our grab context
                }
            }

            if (m_is_scrolling) {
                if (m_children.empty()) return true;
                auto child = m_children.front();

                float diff_w = std::max(0.0f, child->width.get_value() - width.get_value());
                float diff_h = std::max(0.0f, child->height.get_value() - height.get_value());

                if (diff_w > 0) {
                    float new_sx = scroll_x.get_value() - (dx / diff_w);
                    scroll_x.set_value(std::clamp(new_sx, 0.0f, 1.0f));
                    scroll_x.dispatch("on_scroll_x", scroll_x.get_value()); // force update visually
                }

                if (diff_h > 0) {
                    float new_sy = scroll_y.get_value() - (dy / diff_h);
                    scroll_y.set_value(std::clamp(new_sy, 0.0f, 1.0f));
                    scroll_y.dispatch("on_scroll_y", scroll_y.get_value());
                }
            }
            return true;
        }
        return Layout::on_touch_move(touch);
    }

    bool ScrollView::on_touch_up(std::shared_ptr<TouchEvent> touch)
    {
        if (touch->grab_current == this) {
            touch->ungrab(this);
            m_is_scrolling = false;
            return true;
        }
        return Layout::on_touch_up(touch);
    }

    bool ScrollView::set_property(const std::string& name, const std::string& value)
    {
        if (Layout::set_property(name, value)) return true;
        try {
            if (name == "scroll_x") { scroll_x.set_value(std::stof(value)); return true; }
            if (name == "scroll_y") { scroll_y.set_value(std::stof(value)); return true; }
        } catch (...) { return false; }
        return false;
    }
}
