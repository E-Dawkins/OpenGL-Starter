cmake_minimum_required(VERSION 3.16)

# change project name here
project(open-gl-starter)

# find all header / source files for project
add_subdirectory(deps)
include_directories(inc)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS inc/*.h)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE PROJECT_SHADERS CONFIGURE_DEPENDS assets/*.vert assets/*.frag)

# create project
add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${PROJECT_SHADERS})

# create filter for all shaders (file path not necessary)
source_group("_shaders" FILES ${PROJECT_SHADERS})

# Create filters based on file paths (*.h *.cpp)
foreach(_file IN LISTS PROJECT_HEADERS PROJECT_SOURCES)
    # Get relative path from project root
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${_file}")

    # Extract directory and convert to filter format
    get_filename_component(_dir "${_relPath}" DIRECTORY)
    string(REPLACE "/" "\\" _filter "${_dir}")

    # Assign to filter
    source_group("${_filter}" FILES "${_file}")
endforeach()

# link any dependencies to project
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm stb_image tinyobjloader)

# set project to use c++ 20
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# set warning level 4 and treat warnings as errors
target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)

# enable precompiled headers (Visual Studio only)
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

if (MSVC)
    # set external includes on and to have warnings disabled
    target_compile_options(${PROJECT_NAME} PRIVATE /external:anglebrackets /external:W0)

    # set project as vs startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    # set working directory to cmake source directory
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    # enable use of pch and force include it
    target_compile_options(${PROJECT_NAME} PRIVATE /Yu"pch.h" /FI"pch.h")
endif()

# this command will copy assets into bin/assets after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)