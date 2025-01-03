cmake_minimum_required(VERSION 3.1.3)
project(speech_server)

set(target "speech_server")

# proto文件路径
set(proto_path ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
# 生成代码路径
set(proto_out_path ${CMAKE_CURRENT_SOURCE_DIR}/proto)
# 可执行文件输出路径
set(bin_path ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 生成目录
file(MAKE_DIRECTORY ${proto_out_path})
file(MAKE_DIRECTORY ${bin_path})

# proto文件
set(proto_files speech.proto)

# 生成proto代码
set(proto_srcs "")

foreach(proto_file ${proto_files})
    string(REPLACE ".proto" ".pb.cc" proto_cc ${proto_file})
    string(REPLACE ".proto" ".pb.ch" proto_h ${proto_file})

    set(proto_cc_path ${proto_out_path}/${proto_cc})
    set(proto_h_path ${proto_out_path}/${proto_h})

    # if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}${proto_cc})
    #     add_custom_command(
    #         PRE_BUILD
    #         COMMAND protoc
    #         ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I${proto_path} ${proto_path}/${proto_file}
    #         DEPENDS ${proto_path}/${proto_file}
    #         OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${proto_cc}
    #         COMMENT "生成Protobuf框架代码文件: " ${CMAKE_CURRENT_BINARY_DIR}/${proto_cc}
    #     )
    #     list(APPEND proto_srcs ${CMAKE_CURRENT_BINARY_DIR}/${proto_cc})
    # endif()
    add_custom_command(
        OUTPUT ${proto_cc_path} ${proto_h_path}
        COMMAND protoc --cpp_out=${proto_out_path} -I${proto_path} ${proto_path}/${proto_file}
        DEPENDS ${proto_path}/${proto_file}
        COMMENT "生成Protobuf框架代码文件: ${proto_cc_path}"
    )
    list(APPEND proto_srcs ${proto_cc_path})
endforeach()

# 查找源文件
set(src_files "")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src_files)
# 添加可执行文件
add_executable(${target} ${src_files} ${proto_srcs})
# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third/aip-cpp-sdk)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third/Xulog/logs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third/cinatra/include)
include_directories(${proto_out_path})


target_link_libraries(${target} -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lleveldb -ldl -letcd-cpp-api -lcpprest -lcurl /usr/lib/x86_64-linux-gnu/libjsoncpp.so.25)
# INSTALL(TARGETS ${target} RUNTIME DESTINATION bin)
set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${bin_path})

