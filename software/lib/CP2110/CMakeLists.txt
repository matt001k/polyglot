add_library(CP2110 STATIC
    cp2110smt/cp2110smt.cpp
    cp2110smt/OsDep.cpp
    cp2110smt/smt.cpp
    cp2110smt/util.cpp)

target_include_directories(CP2110 PUBLIC
    cp2110smt    
    include)

MESSAGE("Source Dir ${CMAKE_SOURCE_DIR}")

#TODO: This will have to change for when accompanying windows
add_library(SLABHIDTOUART STATIC IMPORTED)
set_target_properties(SLABHIDTOUART PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/CP2110/lib/linux/x86_64/libslabhidtouart.a)

add_library(SLABHIDDEVICE STATIC IMPORTED)
set_target_properties(SLABHIDDEVICE PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/CP2110/lib/linux/x86_64/libslabhiddevice.a)

target_link_libraries(CP2110 PUBLIC
    SLABHIDTOUART
    SLABHIDDEVICE
    pthread
    usb-1.0)
target_link_libraries(${PROJECT_EXECUTABLE} CP2110)