CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = zuma.exe
SRCS = Init.cpp GameModule.cpp FileReader.cpp TerminalUI.cpp Tunnel.cpp RankBoard.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o 2>nul
	del /Q $(TARGET) 2>nul

rebuild: clean all

.PHONY: all clean rebuild
