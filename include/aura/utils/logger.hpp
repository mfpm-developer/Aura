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

#include <iostream>
#include <string>

namespace aura
{
    /**
     * @class Logger
     * @brief Simple logging utility for Aura.
     */
    class Logger
    {
    public:
        enum class Level
        {
            Info,
            Warning,
            Error,
            Debug
        };

        static void log(Level level, const std::string& message)
        {
            switch (level)
            {
                case Level::Info:
                    std::cout << "[INFO] " << message << std::endl;
                    break;
                case Level::Warning:
                    std::cout << "[WARNING] " << message << std::endl;
                    break;
                case Level::Error:
                    std::cerr << "[ERROR] " << message << std::endl;
                    break;
                case Level::Debug:
                    std::cout << "[DEBUG] " << message << std::endl;
                    break;
            }
        }

        static void info(const std::string& message) { log(Level::Info, message); }
        static void warning(const std::string& message) { log(Level::Warning, message); }
        static void error(const std::string& message) { log(Level::Error, message); }
        static void debug(const std::string& message) { log(Level::Debug, message); }
    };
}
