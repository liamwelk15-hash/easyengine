cmake_minimum_required(VERSION 3.15)
project(EasyEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/engine/include)

set(ENGINE_SOURCES
    engine/source/EngineCore.cpp
    engine/source/EasyScript.cpp
    engine/source/Rendering.cpp
    engine/source/Physics.cpp
)

add_library(EasyEngine SHARED ${ENGINE_SOURCES})

target_compile_definitions(EasyEngine PRIVATE ENGINE_EXPORTS)

if(WIN32)
    target_link_libraries(EasyEngine winmm.lib)
endif()

add_executable(GameExample game/main.cpp)
target_link_libraries(GameExample EasyEngine)

install(TARGETS EasyEngine LIBRARY DESTINATION bin)
