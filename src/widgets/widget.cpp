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
#include "aura/widgets/widget.hpp"
#include "aura/input/touch_event.hpp"
#include <algorithm>
#include <sstream>

namespace aura
{
    Widget::Widget()
    {
        canvas = std::make_shared<Canvas>();

        // Bind position and size events to some layout update logic eventually
        x.bind("on_x", [](EventDispatcher*, const std::any&) { return false; });
        y.bind("on_y", [](EventDispatcher*, const std::any&) { return false; });
        width.bind("on_width", [](EventDispatcher*, const std::any&) { return false; });
        height.bind("on_height", [](EventDispatcher*, const std::any&) { return false; });

        // Wire center_x / center_y to update x / y
        center_x.bind("on_center_x", [this](EventDispatcher*, const std::any& v) {
            this->x.set_value(std::any_cast<float>(v) - this->width.get_value() / 2.0f);
            return false;
        });
        center_y.bind("on_center_y", [this](EventDispatcher*, const std::any& v) {
            this->y.set_value(std::any_cast<float>(v) - this->height.get_value() / 2.0f);
            return false;
        });

        // Wire x/y/width/height to update center_x / center_y (preventing infinite loops by checking values internally)
        auto update_center_x = [this](EventDispatcher*, const std::any&) {
            this->center_x.set_value(this->x.get_value() + this->width.get_value() / 2.0f);
            this->pos.set_value(std::vector<float>{this->x.get_value(), this->y.get_value()});
            this->pos.dispatch("on_pos", this->pos.get_value());
            this->size.dispatch("on_size", this->size.get_value());
            return false;
        };
        auto update_center_y = [this](EventDispatcher*, const std::any&) {
            this->center_y.set_value(this->y.get_value() + this->height.get_value() / 2.0f);
            this->pos.set_value(std::vector<float>{this->x.get_value(), this->y.get_value()});
            this->size.set_value(std::vector<float>{this->width.get_value(), this->height.get_value()});
            this->pos.dispatch("on_pos", this->pos.get_value());
            this->size.dispatch("on_size", this->size.get_value());
            return false;
        };
        
        x.bind("on_x", update_center_x);
        width.bind("on_width", update_center_x);
        
        y.bind("on_y", update_center_y);
        height.bind("on_height", update_center_y);

        // Forward property events out through the Widget itself so users
        // binding to `widget->bind("on_size")` actually receive the callbacks.
        x.bind("on_x", [this](auto, const auto& v) { this->dispatch("on_x", v); return false; });
        y.bind("on_y", [this](auto, const auto& v) { this->dispatch("on_y", v); return false; });
        width.bind("on_width", [this](auto, const auto& v) { this->dispatch("on_width", v); return false; });
        height.bind("on_height", [this](auto, const auto& v) { this->dispatch("on_height", v); return false; });
        pos.bind("on_pos", [this](auto, const auto& v) { this->dispatch("on_pos", v); return false; });
        size.bind("on_size", [this](auto, const auto& v) { this->dispatch("on_size", v); return false; });
    }

    void Widget::add_widget(std::shared_ptr<Widget> widget)
    {
        if (widget)
        {
            // First check if it already has a parent, and remove it from there
            if (auto p = widget->parent.lock()) {
                p->remove_widget(widget);
            }
            
            // Set this widget as the new parent using enable_shared_from_this
            // Note: This relies on the Widget already being owned by a shared_ptr
            try {
                widget->parent = shared_from_this();
            } catch (const std::bad_weak_ptr&) {
                // Ignore, widget is likely being added during parent's constructor
            }
            
            m_children.push_back(widget);
            canvas->add(widget->canvas);
        }
    }

    void Widget::remove_widget(std::shared_ptr<Widget> widget)
    {
        auto it = std::find(m_children.begin(), m_children.end(), widget);
        if (it != m_children.end())
        {
            canvas->remove(widget->canvas);
            widget->parent.reset();
            m_children.erase(it);
        }
    }

