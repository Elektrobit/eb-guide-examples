set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 2.6)
set(ARM 1)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(EB_PLATFORM_OS_FAMILY POSIX)
set(EB_PLATFORM_OS Linux)
set(EB_PLATFORM_ARCH ARM)
set(PLATFORM_IMX6 TRUE)
set(PLATFORM_WAYLAND FALSE)


set(HMI_USE_RENDERER_VG OFF)
set(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR OFF CACHE BOOL "Enables compiler warning treatment as error")

message(STATUS "Cross-Compiling for hardfloat Linux IMX6 on Linux framebuffer"  CACHE PATH "GNU TOOLS")

set(CMAKE_C_COMPILER /opt/gcc-linaro-5.3.1-2016.05-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /opt/gcc-linaro-5.3.1-2016.05-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

# Yocto sysroot

set(YOCTO_SYSROOT "/opt/imx6hf")

set(CMAKE_FIND_ROOT_PATH "/opt/imx6hf")

SET(GTF_SCDR_EGL_LIBRARY "${YOCTO_SYSROOT}/usr/lib/libEGL.so")
SET(GTF_SCDR_OGLES_INCLUDE_DIR "${YOCTO_SYSROOT}/usr/include")
SET(GTF_SCDR_OGLES_LIBRARY "${YOCTO_SYSROOT}/usr/lib/libGLESv2.so")
SET(EGL_LIBRARY "${YOCTO_SYSROOT}/usr/lib/libEGL.so")

#
# Compiler and linker flags
#

set(CMAKE_CXX_FLAGS "--sysroot=${YOCTO_SYSROOT} -march=armv7-a -marm  -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9   -fstack-protector -D_LINUX -DLINUX  -DEGL_API_FB")
set(CMAKE_C_FLAGS "--sysroot=${YOCTO_SYSROOT} -march=armv7-a -marm  -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9  -fstack-protector -D_LINUX -DLINUX  -DEGL_API_FB")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-Bsymbolic")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-Bsymbolic")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
