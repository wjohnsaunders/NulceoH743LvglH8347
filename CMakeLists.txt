cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME LVGL_NUCLEO_STM32H743ZI)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

set(LV_BUILD_CONF_PATH ${CMAKE_SOURCE_DIR}/lv_conf.h)
add_subdirectory(lvgl)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/LvglDemo.cpp
    src/hx8347.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    LV_USE_HX8347=1
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lvgl
)

#
# SREC (S19) generation
#
# Converts the linked ELF into an SREC file using the GNU objcopy from the
# arm-none-eabi toolchain (CMAKE_OBJCOPY is set in cmake/gcc-arm-none-eabi.cmake).
#
# The command is tied to the output file, so it only re-runs when the ELF
# changes (proper incremental builds). Build it with:
#     cmake --build <build-dir> --target srec
#     python cbuild.py --srec
#
set(SREC_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.srec)

add_custom_command(
    OUTPUT ${SREC_OUTPUT}
    COMMAND ${CMAKE_OBJCOPY} -O srec "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>" ${SREC_OUTPUT}
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Generating SREC file ${SREC_OUTPUT} from the ELF"
    VERBATIM
)

add_custom_target(srec DEPENDS ${SREC_OUTPUT})

# Also list the SREC file under 'clean' (append to the existing map-file entry)
get_target_property(_clean_files ${CMAKE_PROJECT_NAME} ADDITIONAL_CLEAN_FILES)
list(APPEND _clean_files ${SREC_OUTPUT})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES "${_clean_files}")

#
# Flashing the target
#
set(CUBE_PROGRAMMER "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe")

add_custom_command(
    OUTPUT ${CMAKE_PROJECT_NAME}_flash
    COMMAND ${CUBE_PROGRAMMER} -c port=SWD -d "${SREC_OUTPUT}" -rst
    DEPENDS ${SREC_OUTPUT}
    COMMENT "Flashing the target with ${SREC_OUTPUT}"
    VERBATIM
)

add_custom_target(flash DEPENDS ${CMAKE_PROJECT_NAME}_flash)
