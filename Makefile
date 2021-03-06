FILES = example1.txt

all:
	g++ -Wall -std=c++0x main.cpp def.cpp maths_funcs.cpp tinyobjloader/tiny_obj_loader.cpp imageloader/stb_image.h imageloader/stb_image.c -o main -lGLEW -lglfw -lGLU -lGL -lm
run:
	./main $(FILES)
run-nvidia:
	optirun ./main $(FILES)
clean:
	rm -rf main
