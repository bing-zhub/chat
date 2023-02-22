cmake_minimum_required(VERSION 3.0)

project(main)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g) # 添加-g选项

# 配置最终可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/inlcude)
include_directories(${PROJECT_SOURCE_DIR}/inlcude/server)

# 加载子目录
add_subdirectory(src)