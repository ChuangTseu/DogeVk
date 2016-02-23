cmake_minimum_required(VERSION 2.8)

Set(PROJNAME DogeVk)
Project(DogeVk)

Message(STATUS "-------------------------------")
Message(STATUS "Processing Project ${PROJNAME}:")

Message(STATUS "found sub-folder shared_sources")
SET(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_include.txt)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

_add_project_definitions(${PROJNAME})

_add_package_VulkanSDK()

#####################################################################################
# Source files for this project
#
file(GLOB SOURCE_FILES *.cpp *.hpp *.inl *.h *.c)
# file(GLOB GLSL_FILES *.glsl)

_compile_GLSL("shaders/simple.vert" "shaders/simple_vert.spv" GLSL_FILES SPV_FILES)
_compile_GLSL("shaders/simple.frag" "shaders/simple_frag.spv" GLSL_FILES SPV_FILES)

source_group(GLSL_Files FILES ${GLSL_FILES})

add_definitions(-DSHADERS_FOLDER_PATH="${BASE_DIRECTORY}/shaders/")


#####################################################################################
# Executable
#
if(WIN32)
  add_executable(${PROJNAME} WIN32 ${SOURCE_FILES} ${COMMON_SOURCE_FILES} ${PACKAGE_SOURCE_FILES} ${GLSL_FILES})
else()
  add_executable(${PROJNAME} ${SOURCE_FILES} ${COMMON_SOURCE_FILES} ${PACKAGE_SOURCE_FILES} ${GLSL_FILES})
endif()

if(UNIX)
  add_definitions(-g -std=c++11)
  set(UNIXLINKLIBS dl png assimp pthread)
else()
  set(UNIXLINKLIBS)
endif()

if(MSVC)
  set_target_properties(${PROJNAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif(MSVC)



#####################################################################################
# common source code needed for this sample
#
source_group(common FILES 
  ${COMMON_SOURCE_FILES}
  ${PACKAGE_SOURCE_FILES}
)
source_group(shaders FILES 
  ${GLSL_FILES}
)

#####################################################################################
# Linkage
#
target_link_libraries(${PROJNAME} optimized
    ${LIBRARIES_OPTIMIZED}
    ${PLATFORM_LIBRARIES}
     ${UNIXLINKLIBS}
)
target_link_libraries(${PROJNAME} debug
    ${LIBRARIES_DEBUG}
    ${PLATFORM_LIBRARIES}
    ${UNIXLINKLIBS}
)

_copy_binaries_to_target( ${PROJNAME} )
