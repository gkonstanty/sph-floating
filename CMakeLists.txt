cmake_minimum_required (VERSION 2.6)

project(sph)
file(GLOB SRCFILES *.cpp)
file(GLOB LIB_FILES ./glvu/*.cpp)
ADD_EXECUTABLE(run_sph ${SRCFILES} ${LIB_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


include_directories("./glvu")
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(run_sph ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
