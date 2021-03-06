# Project definition
cmake_minimum_required(VERSION 3.1)
project(GraphKat)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(SOURCES libraries/stb_image.h source/io.h source/messages.h source/ogl/PointModel.cpp source/ogl/PointModel.h source/ogl/ProgramShader.cpp
        source/ogl/ProgramShader.h source/ogl/Shader.cpp source/ogl/Shader.h source/ogl/Plane.cpp source/ogl/Plane.h source/ogl/Texture.h source/ogl/Texture.cpp
        source/main.cpp source/ogl/FontRenderer.h source/ogl/FontRenderer.cpp source/wmgr/WindowLayout.cpp source/wmgr/WindowLayout.h source/wmgr/wsize.cpp source/wmgr/wsize.h
        source/wmgr/LayoutConstraint.cpp source/wmgr/LayoutConstraint.h source/ogl/ShaderLoader.cpp source/ogl/ShaderLoader.h
        source/ogl/UniformType.cpp source/ogl/UniformType.h
        source/nodeprims/Layouts.cpp source/nodeprims/Layouts.h source/nodeprims/Math.cpp source/nodeprims/Math.h
        source/nodeprims/macro_shenanigans.h source/utils/Randoms.cpp source/utils/Randoms.h source/nodeprims/Shaders.h source/testbench.cpp
        source/testbench.h source/utils/BitSet.cpp source/utils/BitSet.h source/utils/BitMap2D.h
        source/utils/std_colors.h source/Foo.cpp source/Foo.h source/Bar.cpp source/Bar.h source/typing/TypeInfo.cpp source/typing/TypeInfo.h source/typing/templates.h
        source/typing/iterator.cpp source/typing/iterator.h source/typing/TypeInfos.h source/ogl/IRenderable.h source/ui/IMouseInteractable.h source/ui/Slider.cpp source/ui/Slider.h source/ui/g_uniforms.h source/ui/IPlanePrimitive.h source/ui/Button.cpp source/ui/Button.h source/ui/Text.cpp source/ui/Text.h source/ui/Marker.cpp source/ui/Marker.h source/ui/FMouseStateMgr.h source/ui/SDF_Renderer.cpp source/ui/SDF_Renderer.h source/ui/Image.cpp source/ui/Image.h source/ui/IKeyboardEvents.h source/ui/IUpdateTick.h source/utils/Timer.h source/inputs/Mouse.cpp source/inputs/Mouse.h source/inputs/Keyboard.cpp source/inputs/Keyboard.h source/utils/oct_tree.cpp source/utils/oct_tree.h)

# Executable definition and properties
set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif (MSVC)

add_library(sse4_1 INTERFACE IMPORTED)
if (MSVC)
    target_compile_options(sse4_1 INTERFACE /arch:SSE4.1)
else ()
    target_compile_options(sse4_1 INTERFACE -msse4.1)
endif ()

#set(CMAKE_REQUIRED_FLAGS "-O2 -std=c++14")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_DIR}/includes")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# SSE4.1 AVX vectorization
target_link_libraries(${PROJECT_NAME} sse4_1)

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS regex)

if (Boost_FOUND)
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
set(GLM_TEST_ENABLE_SIMD_AVX ON)
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

# tiny obj loader
set(TOL_DIR "${LIB_DIR}/tinyobjloader")
#add_subdirectory(${TOL_DIR})
#target_link_libraries(${PROJECT_NAME} tinyobjloader)
target_include_directories(${PROJECT_NAME} PRIVATE "${TOL_DIR}")
