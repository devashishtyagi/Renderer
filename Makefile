all:
	g++ -Wall -std=c++0x main.cpp def.cpp maths_funcs.cpp -o main -lGLEW -lglfw -lGLU -lGL -lm
run:
	./main
clean:
	rm -rf main
