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

namespace aura
{
    /**
     * @class CameraBase
     * @brief Abstract base class for camera providers.
     */
    class CameraBase : public EventDispatcher
    {
    public:
        CameraBase(int index = 0, int resolution_w = 640, int resolution_h = 480)
            : m_index(index), m_resolution_w(resolution_w), m_resolution_h(resolution_h)
        {
            register_event_type("on_load");
            register_event_type("on_texture");
        }

        virtual ~CameraBase() = default;

        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;

        bool is_started() const { return m_started; }
        
    protected:
        int m_index;
        int m_resolution_w, m_resolution_h;
        bool m_started = false;
        
        // m_texture would reference an Aura Graphics Texture when implemented
    };
}
