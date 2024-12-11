# 编译器设置
CXX = g++
CXXFLAGS = -std=c++14 -Wall -I./qemu -I./util

# 目标可执行文件
TARGET = my_program

# 源文件和对象文件
SRC = main.cpp \
      qemu/qemu_driver.cpp qemu/qemu_monitor_json.cpp qemu/qemu_monitor.cpp \
      util/virBuffer.cpp util/virJson.cpp

OBJ = $(SRC:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标可执行文件
$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ)

# 编译每个源文件为对象文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJ)

# 运行项目
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
