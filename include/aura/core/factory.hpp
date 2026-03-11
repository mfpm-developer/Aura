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
#include "aura/graphics/instruction.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace aura
{
    /**
     * @class Factory
     * @brief Singleton registry to map class names to their instantiation functions.
     * Required since C++ lacks native reflection, used by the Builder.
     */
    class Factory
    {
    public:
        using CreatorFunc = std::function<std::shared_ptr<Widget>()>;
        using InstCreatorFunc = std::function<std::shared_ptr<Instruction>()>;

        static Factory& get_instance()
        {
            static Factory instance;
            return instance;
        }

        template<typename T>
        void register_class(const std::string& class_name)
        {
            if (m_registry.find(class_name) != m_registry.end())
            {
                std::cerr << "Warning: Factory class " << class_name << " is already registered!" << std::endl;
                return;
            }

            m_registry[class_name] = []() -> std::shared_ptr<Widget> {
                return std::make_shared<T>();
            };
        }

        std::shared_ptr<Widget> instantiate(const std::string& class_name)
        {
            auto it = m_registry.find(class_name);
            if (it != m_registry.end())
            {
                return it->second();
            }

            std::cerr << "Error: Factory could not instantiate unregistered class: " << class_name << std::endl;
            return nullptr;
        }

        template<typename T>
        void register_instruction(const std::string& class_name)
        {
            if (m_inst_registry.find(class_name) != m_inst_registry.end()) return;
            m_inst_registry[class_name] = []() -> std::shared_ptr<Instruction> {
                return std::make_shared<T>();
            };
        }

        std::shared_ptr<Instruction> instantiate_instruction(const std::string& class_name)
        {
            auto it = m_inst_registry.find(class_name);
            if (it != m_inst_registry.end()) return it->second();
            return nullptr;
        }

    private:
        Factory() = default;
        ~Factory() = default;

        Factory(const Factory&) = delete;
        Factory& operator=(const Factory&) = delete;

        std::unordered_map<std::string, CreatorFunc> m_registry;
        std::unordered_map<std::string, InstCreatorFunc> m_inst_registry;
    };

    /**
     * @brief Initialize the factory registry. 
     * Called automatically by App().
     */
    void init_factory();
}
