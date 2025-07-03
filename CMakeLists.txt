cmake_minimum_required(VERSION 3.15)
project(PipeCompute VERSION 0.1 LANGUAGES CXX)

# Используем C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Только стандартные функции, без расширений компилятора

# Пути к заголовкам
include_directories(${PROJECT_SOURCE_DIR}/include)

# Собрать исполняемый файл из src/
file(GLOB_RECURSE PROJECT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Настройка тестов (опционально, будет доработана)
enable_testing()
add_subdirectory(tests)