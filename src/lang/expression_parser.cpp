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
#include "aura/lang/expression_parser.hpp"
#include <cctype>
#include <iostream>

namespace aura
{
    enum class TokenType { Number, Identifier, Plus, Minus, Mul, Div, LParen, RParen, End, Error };

    struct Token {
        TokenType type;
        std::string value;
        float number = 0.0f;
    };

    class Lexer {
        const std::string& input;
        size_t pos = 0;
    public:
        Lexer(const std::string& str) : input(str) {}
        
        Token next() {
            while (pos < input.size() && std::isspace(input[pos])) pos++;
            if (pos >= input.size()) return {TokenType::End, ""};

            char c = input[pos];
            if (std::isdigit(c) || c == '.') {
                size_t start = pos;
                while (pos < input.size() && (std::isdigit(input[pos]) || input[pos] == '.')) pos++;
                return {TokenType::Number, input.substr(start, pos - start), std::stof(input.substr(start, pos - start))};
            }
            if (std::isalpha(c) || c == '_') {
                size_t start = pos;
                while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_' || input[pos] == '.')) pos++;
                return {TokenType::Identifier, input.substr(start, pos - start)};
            }
            pos++;
            switch (c) {
                case '+': return {TokenType::Plus, "+"};
                case '-': return {TokenType::Minus, "-"};
                case '*': return {TokenType::Mul, "*"};
                case '/': return {TokenType::Div, "/"};
                case '(': return {TokenType::LParen, "("};
                case ')': return {TokenType::RParen, ")"};
            }
            return {TokenType::Error, std::string(1, c)};
        }
    };

    class Parser {
        Lexer lexer;
        Token current;
        std::shared_ptr<Widget> self_widget;
        std::shared_ptr<Widget> root_widget;
        std::vector<std::pair<std::shared_ptr<Widget>, std::string>>& deps;

        void advance() { current = lexer.next(); }

        std::function<float()> parse_primary() {
            if (current.type == TokenType::Number) {
                float val = current.number;
                advance();
                return [val]() { return val; };
            }
            if (current.type == TokenType::Identifier) {
                std::string id = current.value;
                advance();
                
                std::shared_ptr<Widget> target = self_widget;
                std::string prop_name = id;

                size_t dot = id.find('.');
                if (dot != std::string::npos) {
                    std::string prefix = id.substr(0, dot);
                    prop_name = id.substr(dot + 1);
                    if (prefix == "self") {
                        target = self_widget;
                    } else if (prefix == "root") {
                        target = root_widget;
                    } else if (root_widget && root_widget->ids.count(prefix)) {
                        target = root_widget->ids[prefix].lock();
                    } else {
                        // Assume external dependency is invalid if not root or self
                        return nullptr;
                    }
                }

                if (target) {
                    deps.push_back({target, prop_name});
                    std::weak_ptr<Widget> weak_target = target;
                    return [weak_target, prop_name]() -> float {
                        if (auto t = weak_target.lock()) {
                            auto val = t->get_property(prop_name);
                            if (!val.has_value()) return 0.0f;
                            try { return std::any_cast<float>(val); } catch (...) {}
                            try { return (float)std::any_cast<int>(val); } catch (...) {}
                        }
                        return 0.0f;
                    };
                }
                return nullptr;
            }
            if (current.type == TokenType::LParen) {
                advance();
                auto node = parse_expression();
                if (current.type == TokenType::RParen) advance();
                return node;
            }
            return nullptr;
        }

        std::function<float()> parse_term() {
            auto left = parse_primary();
            while (left && (current.type == TokenType::Mul || current.type == TokenType::Div)) {
                auto op = current.type;
                advance();
                auto right = parse_primary();
                if (!right) return nullptr;
                auto lfunc = left;
                if (op == TokenType::Mul) left = [lfunc, right]() { return lfunc() * right(); };
                else left = [lfunc, right]() { float r = right(); return r == 0.0f ? 0.0f : lfunc() / r; };
            }
            return left;
        }

        std::function<float()> parse_expression() {
            auto left = parse_term();
            while (left && (current.type == TokenType::Plus || current.type == TokenType::Minus)) {
                auto op = current.type;
                advance();
                auto right = parse_term();
                if (!right) return nullptr;
                auto lfunc = left;
                if (op == TokenType::Plus) left = [lfunc, right]() { return lfunc() + right(); };
                else left = [lfunc, right]() { return lfunc() - right(); };
            }
            return left;
        }

    public:
        Parser(const std::string& input, std::shared_ptr<Widget> self, std::shared_ptr<Widget> root,
               std::vector<std::pair<std::shared_ptr<Widget>, std::string>>& deps)
            : lexer(input), self_widget(self), root_widget(root), deps(deps)
        {
            advance();
        }

        std::function<float()> parse() {
            return parse_expression();
        }
    };

    std::function<float()> ExpressionParser::parse(
        const std::string& expression, 
        std::shared_ptr<Widget> self, 
        std::shared_ptr<Widget> root,
        std::vector<std::pair<std::shared_ptr<Widget>, std::string>>& out_dependencies)
    {
        Parser parser(expression, self, root, out_dependencies);
        return parser.parse();
    }
}
