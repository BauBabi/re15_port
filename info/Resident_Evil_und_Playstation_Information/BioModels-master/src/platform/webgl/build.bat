@echo off

SET build_path=..\..\..\build\WebGL

if not exist %build_path% (
    mkdir %build_path%
)

SET sources=WebGLMain.cpp ..\..\BioModels.cpp ..\..\Tim.cpp ..\..\libs\imgui\backends\imgui_impl_opengl3.cpp ..\..\libs\imgui\backends\imgui_impl_glfw.cpp  ..\..\libs\imgui\imgui.cpp ..\..\libs\imgui\imgui_widgets.cpp  ..\..\libs\imgui\imgui_tables.cpp ..\..\libs\imgui\imgui_draw.cpp  ..\..\libs\imgui\imgui_demo.cpp ..\..\libs\ImGuizmo\ImGuizmo.cpp
SET includes=-I..\..\libs -I..\..\libs\imgui -I..\..\libs\imgui\backends -I..\..\libs\ImGuizmo
SET flags=-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_GLFW=3 -D_DEBUG -sEXPORTED_FUNCTIONS="['_main', '_JSLoadModel', '_malloc']" -sEXPORTED_RUNTIME_METHODS=ccall,writeArrayToMemory -Wno-delete-incomplete
SET preloads=--preload-file ..\..\..\DroidSans.ttf@DroidSans.ttf --preload-file ..\..\..\fa-solid-900.ttf@fa-solid-900.ttf --preload-file ..\..\..\data\Pl0\emd0\em050.emd@em050.emd --preload-file ..\..\..\data\Pl0\emd0\em050.TIM@em050.TIM

em++ -O2 %sources% -o %build_path%\index.html --shell-file=shell.html %flags% %preloads% %includes%