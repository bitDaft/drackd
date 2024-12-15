cmake_minimum_required(VERSION 3.10)
project(drackd VERSION 1.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable debug and release configurations
set(CMAKE_BUILD_TYPE Debug) # Default to Debug if not specified

# Define build types
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG_BUILD)
  add_definitions(-D_DEBUG_)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DRELEASE_BUILD)
  add_definitions(-D_RELEASE_)
endif()

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/nlohmann)

# Specify output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the JSON library (assuming nlohmann's JSON is header-only)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/nlohmann)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

target_link_libraries(${PROJECT_NAME}
  pthread
  stdc++fs
)

find_package(X11 REQUIRED)

if(X11_FOUND)
  include_directories(${X11_INCLUDE_DIR})
  target_link_libraries(drackd ${X11_LIBRARIES})
endif()