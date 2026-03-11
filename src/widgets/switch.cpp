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
#include "aura/widgets/switch.hpp"
#include "aura/input/touch_event.hpp"

namespace aura
{
    Switch::Switch()
    {
        m_bg_color = std::make_shared<SetColor>(0.3f, 0.3f, 0.3f, 1.0f);
        m_bg_rect = std::make_shared<RoundedRectangle>(x.get_value(), y.get_value(), width.get_value(), height.get_value(), 10.0f);
        
        m_thumb_color = std::make_shared<SetColor>(0.8f, 0.8f, 0.8f, 1.0f);
        m_thumb_rect = std::make_shared<Ellipse>(x.get_value(), y.get_value(), 20, 20);

        canvas->add(m_bg_color);
        canvas->add(m_bg_rect);
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
        active.bind("on_active", trigger_update);
        
        // typical switch size
        width.set(60.0f);
        height.set(30.0f);
        size_hint_x.set(-1.0f);
        size_hint_y.set(-1.0f);
    }

    void Switch::update_graphics()
    {
        float vx = x.get_value();
        float vy = y.get_value();
        float vw = width.get_value();
        float vh = height.get_value();

        // Kivy switches are usually capsule pills
        float track_height = vh * 0.6f;
        m_bg_rect->x = vx;
        m_bg_rect->y = vy + (vh - track_height) / 2.0f;
        m_bg_rect->width = vw;
        m_bg_rect->height = track_height;
        m_bg_rect->radius = track_height / 2.0f;

        float thumb_size = vh;
        m_thumb_rect->width = thumb_size;
        m_thumb_rect->height = thumb_size;
        m_thumb_rect->y = vy;

        if (active.get_value()) {
            m_bg_color->r = 0.2f; m_bg_color->g = 0.71f; m_bg_color->b = 0.898f; // Holo Blue when active
            m_thumb_color->r = 0.1f; m_thumb_color->g = 0.6f; m_thumb_color->b = 0.8f;
            m_thumb_rect->x = vx + vw - thumb_size;
        } else {
            m_bg_color->r = 0.2f; m_bg_color->g = 0.2f; m_bg_color->b = 0.2f; // Gray when inactive
            m_thumb_color->r = 0.6f; m_thumb_color->g = 0.6f; m_thumb_color->b = 0.6f;
            m_thumb_rect->x = vx;
        }
    }

    bool Switch::on_touch_down(std::shared_ptr<TouchEvent> touch)
    {
        if (collide_point(touch->x, touch->y)) {
            active.set(!active.get_value());
            return true;
        }
        return false;
    }

    bool Switch::set_property(const std::string& name, const std::string& value)
    {
        if (Widget::set_property(name, value)) return true;
        try {
            if (name == "active") { active.set(value == "True" || value == "1"); return true; }
        } catch (...) { return false; }
        return false;
    }
}
