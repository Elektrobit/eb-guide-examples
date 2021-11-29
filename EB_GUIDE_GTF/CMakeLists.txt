cmake_minimum_required(VERSION 2.8)

project(sdk_examples)

set(GTF_TARGET_SDK_ROOT "C:\\Program Files\\Elektrobit\\EB GUIDE 6.11\\platform\\win64" CACHE PATH "Install path of EB GUIDE GTF SDK")

include(${GTF_TARGET_SDK_ROOT}/lib/gtf-exports.cmake)
include(${GTF_TARGET_SDK_ROOT}/cmake/utility_macros.cmake)
include(${GTF_TARGET_SDK_ROOT}/cmake/platform.cmake)
include(${GTF_TARGET_SDK_ROOT}/cmake/default_flags.cmake)

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
            GtfGui
            GtfCore
        DEFINITIONS
            ${EB_TEMP_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

    file(RELATIVE_PATH EB_TEMP_RELATIVE_INSTALL_PATH "${sdk_examples_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}")
    set(${PROJECT_NAME}_INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/${EB_TEMP_RELATIVE_INSTALL_PATH}/model/resources")

    install(
        TARGETS
            ${PROJECT_NAME}
        RUNTIME DESTINATION
            ${${PROJECT_NAME}_INSTALL_DESTINATION}
        LIBRARY DESTINATION
            ${${PROJECT_NAME}_INSTALL_DESTINATION}
        )
endmacro()

add_subdirectory("concepts/BasicExample")
add_subdirectory("concepts/DependencyResolverExample")
add_subdirectory("concepts/ModelStageExample")
add_subdirectory("concepts/CoreStageExample")
add_subdirectory("concepts/SettingsExample")
add_subdirectory("concepts/ConfigurationImporterExample")
add_subdirectory("concepts/TraceOutputExample")
add_subdirectory("concepts/BufferSwapHooksExample")

add_subdirectory("model_extensions/TriangleWidget")
add_subdirectory("model_extensions/Multisample")
add_subdirectory("model_extensions/RippleEffect")
add_subdirectory("model_extensions/CrepuscularEffect")
add_subdirectory("model_extensions/GaussianBlur")
add_subdirectory("model_extensions/BloomEffect")
add_subdirectory("model_extensions/ExtendedContainerWidget")
add_subdirectory("model_extensions/ScriptFunction")
add_subdirectory("model_extensions/FocusedWidgetFeature")
add_subdirectory("model_extensions/FocusPolicyWidgetFeature")
add_subdirectory("model_extensions/DownscalingWidgetFeature")
add_subdirectory("model_extensions/BezierCurveExample")
add_subdirectory("model_extensions/LabelShape")
add_subdirectory("model_extensions/TGAImageLoader")
add_subdirectory("model_extensions/PostprocessingEffectExample")

add_subdirectory("model_interactions/ApplicationInteractionExample")
add_subdirectory("model_interactions/DatapoolExample")
add_subdirectory("model_interactions/EventExample")
add_subdirectory("model_interactions/RawImageFromMemoryExample")
add_subdirectory("model_interactions/WindowListExample")
add_subdirectory("model_interactions/ExternalKeyInputExample")
add_subdirectory("model_interactions/ExternalTouchInputExample")
add_subdirectory("model_interactions/ScreenshotExample")
