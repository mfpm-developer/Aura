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
#include "aura/lang/builder.hpp"
#include "aura/lang/expression_parser.hpp"
#include "aura/core/factory.hpp"
#include "aura/graphics/instruction.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <SDL3/SDL.h>

namespace aura
{
    std::map<std::string, DynamicRule> Builder::s_dynamic_rules;
    std::map<std::string, EventDispatcher::EventCallback> Builder::s_callbacks;

    void Builder::register_callback(const std::string& name, EventDispatcher::EventCallback callback)
    {
        s_callbacks[name] = callback;
    }

    static int get_indent_level(const std::string& line)
    {
        int count = 0;
        for (char c : line)
        {
            if (c == ' ') count++;
            else if (c == '\t') count += 4;
            else break;
        }
        return count / 4;
    }

    static std::string trim(const std::string& str)
    {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    static std::string strip_quotes(const std::string& str)
    {
        if (str.length() >= 2 && 
            ((str.front() == '"' && str.back() == '"') || 
             (str.front() == '\'' && str.back() == '\'')))
        {
            return str.substr(1, str.length() - 2);
        }
        return str;
    }

    std::shared_ptr<Widget> Builder::load_file(const std::string& filepath)
    {
        SDL_IOStream* io = SDL_IOFromFile(filepath.c_str(), "r");
        if (!io)
        {
            SDL_Log("Builder: Failed to open file: %s, SDL_Error: %s", filepath.c_str(), SDL_GetError());
            return nullptr;
        }
        
        Sint64 size = SDL_GetIOSize(io);
        if (size <= 0) {
            SDL_CloseIO(io);
            return load_string("");
        }

        std::string buffer(size, '\0');
        size_t read_bytes = SDL_ReadIO(io, &buffer[0], size);
        SDL_CloseIO(io);
        
        if (read_bytes != (size_t)size) {
            SDL_Log("Builder: Warning, did not read full file: %s", filepath.c_str());
        }

        return load_string(buffer);
    }

    std::shared_ptr<Widget> Builder::load_string(const std::string& aura_string)
    {
        std::istringstream stream(aura_string);
        std::string line;
        
        std::stack<std::pair<int, std::shared_ptr<BuilderNode>>> node_stack;
        std::vector<std::shared_ptr<BuilderNode>> root_nodes;

        // Phase 1: Parse to AST
        while (std::getline(stream, line))
        {
            std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#') continue;

            int indent = get_indent_level(line);

            while (!node_stack.empty() && node_stack.top().first >= indent)
            {
                node_stack.pop();
            }

            auto colon_pos = trimmed.find(':');
            if (colon_pos != std::string::npos)
            {
                std::string lhs = trim(trimmed.substr(0, colon_pos));
                std::string rhs = trim(trimmed.substr(colon_pos + 1));

                if (rhs.empty())
                {
                    // Class Declaration
                    if (!lhs.empty() && lhs.back() == ':') lhs.pop_back();

                    if (lhs.front() == '<' && lhs.back() == '>')
                    {
                        if (indent != 0) { std::cerr << "Builder: Rules must be indent 0." << std::endl; continue; }
                        
                        std::string rule_content = lhs.substr(1, lhs.size() - 2);
                        auto at_pos = rule_content.find('@');
                        if (at_pos != std::string::npos)
                        {
                            std::string new_class = trim(rule_content.substr(0, at_pos));
                            std::string base_class = trim(rule_content.substr(at_pos + 1));
                            
                            auto node = std::make_shared<BuilderNode>();
                            node->class_name = base_class;
                            
                            s_dynamic_rules[new_class] = {base_class, node};
                            node_stack.push({indent, node});
                        }
                    }
                    else
                    {
                        auto node = std::make_shared<BuilderNode>();
                        node->class_name = lhs;

                        if (node_stack.empty()) {
                            root_nodes.push_back(node);
                        } else {
                            node_stack.top().second->children.push_back(node);
                        }
                        node_stack.push({indent, node});
                    }
                }
                else
                {
                    // Property Assignment  
                    if (!node_stack.empty())
                    {
                        node_stack.top().second->properties[lhs] = strip_quotes(rhs);
                    }
                }
            }
        }

        // Phase 2: Build Widgets
        std::shared_ptr<Widget> root = nullptr;
        for (auto& rn : root_nodes)
        {
            root = build_node(rn);
        }
        
        return root; // Return last instantiated root node
    }

    std::shared_ptr<Widget> Builder::build_node(std::shared_ptr<BuilderNode> node, std::shared_ptr<Widget> root_widget)
    {
        std::shared_ptr<Widget> widget = nullptr;

        auto apply_node = [&](std::shared_ptr<Widget> w, std::shared_ptr<BuilderNode> n, std::shared_ptr<Widget> active_root) {
            for (const auto& prop : n->properties) {
                if (prop.first == "id" && active_root) {
                    active_root->ids[prop.second] = w;
                } else if (prop.first.find("on_") == 0) {
                    auto cb_it = s_callbacks.find(prop.second);
                    if (cb_it != s_callbacks.end()) {
                        w->bind(prop.first, cb_it->second);
                    } else {
                        SDL_Log("Builder: Callback '%s' not registered for event '%s'!", prop.second.c_str(), prop.first.c_str());
                    }
                } else {
                    if (!w->set_property(prop.first, prop.second)) {
                        std::vector<std::pair<std::shared_ptr<Widget>, std::string>> deps;
                        auto cb = ExpressionParser::parse(prop.second, w, active_root, deps);
                        if (cb) {
                            std::weak_ptr<Widget> weak_w = w;
                            auto update_prop = [weak_w, prop_name = prop.first, cb](EventDispatcher*, const std::any&) {
                                if (auto str_w = weak_w.lock()) {
                                    str_w->set_property(prop_name, std::to_string(cb()));
                                }
                                return false;
                            };
                            for (auto& dep : deps) {
                                dep.first->bind("on_" + dep.second, update_prop);
                            }
                            update_prop(nullptr, {}); // Trigger initial compute
                        } else {
                            SDL_Log("Builder: Failed to parse property or expression '%s': %s", prop.first.c_str(), prop.second.c_str());
                        }
                    }
                }
            }
            for (const auto& child : n->children) {
                if (child->class_name == "canvas" || child->class_name == "canvas.before" || child->class_name == "canvas.after") {
                    std::shared_ptr<InstructionGroup> target_group = w->canvas;
                    if (child->class_name == "canvas.before") target_group = w->canvas->before;
                    else if (child->class_name == "canvas.after") target_group = w->canvas->after;

                    for (const auto& inst_node : child->children) {
                        auto inst = Factory::get_instance().instantiate_instruction(inst_node->class_name);
                        if (inst) {
                            for (const auto& prop : inst_node->properties) {
                                inst->set_property(prop.first, prop.second);
                            }
                            target_group->add(inst);
                        } else {
                            SDL_Log("Builder: Unknown instruction %s", inst_node->class_name.c_str());
                        }
                    }
                } else {
                    w->add_widget(build_node(child, active_root));
                }
            }
        };

        auto it = s_dynamic_rules.find(node->class_name);
        if (it != s_dynamic_rules.end())
        {
            const auto& rule = it->second;
            widget = Factory::get_instance().instantiate(rule.base_class);
            if (widget)
            {
                apply_node(widget, rule.root_node, widget); // Rule nodes construct their own namespace
            }
        }
        else
        {
            widget = Factory::get_instance().instantiate(node->class_name);
        }

        if (widget)
        {
            std::shared_ptr<Widget> active_root = root_widget ? root_widget : widget;
            apply_node(widget, node, active_root); // Caller overrides fall into the caller namespace
        }
        else
        {
            SDL_Log("Builder: Unknown component %s", node->class_name.c_str());
        }

        return widget;
    }
}
