cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("opengl-hw2" CXX)

add_executable("scene"
				"types.cpp"
				"app.cpp")

add_subdirectory("glwrap")
link_directories(${opengl-hw2_SOURCE_DIR}/glwrap/)
target_link_libraries(scene 
						glwrap glfw3 gdi32 opengl32 glew32 png z)

add_definitions("-std=c++11")