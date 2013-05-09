FILES = Data/stochastic_model_level_0000100.txt Data/stochastic_model_level_0000200.txt Data/stochastic_model_level_0000300.txt Data/stochastic_model_level_0000400.txt Data/stochastic_model_level_0000500.txt Data/stochastic_model_level_0000600.txt Data/stochastic_model_level_0000700.txt Data/stochastic_model_level_0000800.txt Data/stochastic_model_level_0000900.txt

all:
	g++ -Wall -std=c++0x main.cpp def.cpp maths_funcs.cpp -o main -lGLEW -lglfw -lGLU -lGL -lm
run:
	./main $(FILES)
run-nvidia:
	optirun ./main $(FILES)
clean:
	rm -rf main
