CXXFLAGS = -std=c++11 -Wall -O3 -g
CXX      = g++

SOURCES := $(wildcard ./main.cpp)
OBJECTS := $(SOURCES:%.cpp=%.o)

INCLUDES = -I../../include -I./
LDFLAGS  = -static-libgcc -static-libstdc++

TARGET   = ./json_codec
all: $(TARGET)
$(TARGET): ${OBJECTS}
	$(CXX) $(LDFLAGS) -o $@ $^
	
$(OBJECTS) : %.o : %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f ${OBJECTS} $(TARGET)