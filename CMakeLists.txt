cmake_minimum_required(VERSION 3.0)
SET(CMAKE_TOOLCHAIN_FILE "/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(echo
    VERSION 0.0.0
    DESCRIPTION "简单测试echo grpc"
    LANGUAGES CXX
)


include(${CMAKE_CURRENT_LIST_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions("-std=c++11")
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} 
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/echo.grpc.pb.cc
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/echo.grpc.pb.h
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/echo.pb.cc
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/echo.pb.h
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/serv.cc
)
target_compile_options(${PROJECT_NAME}
    PRIVATE "-static"
)
target_link_libraries(${PROJECT_NAME}
    ${CONAN_LIBS}
)
