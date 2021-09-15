# set the cmake require
cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Scanner VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ScannerConfig.h.in ScannerConfig.h)

file (GLOB_RECURSE SRC_SET "src/*.cpp" "src/*.h" "src/*.hpp" "src/*.c")

add_subdirectory(deps/glad)
list(APPEND EXTRA_LIBS glad)
list(APPEND EXTRA_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-3.3.4/lib/glfw3.lib")
list(APPEND EXTRA_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include")
list(APPEND EXTRA_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-3.3.4/include")
list(APPEND EXTRA_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm-0.9.8.5/include")
list(APPEND EXTRA_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/deps/opencv-4.5.3/include")

set (OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/opencv-4.5.3")
find_package(OpenCV REQUIRED)

add_executable(Scanner ${SRC_SET})
message("${EXTRA_LIBS}")
target_link_libraries(
    Scanner
    PUBLIC
    ${EXTRA_LIBS}
    ${OpenCV_LIBS}
)

target_include_directories(
    Scanner
    PUBLIC
    "${PROJECT_BINARY_DIR}"
    ${EXTRA_INCLUDE}
)

