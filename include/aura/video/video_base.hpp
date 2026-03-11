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
#include <string>

namespace aura
{
    /**
     * @class VideoBase
     * @brief Abstract base class for video players.
     */
    class VideoBase : public EventDispatcher
    {
    public:
        VideoBase(const std::string& filename) : m_filename(filename)
        {
            register_event_type("on_eos");
            register_event_type("on_load");
            register_event_type("on_frame");
        }

        virtual ~VideoBase() = default;

        virtual void load() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void play() = 0;
        virtual void seek(float percent) = 0;

        const std::string& get_filename() const { return m_filename; }

    protected:
        std::string m_filename;
        float m_duration = -1.0f;
        float m_position = 0.0f;
        std::string m_state = "stop"; // play, pause, stop
    };
}
