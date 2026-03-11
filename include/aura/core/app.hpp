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
#include <memory>

namespace aura
{
    class Widget;

    /**
     * @class App
     * @brief The base class for creating Aura applications.
     */
    class App : public EventDispatcher
    {
    public:
        App();
        virtual ~App();

        /**
         * @brief Initialize the application (often creates the root widget).
         */
        virtual void build() {}

        /**
         * @brief Load an .aura script and attach its root widget to the Window.
         * @param filepath The .aura file to parse.
         * @return The root widget created.
         */
        std::shared_ptr<Widget> load_aura(const std::string& filepath);

        /**
         * @brief Run the main application loop.
         */
        void run();

        /**
         * @brief Stop the application loop.
         */
        void stop();

    protected:
        bool m_running;
    };
}
