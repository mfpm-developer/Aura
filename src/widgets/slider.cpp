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
#include "aura/widgets/slider.hpp"
#include "aura/input/touch_event.hpp"
#include <algorithm>

namespace aura
{
    Slider::Slider()
    {
        m_track_color = std::make_shared<SetColor>(0.4f, 0.4f, 0.4f, 1.0f);
        m_track_line = std::make_shared<Line>();
        m_track_line->width_val = 4.0f;
        
        m_thumb_color = std::make_shared<SetColor>(0.8f, 0.8f, 0.8f, 1.0f);
        m_thumb_rect = std::make_shared<Ellipse>(x.get_value(), y.get_value(), 20, 20);

        canvas->add(m_track_color);
        canvas->add(m_track_line);
        canvas->add(m_thumb_color);
        canvas->add(m_thumb_rect);

        auto trigger_update = [this](EventDispatcher*, const std::any&) {
            this->update_graphics();
            return false;
        };

        x.bind("on_x", trigger_update);
        y.bind("on_y", trigger_update);
        width.bind("on_width", trigger_update);
        height.bind("on_height", trigger_update);
        min.bind("on_min", trigger_update);
        max.bind("on_max", trigger_update);
        value.bind("on_value", trigger_update);
        orientation.bind("on_orientation", trigger_update);
    }

    void Slider::update_graphics()
    {
        float vx = x.get_value();
        float vy = y.get_value();
        float vw = width.get_value();
        float vh = height.get_value();
        
        bool is_horizontal = orientation.get_value() == "horizontal";

        m_track_line->points.clear();
        if (is_horizontal) {
            m_track_line->points = {
                vx, vy + vh / 2.0f,
                vx + vw, vy + vh / 2.0f
            };
        } else {
            m_track_line->points = {
                vx + vw / 2.0f, vy,
                vx + vw / 2.0f, vy + vh
            };
        }

        // Calculate thumb position based on value mapped between min and max
        float v_min = min.get_value();
        float v_max = max.get_value();
        float v_val = std::clamp(value.get_value(), v_min, v_max);
        
        float ratio = (v_max > v_min) ? (v_val - v_min) / (v_max - v_min) : 0.0f;

        float thumb_size = 20.0f;
        m_thumb_rect->width = thumb_size;
        m_thumb_rect->height = thumb_size;

        if (is_horizontal) {
            m_thumb_rect->x = vx + ratio * vw - thumb_size / 2.0f;
            m_thumb_rect->y = vy + vh / 2.0f - thumb_size / 2.0f;
        } else {
            m_thumb_rect->x = vx + vw / 2.0f - thumb_size / 2.0f;
            m_thumb_rect->y = vy + ratio * vh - thumb_size / 2.0f;
        }
    }

    void Slider::set_value_from_pos(float tx, float ty)
    {
        float vx = x.get_value();
        float vy = y.get_value();
        float vw = width.get_value();
        float vh = height.get_value();

        bool is_horizontal = orientation.get_value() == "horizontal";
        float ratio = 0.0f;

        if (is_horizontal) {
            ratio = (tx - vx) / vw;
        } else {
            ratio = (ty - vy) / vh;
        }

        ratio = std::clamp(ratio, 0.0f, 1.0f);

        float v_min = min.get_value();
        float v_max = max.get_value();
        float new_val = v_min + ratio * (v_max - v_min);

        value.set(new_val);
    }

    bool Slider::on_touch_down(std::shared_ptr<TouchEvent> touch)
    {
        if (collide_point(touch->x, touch->y)) {
            touch->grab(this);
            set_value_from_pos(touch->x, touch->y);
            return true;
        }
        return false;
    }

    bool Slider::on_touch_move(std::shared_ptr<TouchEvent> touch)
    {
        if (touch->grab_current == this) {
            set_value_from_pos(touch->x, touch->y);
            return true;
        }
        return false;
    }

    bool Slider::on_touch_up(std::shared_ptr<TouchEvent> touch)
    {
        if (touch->grab_current == this) {
            touch->ungrab(this);
            return true;
        }
        return false;
    }

    bool Slider::set_property(const std::string& name, const std::string& value_str)
    {
        if (Widget::set_property(name, value_str)) return true;
        try {
            if (name == "min") { min.set_value(std::stof(value_str)); return true; }
            if (name == "max") { max.set_value(std::stof(value_str)); return true; }
            if (name == "value") { value.set_value(std::stof(value_str)); return true; }
            if (name == "orientation") { orientation.set(value_str); return true; }
        } catch (...) { return false; }
        return false;
    }
}
