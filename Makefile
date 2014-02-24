FILES = combined.obj texture/rusted_texture.bmp texture/rusted_texture.bmp

all:
	g++ -o main main.cpp -I/usr/include/ImageMagick `pkg-config --cflags glfw3` -Wall -std=c++11 -lGLEW -lm -lassimp -lGL -lMagick++ -lMagickCore `pkg-config --static --libs glfw3`
run:
	./main $(FILES)
run-nvidia:
	optirun ./main $(FILES)
clean:
	rm -rf main
