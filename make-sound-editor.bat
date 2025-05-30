rc icon\resource.rc
move icon\resource.res obj\icon.res
cl /Iinclude /Fe:tools/sound_editor.exe /Fo:obj/ tools/sound_editor.cpp tools/base_editor.cpp src/screen.cpp src/log.cpp src/sound.cpp src/menu.cpp src/ui.cpp src/input.cpp src/system.cpp user32.lib lib/portaudio_x86.lib obj/icon.res
