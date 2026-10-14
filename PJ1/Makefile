CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = zuma.exe
SRCS = Init.cpp GameModule.cpp FileReader.cpp TerminalUI.cpp Tunnel.cpp RankBoard.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	-del /Q $(OBJS) $(DEPS) $(TARGET) 2>nul

rebuild: clean all

.PHONY: all clean rebuild
