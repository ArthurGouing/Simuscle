# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.22.1)

# Notre projet estim étiqueté hello
project(Simuscle)

# Add imgui library
add_subdirectory(imgui)
# Add GLFW3 library
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW3 REQUIRED glfw3)
# Add OpenGL library
find_package(OpenGL REQUIRED)

set(LIBS imgui GL glfw rt m dl)


# Crée des variables avec les fichiers à compiler
# set(SRCS main.cpp) # Utiliser file(GLOB RESCUE ...) plutot
file(GLOB SRCS *.cpp)

# set(HEADERS ...) # Idem

# Crée l'exécutable apartir des sources et des headers
add_executable(exe ${SRCS} ${HEADERS})

# Indique les librairie à utiliser
target_link_libraries(exe ${LIBS})
# Install
message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")
install(TARGETS exe DESTINATION ${CMAKE_SOURCE_DIR})
