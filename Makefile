HELLTRACER_NAME="helltracer"

HELLTRACER_SRC="./src"
HELLTRACER_INC="./headers"
HELLTRACER_OUT="./build"

CC=g++
COMPILE_FLAGS=-std=c++17

HellTracer:	clean compile

compile:
	mkdir -p ./build
	$(CC) $(COMPILE_FLAGS) $(HELLTRACER_SRC)/*.cpp -I$(HELLTRACER_INC) -o$(HELLTRACER_OUT)/$(HELLTRACER_NAME)
			  
clean:
	rm -rf ./build
