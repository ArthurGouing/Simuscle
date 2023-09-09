# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.22.1)

# Notre projet est étiqueté libhello
project(libimgui)

add_subdirectory(backends)

# Crée des variables avec les fichiers à compiler
set(SRCS
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp # Delete
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
    )
    
set(HEADERS
    imconfig.h
    imgui.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_impl_glfw.h
    imgui_impl_opengl3.h
    )

add_library(imgui ${SRCS} ${HEADERS})

target_include_directories(imgui PUBLIC .)
