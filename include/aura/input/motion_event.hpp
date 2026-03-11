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

#include "aura/core/object.hpp"
#include <string>
#include <map>
#include <any>

namespace aura
{
    /**
     * @class MotionEvent
     * @brief Represents a generic motion event (mouse, touch, joystick).
     */
    class MotionEvent : public Object
    {
    public:
        MotionEvent(const std::string& device, int id, float input_x, float input_y)
            : device(device), id(id), sx(0.0f), sy(0.0f), x(input_x), y(input_y),
              is_touch(false), button("left") {}

        virtual ~MotionEvent() = default;

        std::string device;
        int id;
        
        // Normalized screen coordinates (0 to 1)
        float sx, sy;

        // Window coordinates (pixels)
        float x, y;

        bool is_touch;
        std::string button; // "left", "right", "middle", "scrollup", "scrolldown"

        std::map<std::string, std::any> profile;
    };
}