    void Widget::clear_widgets()
    {
        for (auto& child : m_children) {
            child->parent.reset();
        }
        m_children.clear();
        canvas->clear();
    }

    bool Widget::on_touch_down(std::shared_ptr<TouchEvent> touch)
    {
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
        {
            if ((*it)->on_touch_down(touch))
                return true;
        }
        if (dispatch("on_touch_down", touch)) return true;
        return false;
    }

    bool Widget::on_touch_move(std::shared_ptr<TouchEvent> touch)
    {
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
        {
            if ((*it)->on_touch_move(touch))
                return true;
        }
        if (dispatch("on_touch_move", touch)) return true;
        return false;
    }

    bool Widget::on_touch_up(std::shared_ptr<TouchEvent> touch)
    {
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
        {
            if ((*it)->on_touch_up(touch))
                return true;
        }
        if (dispatch("on_touch_up", touch)) return true;
        return false;
    }

    static std::vector<float> parse_float_list(const std::string& value)
    {
        std::vector<float> result;
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            result.push_back(std::stof(item));
        }
        return result;
    }

    bool Widget::set_property(const std::string& name, const std::string& value)
    {
        try {
            if (name == "x") { x.set_value(std::stof(value)); return true; }
            if (name == "y") { y.set_value(std::stof(value)); return true; }
            if (name == "width") { width.set_value(std::stof(value)); return true; }
            if (name == "height") { height.set_value(std::stof(value)); return true; }
            if (name == "center_x") { center_x.set_value(std::stof(value)); return true; }
            if (name == "center_y") { center_y.set_value(std::stof(value)); return true; }
            if (name == "pos") { pos.set_value(parse_float_list(value)); return true; }
            if (name == "size") { size.set_value(parse_float_list(value)); return true; }
            if (name == "center") { center.set_value(parse_float_list(value)); return true; }

            if (name == "size_hint_x") { size_hint_x.set_value(value == "None" ? -1.0f : std::stof(value)); return true; }
            if (name == "size_hint_y") { size_hint_y.set_value(value == "None" ? -1.0f : std::stof(value)); return true; }
            if (name == "pos_hint") { pos_hint.set(value); return true; }
        } catch (...) {
            return false;
        }
        return false;
    }

    std::pair<float, float> Widget::to_local(float window_x, float window_y) const
    {
        // Simple translation offset. Real implementation needs Canvas Affine stack integration.
        if (auto p = parent.lock()) {
            auto [px, py] = p->to_local(window_x, window_y);
            return {px - x.get_value(), py - y.get_value()};
        }
        return {window_x - x.get_value(), window_y - y.get_value()};
    }

    std::pair<float, float> Widget::to_parent(float local_x, float local_y) const
    {
        if (auto p = parent.lock()) {
            // Apply canvas transforms from local to parent
        }
        return {local_x + x.get_value(), local_y + y.get_value()};
    }

    std::pair<float, float> Widget::to_window(float local_x, float local_y) const
    {
        if (auto p = parent.lock()) {
            auto [px, py] = to_parent(local_x, local_y);
            return p->to_window(px, py);
        }
        return {local_x + x.get_value(), local_y + y.get_value()};
    }

    bool Widget::collide_point(float window_x, float window_y) const
    {
        float vx = x.get_value();
        float vy = y.get_value();
        float vw = width.get_value();
        float vh = height.get_value();

        return window_x >= vx && window_x <= vx + vw && window_y >= vy && window_y <= vy + vh;
    }

    std::any Widget::get_property(const std::string& name) const
    {
        if (name == "x") return x.get();
        if (name == "y") return y.get();
        if (name == "width") return width.get();
        if (name == "height") return height.get();
        if (name == "center_x") return center_x.get();
        if (name == "center_y") return center_y.get();
        return {};
    }
}
