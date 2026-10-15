cmake_minimum_required(VERSION 3.6)

project(GS1AIScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/GS1AIScanner.cpp)
set(TARGET_NAME GS1AIScanner)

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

set(DS_LIB_PATH ${ROOT_DIR}/Dist/Lib/Linux/x64)
set(DS_LIBS DynamsoftCaptureVisionRouter DynamsoftCore DynamsoftUtility DynamsoftLicense)
link_directories(${DS_LIB_PATH})

set(DCPRES_PATH ${ROOT_DIR}/Dist/ParserResources)
set(DS_JSON_PATH ${ROOT_DIR}/Dist/Templates)
set(DS_MODELS_PATH ${ROOT_DIR}/Dist/Models)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L${DS_LIB_PATH} -Wl,-rpath,${DS_LIB_PATH} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_link_libraries(${TARGET_NAME} stdc++ ${DS_LIBS})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};./")

add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DCPRES_PATH} ${DS_LIB_PATH}/ParserResources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DS_JSON_PATH} ${DS_LIB_PATH}/Templates
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DS_MODELS_PATH} ${DS_LIB_PATH}/Models
)

add_custom_target(my_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_NAME} GS1AIScanner.o
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/ParserResources
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/Templates
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/Models
)