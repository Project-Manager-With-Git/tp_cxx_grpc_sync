#项目编译环境
cmake_minimum_required (VERSION 3.13)
project (tp_cxx_grpc_sync
    VERSION 0.0.0
    DESCRIPTION ""
    LANGUAGES CXX
)

# 下载外部依赖源码
include(FetchContent)

## 安装grpc
message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
FetchContent_Declare(
    grpc
    # GIT_REPOSITORY https://dev.hszofficial.site:9080/Mirror/grpc.git 
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG        v1.38.1
    TLS_VERIFY     FALSE
)
FetchContent_MakeAvailable(grpc)
# Since FetchContent uses add_subdirectory under the hood, we can use
# the grpc targets directly from this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

## 安装json
message(STATUS "Using nlohmann/json via add_subdirectory (FetchContent).")
FetchContent_Declare(json
  # GIT_REPOSITORY https://dev.hszofficial.site:9080/Mirror/nlohmann-json.git  
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.7.3
  TLS_VERIFY     FALSE
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

## 安装spdlog
message(STATUS "Using spdlog via add_subdirectory (FetchContent).")
FetchContent_Declare(spdlog
  # GIT_REPOSITORY https://dev.hszofficial.site:9080/Mirror/spdlog.git 
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.8.2
  TLS_VERIFY     FALSE
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# 安装argparse
message(STATUS "Using argparse via add_subdirectory (FetchContent).")
FetchContent_Declare(argparse
  #GIT_REPOSITORY https://dev.hszofficial.site:9080/Mirror/argparse.git 
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG        9903a22904fed8176c4a1f69c4b691304b23c78e
  TLS_VERIFY     FALSE
)
FetchContent_GetProperties(argparse)
if(NOT argparse_POPULATED)
  FetchContent_Populate(argparse)
  add_subdirectory(${argparse_SOURCE_DIR} ${argparse_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# include(cmake/common.cmake)

# Proto file
get_filename_component(hw_proto "pbschema/echo.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)


# Generated sources
set(hw_path "${CMAKE_CURRENT_BINARY_DIR}/echo_pb")
set(hw_proto_srcs "${hw_path}/echo.pb.cc")
set(hw_proto_hdrs "${hw_path}/echo.pb.h")
set(hw_grpc_srcs "${hw_path}/echo.grpc.pb.cc")
set(hw_grpc_hdrs "${hw_path}/echo.grpc.pb.h")

add_custom_command(
    OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${hw_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${hw_proto}"
    DEPENDS "${hw_proto}")

include_directories("${hw_path}")


# 编译
add_compile_options(-std=c++17)
set(SRC_DIR 
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# hw_grpc_proto
add_library(hw_grpc_proto STATIC
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs})
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})


add_executable(cpptestgrpc
    hw_grpc_proto
    ${SRC_DIR}/echo.cc
    ${SRC_DIR}/echo_serv.cc
   
)
message("static as False")
    target_link_libraries(cpptestgrpc 
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE spdlog::spdlog
        PRIVATE argparse::argparse
        # PRIVATE gRPC::grpc++_reflection
        # PRIVATE protobuf::libprotobuf
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
    )
# option(TOTAL_STATIC "是否完全编译为静态" OFF)

# if(TOTAL_STATIC)
#     message("static as True")
#     #libaddress_sorting.a
#     # https://github.com/c-ares/c-ares/archive/refs/tags/cares-1_17_1.tar.gz
    
#     find_library(LIBCARES
#         NAMES libcares.a
#         PATHS /usr/local/cares/lib
#     )

#     find_library(LIBSSL
#         NAMES libssl.a
#         PATHS /usr/local/openssl/lib
#     )
#     find_library(LIBCRYPTO
#         NAMES libcrypto.a
#         PATHS /usr/local/openssl/lib
#     )
#     find_library(LIBZ
#         NAMES libz.a
#     )
#     find_library(LIBGPR
#         NAMES libgpr.a
#     )
#     find_library(LIBDL
#         NAMES libdl.a 
#     )
    
#     target_include_directories(cpptestgrpc 
#         PRIVATE "/usr/local/openssl/include"
#         PRIVATE "/usr/local/cares/include"
#         PRIVATE "${SRC_DIR}"
#     )
    
    
#     target_link_libraries(cpptestgrpc 
#         PUBLIC  "-static"
#         PUBLIC  "-static-libgcc"
#         PUBLIC  "-static-libstdc++"
#         PRIVATE nlohmann_json::nlohmann_json
#         PRIVATE spdlog::spdlog
#         PRIVATE argparse::argparse
#         # PRIVATE gRPC::grpc_static
#         # PRIVATE gRPC::grpc++_static
#         # PRIVATE gRPC::grpc++_reflection_static
#         # PRIVATE protobuf::libprotobuf_static
#         # PRIVATE gRPC::grpc++_cronet_static
#         # PRIVATE gRPC::grpc_cronet_static
#         # PRIVATE gRPC::grpc++_error_details_static
#         # PRIVATE gRPC::grpc++_unsecure_static 
#         # PRIVATE gRPC::grpc_unsecure_static
#         # PRIVATE ${LIBSSL}
#         # PRIVATE ${LIBCRYPTO}
#         # PRIVATE ${LIBZ}
#         # PRIVATE ${LIBGPR}
#         # PRIVATE ${LIBDL}
#         # PRIVATE ${LIBCARES}
#     )
# else()
    message("static as False")
    target_link_libraries(cpptestgrpc
        PUBLIC  "-static"
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE spdlog::spdlog
        PRIVATE argparse::argparse
        # PRIVATE gRPC::grpc++_reflection
        # PRIVATE protobuf::libprotobuf
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
    )
    # target_include_directories(cpptestgrpc 
    #     PRIVATE "${SRC_DIR}"
    # )
# endif()

