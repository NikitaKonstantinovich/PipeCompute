cmake_minimum_required(VERSION 3.15)
project(PipeCompute VERSION 0.1 LANGUAGES CXX)

# — Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# — List all source files explicitly
set(PROJECT_SOURCES
    src/main.cpp
    src/MockThermo.cpp
    src/Pipe.cpp
    src/Bend.cpp
    src/Tee.cpp
)

# — Create the executable target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# — Include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# — (Optional) Tests
enable_testing()
if(EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
else()
    message(STATUS "No tests/CMakeLists.txt found, skipping tests.")
endif()
