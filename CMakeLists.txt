# Project definition
cmake_minimum_required(VERSION 3.1)
project(GraphKat)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(SOURCES libraries/stb_image.h source/io.h source/messages.h source/ogl/PointModel.cpp source/ogl/PointModel.h source/ogl/ProgramShader.cpp
        source/ogl/ProgramShader.h source/ogl/Shader.cpp source/ogl/Shader.h source/ogl/Plane.cpp source/ogl/Plane.h source/ogl/Texture.h source/ogl/Texture.cpp
        source/main.cpp source/ogl/FontRenderer.h source/ogl/FontRenderer.cpp source/wmgr/LayoutManager.cpp source/wmgr/LayoutManager.h source/wmgr/wsize.cpp source/wmgr/wsize.h source/wmgr/LayoutConstraint.cpp source/wmgr/LayoutConstraint.h source/ogl/ShaderLoader.cpp source/ogl/ShaderLoader.h source/dataflow/Context.cpp source/dataflow/Context.h source/dataflow/ContextPath.cpp source/dataflow/ContextPath.h source/dataflow/Endpoint.cpp source/dataflow/Endpoint.h source/ogl/UniformType.cpp source/ogl/UniformType.h source/dataflow/ComputeNode.cpp source/dataflow/ComputeNode.h source/nodeprims/Layouts.cpp source/nodeprims/Layouts.h source/nodeprims/Math.cpp source/nodeprims/Math.h source/nodeprims/macro_shenanigans.h source/utils/Randoms.cpp source/utils/Randoms.h source/nodeprims/Shaders.cpp source/nodeprims/Shaders.h)

# Executable definition and properties
set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif (MSVC)
#set(CMAKE_REQUIRED_FLAGS "-O2 -std=c++14")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_DIR}/includes")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS regex)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# glm
set(GLM_DIR "${LIB_DIR}/glm")
set(GLM_TEST_ENABLE OFF)
add_subdirectory("${GLM_DIR}")
target_link_libraries(${PROJECT_NAME} "glm" "${GLM_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}/include")

# freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})

# pugixml
set(PUGIXML_DIR "${LIB_DIR}/pugixml")
add_subdirectory(${PUGIXML_DIR})
target_link_libraries(${PROJECT_NAME} pugixml)
