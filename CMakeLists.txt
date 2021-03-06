cmake_minimum_required(VERSION 3.7)

Set(PROJNAME DogeVk)
Project(DogeVk)

Message(STATUS "-------------------------------")
Message(STATUS "Processing Project ${PROJNAME}:")

Message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

SET(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

Message("BASE_DIRECTORY: ${BASE_DIRECTORY}")

find_package(glm CONFIG REQUIRED)

Message("glm_FOUND: ${glm_FOUND}")
Message("glm_CONFIG: ${glm_CONFIG}")
Message("glm_VERSION: ${glm_VERSION}")

find_package(glfw3 CONFIG REQUIRED)

Message("glfw3_FOUND: ${glfw3_FOUND}")
Message("glfw3_CONFIG: ${glfw3_CONFIG}")
Message("glfw3_VERSION: ${glfw3_VERSION}")

find_package(assimp CONFIG REQUIRED)

Message("assimp_FOUND: ${assimp_FOUND}")
Message("assimp_CONFIG: ${assimp_CONFIG}")
Message("assimp_VERSION: ${assimp_VERSION}")
Message("ASSIMP_LIBRARIES: ${ASSIMP_LIBRARIES}")
Message("ASSIMP_LIBRARY_DIRS: ${ASSIMP_LIBRARY_DIRS}")
Message("ASSIMP_INCLUDE_DIRS: ${ASSIMP_INCLUDE_DIRS}")

find_package(Vulkan REQUIRED)

Message("Vulkan_FOUND: ${Vulkan_FOUND}")
Message("Vulkan_INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}")
Message("Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")

set(GLSLANGVALIDATOR ${BASE_DIRECTORY}/tools/glslang/glslangValidator.exe)
set(GLSLC ${BASE_DIRECTORY}/tools/glslc.exe)

include_directories(${ASSIMP_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIR})
link_directories(${ASSIMP_LIBRARY_DIRS})

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_include.txt)

#####################################################################################
# Source files for this project
#
file(GLOB SOURCE_FILES *.cpp *.hpp *.inl *.h *.c)
# file(GLOB GLSL_FILES *.glsl)

set(GlobalShaderDependencies "shaders/global_descriptor_sets.h")

_compile_GLSL("shaders/simple.vert" ${GlobalShaderDependencies} "shaders/simple_vert.spv" GLSL_FILES)
_compile_GLSL("shaders/simple.frag" ${GlobalShaderDependencies} "shaders/simple_frag.spv" GLSL_FILES)
_compile_GLSL("shaders/shadow.vert" ${GlobalShaderDependencies} "shaders/shadow_vert.spv" GLSL_FILES)

Message("GLSL_FILES: ${GLSL_FILES}")

source_group(GLSL_Files FILES ${GLSL_FILES})

add_definitions(-DSHADERS_FOLDER_PATH="${BASE_DIRECTORY}/shaders/")
add_definitions(-DMODELS_FOLDER_PATH="${BASE_DIRECTORY}/data/models/")

source_group(shaders FILES 
  ${GLSL_FILES}
)

add_executable(${PROJNAME} ${SOURCE_FILES} ${GLSL_FILES})
target_link_libraries(${PROJNAME} glm glfw ${Vulkan_LIBRARIES})
target_link_libraries(${PROJNAME} ${ASSIMP_LIBRARIES})

_copy_binaries_to_target( ${PROJNAME} )



# if(MSVC)
#   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
# endif(MSVC)

# _add_project_definitions(${PROJNAME})

# _add_package_VulkanSDK()

# #####################################################################################
# # Source files for this project
# #
# file(GLOB SOURCE_FILES *.cpp *.hpp *.inl *.h *.c)
# # file(GLOB GLSL_FILES *.glsl)

# _compile_GLSL("shaders/simple.vert" "shaders/simple_vert.spv" GLSL_FILES SPV_FILES)
# _compile_GLSL("shaders/simple.frag" "shaders/simple_frag.spv" GLSL_FILES SPV_FILES)

# source_group(GLSL_Files FILES ${GLSL_FILES})

# add_definitions(-DSHADERS_FOLDER_PATH="${BASE_DIRECTORY}/shaders/")


# #####################################################################################
# # Executable
# #
# if(WIN32)
#   add_executable(${PROJNAME} WIN32 ${SOURCE_FILES} ${COMMON_SOURCE_FILES} ${PACKAGE_SOURCE_FILES} ${GLSL_FILES})
# else()
#   add_executable(${PROJNAME} ${SOURCE_FILES} ${COMMON_SOURCE_FILES} ${PACKAGE_SOURCE_FILES} ${GLSL_FILES})
# endif()

# if(UNIX)
#   add_definitions(-g -std=c++11)
#   set(UNIXLINKLIBS dl png assimp pthread)
# else()
#   set(UNIXLINKLIBS)
# endif()

# if(MSVC)
#   set_target_properties(${PROJNAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
# endif(MSVC)



# #####################################################################################
# # common source code needed for this sample
# #
# source_group(common FILES 
#   ${COMMON_SOURCE_FILES}
#   ${PACKAGE_SOURCE_FILES}
# )
# source_group(shaders FILES 
#   ${GLSL_FILES}
# )

# #####################################################################################
# # Linkage
# #
# target_link_libraries(${PROJNAME} optimized
#     ${LIBRARIES_OPTIMIZED}
#     ${PLATFORM_LIBRARIES}
#      ${UNIXLINKLIBS}
# )
# target_link_libraries(${PROJNAME} debug
#     ${LIBRARIES_DEBUG}
#     ${PLATFORM_LIBRARIES}
#     ${UNIXLINKLIBS}
# )

# _copy_binaries_to_target( ${PROJNAME} )
