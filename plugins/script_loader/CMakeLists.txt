cmake_minimum_required(VERSION 3.16)

# Change this to the name of your plugin #
project(script_loader)

# Add your source files here #

find_package(CoreClrEmbed)

add_library(${PROJECT_NAME} SHARED
        source/plugin_script_loader.cpp
)
# Add additional include directories here #
target_include_directories(${PROJECT_NAME} PRIVATE include)
# Add additional libraries here #
target_link_libraries(${PROJECT_NAME} PRIVATE libimhex ${FMT_LIBRARIES})

if (CoreClrEmbed_FOUND)
    add_library(nethost SHARED IMPORTED)
    target_include_directories(nethost INTERFACE "${CoreClrEmbed_INCLUDE_DIRS}")
    get_filename_component(CoreClrEmbed_FOLDER ${CoreClrEmbed_SHARED_LIBRARIES} DIRECTORY)
    set_target_properties(nethost
            PROPERTIES
            IMPORTED_IMPLIB     ${CoreClrEmbed_SHARED_LIBRARIES}
            IMPORTED_LOCATION   ${CoreClrEmbed_LIBRARIES}
            BUILD_RPATH         ${CoreClrEmbed_FOLDER}
            INSTALL_RPATH       ${CoreClrEmbed_FOLDER})

    target_link_directories(${PROJECT_NAME} PRIVATE ${CoreClrEmbed_FOLDER})
    target_include_directories(${PROJECT_NAME} PRIVATE ${CoreClrEmbed_INCLUDE_DIRS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE DOTNET_PLUGINS=1)
    target_sources(${PROJECT_NAME} PRIVATE
            source/loaders/dotnet/dotnet_loader.cpp

            source/script_api/v1/mem.cpp
            source/script_api/v1/bookmarks.cpp
            source/script_api/v1/ui.cpp
    )

    set(EXTRA_BUNDLE_LIBRARY_PATHS "${CoreClrEmbed_FOLDER}" PARENT_SCOPE)

    if (IMHEX_BUNDLE_DOTNET)
        install(FILES ${CoreClrEmbed_SHARED_LIBRARIES} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif ()

    add_subdirectory(dotnet)
    add_dependencies(${PROJECT_NAME} AssemblyLoader)

endif ()


# ---- No need to change anything from here downwards unless you know what you're doing ---- #

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".hexplug")

target_compile_definitions(${PROJECT_NAME} PRIVATE IMHEX_PROJECT_NAME="${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME} PRIVATE IMHEX_VERSION="${IMHEX_VERSION_STRING}")
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
setupCompilerFlags(${PROJECT_NAME})

set(LIBROMFS_RESOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/romfs)
set(LIBROMFS_PROJECT_NAME ${PROJECT_NAME})
add_subdirectory(../../lib/external/libromfs ${CMAKE_CURRENT_BINARY_DIR}/libromfs)
set_target_properties(${LIBROMFS_LIBRARY} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBROMFS_LIBRARY})