# Aura Framework

Aura is a high-performance, cross-platform C++ UI framework powered by **SDL3**. It brings Kivy-like declarative UI development to native C++, featuring a meticulously implemented **Material Design 3 (M3)** widget layer.

## Features
- **Hardware Accelerated:** Uses `SDL_Renderer` under the hood. 
- **Cross-Platform:** Write once, deploy to Linux, Windows, macOS, and Android natively.
- **Aura Language (`.aura`):** A custom declarative parser that generates complete widget hierarchies at runtime. 
- **Material Design 3:** Full parity with Google's M3 specifications, including dynamic Color Profiling and Dark Mode support.

---

## 🚀 Quick Start (CMake)

Aura is completely self-contained. The `Vendor/` folder contains local copies of `SDL3`, `SDL3_image`, and `SDL3_ttf`. No GitHub downloads or system package dependencies are required!

To consume the framework in your own project, structure it like this:
```txt
MyGameProject/
  CMakeLists.txt
  src/
    main.cpp
  third_party/
    Aura/           <--- Drop the Aura repository folder here
```

In your `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyGameProject LANGUAGES CXX)

# Hook into the offline Aura framework
add_subdirectory(third_party/Aura)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE aura)
```

---

## 🎨 The Aura Language (`.aura`)

Writing complex C++ boilerplate for UI trees is tedious. Aura solves this with the `.aura` file syntax, mapping directly to C++ classes registered in the internal `Factory`.

**`layout.aura`**
```yaml
#:import app my_app_instance

BoxLayout:
    orientation: 'vertical'
    spacing: 12
    padding: 16

    MDTopAppBar:
        headline: "Aura Settings"
        
    MDCard:
        type: 'elevated'
        height: 60

        MDSwitch:
            id: theme_switch
            active: False
            center_y: self.parent.center_y
            on_active: app.toggle_theme

    MDButton:
        text: "Submit"
        variant: 'filled'
        on_release: app.submit_form
```

**`main.cpp`**
```cpp
#include "aura/core/app.hpp"
#include "aura/md/theme.hpp"

class MyApp : public aura::App {
public:
    void build() override {
        // Initializes the root Canvas and builds the UI from layout.aura
        load_aura("layout.aura");
    }

    // Callbacks linked in the `.aura` file
    void toggle_theme(aura::EventDispatcher* sender, const std::any& active_state) {
        bool is_dark = std::any_cast<bool>(active_state);
        aura::md::Theme::get().set_dark_mode(is_dark);
    }
};

int main(int argc, char* argv[]) {
    MyApp app;
    app.run();
    return 0;
}
```

---

## 🧩 Available M3 Components

The `AuraMD` layer provides the following widgets, completely ready to use in C++ or `.aura` scripts:

**Interactive Primitives**
* `MDButton` (Filled, Outlined, Text, Tonal)
* `MDText` (M3 Typography specs like Display, Headline, Title)
* `MDFAB` (Floating Action Button)
* `MDIconButton`

**Inputs & Forms**
* `MDTextInput` (Outlined, Filled text fields)
* `MDSwitch`
* `MDCheckbox`
* `MDRadioButton`
* `MDSlider`
* `MDSelect` (Dropdown form menu)

**Layout & Organization**
* `MDCard` (Elevated, Filled, Outlined)
* `MDDivider`
* `MDList`, `MDListItem`
* `MDChip`

**Navigation**
* `MDTopAppBar`
* `MDNavigationBar`
* `MDTabs`, `MDTab`

**Overlays & Progress**
* `MDDialog` (Modal Popups)
* `MDMenu` (Context overlays)
* `MDLinearProgress`, `MDCircularProgress`

---

## Under the Hood

Aura uses an Event Dispatcher similar to Kivy. Every class inheriting from `Widget` has reactive properties:
```cpp
NumericProperty<int> width;
ReferenceListProperty pos;
```
When `width` changes, `on_width` will implicitly be dispatched. `.aura` scripts use these reactive events to layout dynamic UIs without fixed pixels (e.g., `center_x: self.parent.center_x`).
