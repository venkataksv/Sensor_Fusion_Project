cmake_minimum_required(VERSION 3.15.0)

project(sensor_fusion)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Include FetchContent
include(FetchContent)

# Fetch nlohmann json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(main main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main PRIVATE nlohmann_json)