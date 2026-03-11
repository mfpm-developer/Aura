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

#include "aura/widgets/widget.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"

namespace aura
{
    /**
     * @class ProgressBar
     * @brief A read-only bar indicating progress.
     */
    class ProgressBar : public Widget
    {
    public:
        ProgressBar()
        {
            m_bg_color = std::make_shared<SetColor>(0.2f, 0.2f, 0.2f, 1.0f);
            m_bg_rect = std::make_shared<Rectangle>(x.get_value(), y.get_value(), width.get_value(), height.get_value());
            
            m_fill_color = std::make_shared<SetColor>(0.2f, 0.6f, 1.0f, 1.0f);
            m_fill_rect = std::make_shared<Rectangle>(x.get_value(), y.get_value(), 0, height.get_value());

            canvas->add(m_bg_color);
            canvas->add(m_bg_rect);
            canvas->add(m_fill_color);
            canvas->add(m_fill_rect);

            auto trigger_update = [this](EventDispatcher*, const std::any&) {
                this->update_graphics();
                return false;
            };

            x.bind("on_x", trigger_update);
            y.bind("on_y", trigger_update);
            width.bind("on_width", trigger_update);
            height.bind("on_height", trigger_update);
            value.bind("on_value", trigger_update);
            max.bind("on_max", trigger_update);
            
            height.set(10.0f);
            size_hint_y.set(-1.0f);
        }

        virtual ~ProgressBar() = default;

        NumericProperty<float> value{"value", 0.0f};
        NumericProperty<float> max{"max", 100.0f};

        bool set_property(const std::string& name, const std::string& val_str) override
        {
            if (Widget::set_property(name, val_str)) return true;
            try {
                if (name == "value") { value.set_value(std::stof(val_str)); return true; }
                if (name == "max") { max.set_value(std::stof(val_str)); return true; }
            } catch (...) { return false; }
            return false;
        }

    private:
        void update_graphics()
        {
            float vx = x.get_value();
            float vy = y.get_value();
            float vw = width.get_value();
            float vh = height.get_value();

            m_bg_rect->x = vx;
            m_bg_rect->y = vy;
            m_bg_rect->width = vw;
            m_bg_rect->height = vh;

            float m = max.get_value();
            float v = value.get_value();
            if (m <= 0) m = 1;
            if (v < 0) v = 0;
            if (v > m) v = m;

            float ratio = v / m;

            m_fill_rect->x = vx;
            m_fill_rect->y = vy;
            m_fill_rect->width = vw * ratio;
            m_fill_rect->height = vh;
        }

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<Rectangle> m_bg_rect;
        
        std::shared_ptr<SetColor> m_fill_color;
        std::shared_ptr<Rectangle> m_fill_rect;
    };
}
