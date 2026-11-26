cmake_minimum_required(VERSION 3.6)

project(DocumentScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DocumentScanner.cpp)
set(TARGET_NAME DocumentScanner)

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

if(WIN32)
    # CMake links the SDK by explicit full path. Skip the #pragma comment(lib, ...) entries in
    # the sources: their relative paths only resolve when the linker's working directory matches
    # the vcxproj layout, which is not the case for CMake build directories.
    add_compile_definitions(DCV_NO_PRAGMA_LINK)
    # Windows SDK import libraries and DLLs carry an architecture suffix (...x64 / ...x86).
    # AUTO follows the build bitness (CMAKE_SIZEOF_VOID_P); pass -DTARGET_ARCH=X64|X86 to override.
    set(TARGET_ARCH "AUTO" CACHE STRING "Target CPU architecture of the SDK libraries: AUTO|X64|X86")
    if(TARGET_ARCH STREQUAL "AUTO")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(TARGET_ARCH "X64")
        else()
            set(TARGET_ARCH "X86")
        endif()
    endif()
    if(TARGET_ARCH STREQUAL "X64")
        set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Windows/x64)
        set(DS_LIB_SUFFIX "x64")
    elseif(TARGET_ARCH STREQUAL "X86")
        set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Windows/x86)
        set(DS_LIB_SUFFIX "x86")
    else()
        message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or X86)")
    endif()
    if(NOT EXISTS "${DS_LIB_PATH}")
        message(FATAL_ERROR "Dynamsoft Capture Vision libraries for ${TARGET_ARCH} not found at ${DS_LIB_PATH}")
    endif()
    set(DS_LIB_PREFIX "${DS_LIB_PATH}/")
    set(DS_LIB_EXTENSION ".lib")
    # No rpath on Windows: DLLs load from the executable's folder, so the post-build
    # step copies the SDK DLLs and runtime assets next to the executable.
    file(GLOB DS_RUNTIME_DLLS "${DS_LIB_PATH}/*.dll")
elseif(APPLE)
    set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Mac/universal2)
    set(DS_LIB_SUFFIX "")
    if(NOT EXISTS "${DS_LIB_PATH}")
        message(FATAL_ERROR "Dynamsoft Capture Vision libraries not found at ${DS_LIB_PATH}")
    endif()
else()
    # Pick the SDK libraries that match the target CPU architecture. AUTO derives the
    # architecture from the compiler target; pass -DTARGET_ARCH=X64|ARM64 when
    # cross-compiling for an architecture other than the build host's.
    set(TARGET_ARCH "AUTO" CACHE STRING "Target CPU architecture of the SDK libraries: AUTO|X64|ARM64")
    if(TARGET_ARCH STREQUAL "AUTO")
        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYS_PROC)
        if(SYS_PROC MATCHES "aarch64|arm64")
            set(TARGET_ARCH "ARM64")
        elseif(SYS_PROC MATCHES "arm")
            message(FATAL_ERROR "32-bit ARM (${CMAKE_SYSTEM_PROCESSOR}) is not supported: the SDK only ships x64 and ARM64 libraries.")
        else()
            set(TARGET_ARCH "X64")
        endif()
    endif()
    if(TARGET_ARCH STREQUAL "X64")
        set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Linux/x64)
    elseif(TARGET_ARCH STREQUAL "ARM64")
        set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Linux/ARM64)
    else()
        message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or ARM64)")
    endif()
    if(NOT EXISTS "${DS_LIB_PATH}")
        message(FATAL_ERROR "Dynamsoft Capture Vision libraries for ${TARGET_ARCH} not found at ${DS_LIB_PATH}")
    endif()
    set(DS_LIB_SUFFIX "")
endif()

set(DS_LIBS
    DynamsoftCaptureVisionRouter${DS_LIB_SUFFIX}
    DynamsoftCore${DS_LIB_SUFFIX}
    DynamsoftUtility${DS_LIB_SUFFIX}
    DynamsoftLicense${DS_LIB_SUFFIX}
)

set(DS_TEMPLATE_FILE ${ROOT_DIR}/Dist/Templates/DDN-PresetTemplates.json)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})
target_link_libraries(${TARGET_NAME} ${DS_LIBS})

if(NOT WIN32)
    if(APPLE)
        target_link_options(${TARGET_NAME} PRIVATE
            -O2
            -fvisibility=hidden
            -fvisibility-inlines-hidden
            -Wl,-rpath,${DS_LIB_PATH}
            -Wl,-rpath,@loader_path
            -Wl,-dead_strip)
        set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};@loader_path")
    else()
        target_link_options(${TARGET_NAME} PRIVATE
            -O2
            -fvisibility=hidden
            -fvisibility-inlines-hidden
            -Wl,-rpath,${DS_LIB_PATH}
            -Wl,-rpath,$ORIGIN
            -static-libgcc
            -static-libstdc++
            -s)
        set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};$ORIGIN")
    endif()
endif()

if(EXISTS "${DS_TEMPLATE_FILE}")
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DS_LIB_PATH}/Templates
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DS_TEMPLATE_FILE} ${DS_LIB_PATH}/Templates
        COMMENT "Copying DDN preset templates into Dist"
        VERBATIM
    )
endif()

if(WIN32)
    if(DS_RUNTIME_DLLS)
        add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DS_RUNTIME_DLLS} "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copying SDK DLLs next to the executable"
            VERBATIM
        )
    endif()
endif()
