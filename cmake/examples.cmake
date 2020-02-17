set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/utility_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/default_flags.cmake)

if(EB_PLATFORM_OS_QNX)
    set(GTF_TARGET_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../platform/qnx_6.6_armv7" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/qnx_6.6_armv7)")
    message(STATUS "Used qnx build for sdk examples")
elseif(EB_PLATFORM_OS_LINUX)
    set(GTF_TARGET_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../platform/linux_2.4_armv7_vivante" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/linux_2.4_armv7_vivante or ../../platform/debian_8.2_i686)")
    message(STATUS "Used linux build for sdk examples")
elseif(EB_PLATFORM_OS_ANDROID)
    set(GTF_TARGET_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../platform/armv7" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/armv7)")
    message(STATUS "Used Android build for sdk examples")
else()
    set(GTF_TARGET_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../platform/x64" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/x64)")
    message(STATUS "Used windows build for sdk examples")
endif()

include(${GTF_TARGET_SDK_ROOT}/lib/gtf-exports.cmake)

include_directories(${GTF_TARGET_SDK_ROOT}/include)

macro(gtf_add_sdk_example)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    eb_add_library(${PROJECT_NAME}
        SHARED
        INCLUDE_PATH_VARS
            ${EB_TEMP_INCLUDE_PATH_VARS}
        INCLUDE_PATHS
            ${EB_TEMP_INCLUDE_PATHS}
        LIB_PATH_VARS
            ${EB_TEMP_LIB_PATH_VARS}
        LIB_VARS
            ${EB_TEMP_LIB_VARS}
        SOURCES
            ${EB_TEMP_SOURCES}
        LIBS
            ${EB_TEMP_LIBS}
            GtfCore
        DEFINITIONS
            ${EB_TEMP_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

	set(${PROJECT_NAME}_INSTALL_DESTINTATION ${CMAKE_INSTALL_PREFIX}/${GTF_EXAMPLES_NAME}/${PROJECT_NAME}/bin)

	install(
		TARGETS
			${PROJECT_NAME}
		RUNTIME DESTINATION
			${${PROJECT_NAME}_INSTALL_DESTINTATION}
		LIBRARY DESTINATION
			${${PROJECT_NAME}_INSTALL_DESTINTATION}
		)
endmacro()
