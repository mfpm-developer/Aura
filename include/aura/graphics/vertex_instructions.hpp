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

#include "aura/graphics/instruction.hpp"
#include "aura/core/window.hpp"
#include <SDL3/SDL.h>
#include <cmath>
#include <vector>

namespace aura
{
    /**
     * @class VertexInstruction
     * @brief Base class for geometry instructions.
     */
    class VertexInstruction : public Instruction
    {
    public:
        VertexInstruction() = default;
        virtual ~VertexInstruction() = default;
    };

    /**
     * @class Rectangle
     * @brief Geometry instruction to draw a rectangle.
     */
    class Rectangle : public VertexInstruction
    {
    public:
        Rectangle(float x = 0.0f, float y = 0.0f, float width = 100.0f, float height = 100.0f)
            : x(x), y(y), width(width), height(height) {}

        float x, y, width, height;

        void apply() override
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (renderer)
            {
                auto t = Window::get_instance().get_transform();
                SDL_FRect rect = { x * t.sx + t.tx, y * t.sy + t.ty, width * t.sx, height * t.sy };
                
                float r,g,b,a;
                SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);
                
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "x") x = std::stof(value);
            else if (name == "y") y = std::stof(value);
            else if (name == "width" || name == "w") width = std::stof(value);
            else if (name == "height" || name == "h") height = std::stof(value);
            else return false;
            return true;
        }
    };

    /**
     * @class RoundedRectangle
     * @brief Geometry instruction to draw a rectangle with rounded corners matching Kivy.
     */
    class RoundedRectangle : public VertexInstruction
    {
    public:
        RoundedRectangle(float x = 0.0f, float y = 0.0f, float width = 100.0f, float height = 100.0f, float radius = 10.0f)
            : x(x), y(y), width(width), height(height), radius(radius) {}

        float x, y, width, height, radius;
        int segments = 10;

        void apply() override
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (!renderer) return;

            float r=1.0f, g=1.0f, b=1.0f, a=1.0f;
            SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);
            SDL_FColor fcolor = {r, g, b, a};

            auto t = Window::get_instance().get_transform();

            float cx = x + width / 2.0f;
            float cy = y + height / 2.0f;
            float rad = std::min({radius, width / 2.0f, height / 2.0f});
            if (rad < 0) rad = 0;

            int actual_segments = std::max(2, segments);
            int total_vertices = 4 * actual_segments + 1;
            std::vector<SDL_Vertex> vertices(total_vertices);
            std::vector<int> indices(4 * actual_segments * 3);

            vertices[0].position.x = cx * t.sx + t.tx;
            vertices[0].position.y = cy * t.sy + t.ty;
            vertices[0].color = fcolor;

            struct Corner { float cx, cy; float start_angle; };
            Corner cw_corners[4] = {
                { x + width - rad, y + rad, -3.14159265f / 2.0f },          // TR
                { x + width - rad, y + height - rad, 0.0f },                // BR
                { x + rad, y + height - rad, 3.14159265f / 2.0f },          // BL
                { x + rad, y + rad, 3.14159265f }                           // TL
            };

            int v_idx = 1;
            for (int c = 0; c < 4; ++c) {
                for (int i = 0; i < actual_segments; ++i) {
                    float theta = cw_corners[c].start_angle + (3.14159265f / 2.0f) * float(i) / float(actual_segments - 1);
                    float px = cw_corners[c].cx + rad * cosf(theta);
                    float py = cw_corners[c].cy + rad * sinf(theta);
                    vertices[v_idx].position.x = px * t.sx + t.tx;
                    vertices[v_idx].position.y = py * t.sy + t.ty;
                    vertices[v_idx].color = fcolor;
                    
                    indices[(v_idx - 1) * 3] = 0;
                    indices[(v_idx - 1) * 3 + 1] = v_idx;
                    indices[(v_idx - 1) * 3 + 2] = (v_idx == 4 * actual_segments) ? 1 : v_idx + 1;
                    v_idx++;
                }
            }

            SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), indices.data(), indices.size());
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "x") x = std::stof(value);
            else if (name == "y") y = std::stof(value);
            else if (name == "width" || name == "w") width = std::stof(value);
            else if (name == "height" || name == "h") height = std::stof(value);
            else if (name == "radius") radius = std::stof(value);
            else return false;
            return true;
        }
    };

    /**
     * @class Line
     * @brief Geometry instruction to draw lines, outlines, or stroked shapes.
     */
    class Line : public VertexInstruction
    {
    public:
        std::vector<float> points;
        float width_val = 1.0f;
        std::vector<float> rectangle;

        Line() = default;

        void apply() override
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (!renderer) return;

            float r=1.0f, g=1.0f, b=1.0f, a=1.0f;
            SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);

            auto t = Window::get_instance().get_transform();

            if (!rectangle.empty() && rectangle.size() == 4) {
                float rx = rectangle[0], ry = rectangle[1], rw = rectangle[2], rh = rectangle[3];
                float hw = width_val / 2.0f;
                SDL_FRect rects[4] = {
                    { (rx - hw)*t.sx + t.tx, (ry - hw)*t.sy + t.ty, (rw + width_val)*t.sx, width_val*t.sy }, // Top
                    { (rx - hw)*t.sx + t.tx, (ry + rh - hw)*t.sy + t.ty, (rw + width_val)*t.sx, width_val*t.sy }, // Bottom
                    { (rx - hw)*t.sx + t.tx, (ry - hw)*t.sy + t.ty, width_val*t.sx, (rh + width_val)*t.sy }, // Left
                    { (rx + rw - hw)*t.sx + t.tx, (ry - hw)*t.sy + t.ty, width_val*t.sx, (rh + width_val)*t.sy } // Right
                };
                for (int i=0; i<4; ++i) SDL_RenderFillRect(renderer, &rects[i]);
            }
            else if (points.size() >= 4) {
                for (size_t i = 0; i < points.size() - 2; i += 2) {
                    float x1 = points[i]*t.sx + t.tx;
                    float y1 = points[i+1]*t.sy + t.ty;
                    float x2 = points[i+2]*t.sx + t.tx;
                    float y2 = points[i+3]*t.sy + t.ty;
                    float hw = width_val / 2.0f;
                    float dx = x2 - x1, dy = y2 - y1;
                    float len = std::sqrt(dx*dx + dy*dy);
                    if (len > 0) { dx /= len; dy /= len; }
                    float nx = -dy * hw, ny = dx * hw;
                    
                    SDL_Vertex verts[4];
                    SDL_FColor c = {r, g, b, a};
                    verts[0] = {{x1 + nx, y1 + ny}, c, {0, 0}};
                    verts[1] = {{x1 - nx, y1 - ny}, c, {0, 0}};
                    verts[2] = {{x2 + nx, y2 + ny}, c, {0, 0}};
                    verts[3] = {{x2 - nx, y2 - ny}, c, {0, 0}};
                    int idx[6] = {0, 1, 2, 1, 3, 2};
                    SDL_RenderGeometry(renderer, nullptr, verts, 4, idx, 6);
                }
            }
        }

        static std::vector<float> parse_float_list(const std::string& value)
        {
            std::vector<float> result;
            size_t start = 0;
            while (start < value.length()) {
                size_t end = value.find(',', start);
                if (end == std::string::npos) end = value.length();
                if (end > start) {
                    try { result.push_back(std::stof(value.substr(start, end - start))); } catch (...) {}
                }
                start = end + 1;
            }
            return result;
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "width") width_val = std::stof(value);
            else if (name == "points") points = parse_float_list(value);
            else if (name == "rectangle") rectangle = parse_float_list(value);
            else return false;
            return true;
        }
    };

    /**
     * @class Ellipse
     * @brief Geometry instruction to draw an ellipse or circle.
     */
    class Ellipse : public VertexInstruction
    {
    public:
        Ellipse(float x = 0.0f, float y = 0.0f, float width = 100.0f, float height = 100.0f)
            : x(x), y(y), width(width), height(height) {}

        float x, y, width, height;

        void apply() override
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (!renderer) return;

            float r=1.0f, g=1.0f, b=1.0f, a=1.0f;
            SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);
            SDL_FColor fcolor = {r, g, b, a};

            auto t = Window::get_instance().get_transform();

            int segments = 32;
            std::vector<SDL_Vertex> vertices(segments + 1);
            std::vector<int> indices(segments * 3);

            float cx = x + width / 2.0f;
            float cy = y + height / 2.0f;
            float rx = width / 2.0f;
            float ry = height / 2.0f;

            float tcx = cx * t.sx + t.tx;
            float tcy = cy * t.sy + t.ty;
            float trx = rx * t.sx;
            float try_ = ry * t.sy;

            vertices[0].position.x = tcx;
            vertices[0].position.y = tcy;
            vertices[0].color = fcolor;

            for (int i = 0; i < segments; ++i) {
                float theta = 2.0f * 3.1415926535f * float(i) / float(segments);
                vertices[i + 1].position.x = tcx + trx * cosf(theta);
                vertices[i + 1].position.y = tcy + try_ * sinf(theta);
                vertices[i + 1].color = fcolor;

                indices[i * 3] = 0;
                indices[i * 3 + 1] = i + 1;
                indices[i * 3 + 2] = (i + 1 == segments) ? 1 : i + 2;
            }

            SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), indices.data(), indices.size());
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "x") x = std::stof(value);
            else if (name == "y") y = std::stof(value);
            else if (name == "width" || name == "w") width = std::stof(value);
            else if (name == "height" || name == "h") height = std::stof(value);
            else return false;
            return true;
        }
    };
}
