CXXFLAGS=-Wall -std=c++11 -g `sdl2-config --cflags`
OBJS=maze.o map.o makemaze.o font.o
TARGET=./maze

$(TARGET):  $(OBJS)
	$(CXX) $(OBJS) `sdl2-config --libs` -o $(TARGET)

clean:
	$(RM) *.o $(TARGET)

.PHONY: clean
