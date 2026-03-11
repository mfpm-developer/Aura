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

namespace aura
{
    /**
     * @class Layout
     * @brief Base class for layout managers.
     */
    class Layout : public Widget
    {
    public:
        Layout() {
            auto trigger = [this](EventDispatcher*, const std::any&) {
                this->do_layout();
                return false;
            };
            this->width.bind("on_width", trigger);
            this->height.bind("on_height", trigger);
        }
        virtual ~Layout() = default;

        void add_widget(std::shared_ptr<Widget> widget) override
        {
            Widget::add_widget(widget);
            do_layout();
        }

        /**
         * @brief Recalculate and apply positioning and sizing to all children.
         */
        virtual void do_layout() = 0;
    };
}
