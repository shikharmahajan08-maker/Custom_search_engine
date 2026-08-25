# ==============================================================================
# CMakeLists.txt — Custom Search Engine Build Configuration
# ==============================================================================
# Build system for the Custom Search Engine project.
# Requires CMake 3.16+ and a C++17-compliant compiler.
#
# Quick build:
#   mkdir build && cd build
#   cmake .. && cmake --build .
#
# Or without CMake (single g++ command):
#   g++ -std=c++17 -O2 -Wall -Wextra -I include src/*.cpp -o search_engine
# ==============================================================================

cmake_minimum_required(VERSION 3.16)

# Project metadata
project(
    CustomSearchEngine
    VERSION 1.0.0
    DESCRIPTION "High-performance TF-IDF search engine in C++17"
    LANGUAGES CXX
)

# ==============================================================================
# Compiler Configuration
# ==============================================================================

# Require C++17 standard (mandatory — we use string_view, structured bindings,
# if-constexpr, and other C++17 features throughout the codebase)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)    # Disable compiler-specific extensions

# ==============================================================================
# Source Files
# ==============================================================================

# Explicitly list all source files (prefer over globbing for reliability)
set(SOURCES
    src/main.cpp
    src/document.cpp
    src/tokenizer.cpp
    src/inverted_index.cpp
    src/tfidf_ranker.cpp
    src/search_engine.cpp
    src/stemmer.cpp
)

# ==============================================================================
# Executable Target
# ==============================================================================

add_executable(search_engine ${SOURCES})

# Add the include directory to the header search path
target_include_directories(search_engine PRIVATE include)

# ==============================================================================
# Compiler Warnings (catch bugs early)
# ==============================================================================

# Enable comprehensive warnings for all compilers
target_compile_options(search_engine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion>
)

# ==============================================================================
# Optimization for Release builds
# ==============================================================================

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Release-mode flags
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
