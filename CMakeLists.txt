cmake_minimum_required(VERSION 3.15)
project(PipeCompute VERSION 0.1 LANGUAGES CXX)

# Используем C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Только стандартные функции, без расширений компилятора

# Пути к заголовкам
include_directories(${PROJECT_SOURCE_DIR}/include)

# Собрать исполняемый файл из src/
# Глобальный поиск всех .cpp в src/
file(GLOB_RECURSE PROJECT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Если есть .cpp файлы, создаём таргет, иначе предупреждаем
if(PROJECT_SOURCES)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
else()
    message(WARNING "No source files found in src/, please add at least one .cpp stub (e.g., main.cpp) to src/.")
endif()

# Настройка тестов (опционально, будет доработана) (опционально, будет доработана)
# Проверка на существование CMakeLists.txt в tests
if(EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
else()
    message(WARNING "Directory tests/ exists but no CMakeLists.txt found. Skipping tests.")
endif()