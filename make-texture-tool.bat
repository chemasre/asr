rc icon\resource.rc
move icon\resource.res obj\icon.res
cl /Iinclude /Fe:tools/bmp2texture.exe /Fo:obj/ tools/bmp2texture.cpp tools/cbmp/cbmp.c user32.lib obj/icon.res
