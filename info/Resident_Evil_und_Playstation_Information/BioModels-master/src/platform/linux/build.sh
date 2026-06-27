#!/bin/bash

# Ubuntu packages
# sudo apt install g++ mesa-common-dev libx11-dev libxi-dev

build_path="../../../build/Linux"

lib_sources=(
    "imgui/imgui.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
    "imgui/backends/imgui_impl_xlib.cpp"
    "ImGuizmo/ImGuizmo.cpp"
)

lib_objs=(
    "$build_path/imgui.o"
    "$build_path/imgui_widgets.o"
    "$build_path/imgui_tables.o"
    "$build_path/imgui_draw.o"
    "$build_path/imgui_demo.o"
    "$build_path/imgui_impl_opengl3.o"
    "$build_path/imgui_impl_xlib.o"
    "$build_path/ImGuizmo.o"
)

sources=(
    "./LinuxMain.cpp"
    "../../BioModels.cpp"
    "../../Tim.cpp"
)

includes="-I../../libs -I../../libs/imgui -I../../libs/imgui/backends -I../../libs/ImGuizmo"
flags="-DLINUX -DOGL_RENDER -Wno-delete-incomplete -Wall -Wno-sign-compare"
libs="-lX11 -lGL -lXi"

if [ ! -e  $build_path ]; then
    mkdir $build_path
fi

if [ ! -e  "$build_path/Debug" ]; then
    mkdir "$build_path/Debug"
fi

if [ ! -e  "$build_path/Release" ]; then
    mkdir "$build_path/Release"
fi

# Compile libs
if [ ! -f "$build_path/imgui.o" ]; then
    for filename in "${lib_sources[@]}"; do
        filepath="../../libs/$filename"
        output_file=$(basename "$filename" .cpp)
        g++ -c $filepath -o "$build_path/${output_file}.o" -I../../libs/imgui
    done
fi

g++ $flags -D_DEBUG "${sources[@]}" "${lib_objs[@]}" -o "$build_path/Debug/BioModels.bin" $includes $libs
g++ $flags "${sources[@]}" "${lib_objs[@]}" -o "$build_path/Release/BioModels.bin" $includes $libs
