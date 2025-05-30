rc icon\resource.rc
move icon\resource.res obj\icon.res
cl /Iinclude /Fe:tools/map_editor.exe /Fo:obj/ tools/map_editor.cpp tools/base_editor.cpp src/screen.cpp src/menu.cpp src/ui.cpp src/input.cpp src/system.cpp user32.lib obj/icon.res
