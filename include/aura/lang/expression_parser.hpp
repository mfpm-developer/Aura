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
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace aura
{
    /**
     * @class ExpressionParser
     * @brief Parses mathematical strings to build auto-calculating bindings (like Kivy's KV evaluator).
     */
    class ExpressionParser
    {
    public:
        /**
         * @brief Builds a dynamic float evaluation callback from a string.
         *        Identifies dependencies like `self.width` or `my_id.x`.
         * 
         * @param expression The math expression (e.g. "self.width * 0.5")
         * @param self The context widget
         * @param root The root widget context for resolving IDs
         * @param out_dependencies A list of property triggers (widget + property name) to bind to.
         * @return A callable that calculates the math result at runtime. Returns nullptr if failed.
         */
        static std::function<float()> parse(
            const std::string& expression, 
            std::shared_ptr<Widget> self, 
            std::shared_ptr<Widget> root,
            std::vector<std::pair<std::shared_ptr<Widget>, std::string>>& out_dependencies);
    };
}
