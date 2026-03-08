cmake_minimum_required(VERSION 3.20)
project(sentinelid-server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.18.3
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(httplib json)

add_library(sentinelid-server
  src/client.cpp
)

target_include_directories(sentinelid-server PUBLIC include)
target_link_libraries(sentinelid-server PUBLIC httplib::httplib nlohmann_json::nlohmann_json)

if(WIN32)
  target_link_libraries(sentinelid-server PUBLIC ws2_32 crypt32)
endif()
