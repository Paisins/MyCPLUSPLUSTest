# -------------------------------- configuration --------------------------------
cmake_minimum_required(VERSION 3.26.0)

project(MyTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE vcpkg/scripts/buildsystems/vcpkg.cmake)

add_executable(${PROJECT_NAME} main.cpp)

# -------------------------------- generation --------------------------------

# ---- libraries installed from vcpkg ----

# add path to serach config.cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-osx)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows)

    # set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows-static)
endif()

# todo not work
# if(MSVC)
# message("test test")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
# endif()

# jsoncpp
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::sqlite3::sqlite3)

# ---- my own libraries from outside ----
# my_print
set(LIBRARY_PATH "${CMAKE_SOURCE_DIR}/libs")
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBRARY_PATH})
message("LIBRARY_PATH: ${LIBRARY_PATH}")
find_library(MYPRINT_LIB MyPrint PATHS ${LIBRARY_PATH})

if(MYPRINT_LIB)
    message("use myprint")
    set(USE_MYPRINT "use myprint lib or not" On)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MYPRINT_LIB})
else()
    set(USE_MYPRINT "use myprint lib or not" off)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "USE_MYPRINT")

# -------------------------------- build --------------------------------

# copy files to build path
# if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
# # this means to move pylib to build path,
# # but it seems like target can find pylib anyway, so it's not neccessary
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
# COMMAND_EXPAND_LISTS
# )
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # copy files to build path
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()

# -------------------------------- install --------------------------------

# my install path
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/install)

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# install libraries build inside this project
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# install libraries download from vcpkg
# and finally!!!
install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# install libraries copied outside of this project
set(SHRAED_FILE_EXTENSIONS dll)
set(SHRAED_LIB_FILES "")

foreach(EXT ${SHRAED_FILE_EXTENSIONS})
    file(GLOB FILES "${CMAKE_SOURCE_DIR}/libs/*.${EXT}")
    list(APPEND SHRAED_LIB_FILES ${FILES})
endforeach()

set(H_FILE_EXTENSIONS h lib dylib)
set(H_LIB_FILES "")

foreach(EXT ${H_FILE_EXTENSIONS})
    file(GLOB FILES "${CMAKE_SOURCE_DIR}/libs/*.${EXT}")
    list(APPEND H_LIB_FILES ${FILES})
endforeach()

message(STATUS "H_LIB_FILES: ${H_LIB_FILES}")
message(STATUS "SHRAED_LIB_FILES: ${SHRAED_LIB_FILES}")

# copy files to bin
install(FILES ${SHRAED_LIB_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# copy files to lib
install(FILES ${H_LIB_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# add dynamic files search path after install
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "@executable_path/../lib")

    # elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # todo when move install to another windows system, it can't work, I guess because it can't find dll
    # todo but I already put dll with target in same directory, so why?
    # maybe it is about runtime libraries
    # install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E env PATH=\"${CMAKE_INSTALL_PREFIX}/lib;$ENV{PATH}\" ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe)")
endif()
