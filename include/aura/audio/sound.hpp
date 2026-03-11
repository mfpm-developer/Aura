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

namespace aura
{
    /**
     * @class Sound
     * @brief Abstract base class for playable audio elements.
     */
    class Sound : public EventDispatcher
    {
    public:
        Sound()
        {
            register_event_type("on_play");
            register_event_type("on_stop");
        }

        virtual ~Sound() = default;

        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void set_volume(float volume) = 0;
        virtual float get_volume() const = 0;

        virtual float get_length() const = 0;
        virtual float get_position() const = 0;
        virtual void seek(float position) = 0;

        const std::string& get_source() const { return m_source; }

    protected:
        std::string m_source;
        float m_volume = 1.0f;
    };
}
