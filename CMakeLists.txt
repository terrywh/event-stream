cmake_minimum_required(VERSION 3.13)

# -----------------------------------------------------------------------------
set(VENDOR_BOOST /data/vendor/boost-1.69.0)
set(VENDOR_JSON /data/vendor/json-3.5.0)
set(VENDOR_PARSER /data/vendor/parser-1.0.0)
# -----------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_C_COMPILER)
#    set(CMAKE_C_COMPILER /data/vendor/gcc-8.2.0/bin/gcc)
    set(CMAKE_C_COMPILER /data/vendor/llvm-7.0.1/bin/clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
#    set( /data/vendor/gcc-8.2.0/bin/g++)
    set(CMAKE_CXX_COMPILER /data/vendor/llvm-7.0.1/bin/clang++)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED 17)
# set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# -----------------------------------------------------------------------------
project(commas-event-source)

aux_source_directory(./src SOURCES)
add_executable(event-source ${SOURCES})

# -----------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(event-source BEFORE
        PUBLIC "-std=c++17"
        PUBLIC "--gcc-toolchain=/data/vendor/gcc-8.2.0"
		PUBLIC "-fcolor-diagnostics")
else()
    target_compile_options(event-source BEFORE
        PUBLIC "-std=c++17"
		PUBLIC "-fdiagnostics-color=always")
endif()


# -----------------------------------------------------------------------------
set_target_properties(event-source PROPERTIES
    LINK_FLAGS "-static-libstdc++"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
target_include_directories(event-source SYSTEM
    PUBLIC ${VENDOR_BOOST}/include
    PUBLIC ${VENDOR_JSON}/include
    PUBLIC ${VENDOR_PARSER}/include)
target_link_directories(event-source
	PRIVATE ${VENDOR_BOOST}/lib
)
target_link_libraries(event-source
	libboost_program_options.a
    libboost_log.a
    libboost_filesystem.a
    libboost_system.a
    libboost_coroutine.a
    libboost_context.a
    libboost_thread.a
    libssl.a
    libcrypto.a
    pthread)
