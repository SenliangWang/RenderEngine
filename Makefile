CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++11
LDFLAGS := -lGL -lGLEW -lglut

TARGET := demo
SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean