# -------------------------------
# Add project modules:
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../tf/build_system/cmake/Modules/ )
# -------------------------------

SET(CMAKE_SYSTEM_NAME QNX)
SET(CMAKE_SYSTEM_VERSION 7.0.0)
SET(ARM 1)
SET(CMAKE_SYSTEM_PROCESSOR ARM)
set(EB_PLATFORM_ARCH_ARM_NEON ON CACHE BOOL "enable ARM NEON support")
set(EB_PLATFORM_ARCH_ARM_A64 ON CACHE BOOL "enable ARM A64 support")

# we use qcc for compiling
set(CMAKE_COMPILER_IS_QCC 1)

set(EB_PLATFORM_OS_FAMILY POSIX)
set(EB_PLATFORM_OS ${CMAKE_SYSTEM_NAME})
set(EB_PLATFORM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
set(EB_PLATFORM_PATH POSIX)
set(CPU_ARM_SOC_SAVE NOT-DEFINED)
set(EB_PLATFORM_OS_VERSION 7)

## The following to env variables must be defined, otherwise gcc wont work
SET(QNX_TARGET $ENV{QNX_TARGET} CACHE PATH "Path to QNX Target env")
SET(QNX_HOST $ENV{QNX_HOST} CACHE PATH "Path to QNX host tool chain")
# hence we reuse the variables, but the Cygwin paths must be converted into
# windows paths before that:

IF(QNX_HOST STREQUAL "")
    MESSAGE(SEND_ERROR "Error: No QNX build environment specified. Use the environment variable QNX_HOST for host environment!")
ELSE(QNX_HOST STREQUAL "")
    SET(QNX_HOST_CMAKE_PATH ${QNX_HOST})
ENDIF(QNX_HOST STREQUAL "")

IF(QNX_TARGET STREQUAL "")
    MESSAGE(SEND_ERROR "Error: No QNX build environment specified. Use the environment variables QNX_TARGET for target environment!")
ELSE(QNX_TARGET STREQUAL "")
    SET(QNX_TARGET_CMAKE_PATH ${QNX_TARGET})
ENDIF(QNX_TARGET STREQUAL "")

MESSAGE(STATUS "Compiling for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")

option(GTFRENDERER_QNX_SCREEN_ENABLE "Enables QNX screen version for Renderer Displays" ON)
option(GTFRENDERER_QNX_SCREEN_ENABLE_PIPELINE "Enables QNX screen pipeline version for Renderer Displays" ON)
SET(OGLES2_ROOT "${QNX_TARGET}" CACHE PATH "Location of OGLES2 install prefix (if specified, lookup is executed firstly in this directory)")
set(SCREEN_ROOT "${QNX_TARGET}" CACHE PATH "Location of screen install prefix (if specified, lookup is executed firstly in this directory)")


# Added here so the platform file can be used for other projects, too:
ADD_DEFINITIONS(
    -D_QNX_SOURCE  # Required for the gnu headers: http://gcc.gnu.org/ml/gcc/2004-04/msg00230.html
)

SET(CMAKE_FIND_ROOT_PATH ${QNX_HOST_CMAKE_PATH} ${QNX_TARGET_CMAKE_PATH})

SET(CPU_ARM_SOC "armv8-a_generic" CACHE STRING "Select ARM SoC")
SET_PROPERTY(CACHE CPU_ARM_SOC PROPERTY STRINGS armv8-a_generic armv8-a_cortex-a57.cortex-a53 )

SET(QCC_GCC_BACKEND_VERSION "5.4.0" CACHE STRING "Select GCC version used by QCC")
SET_PROPERTY(CACHE QCC_GCC_BACKEND_VERSION PROPERTY STRINGS 5.4.0)

SET(CPU_ARM "")

if (CPU_ARM_SOC STREQUAL "armv8-a_generic")
    MESSAGE(STATUS "CPU_ARM_SOC: armv8-a_generic")
    SET(CPU_ARM_ARCH "armv8-a")
    SET(CPU_QNX_PATH "aarch64le")
    SET(CPU_ARM "armv8-a")
    SET(CPU_ARM_SPECIFIC "generic")
    SET(QCC_GCC_BACKEND_TYPE "gcc_ntoaarch64le_gpp")
    SET(QNX_CPU_ARCH "aarch64le")
    SET(EB_PLATFORM_ARM_ARCH 8)
endif(CPU_ARM_SOC STREQUAL "armv8-a_generic")

if (CPU_ARM_SOC STREQUAL "armv8-a_cortex-a57.cortex-a53")
    MESSAGE(STATUS "CPU_ARM_SOC: armv8-a_cortex-a57.cortex-a53")
    SET(CPU_ARM_ARCH "armv8-a")
    SET(CPU_QNX_PATH "aarch64le")
    SET(CPU_ARM "armv8-a")
    SET(CPU_ARM_SPECIFIC "cortex-a57.cortex-a53")
    SET(QCC_GCC_BACKEND_TYPE "gcc_ntoaarch64le_gpp")
    SET(QNX_CPU_ARCH "aarch64le")
    SET(EB_PLATFORM_ARM_ARCH 8)
endif(CPU_ARM_SOC STREQUAL "armv8-a_cortex-a57.cortex-a53")

if (NOT CPU_ARM_SOC STREQUAL ${CPU_ARM_SOC_SAVE})
    unset(GTF_SCDR_OGLES2_INCLUDE_DIR CACHE)
    unset(GTF_SCDR_OGLES2_LIBRARY CACHE)
    unset(GTF_SCDR_EGL_LIBRARY CACHE)
    unset(OVG_INCLUDE_DIR CACHE)
    unset(OVG_LIBRARY CACHE)
    unset(EGL_LIBRARY CACHE)
    unset(GTF3DRENDERER_OPENGLES20_SCREEN_LIBRARY CACHE)
    unset(GtfOpenGLRenderer_CORE_EXTERNAL_LIBS CACHE)
    unset(GtfRendererDisplayFactory_CORE_EXTERNAL_LIBS CACHE)
    set(CPU_ARM_SOC_SAVE ${CPU_ARM_SOC})
endif()

SET(CMAKE_CXX_COMPILER "${QNX_HOST_CMAKE_PATH}/usr/bin/qcc.exe" CACHE STRING "" FORCE)
SET(CMAKE_C_COMPILER "${QNX_HOST_CMAKE_PATH}/usr/bin/qcc.exe" CACHE STRING "" FORCE)
SET(CMAKE_LINKER "${QNX_HOST_CMAKE_PATH}/usr/bin/qcc.exe" CACHE STRING "" FORCE)
SET(CMAKE_AR "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-ar.exe" CACHE STRING "" FORCE)
SET(CMAKE_OBJCOPY "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-objcopy.exe" CACHE STRING "" FORCE)
SET(CMAKE_NM "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-nm.exe" CACHE STRING "" )
SET(CMAKE_RANLIB "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-ranlib.exe" CACHE STRING "" FORCE)
SET(CMAKE_OBJDUMP "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-objdump.exe" CACHE STRING "" FORCE)
SET(CMAKE_STRIP "${QNX_HOST_CMAKE_PATH}/usr/bin/ntoaarch64-strip.exe" CACHE STRING "" FORCE)


SET(CMAKE_CXX_FLAGS "-vv -V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -march=${CPU_ARM_ARCH} -mtune=${CPU_ARM_SPECIFIC} -fno-stack-protector -fno-exceptions" CACHE STRING "" FORCE)
SET(CMAKE_C_FLAGS   "-vv -V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -march=${CPU_ARM_ARCH} -mtune=${CPU_ARM_SPECIFIC} -fno-stack-protector -fno-exceptions" CACHE STRING "" FORCE)


SET(CMAKE_SHARED_LIBRARY_C_FLAGS "-Wc,-fPIC")
SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS "-Wc,-fPIC")

SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -lm -fPIC -shared")
SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -lm -fPIC -shared")

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -lm -Bdynamic")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-V${QCC_GCC_BACKEND_VERSION},${QCC_GCC_BACKEND_TYPE} -lm -Bdynamic")

SET(CMAKE_CXX_LINK_EXECUTABLE  "<CMAKE_LINKER>   <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lm")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
