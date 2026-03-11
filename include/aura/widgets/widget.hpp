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

#include "aura/core/event_dispatcher.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/properties/string_property.hpp"
#include "aura/properties/reference_list_property.hpp"
#include "aura/graphics/canvas.hpp"
#include <vector>
#include <memory>

namespace aura
{
    /**
     * @class Widget
     * @brief The base class for all UI elements.
     */
    class Widget : public EventDispatcher, public std::enable_shared_from_this<Widget>
    {
    public:
        Widget();
        virtual ~Widget() = default;

        /**
         * @brief Add a child widget to this widget.
         * @param widget The widget to add.
         */
        virtual void add_widget(std::shared_ptr<Widget> widget);

        /**
         * @brief Remove a child widget.
         * @param widget The widget to remove.
         */
        virtual void remove_widget(std::shared_ptr<Widget> widget);

        /**
         * @brief Clear all children from this widget.
         */
        virtual void clear_widgets();

        /**
         * @brief Dispatches a touch down event to children.
         */
        virtual bool on_touch_down(std::shared_ptr<class TouchEvent> touch);

        /**
         * @brief Dispatches a touch move event to children.
         */
        virtual bool on_touch_move(std::shared_ptr<class TouchEvent> touch);

        /**
         * @brief Dispatches a touch up event to children.
         */
        virtual bool on_touch_up(std::shared_ptr<class TouchEvent> touch);

        const std::vector<std::shared_ptr<Widget>>& get_children() const { return m_children; }

        /**
         * @brief Set a property by string name (used by Builder).
         * @return true if the property was found and set.
         */
        virtual bool set_property(const std::string& name, const std::string& value);

        /**
         * @brief Get a property by string name (used by ExpressionParser).
         * @return std::any representing the value.
         */
        virtual std::any get_property(const std::string& name) const;

        NumericProperty<float> x{"x", 0.0f};
        NumericProperty<float> y{"y", 0.0f};
        NumericProperty<float> width{"width", 100.0f};
        NumericProperty<float> height{"height", 100.0f};

        // Pseudo-properties built out of standard coordinates
        NumericProperty<float> center_x{"center_x", 50.0f};
        NumericProperty<float> center_y{"center_y", 50.0f};

        // Kivy-like composite coordinate properties
        ReferenceListProperty<float> pos{"pos", {&x, &y}};
        ReferenceListProperty<float> size{"size", {&width, &height}};
        ReferenceListProperty<float> center{"center", {&center_x, &center_y}};

        NumericProperty<float> size_hint_x{"size_hint_x", 1.0f};
        NumericProperty<float> size_hint_y{"size_hint_y", 1.0f};
        
        // Storing as JSON-like string, e.g. "center_x:0.5;center_y:0.5" or "x:0.1;y:0.2"
        StringProperty pos_hint{"pos_hint", ""};

        /**
         * @brief Transforms point from window coordinates to local widget coordinates.
         * @return pair of (local_x, local_y)
         */
        virtual std::pair<float, float> to_local(float window_x, float window_y) const;

        /**
         * @brief Transforms point from local widget coordinates to parent coordinates.
         * @return pair of (parent_x, parent_y)
         */
        virtual std::pair<float, float> to_parent(float local_x, float local_y) const;

        /**
         * @brief Transforms point from local widget coordinates to window coordinates.
         * @return pair of (window_x, window_y)
         */
        virtual std::pair<float, float> to_window(float local_x, float local_y) const;

        /**
         * @brief Determine if a touch point (in window coords) collides with this widget's bounds.
         * @return true if point is inside widget bounds.
         */
        virtual bool collide_point(float window_x, float window_y) const;

        std::weak_ptr<Widget> parent;
        std::map<std::string, std::weak_ptr<Widget>> ids;

        std::shared_ptr<Canvas> canvas;

    protected:
        std::vector<std::shared_ptr<Widget>> m_children;
    };
}
