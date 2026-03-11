cmake_minimum_required(VERSION 3.16)
project(Aura VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------
# 1. Provide SDL3 Base Library
# -------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL/CMakeLists.txt")
    message(STATUS "[Aura] Found local SDL3 in Vendor folder.")
    set(SDL_TEST OFF CACHE BOOL "Disable SDL3 test scope" FORCE)
    if (NOT TARGET SDL3-shared AND NOT TARGET SDL3::SDL3)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "[Aura] Vendor/SDL is missing. Attempting to fetch or find globally...")
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND AND NOT TARGET SDL3::SDL3)
        include(FetchContent)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.0-preview-1
        )
        set(SDL_TEST OFF CACHE BOOL "Disable SDL3 tests" FORCE)
        FetchContent_MakeAvailable(SDL3)
    endif()
endif()

# -------------------------------------------------------------
# 2. Provide SDL3_ttf Font Rendering Library
# -------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL_ttf/CMakeLists.txt")
    message(STATUS "[Aura] Found local SDL3_ttf in Vendor folder.")
    if (NOT TARGET SDL3_ttf-shared AND NOT TARGET SDL3_ttf::SDL3_ttf)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL_ttf EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "[Aura] Vendor/SDL_ttf is missing. Attempting to fetch or find globally...")
    find_package(SDL3_ttf QUIET)
    if(NOT SDL3_ttf_FOUND AND NOT TARGET SDL3_ttf::SDL3_ttf)
        include(FetchContent)
        FetchContent_Declare(
            SDL3_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG release-3.2.0
        )
        FetchContent_MakeAvailable(SDL3_ttf)
    endif()
endif()

# -------------------------------------------------------------
# 3. Provide SDL3_image Asset Decoding Library
# -------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL_image/CMakeLists.txt")
    message(STATUS "[Aura] Found local SDL3_image in Vendor folder.")
    if (NOT TARGET SDL3_image-shared AND NOT TARGET SDL3_image::SDL3_image)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL_image EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "[Aura] Vendor/SDL_image is missing. Attempting to fetch or find globally...")
    find_package(SDL3_image QUIET)
    if(NOT SDL3_image_FOUND AND NOT TARGET SDL3_image::SDL3_image)
        include(FetchContent)
        FetchContent_Declare(
            SDL3_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG release-3.2.0
        )
        FetchContent_MakeAvailable(SDL3_image)
    endif()
endif()

# -------------------------------------------------------------
# 4. Aura Framework Compilation
# -------------------------------------------------------------
add_library(aura STATIC)

file(GLOB_RECURSE AURA_SOURCES "src/*.cpp")
target_sources(aura PRIVATE ${AURA_SOURCES})

target_include_directories(aura PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Robust target linking depending on how the dependencies were resolved
# (add_subdirectory creates Foo-shared, find_package creates Foo::Foo)
if(TARGET SDL3::SDL3)
    target_link_libraries(aura PUBLIC SDL3::SDL3)
else()
    target_link_libraries(aura PUBLIC SDL3-shared)
endif()

if(TARGET SDL3_ttf::SDL3_ttf)
    target_link_libraries(aura PUBLIC SDL3_ttf::SDL3_ttf)
else()
    target_link_libraries(aura PUBLIC SDL3_ttf-shared)
endif()

if(TARGET SDL3_image::SDL3_image)
    target_link_libraries(aura PUBLIC SDL3_image::SDL3_image)
else()
    target_link_libraries(aura PUBLIC SDL3_image-shared)
endif()

message(STATUS "[Aura] Library configuration complete. Target 'aura' is now available.")
