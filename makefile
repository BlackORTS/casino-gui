INCLUDES = -I./src/raygui/src/ -I ./src/raylib-5.5_macos/include
LIBRARIES = -L./src/raylib-5.5_macos/lib -lraylib

compile:
	gcc prueba.c -o bin $(INCLUDES) $(LIBRARIES) -rpath ./src/raylib-5.5_macos/lib 

clean:
	rm bin
