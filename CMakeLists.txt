cmake_minimum_required(VERSION 3.15)
project(Flexer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM first
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find Clang
find_package(Clang REQUIRED CONFIG)

# Add LLVM definitions and include directories
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

add_executable(flexer src/main.cpp)

# Link against both clang-cpp and LLVM
target_link_libraries(flexer
  PRIVATE
    clang-cpp
    LLVM
)
