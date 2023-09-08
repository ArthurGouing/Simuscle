# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.22.1)

# Notre projet estim étiqueté hello
project(Simuscle)

# Crée des variables avec les fichiers à compiler
set(SRCS
    main.cpp
    )
    
 #set(HEADERS
 #   hello.h
 #   )

# On indique que l'on veut un exécutable "hello" compilé à partir des fichiers décrits par les variables SRCS et HEADERS
add_executable(exe ${SRCS} ${HEADERS})
