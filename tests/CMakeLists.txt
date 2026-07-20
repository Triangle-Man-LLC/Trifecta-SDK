cmake_minimum_required(VERSION 3.10)
project(TrifectaDriverTests C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Force full debug info for everything (tests + driver)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# Ensure debug symbols are generated
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

# AddressSanitizer flags (compile + link)
set(SAN_FLAGS
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
)

enable_testing()

# Bring in the actual Trifecta-Driver module
# IMPORTANT: do this AFTER setting debug flags
add_subdirectory(${CMAKE_SOURCE_DIR}/../ TrifectaDriver_build)

# Copy test CSV resources
file(GLOB TEST_CSVS "${CMAKE_SOURCE_DIR}/test_resources/*.csv")
foreach(csv ${TEST_CSVS})
    get_filename_component(name "${csv}" NAME)
    configure_file("${csv}" "${CMAKE_BINARY_DIR}/test_resources/${name}" COPYONLY)
endforeach()

# Collect all test .c files
file(GLOB TEST_SOURCES *.c)
message(STATUS "Test sources: ${TEST_SOURCES}")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME}
        PRIVATE
            DriverTrifecta
            m
    )

    # Apply sanitizer + debug flags to tests
    target_compile_options(${TEST_NAME} PRIVATE ${SAN_FLAGS})
    target_link_options(${TEST_NAME} PRIVATE ${SAN_FLAGS})

    add_test(
        NAME ${TEST_NAME}
        COMMAND /usr/bin/time -v $<TARGET_FILE:${TEST_NAME}>
    )
endforeach()
