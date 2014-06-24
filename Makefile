# PLY_FILE OBJ_FILE WEATHERING_DEGREE_FILE WEATHERING_MAP COLOR_MAP DISPLACEMENT_MAP NORMAL_MAP
FILES = surface_3.ply surface_2.obj surface_2.dat texture/weathering_degree_0.dat texture/Texture_1_diffuse_small.jpg texture/Maps_DISP.png texture/Normal_0.png 

all:
	g++ -o main main.cpp -I/usr/include/ImageMagick `pkg-config --cflags glfw3` -Wall -std=c++11 -lGLEW -lm -lassimp -lGL -lMagick++ -lMagickCore `pkg-config --static --libs glfw3`
run:
	./main $(FILES)
run-nvidia:
	optirun ./main $(FILES)
clean:
	rm -rf main
