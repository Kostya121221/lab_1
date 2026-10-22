CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
SOURCES = src/main.cpp src/array.cpp src/singly_list.cpp src/doubly_list.cpp src/stack.cpp src/queue.cpp src/avl_tree.cpp src/storage.cpp
TARGET = dbms

all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard src/*.h)
	@printf '#include <nlohmann/json.hpp>\n' | $(CXX) -std=c++17 -x c++ -fsyntax-only - >/dev/null 2>&1 || { echo "Не найдена nlohmann/json.hpp. Установите: sudo apt install nlohmann-json3-dev"; exit 1; }
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
