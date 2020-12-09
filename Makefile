
GL_LIBS = `pkg-config --static --libs glfw3 glew` -std=gnu++11
EXT =
CPPFLAGS = `pkg-config --cflags glfw3` -std=gnu++11

CC = g++
EXE = assign3_part2
SHADER_PATH = external_files/Shader.cpp

CPPFILES = $(wildcard *.cpp)
OBJS = $(CPPFILES:%.cpp=%.o)
OBJS += Shader.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS) $(SHADER)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

Shader.o:
	$(CC) $(CPPFLAGS) -c $(SHADER_PATH)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $<

clean:
	rm -f *.o $(EXE)$(EXT)
