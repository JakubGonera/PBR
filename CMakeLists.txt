cmake_minimum_required(VERSION 3.15)

#macro for debug printing all variables
macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

project(Graphics3D CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

set(3rdParty ${PROJECT_SOURCE_DIR}/src/3rdParty)
include_directories(${3rdParty})
include_directories(${PROJECT_SOURCE_DIR}/src)

# GLAD
include_directories(${3rdParty}/glad/include)
file(GLOB_RECURSE glad_SOURCES "${3rdParty}/glad/src/*.c")
add_library(glad OBJECT ${glad_SOURCES})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${3rdParty}/glfw)

# GLFW
add_subdirectory(${3rdParty}/glm)

find_package(OpenGL REQUIRED)

link_libraries(OpenGL::GL glfw glm)
add_subdirectory(src/Application)

# ImGui
add_subdirectory(${3rdParty}/imgui)

#Examples

link_libraries(application)
link_libraries(imgui)
add_subdirectory(src/Examples/Triangle)
add_subdirectory(src/Examples/Perspective)
add_subdirectory(src/Examples/CameraMovement)
add_subdirectory(src/Examples/PhongShading)