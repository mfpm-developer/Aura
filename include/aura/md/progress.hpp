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
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/md/theme.hpp"
#include <cmath>
#include <algorithm>

namespace aura {
namespace md {

    class MDLinearProgress : public Widget
    {
    public:
        MDLinearProgress()
        {
            m_track_color = std::make_shared<SetColor>();
            m_track = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 100.0f, 4.0f, 2.0f);

            m_progress_color = std::make_shared<SetColor>();
            m_progress = std::make_shared<RoundedRectangle>(0.0f, 0.0f, 0.0f, 4.0f, 2.0f);

            this->canvas->before->add(m_track_color);
            this->canvas->before->add(m_track);
            
            this->canvas->after->add(m_progress_color);
            this->canvas->after->add(m_progress);

            size_hint_y.set(-1.0f);
            height.set(4.0f); // M3 standard thickness

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            
            value.bind("on_value", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });

            on_theme_changed();
        }

        NumericProperty<float> value{"value", 0.0f}; // 0.0 to 1.0

        bool set_property(const std::string& name, const std::string& value_str) override
        {
            if (name == "value") { value.set_value(std::stof(value_str)); return true; }
            return Widget::set_property(name, value_str);
        }

    protected:
        void on_theme_changed()
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            const auto& track_c = colors.surface_variant;
            m_track_color->r = track_c.r; m_track_color->g = track_c.g; m_track_color->b = track_c.b; m_track_color->a = track_c.a;

            const auto& prog_c = colors.primary;
            m_progress_color->r = prog_c.r; m_progress_color->g = prog_c.g; m_progress_color->b = prog_c.b; m_progress_color->a = prog_c.a;
        }

    private:
        void update_geometry()
        {
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1] + (h - 4.0f) / 2.0f; // Center if height gets expanded

            m_track->x = x;
            m_track->y = y;
            m_track->width = w;
            m_track->height = 4.0f;

            m_progress->x = x;
            m_progress->y = y;
            m_progress->width = w * std::clamp(value.get_value(), 0.0f, 1.0f);
            m_progress->height = 4.0f;
        }

        std::shared_ptr<SetColor> m_track_color;
        std::shared_ptr<RoundedRectangle> m_track;

        std::shared_ptr<SetColor> m_progress_color;
        std::shared_ptr<RoundedRectangle> m_progress;
    };


    class MDCircularProgress : public Widget
    {
    public:
        MDCircularProgress()
        {
            m_track_color = std::make_shared<SetColor>();
            m_track = std::make_shared<Line>();
            m_track->width_val = 4.0f;

            m_progress_color = std::make_shared<SetColor>();
            m_progress = std::make_shared<Line>();
            m_progress->width_val = 4.0f;

            this->canvas->before->add(m_track_color);
            this->canvas->before->add(m_track);
            
            this->canvas->after->add(m_progress_color);
            this->canvas->after->add(m_progress);

            size_hint_x.set(-1.0f);
            size_hint_y.set(-1.0f);
            width.set(40.0f); // Default size
            height.set(40.0f);

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            
            value.bind("on_value", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });

            on_theme_changed();
        }

        NumericProperty<float> value{"value", 0.0f}; // 0.0 to 1.0

        bool set_property(const std::string& name, const std::string& value_str) override
        {
            if (name == "value") { value.set_value(std::stof(value_str)); return true; }
            return Widget::set_property(name, value_str);
        }

    protected:
        void on_theme_changed()
        {
            const auto& theme = Theme::get();
            const auto& colors = theme.colors();
            
            const auto& track_c = colors.surface_variant;
            m_track_color->r = track_c.r; m_track_color->g = track_c.g; m_track_color->b = track_c.b; m_track_color->a = track_c.a;

            const auto& prog_c = colors.primary;
            m_progress_color->r = prog_c.r; m_progress_color->g = prog_c.g; m_progress_color->b = prog_c.b; m_progress_color->a = prog_c.a;
        }

    private:
        void update_geometry()
        {
            float cx = pos.get_value()[0] + width.get_value() / 2.0f;
            float cy = pos.get_value()[1] + height.get_value() / 2.0f;
            float rad = std::min(width.get_value(), height.get_value()) / 2.0f - 2.0f;

            if (rad <= 0) return;

            int segments = 40;
            std::vector<float> track_pts;
            for (int i=0; i<=segments; ++i) {
                float theta = 2.0f * 3.14159265f * float(i) / float(segments);
                track_pts.push_back(cx + rad * std::cos(theta));
                track_pts.push_back(cy + rad * std::sin(theta));
            }
            m_track->points = track_pts;

            float val = std::clamp(value.get_value(), 0.0f, 1.0f);
            std::vector<float> prog_pts;
            if (val > 0.01f) {
                int active_segments = std::max(2, (int)(segments * val));
                for (int i=0; i<=active_segments; ++i) {
                    float theta = -3.14159265f / 2.0f + (2.0f * 3.14159265f * val * float(i) / float(active_segments));
                    prog_pts.push_back(cx + rad * std::cos(theta));
                    prog_pts.push_back(cy + rad * std::sin(theta));
                }
            }
            m_progress->points = prog_pts;
        }

        std::shared_ptr<SetColor> m_track_color;
        std::shared_ptr<Line> m_track;

        std::shared_ptr<SetColor> m_progress_color;
        std::shared_ptr<Line> m_progress;
    };

} // namespace md
} // namespace aura
