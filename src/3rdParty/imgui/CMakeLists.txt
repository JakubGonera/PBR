cmake_minimum_required(VERSION 3.15)
project(imgui CXX)

file(GLOB imgui_SOURCES "${PROJECT_SOURCE_DIR}/*.h")
file(GLOB imgui_SOURCES_CPP "${PROJECT_SOURCE_DIR}/*.cpp")
add_library(${PROJECT_NAME} ${imgui_SOURCES} ${imgui_SOURCES_CPP})