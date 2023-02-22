cmake_minimum_required(VERSION 3.0)

project(main)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g) # 添加-g选项

# 配置头文件搜索路径
# include_directories()

# 配置库文件搜索路径
# link_directories()

# 设置需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)

# 设置可执行文件存放位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 把指定路径下所有源文件放入变量名SRC_LIST里面
# aux_source_directory(. SRC_LIST)

# 通过编译SRC_LIST定义的源文件编译 可以生成可执行文件server
add_executable(server ${SRC_LIST})

# server目标程序, 需要muduo_net muduo_base pthread三个库文件
target_link_libraries(server muduo_net muduo_base pthread)