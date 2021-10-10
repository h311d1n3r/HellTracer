HELLTRACER_NAME="helltracer"

HELLTRACER_SRC="./src"
HELLTRACER_INC="./headers"
HELLTRACER_OUT="./bin"

CC=g++
COMPILE_FLAGS=-std=c++17

HellTracer:	clean compile

compile:
	mkdir -p $(HELLTRACER_OUT)
	$(CC) $(COMPILE_FLAGS) $(HELLTRACER_SRC)/*.cpp -I$(HELLTRACER_INC) -o$(HELLTRACER_OUT)/$(HELLTRACER_NAME)
			  
clean:
	rm -rf $(HELLTRACER_OUT)
