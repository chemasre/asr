rc icon\resource.rc
move icon\resource.res obj\icon.res
cl /Iinclude /Fe:bin/main.exe /Fo:obj/ src/*.cpp user32.lib lib/portaudio_x86.lib obj/icon.res