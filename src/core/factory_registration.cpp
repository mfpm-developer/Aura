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
#include "aura/core/factory.hpp"
#include "aura/widgets/button.hpp"
#include "aura/widgets/widget.hpp"
#include "aura/widgets/layouts/box_layout.hpp"
#include "aura/widgets/label.hpp"
#include "aura/widgets/image.hpp"
#include "aura/widgets/layouts/float_layout.hpp"
#include "aura/widgets/layouts/grid_layout.hpp"
#include "aura/widgets/text_input.hpp"
#include "aura/widgets/checkbox.hpp"
#include "aura/widgets/slider.hpp"
#include "aura/widgets/scroll_view.hpp"
#include "aura/widgets/screen_manager.hpp"
#include "aura/widgets/toggle_button.hpp"
#include "aura/widgets/switch.hpp"
#include "aura/widgets/progress_bar.hpp"

// M3 Components
#include "aura/md/theme.hpp"
#include "aura/md/text.hpp"
#include "aura/md/card.hpp"
#include "aura/md/button.hpp"
#include "aura/md/fab.hpp"
#include "aura/md/checkbox.hpp"
#include "aura/md/radiobutton.hpp"
#include "aura/md/switch.hpp"
#include "aura/md/slider.hpp"
#include "aura/md/text_input.hpp"
#include "aura/md/navigation_bar.hpp"
#include "aura/md/top_app_bar.hpp"
#include "aura/md/divider.hpp"
#include "aura/md/icon_button.hpp"
#include "aura/md/progress.hpp"
#include "aura/md/list.hpp"
#include "aura/md/tabs.hpp"
#include "aura/md/chips.hpp"
#include "aura/md/dialog.hpp"
#include "aura/md/select.hpp"
#include "aura/graphics/context_instructions.hpp"
#include "aura/graphics/vertex_instructions.hpp"

namespace aura
{
    // Register core widget classes with the Factory.
    // This function should be called during static initialization.
    static void register_widgets()
    {
        Factory::get_instance().register_class<Widget>("Widget");
        Factory::get_instance().register_class<Button>("Button");
        Factory::get_instance().register_class<Label>("Label");
        Factory::get_instance().register_class<Image>("Image");
        Factory::get_instance().register_class<BoxLayout>("BoxLayout");
        Factory::get_instance().register_class<FloatLayout>("FloatLayout");
        Factory::get_instance().register_class<GridLayout>("GridLayout");
        Factory::get_instance().register_class<TextInput>("TextInput");
        Factory::get_instance().register_class<CheckBox>("CheckBox");
        Factory::get_instance().register_class<Slider>("Slider");
        Factory::get_instance().register_class<ScrollView>("ScrollView");
        Factory::get_instance().register_class<ScreenManager>("ScreenManager");
        Factory::get_instance().register_class<Screen>("Screen");
        Factory::get_instance().register_class<ToggleButton>("ToggleButton");
        Factory::get_instance().register_class<Switch>("Switch");
        Factory::get_instance().register_class<ProgressBar>("ProgressBar");
        
        // M3 Widgets
        Factory::get_instance().register_class<md::MDText>("MDText");
        Factory::get_instance().register_class<md::MDCard>("MDCard");
        Factory::get_instance().register_class<md::MDButton>("MDButton");
        Factory::get_instance().register_class<md::MDFAB>("MDFAB");
        Factory::get_instance().register_class<md::MDCheckbox>("MDCheckbox");
        Factory::get_instance().register_class<md::MDRadioButton>("MDRadioButton");
        Factory::get_instance().register_class<md::MDSwitch>("MDSwitch");
        Factory::get_instance().register_class<md::MDSlider>("MDSlider");
        Factory::get_instance().register_class<md::MDTextInput>("MDTextInput");
        Factory::get_instance().register_class<md::MDNavigationBar>("MDNavigationBar");
        Factory::get_instance().register_class<md::MDNavigationItem>("MDNavigationItem");
        Factory::get_instance().register_class<md::MDTopAppBar>("MDTopAppBar");
        Factory::get_instance().register_class<md::MDDivider>("MDDivider");
        Factory::get_instance().register_class<md::MDIconButton>("MDIconButton");
        Factory::get_instance().register_class<md::MDCircularProgress>("MDCircularProgress");
        Factory::get_instance().register_class<md::MDLinearProgress>("MDLinearProgress");
        Factory::get_instance().register_class<md::MDList>("MDList");
        Factory::get_instance().register_class<md::MDListItem>("MDListItem");
        Factory::get_instance().register_class<md::MDTabs>("MDTabs");
        Factory::get_instance().register_class<md::MDTab>("MDTab");
        Factory::get_instance().register_class<md::MDChip>("MDChip");
        Factory::get_instance().register_class<md::MDDialog>("MDDialog");
        Factory::get_instance().register_class<md::MDSelect>("MDSelect");
        
        // Add other widget registrations here as needed.

        Factory::get_instance().register_instruction<SetColor>("Color");
        Factory::get_instance().register_instruction<Rectangle>("Rectangle");
        Factory::get_instance().register_instruction<RoundedRectangle>("RoundedRectangle");
        Factory::get_instance().register_instruction<Line>("Line");
        Factory::get_instance().register_instruction<Ellipse>("Ellipse");
        Factory::get_instance().register_instruction<PushMatrix>("PushMatrix");
        Factory::get_instance().register_instruction<PopMatrix>("PopMatrix");
        Factory::get_instance().register_instruction<Translate>("Translate");
        Factory::get_instance().register_instruction<Scale>("Scale");
        Factory::get_instance().register_instruction<Rotate>("Rotate");
    }

    // Explictly initialize factory to prevent static libs from stripping it.
    void init_factory()
    {
        static bool initialized = false;
        if (!initialized)
        {
            register_widgets();
            initialized = true;
        }
    }
}
