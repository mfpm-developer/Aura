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

#include "aura/widgets/layouts/layout.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/instruction.hpp"
#include <SDL3/SDL.h>
#include <chrono>

namespace aura
{
    /**
     * @class ScissorInstruction
     * @brief Glues SDL_RenderSetClipRect into the rendering pipeline to crop oversized children.
     */
    class ScissorInstruction : public Instruction
    {
    public:
        ScissorInstruction(float x, float y, float w, float h) : m_x(x), m_y(y), m_w(w), m_h(h) {}
        void set_rect(float x, float y, float w, float h) { m_x = x; m_y = y; m_w = w; m_h = h; }
        
        void apply() override;
        
    private:
        float m_x, m_y, m_w, m_h;
    };

    /**
     * @class ScissorDisableInstruction
     * @brief Disables the current SDL_RenderSetClipRect.
     */
    class ScissorDisableInstruction : public Instruction
    {
    public:
        void apply() override;
    };


    /**
     * @class ScrollView
     * @brief A layout that allows scrolling its oversized child.
     */
    class ScrollView : public Layout
    {
    public:
        ScrollView();
        virtual ~ScrollView() = default;

        NumericProperty<float> scroll_x{"scroll_x", 0.0f};
        NumericProperty<float> scroll_y{"scroll_y", 1.0f}; // 1.0 = top in kivy

        void add_widget(std::shared_ptr<Widget> widget) override;
        void do_layout() override;

        bool on_touch_down(std::shared_ptr<TouchEvent> touch) override;
        bool on_touch_move(std::shared_ptr<TouchEvent> touch) override;
        bool on_touch_up(std::shared_ptr<TouchEvent> touch) override;
        
        bool set_property(const std::string& name, const std::string& value) override;

    private:
        void update_clip_rect();
        void update_child_pos();

        std::shared_ptr<ScissorInstruction> m_clip_start;
        std::shared_ptr<ScissorDisableInstruction> m_clip_end;

        // Kinematics for dragging
        float m_last_touch_x = 0;
        float m_last_touch_y = 0;
        
        float m_touch_down_x = 0;
        float m_touch_down_y = 0;
        bool m_is_scrolling = false;
    };
}
