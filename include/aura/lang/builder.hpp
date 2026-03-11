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
#pragma once
#include "aura/widgets/widget.hpp"
#include "aura/core/event_dispatcher.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace aura
{
    struct BuilderNode
    {
        std::string class_name;
        std::map<std::string, std::string> properties;
        std::vector<std::shared_ptr<BuilderNode>> children;
    };

    struct DynamicRule
    {
        std::string base_class;
        std::shared_ptr<BuilderNode> root_node;
    };

    /**
     * @class Builder
     * @brief Parses an `.aura` language file and instantiates a UI tree.
     */
    class Builder
    {
    public:
        /**
         * @brief Load and parse an .aura layout file.
         * @param filepath The path to the file.
         * @return The root Widget parsed from the file, or nullptr on failure.
         */
        static std::shared_ptr<Widget> load_file(const std::string& filepath);

        /**
         * @brief Parse an .aura layout string.
         * @param aura_string The raw content.
         * @return The root Widget parsed from the content.
         */
        static std::shared_ptr<Widget> load_string(const std::string& aura_string);

        /**
         * @brief Map a string callback identifier to a C++ std::function lambda.
         * Used to link `.aura` event handlers to compiled C++ routines.
         */
        static void register_callback(const std::string& name, EventDispatcher::EventCallback callback);

    private:
        static std::map<std::string, DynamicRule> s_dynamic_rules;
        static std::map<std::string, EventDispatcher::EventCallback> s_callbacks;

        static std::shared_ptr<Widget> build_node(std::shared_ptr<BuilderNode> node, std::shared_ptr<Widget> root_widget = nullptr);
        static void apply_rule(std::shared_ptr<Widget> widget, const DynamicRule& rule);
    };
}
