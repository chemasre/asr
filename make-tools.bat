cl /Iinclude /Fe:tools/bmp2texture.exe /Fo:obj/ tools/bmp2texture.cpp tools/cbmp/cbmp.c user32.lib
cl /Iinclude /Fe:tools/sprite_editor.exe /Fo:obj/ tools/sprite_editor.cpp src/screen.cpp src/menu.cpp src/ui.cpp src/input.cpp src/system.cpp user32.lib
