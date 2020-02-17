SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 2.6)
SET(ARM 1)
SET(CMAKE_SYSTEM_PROCESSOR ARM)

set(EB_PLATFORM_OS_FAMILY POSIX)
set(EB_PLATFORM_OS ANDROID)
set(EB_PLATFORM_ARCH ARM)

MESSAGE(STATUS "Compiling for Android ARM")
MESSAGE(STATUS "${EB_PLATFORM_OS}")

#
# Common android NDK env vars
#

# Please do not commit changes to ANDROID_NDK_ROOT, the nightbuild needs this value.
# see http://thread.gmane.org/gmane.comp.programming.tools.cmake.user/33364/focus=34200
# http://public.kitware.com/Bug/view.php?id=12496
if(DEFINED ENV{ANDROID_NDK_ROOT})
    set(NDK "$ENV{ANDROID_NDK_ROOT}")
elseif(IS_DIRECTORY "$ENV{HOME}/android/android-ndk-r15")
    set(NDK "$ENV{HOME}/android/android-ndk-r15")
elseif(IS_DIRECTORY "/opt/android/android-ndk-r15")
    set(NDK "/opt/android/android-ndk-r15")
elseif(IS_DIRECTORY "/usr/local/android/android-ndk-r15")
    set(NDK "/usr/local/android/android-ndk-r15")
else()
    message(FATAL "Can not find Android NDK. Please set the environment variable ANDROID_NDK_ROOT.")
endif()
set(ANDROID_NDK_ROOT "${NDK}" CACHE PATH "Android NDK root")
message(STATUS "Using Android NDK in ${ANDROID_NDK_ROOT}")

SET(ANDROID_NDK_PREBUILT "linux-x86_64" CACHE STRING "Android NDK prebuilt platform")
SET(ANDROID_NDK_ANDROID_API "android-21" CACHE STRING "Android API level")
SET(ANDROID_NDK_TOOLCHAIN_EABI "arm-linux-androideabi-4.9" CACHE STRING "Android NDK toolchain")

SET(CMAKE_NDK_SOURCES "${ANDROID_NDK_ROOT}/sources")
SET(CMAKE_NDK_NATIVE_APP_GLUE "${CMAKE_NDK_SOURCES}/android/native_app_glue")
SET(CMAKE_NDK_GNU_LIBSTDC "${CMAKE_NDK_SOURCES}/cxx-stl/gnu-libstdc++/4.9")
SET(CMAKE_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK_ROOT}/toolchains/${ANDROID_NDK_TOOLCHAIN_EABI}/prebuilt")
SET(CMAKE_NDK_TOOLCHAIN_HOST "${ANDROID_NDK_ROOT}/toolchains/${ANDROID_NDK_TOOLCHAIN_EABI}/prebuilt/${ANDROID_NDK_PREBUILT}")
SET(CMAKE_NDK_SYSROOT "${ANDROID_NDK_ROOT}/platforms/${ANDROID_NDK_ANDROID_API}/arch-arm")

#
# Convenience settings
#
SET(CMAKE_NDK_OGLES_EGL_LIB "${CMAKE_NDK_SYSROOT}/usr/lib/libEGL.so")
SET(CMAKE_NDK_OGLES_INCLUDE_DIR "${CMAKE_NDK_SYSROOT}/usr/include")
SET(CMAKE_NDK_OGLES_LIB "${CMAKE_NDK_SYSROOT}/usr/lib/libGLESv3.so")
SET(GTF_SCDR_EGL_LIBRARY "${CMAKE_NDK_OGLES_EGL_LIB}")
SET(GTF_SCDR_OGLES2_INCLUDE_DIR "${CMAKE_NDK_OGLES_INCLUDE_DIR}")
SET(GTF_SCDR_OGLES2_LIBRARY "${CMAKE_NDK_OGLES_LIB}")

#
# The android NDK compiler
#
SET(CMAKE_CXX_COMPILER "${CMAKE_NDK_TOOLCHAIN_HOST}/bin/arm-linux-androideabi-g++")
SET(CMAKE_C_COMPILER "${CMAKE_NDK_TOOLCHAIN_HOST}/bin/arm-linux-androideabi-gcc")

#
# Override the default linker
#
#SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_NDK_TOOLCHAIN_HOST}/bin/arm-linux-androideabi-ld")
#SET(CMAKE_C_LINK_EXECUTABLE "${CMAKE_NDK_TOOLCHAIN_HOST}/bin/arm-linux-androideabi-ld")

#
# Additional includes
#
SET(CMAKE_NDK_INC "-I${CMAKE_NDK_GNU_LIBSTDC}/include")
SET(CMAKE_NDK_INC "${CMAKE_NDK_INC} -I${CMAKE_NDK_GNU_LIBSTDC}/libs/armeabi/include")
#SET(CMAKE_NDK_INC "${CMAKE_NDK_INC} ${CMAKE_NDK_GNU_LIBSTDC}/libs/armeabi-v7a/include")

#
# Necessary common compiler flags
#
SET(CMAKE_NDK_CC_FLAGS "--sysroot=${CMAKE_NDK_SYSROOT}")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -L${CMAKE_NDK_SYSROOT}/usr/lib")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -Wl,-rpath-link=${CMAKE_NDK_SYSROOT}/usr/lib")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -Wl,--no-undefined -Wl,-z,noexecstack")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -DANDROID")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -D_GLIBCXX_USE_C99_MATH=1")

SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -fpic -ffunction-sections -fstack-protector")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -D__ARM_ARCH_7__ -D__ARM_ARCH_7A__")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -Wno-psabi -march=armv7-a -msoft-float -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} ${CMAKE_NDK_INC}")
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} ${CMAKE_NDK_LIBS}")


#
# ARM EABI
#
SET(CMAKE_NDK_CC_FLAGS "${CMAKE_NDK_CC_FLAGS} -mfloat-abi=softfp -mfpu=vfp")

#
# libGLESv2.so : external reference suppress for executable
# (e.g. android:egl_get_image_for_current_context)
#
SET(CMAKE_NDK_ALLOW_SHLIB_UNDEF "-Wl,-allow-shlib-undefined")

#
# Necessary common linker flags
#
SET(CMAKE_NDK_CL_FLAGS "-Wl,--no-undefined")

#
# EXE linker flags
#
SET(CMAKE_NDK_EXE_FLAGS "${CMAKE_NDK_CL_FLAGS}")

#
# Compiler flags
#
SET(CMAKE_CXX_FLAGS "${CMAKE_NDK_CC_FLAGS} -fno-exceptions -fno-rtti -std=c++11" CACHE STRING "C++ Compiler flags")
SET(CMAKE_C_FLAGS "${CMAKE_NDK_CC_FLAGS}" CACHE STRING "C Compiler flags")

#
# Linker flags
#
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_NDK_EXE_FLAGS} ${CMAKE_NDK_ALLOW_SHLIB_UNDEF} -L${CMAKE_NDK_GNU_LIBSTDC}/libs/armeabi-v7a -lgnustl_static" CACHE STRING "Executable linker flags")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_NDK_CL_FLAGS}" CACHE STRING "Module linker flags")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_NDK_CL_FLAGS} -Wl,-shared,-Bsymbolic" CACHE STRING "Shared linker flags")

set (CMAKE_INCLUDE_PATH "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
