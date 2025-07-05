cmake_minimum_required(VERSION 3.15)
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2   # или последнюю стабильную версию
)
FetchContent_MakeAvailable(json)
project(PipeCompute VERSION 0.1 LANGUAGES CXX)

# — используем C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# — все .cpp в src (чтобы не забыть про новые файлы)
file(GLOB PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# — создаём исполняемый target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} "src/BendSimulator.cpp")

# После того, как собран exe, скопировать папку configs/ рядом с ним
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/configs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/configs"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/configs"
)

# — подключаем наш include/ для заголовков PipeCompute/...
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# — подключаем nlohmann::json (header-only, через vcpkg или find_package)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    nlohmann_json::nlohmann_json
)

# — опционально: тесты
enable_testing()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
  add_subdirectory(tests)
else()
  message(STATUS "No tests/CMakeLists.txt found, skipping tests.")
endif()
