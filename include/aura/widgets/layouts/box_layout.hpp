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

#include "aura/widgets/layouts/layout.hpp"
#include "aura/properties/string_property.hpp"
#include "aura/properties/numeric_property.hpp"

namespace aura
{
    /**
     * @class BoxLayout
     * @brief Arranges children in a vertical or horizontal box.
     */
    class BoxLayout : public Layout
    {
    public:
        BoxLayout(const std::string& orientation_val = "horizontal")
        {
            orientation.set(orientation_val);
            
            auto trigger_layout = [this](EventDispatcher*, const std::any&) {
                this->do_layout();
                return false; // let other bindings fire
            };

            orientation.bind("on_orientation", trigger_layout);
            spacing.bind("on_spacing", trigger_layout);
            padding.bind("on_padding", trigger_layout);

            x.bind("on_x", trigger_layout);
            y.bind("on_y", trigger_layout);
            width.bind("on_width", trigger_layout);
            height.bind("on_height", trigger_layout);
        }

        virtual ~BoxLayout() = default;

        StringProperty orientation{"orientation", "horizontal"};
        NumericProperty<float> spacing{"spacing", 0.0f};
        NumericProperty<float> padding{"padding", 0.0f};

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (Widget::set_property(name, value)) return true;
            try {
                if (name == "orientation") { orientation.set(value); return true; }
                if (name == "spacing") { spacing.set_value(std::stof(value)); return true; }
                if (name == "padding") { padding.set_value(std::stof(value)); return true; }
            } catch (...) { return false; }
            return false;
        }

        void do_layout() override;
        
        void add_widget(std::shared_ptr<Widget> widget) override
        {
            Widget::add_widget(widget);
            do_layout();
        }

        void remove_widget(std::shared_ptr<Widget> widget) override
        {
            Widget::remove_widget(widget);
            do_layout();
        }
    };
}
