#

# Try to find GLM include path.
# Once done this will define
#
# GLM_FOUND
# GLM_INCLUDE_DIR
#

include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path( GLM_INCLUDE_DIR
        NAMES
            glm/glm.hpp
        PATHS
            ${PROJECT_SOURCE_DIR}/shared_external/glm/include
            ${PROJECT_SOURCE_DIR}/../shared_external/glm/include
            ${GLM_LOCATION}/include
            $ENV{GLM_LOCATION}/include
            $ENV{PROGRAMFILES}/GLM/include
            ${GLM_LOCATION}
            $ENV{GLM_LOCATION}
            DOC "The directory where glm/glm.hpp resides" )
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( GLM_INCLUDE_DIR
        NAMES
            glm/glm.hpp
        PATHS
            ${GLM_LOCATION}/include
            $ENV{GLM_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where glm/glm.hpp resides"
    )
endif ()

find_package_handle_standard_args(GLM DEFAULT_MSG
    GLM_INCLUDE_DIR
)

mark_as_advanced( GLM_FOUND )
