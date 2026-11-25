cmake_minimum_required(VERSION 3.21)

# 如果支持，请为 MSVC 编译器启用热重载
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(keel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- 可执行文件：程序入口 ----
# 以后会有多个目标：keel（服务器）、keel_tests（单元测试）、keel_bench（压测）
add_executable(keel
    "src/main.cpp"
    "src/core/thread_pool.cpp"
)

# 让 #include "core/thread_pool.h" 这种写法能找到 src 目录下的头文件
target_include_directories(keel PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# /W4            严格警告，我们的第一道防线
# /permissive-   关掉 MSVC 的历史宽松模式，按标准来
# /Zc:__cplusplus 让 __cplusplus 说真话（默认它永远报 199711）
# /utf-8         源码和字符串按 UTF-8 处理，中文注释不乱码
target_compile_options(keel PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /Zc:__cplusplus /utf-8>
)
