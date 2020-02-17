INCLUDE(MacroAddFileDependencies)
INCLUDE(CMakeDependentOption)

# checks if the list contains \a value
# result is written to var
MACRO(gtf_list_contains var value)
    SET(${var})
    FOREACH (value2 ${ARGN})
        IF (${value} STREQUAL ${value2})
            SET(${var} TRUE)
        ENDIF (${value} STREQUAL ${value2})
    ENDFOREACH (value2)
ENDMACRO(gtf_list_contains)

# parsers macro arguments
# The gtf_parse_arguments macro will take the arguments of another macro and define
# several variables. The first argument to gtf_parse_arguments is a prefix to put
# on all variables it creates. The second argument is a list of names, and the
# third argument is a list of options. Both of these lists should be quoted.
# The rest of gtf_parse_arguments are arguments from another macro to be parsed.
#
#   gtf_parse_arguments(prefix arg_names options arg1 arg2...)
#
# For each item in options, gtf_parse_arguments will create a variable with that name,
# prefixed with prefix_. So, for example, if prefix is MY_MACRO and options is
# OPTION1;OPTION2, then gtf_parse_arguments will create the variables MY_MACRO_OPTION1
# and MY_MACRO_OPTION2. These variables will be set to true if the option
# exists in the command line or false otherwise.
#
# For each item in arg_names, gtf_parse_arguments will create a variable with that
# name, prefixed with prefix_. Each variable will be filled with the arguments
# that occur after the given arg_name is encountered up to the next arg_name or
# the end of the arguments. All options are removed from these lists.
# gtf_parse_arguments also creates a prefix_DEFAULT_ARGS variable containing the
# list of all arguments up to the first arg_name encountered.
#
# Example:
# Let us say that we are creating an application that accepts plugins with
# components, and we need a CMake macro that makes it easy to compile and link a
# plugin. The plugin will have some code in it, and it will have to have some
# sort of factory built in that allows the introspection and creation of components
# in the plugin. We also may want to embed some scripts in the plugin. The plugin
# may depend on other libraries and plugins. Finally, we may have some options
# to determine how the plugin may be compiled or used. We will create a macro
# command that can be called as follows.

# ADD_PLUGIN(name [AUTO_INSTALL] [NO_MODULE] source1 source2 ...
#    EXPORTS object1 object2 ...
#    [AUTOLOAD_SCRIPTS script1 script2 ...]
#    [LINK_LIBRARIES lib1 lib2 ...]
#    [DEPENDS plugin1 plugin2 ...]
#   )
#
# Although the argument usage is easy for a user to understand (especially one
# familiar with using other CMake commands), but it can be a hassle for a macro
# to parse. Of course, gtf_parse_arguments will do it for us.
#
# Here is a vacuous implementation of ADD_PLUGIN. It uses gtf_parse_arguments to parse
# the argument specification above, and then simply prints out each argument list
# and option. Note that this example uses the CAR and CDR macros.

# MACRO(ADD_PLUGIN)
#     gtf_parse_arguments(PLUGIN
#         "EXPORTS;AUTOLOAD_SCRIPTS;LINK_LIBRARIES;DEPENDS"
#         "AUTO_INSTALL;NO_MODULE"
#         ${ARGN}
#         )
#     CAR(PLUGIN_NAME ${PLUGIN_DEFAULT_ARGS})
#     CDR(PLUGIN_SOURCES ${PLUGIN_DEFAULT_ARGS})
#
#     MESSAGE("*** Arguments for ${PLUGIN_NAME}")
#     MESSAGE("Sources: ${PLUGIN_SOURCES}")
#     MESSAGE("Exports: ${PLUGIN_EXPORTS}")
#     MESSAGE("Autoload scripts: ${PLUGIN_AUTOLOAD_SCRIPTS}")
#     MESSAGE("Link libraries: ${PLUGIN_LINK_LIBRARIES}")
#     MESSAGE("Depends: ${PLUGIN_DEPENDS}")
#     IF (PLUGIN_AUTO_INSTALL)
#         MESSAGE("Auto install")
#     ENDIF (PLUGIN_AUTO_INSTALL)
#     IF (PLUGIN_NO_MODULE)
#         MESSAGE("No module")
#     ENDIF (PLUGIN_NO_MODULE)
# ENDMACRO(ADD_PLUGIN)
#
# Here are some examples of using this macro so you can see how gtf_parse_arguments breaks up the arguments.

MACRO(gtf_parse_arguments prefix arg_names option_names)
    SET(DEFAULT_ARGS)
    FOREACH(arg_name ${arg_names})
        SET(${prefix}_${arg_name})
    ENDFOREACH(arg_name)
    FOREACH(option ${option_names})
        SET(${prefix}_${option} FALSE)
    ENDFOREACH(option)

    SET(current_arg_name DEFAULT_ARGS)
    SET(current_arg_list)
    FOREACH(arg ${ARGN})
        gtf_list_contains(is_arg_name ${arg} ${arg_names})
        IF (is_arg_name)
            SET(${prefix}_${current_arg_name} ${current_arg_list})
            SET(current_arg_name ${arg})
            SET(current_arg_list)
        ELSE (is_arg_name)
            gtf_list_contains(is_option ${arg} ${option_names})
            IF (is_option)
                SET(${prefix}_${arg} TRUE)
            ELSE (is_option)
                SET(current_arg_list ${current_arg_list} ${arg})
            ENDIF (is_option)
        ENDIF (is_arg_name)
    ENDFOREACH(arg)
    SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(gtf_parse_arguments)

###############################################################
# This will add arguments not found in ${parameter} to the end.  It
# does not attempt to remove duplicate arguments already existing in
# ${parameter}.
MACRO(gtf_force_add_flags parameter)
  # Create a separated list of the arguments to loop over
  SET(p_list ${${parameter}})
  SEPARATE_ARGUMENTS(p_list)
  # Make a copy of the current arguments in ${parameter}
  SET(new_parameter ${${parameter}})
  # Now loop over each required argument and see if it is in our
  # current list of arguments.
  FOREACH(required_arg ${ARGN})
    # This helps when we get arguments to the function that are
    # grouped as a string:
    #
    # ["-O3 -g"]  instead of [-O3 -g]
    SET(TMP ${required_arg}) #elsewise the Seperate command doesn't work)
    SEPARATE_ARGUMENTS(TMP)
    FOREACH(option ${TMP})
      # Look for the required argument in our list of existing arguments
      SET(found FALSE)
      FOREACH(p_arg ${p_list})
        IF (${p_arg} STREQUAL ${option})
          SET(found TRUE)
        ENDIF (${p_arg} STREQUAL ${option})
      ENDFOREACH(p_arg)
      IF(NOT found)
        # The required argument wasn't found, so we need to add it in.
        SET(new_parameter "${new_parameter} ${option}")
      ENDIF(NOT found)
    ENDFOREACH(option ${TMP})
  ENDFOREACH(required_arg ${ARGN})
  SET(${parameter} ${new_parameter} CACHE STRING "" FORCE)
ENDMACRO(gtf_force_add_flags)

###################################################################
# gtf_create_source_group(
#           [FOLDERS                 FOLDER1 [FOLDER2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           )
#
# This macro takes a list of source files as input and generates source
# groups that follow the shortest relative path of the source files from the
# source folders. This is strictly a cosmetic change, but it's nice if you're
# using the CMake generated projects also for development
#
MACRO ( gtf_create_source_group )
    gtf_parse_arguments(PARAMS_VS_FILTER
            "FOLDERS;SOURCES"
            ""
            ${ARGN}
            )
    FOREACH(SOURCE_FILE ${PARAMS_VS_FILTER_SOURCES})
        get_filename_component(SOURCE_FILE_ABSOLUTE "${SOURCE_FILE}" ABSOLUTE)
        get_filename_component(PARENT_FOLDER_ABSOLUTE "${SOURCE_FILE_ABSOLUTE}" PATH)
        set(FOLDER_RELATIVE "${PARENT_FOLDER_ABSOLUTE}")
        FOREACH(SOURCE_FOLDER ${PARAMS_VS_FILTER_FOLDERS})
            if(IS_ABSOLUTE "${FOLDER_RELATIVE}")
                file(RELATIVE_PATH FOLDER_RELATIVE "${SOURCE_FOLDER}" "${PARENT_FOLDER_ABSOLUTE}")
            else()
                file(RELATIVE_PATH TMP_FOLDER_RELATIVE "${SOURCE_FOLDER}" "${PARENT_FOLDER_ABSOLUTE}")
                string(LENGTH "${TMP_FOLDER_RELATIVE}" TMP_FOLDER_RELATIVE_LENGTH)
                string(LENGTH "${FOLDER_RELATIVE}" FOLDER_RELATIVE_LENGTH)
                if (TMP_FOLDER_RELATIVE_LENGTH LESS FOLDER_RELATIVE_LENGTH)
                    set(FOLDER_RELATIVE "${TMP_FOLDER_RELATIVE}")
                endif()
            endif()
        ENDFOREACH()
        # replace all double slash characters by single character
        STRING ( REGEX REPLACE "//" "/" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        # remove leading slash character
        STRING ( REGEX REPLACE "^[\\\\/]" "" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        # remove trailing slash character
        STRING ( REGEX REPLACE "[^\\\\/]*[\\\\/]$" "" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        # replace all slash characters by double backslash characters
        STRING ( REGEX REPLACE "/" "\\\\\\\\" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        SOURCE_GROUP ( "${FOLDER_RELATIVE}" FILES ${SOURCE_FILE_ABSOLUTE} )
    ENDFOREACH()
ENDMACRO ( gtf_create_source_group )

###################################################################
# Create Final File
#
macro(CREATE_FINAL_FILE ProjectDir _filename)
    file(WRITE ${PROJECT_BINARY_DIR}/${_filename}_temp "//autogenerated file\n")
    foreach(_current_FILE ${ARGN})
        if(IS_DIRECTORY ${_current_FILE})
            message(WARNING "The entry ${_current_FILE} is a directoy in project ${PROJECT_NAME}")
        else()
            get_filename_component(_current_FILE_EXT ${_current_FILE} EXT)
            string(REPLACE "." "" _current_FILE_EXT ${_current_FILE_EXT})
            list(FIND CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${_current_FILE_EXT} EXT_INDEX)
            if(EXT_INDEX GREATER -1)
                set(RelativePath ${_current_FILE})
                if(NOT IS_ABSOLUTE ${RelativePath})
                   set(RelativePath ${PROJECT_SOURCE_DIR}/${_current_FILE})
                endif(NOT IS_ABSOLUTE ${RelativePath})
                file(RELATIVE_PATH RelativePath ${PROJECT_BINARY_DIR} ${RelativePath})
                file(APPEND ${PROJECT_BINARY_DIR}/${_filename}_temp "#include \"${RelativePath}\"\n")
                set_source_files_properties(${_current_FILE} PROPERTIES HEADER_FILE_ONLY ON)
            endif(EXT_INDEX GREATER -1)
        endif()
    endforeach(_current_FILE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_BINARY_DIR}/${_filename}_temp ${PROJECT_BINARY_DIR}/${_filename})
endmacro(CREATE_FINAL_FILE)

option(GTF_FAST_BUILD_GLOBAL_ON OFF "Force fast build creation globally, overriding any local settings")

###############################################################################
# Turn a list into a string
#
function(eb_join_list _output _list)
    set(tmp_string "")
    foreach(_item ${_list})
        string(LENGTH "${tmp_string}" _len)
        if(${_len} GREATER 0)
          set(tmp_string "${tmp_string} ${_item}")
        else()
          set(tmp_string "${_item}")
        endif()
    endforeach()
    set (${_output} "${tmp_string}" PARENT_SCOPE)
endfunction()

###################################################################
#  eb_add_compile_flags(
#           [TARGETS                 TARGET1 [TARGET2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds flags to the compile flag property
#  of targets or source files.
#
#  TARGET an optional list of targets.
#  SOURCE an optional list of source files.
#  FLAGS a list of flags.
#
macro(eb_add_compile_flags)
    gtf_parse_arguments(EB_ADD_FLAGS_PARAMETER
         "TARGETS;SOURCES;FLAGS"
         ""
         ${ARGN}
    )
    if(EB_ADD_FLAGS_PARAMETER_TARGETS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_TARGETS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_TARGETS)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_SOURCES)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_SOURCES)
        eb_to_absolute_paths(EB_ADD_FLAGS_PARAMETER_SOURCES)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_SOURCES)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_FLAGS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_FLAGS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_FLAGS)
        foreach(CURRENT_SCOPE "TARGET" "SOURCE")
            foreach(CURRENT_TARGET ${EB_ADD_FLAGS_PARAMETER_${CURRENT_SCOPE}S})
                get_property(
                        TMP_FLAGS
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                )
                separate_arguments(TMP_FLAGS)
                list(APPEND TMP_FLAGS ${EB_ADD_FLAGS_PARAMETER_FLAGS})
                list(REMOVE_DUPLICATES TMP_FLAGS)
                eb_join_list(TMP_FLAGS "${TMP_FLAGS}")
                # MESSAGE("setting compile flags of ${CURRENT_SCOPE} ${CURRENT_TARGET} to ${TMP_FLAGS}")
                set_property(
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                        ${TMP_FLAGS}
                )
            endforeach()
        endforeach()
    endif()
endmacro()

###################################################################
#  eb_remove_compile_flags(
#           [TARGETS                 TARGET1 [TARGET2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Remove flags from compile flag property
#  of targets or source files.
#
#  TARGET an optional list of targets.
#  SOURCE an optional list of source files.
#  FLAGS a list of flags.
#
macro(eb_remove_compile_flags)
    gtf_parse_arguments(EB_ADD_FLAGS_PARAMETER
         "TARGETS;SOURCES;FLAGS"
         ""
         ${ARGN}
    )
    if(EB_ADD_FLAGS_PARAMETER_TARGETS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_TARGETS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_TARGETS)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_SOURCES)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_SOURCES)
        eb_to_absolute_paths(EB_ADD_FLAGS_PARAMETER_SOURCES)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_SOURCES)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_FLAGS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_FLAGS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_FLAGS)
        foreach(CURRENT_SCOPE "TARGET" "SOURCE")
            foreach(CURRENT_TARGET ${EB_ADD_FLAGS_PARAMETER_${CURRENT_SCOPE}S})
                get_property(
                        TMP_FLAGS
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                )
                separate_arguments(TMP_FLAGS)
                foreach(REMOVE_FLAG ${EB_ADD_FLAGS_PARAMETER_FLAGS})
                    list(REMOVE_ITEM TMP_FLAGS ${REMOVE_FLAG})
                endforeach()
                eb_join_list(TMP_FLAGS "${TMP_FLAGS}")
                # MESSAGE("setting compile flags of ${CURRENT_SCOPE} ${CURRENT_TARGET} to ${TMP_FLAGS}")
                set_property(
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                        ${TMP_FLAGS}
                )
            endforeach()
        endforeach()
    endif()
endmacro()

###################################################################
#
#  eb_add_library(name
#           [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS       VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS           PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATHS_VARS          VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [DEFINITIONS             DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds a static or shared library. This is a wrapper around add_library
#  defined for EB internal use.
#
#  INCLUDE_PATH_VARS are a list of variable names, that contain
#  include directories of the dependencies of this lib. Variable
#  names are expected to be able to track missing dependencies.
#
#  INCLUDE_PATHS a set of include paths required by this project.
#  use this for include paths not defined in variables.
#
#  SOURCES a list of source files.
#
#  NO_FAST_BUILD_SOURCES a list of source files that cannot be built
#  using fast build. I.e. because they define static symbols that could
#  collide, or if they require special preprocessor settings.
#
#  LIB_VARS are a list of variable names, that contain library dependencies
#  of this lib. Variable names are expected to be able to track missing
#  dependencies.
#
#  LIB_PATH_VARS is the counterpart of INCLUDE_PATH_VARS. It just expects
#  library directories that contain libraries this target depends on.
#  Again here variable names are expected to be able to detect missing
#  dependencies. Note this feature will be dropped as soon as we switch
#  to cmake-2.6.4 or later
#
#  LIBS is like LIB_VARS, but expects targets instead of variable names.
#
#  DEFINITIONS is a set of definitions passed to add_definitions()
#
#  FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_library name)
    gtf_parse_arguments(EB_ADD_LIB
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;SOURCES;NO_FAST_BUILD_SOURCES;LIB_VARS;LIB_PATH_VARS;LIBS;DEFINITIONS;FLAGS;CONVENIENCE_TARGETS"
         ""
         ${ARGN}
         )

    if (TOOLCHAIN_USE_GNU_ARCHIVER_FOR_LIBRARIES)
   # message(STATUS "Toolchain uses gnu archiver instead of linker for for creating ${name}")
    else()
    #message(STATUS "Default toolchain behaviour for creating ${name}")
    endif()


    include_directories(${EB_ADD_LIB_INCLUDE_PATHS})
    foreach(inpath ${EB_ADD_LIB_INCLUDE_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs include path variable ${inpath} defined")
        else(NOT ${inpath})
            include_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    foreach(inpath ${EB_ADD_LIB_LIB_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs library path variable ${inpath} defined")
        else(NOT ${inpath})
            link_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    OPTION(GTF_FAST_BUILD_${name} "Enable final all-in-one compilation, for the library ${name}")
    IF(GTF_FAST_BUILD_${name} OR GTF_FAST_BUILD_GLOBAL_ON)
        CREATE_FINAL_FILE(
                    ${PROJECT_SOURCE_DIR}
                    ${name}_final_file.cpp
                    ${EB_ADD_LIB_SOURCES}
                    )
        gtf_create_source_group(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_LIB_SOURCES}
                      ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                  )
       add_library(${name} ${EB_ADD_LIB_DEFAULT_ARGS}
                ${EB_ADD_LIB_SOURCES}
                ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                ${PROJECT_BINARY_DIR}/${name}_final_file.cpp
                )
    ELSE()
        gtf_create_source_group(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_LIB_SOURCES}
                      ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                )
        add_library(${name} ${EB_ADD_LIB_DEFAULT_ARGS}
                ${EB_ADD_LIB_SOURCES}
                ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                )
    ENDIF()

    set(EB_ADD_LIB_LIB_DEPENDENCIES)
    foreach(inpath ${EB_ADD_LIB_LIB_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs library variable ${inpath} defined")
        else(NOT ${inpath})
            list(APPEND EB_ADD_LIB_LIB_DEPENDENCIES ${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)
    ## TODO check if this causes problems
    if(QNX_ENABLE_FUNCTION_PROFILING)
        list(APPEND EB_ADD_LIB_LIBS profilingS)
    endif()

    target_link_libraries(${name} ${EB_ADD_LIB_LIB_DEPENDENCIES} ${EB_ADD_LIB_LIBS})
    add_definitions(${EB_ADD_LIB_DEFINITIONS})
    eb_add_compile_flags(TARGETS ${name} FLAGS ${EB_ADD_LIB_FLAGS})

    if( COMMAND add_clang_analyzer )
        add_clang_analyzer(${name} ${EB_ADD_LIB_SOURCES} ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES})
    endif( COMMAND add_clang_analyzer )

    set_property (GLOBAL APPEND PROPERTY EB_TARGETS ${name})

endmacro(eb_add_library)

macro(eb_to_absolute_paths list_name)
    set(__eb_to_absolute_paths_temp)
    foreach(src_file ${${list_name}})
        if(IS_ABSOLUTE ${src_file})
            list(APPEND __eb_to_absolute_paths_temp ${src_file})
        else()
            list(APPEND __eb_to_absolute_paths_temp ${PROJECT_SOURCE_DIR}/${src_file})
        endif()
    endforeach(src_file)

    set(${list_name} ${__eb_to_absolute_paths_temp})
endmacro(eb_to_absolute_paths)

###################################################################
#
#  eb_define_convenience_library(name)
#
#  Defines a new convenience library (e.g. GtfCore).
#  Some CMAKE variables corresponding to the convenience library
#  are initialized.
#
macro(eb_define_convenience_library name)
    set(GTF_CONVENIENCE_LIB_${name}_PROCESSED OFF CACHE BOOL "" FORCE)
    set(GTF_CONVENIENCE_LIB_${name}_PARTS "" CACHE "" INTERNAL FORCE)
endmacro(eb_define_convenience_library)

###################################################################
#
#  eb_add_convenience_library(name
#           [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS      VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS          PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATHS_VARS          VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [SHARED_DEFINITIONS      DEFINE1 [DEFINE2 ...]]
#           [STATIC_DEFINITIONS      DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
# Add new content to the convenience library. The convenience library
# must be previously defined with eb_define_convenience_library(). The
# convenience library must not yet processed by eb_process_convenience_library()
#
# The meaning of most parameters corresponds to the meaning of parameters
# of eb_add_library().
#
# There are additional parameters:
#
# SHARED_DEFINITIONS is a set of definitions passed to add_definitions()
# when building the convenience library as shared library (e.g. -DGtfOSAL_EXPORTS)
#
# STATIC_DEFINITIONS is a set of definitions passed to add_definitions()
# when building the convenience library as shared library (e.g. -DGtfOSAL_STATIC)
#
# FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_convenience_library name)
    if(GTF_CONVENIENCE_LIB_${name}_PROCESSED)
        MESSAGE( FATAL_ERROR "Adding new contents to convenience library ${name} must not happen after processing." )
    endif()

    gtf_parse_arguments(EB_ADD_LIB
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;SOURCES;NO_FAST_BUILD_SOURCES;LIB_VARS;LIB_PATH_VARS;LIBS;SHARED_DEFINITIONS;STATIC_DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    eb_to_absolute_paths(EB_ADD_LIB_SOURCES)
    eb_to_absolute_paths(EB_ADD_LIB_NO_FAST_BUILD_SOURCES)

    set(convlib_params INCLUDE_PATH_VARS
                       INCLUDE_PATHS
                       SOURCES
                       NO_FAST_BUILD_SOURCES
                       LIB_VARS
                       LIB_PATH_VARS
                       LIBS
                       SHARED_DEFINITIONS
                       STATIC_DEFINITIONS
                       FLAGS)

    foreach(param ${convlib_params})
        set(GTF_CONVENIENCE_LIB_${name}_PART_${PROJECT_NAME}_${param}
            ${EB_ADD_LIB_${param}}
            CACHE "" INTERNAL FORCE)
    endforeach(param)

    gtf_list_contains(existing_val ${PROJECT_NAME} ${GTF_CONVENIENCE_LIB_${name}_PARTS})
    if (NOT existing_val)
        list(APPEND GTF_CONVENIENCE_LIB_${name}_PARTS ${PROJECT_NAME})
        set(GTF_CONVENIENCE_LIB_${name}_PARTS ${GTF_CONVENIENCE_LIB_${name}_PARTS} CACHE "" INTERNAL FORCE)
    endif (NOT existing_val)
endmacro(eb_add_convenience_library)

###################################################################
#
#  eb_process_convenience_library(name)
#
#  Process all convenience library contents previously added with
#  eb_add_convenience_library(). Merge all the parameters
#  of all convenience library parts to one set of variables.
#
macro(eb_process_convenience_library name)
    if(GTF_CONVENIENCE_LIB_${name}_PROCESSED)
        MESSAGE( FATAL_ERROR "Error while processing convenience library ${name}. It was already processed." )
    endif()
    set(GTF_CONVENIENCE_LIB_${name}_PROCESSED ON CACHE BOOL "" FORCE)

    set(convlib_params INCLUDE_PATH_VARS
                       INCLUDE_PATHS
                       SOURCES
                       NO_FAST_BUILD_SOURCES
                       LIB_VARS
                       LIB_PATH_VARS
                       LIBS
                       SHARED_DEFINITIONS
                       STATIC_DEFINITIONS
                       FLAGS)

    foreach(param ${convlib_params})
        set(GTF_CONVENIENCE_LIB_${name}_RESULT_${param} "" CACHE "" INTERNAL FORCE)
        foreach(part ${GTF_CONVENIENCE_LIB_${name}_PARTS})
            foreach(val ${GTF_CONVENIENCE_LIB_${name}_PART_${part}_${param}})
                gtf_list_contains(existing_val ${val} ${GTF_CONVENIENCE_LIB_${name}_RESULT_${param}})
                if (NOT existing_val)
                    list(APPEND GTF_CONVENIENCE_LIB_${name}_RESULT_${param} ${val})
                endif (NOT existing_val)
            endforeach(val)
        endforeach(part)
        set(GTF_CONVENIENCE_LIB_${name}_RESULT_${param} ${GTF_CONVENIENCE_LIB_${name}_RESULT_${param}} CACHE "" INTERNAL FORCE)
    endforeach(param)
endmacro(eb_process_convenience_library)

###################################################################
#
#  eb_add_executable(name
#           [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS       VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS           PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATH_VARS           VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [DEFINITIONS             DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds an executeable. This is a wrapper around add_executable
#  defined for EB internal use.
#
#  INCLUDE_PATH_VARS are a list of variable names, that contain
#  include directories of the dependencies of this lib. Variable
#  names are expected to be able to track missing dependencies.
#
#  INCLUDE_PATHS a set of include paths required by this project.
#  use this for include paths not defined in variables.
#
#  SOURCES a list of source files.
#
#  NO_FAST_BUILD_SOURCES a list of source files that cannot be built
#  using fast build. I.e. because they define static symbols that could
#  collide, or if they require special preprocessor settings.
#
#  LIB_VARS are a list of variable names, that contain library dependencies
#  of this lib. Variable names are expected to be able to track missing
#  dependencies.
#
#  LIB_PATH_VARS is the counterpart of INCLUDE_PATH_VARS. It just expects
#  library directories that contain libraries this target depends on.
#  Again here variable names are expected to be able to detect missing
#  dependencies. Note this feature will be dropped as soon as we switch
#  to cmake-2.6.4 or later
#
#  LIBS is like LIB_VARS, but expects targets instead of variable names.
#
#  DEFINITIONS is a set of definitions passed to add_definitions()
#
#  FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_executable name)
    gtf_parse_arguments(EB_ADD_PRG
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;NO_FAST_BUILD_SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    include_directories(${EB_ADD_PRG_INCLUDE_PATHS})
    foreach(inpath ${EB_ADD_PRG_INCLUDE_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs include path variable ${inpath} defined")
        else(NOT ${inpath})
            include_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    foreach(linkpath ${EB_ADD_PRG_LIB_PATH_VARS})
        if(NOT ${linkpath})
            #message(STATUS "${name} needs lib path variable ${linkpath} defined")
        else(NOT ${linkpath})
            link_directories(${${linkpath}})
        endif(NOT ${linkpath})
    endforeach(linkpath)


    OPTION(GTF_FAST_BUILD_${name} "Enable final all-in-one compilation, for executeable ${name}")
    IF(GTF_FAST_BUILD_${name} OR GTF_FAST_BUILD_GLOBAL_ON)
        CREATE_FINAL_FILE(
                    ${PROJECT_SOURCE_DIR}
                    ${name}_final_file.cpp
                    ${EB_ADD_PRG_SOURCES}
                    )
        gtf_create_source_group(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_PRG_SOURCES}
                      ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                  )
        add_executable(${name}  ${EB_ADD_PRG_DEFAULT_ARGS}
                    ${EB_ADD_PRG_SOURCES}
                    ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                    ${PROJECT_BINARY_DIR}/${name}_final_file.cpp
                    )
    ELSE()
        gtf_create_source_group(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_PRG_SOURCES}
                      ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                  )
        add_executable(${name}  ${EB_ADD_PRG_DEFAULT_ARGS}
                    ${EB_ADD_PRG_SOURCES}
                    ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                    )
    ENDIF()

    add_definitions(${EB_ADD_PRG_DEFINITIONS})
    eb_add_compile_flags(TARGETS ${name} FLAGS ${EB_ADD_PRG_FLAGS})

    if( COMMAND add_clang_analyzer )
        add_clang_analyzer(${name} ${EB_ADD_PRG_SOURCES} ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES})
    endif( COMMAND add_clang_analyzer )
    set(lib_dependencies)
    foreach(lib ${EB_ADD_PRG_LIB_VARS})
        if(NOT ${lib})
            #message(STATUS "${name} needs lib variable ${lib} defined")
        else(NOT ${lib})
            list(APPEND lib_dependencies ${${lib}})
        endif(NOT ${lib})
    endforeach(lib)

    if(QNX_ENABLE_FUNCTION_PROFILING)
        list(APPEND EB_ADD_PRG_LIBS profilingS)
    endif()

    target_link_libraries(${name} ${lib_dependencies} ${EB_ADD_PRG_LIBS})

    update_LinkerFlag_ForMapfile(${name})
    update_LinkerFlag_ForStrip(${name} "gtf_startup.map" "gtf_startup_asan.map")

    set_property (GLOBAL APPEND PROPERTY EB_TARGETS ${name})

endmacro(eb_add_executable)

macro(gtf_add_include_path _paths)
    foreach (_path ${_paths})
        if(EXISTS ${_path})
            list (FIND GTF_INCLUDE_PATHS ${_path} _index)
            if (${_index} EQUAL -1)
                message(STATUS "Added include path: ${_path}")
                list(APPEND GTF_INCLUDE_PATHS "${_path}")
                set(GTF_INCLUDE_PATHS ${GTF_INCLUDE_PATHS} CACHE "" INTERNAL FORCE)
            endif()
        endif()
    endforeach()
endmacro(gtf_add_include_path)

macro(gtf_install_folder path install_type)
    if(EXISTS "${path}")
        file(GLOB subfolders RELATIVE "${path}" "${path}/*")
        list(REMOVE_ITEM subfolders "pd")
        foreach(subfolder ${subfolders})
            eb_install("${install_type}"
                DIRECTORY
                    "${path}/${subfolder}"
                DESTINATION
                    "${GTF_INSTALL_SUBFOLDER}include")
        endforeach()
    endif()
endmacro()

macro(gtf_install_include_folder path)
    gtf_install_folder("${path}" "All")
endmacro()

macro(gtf_install_interface_folder path)
    gtf_install_folder("${path}" "SDK")
endmacro()

macro(gtf_add_tools_library name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    eb_add_library(${name}
        STATIC
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
        DEFINITIONS
            ${EB_TEMP_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

    set_property (TARGET ${name} PROPERTY FOLDER "TF projects/tools")
endmacro()

macro(gtf_add_tools_executable name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_DESTINTATION"
         ""
         ${ARGN}
         )

    if(EB_PLATFORM_OS_FAMILY_WINDOWS)
        configure_file(${Gtf_SOURCE_DIR}/build_system/src/windows/exe_resource.rc.in ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.rc)
        configure_file(${Gtf_SOURCE_DIR}/build_system/src/windows/exe_resource.h.in ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.h)

        # copy the icon for the windows DLL
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Gtf_SOURCE_DIR}/build_system/resources/icons/eb32.ico ${PROJECT_BINARY_DIR}/include/gtf/${name}/eb32.ico)

        set(${name}_resources ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.rc
                              ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.h
                              ${PROJECT_BINARY_DIR}/include/gtf/${name}/eb32.ico)
    endif(EB_PLATFORM_OS_FAMILY_WINDOWS)

    gtf_add_include_path(${PROJECT_SOURCE_DIR}/include)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/include)

    eb_add_executable(${name}
        INCLUDE_PATH_VARS
            ${EB_TEMP_INCLUDE_PATH_VARS}
        INCLUDE_PATHS
            ${EB_TEMP_INCLUDE_PATHS}
        LIB_PATH_VARS
            ${EB_TEMP_LIB_PATH_VARS}
        LIB_VARS
            ${EB_TEMP_LIB_VARS}
        SOURCES
            ${${name}_resources}
            ${EB_TEMP_SOURCES}
        LIBS
            ${EB_TEMP_LIBS}
        DEFINITIONS
            ${EB_TEMP_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

    set_property (TARGET ${name} PROPERTY FOLDER "TF projects/tools")

    if(EB_TEMP_INSTALL_DESTINTATION)
        set(MODULE_INSTALL_DESTINTATION ${EB_TEMP_INSTALL_DESTINTATION})
    else()
        set(MODULE_INSTALL_DESTINTATION tools)
    endif()

    postBuild_Executable_StripAndInstall(${name}
                                         "SDK"
                                         "${MODULE_INSTALL_DESTINTATION}")
endmacro()

macro(gtf_add_tools_unit_test name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_FILES;INSTALL_FOLDERS;INSTALL_DESTINATION"
         ""
         ${ARGN}
         )

    if((GTF_INSTALL_TYPE STREQUAL "Unittests") OR (GTF_INSTALL_TYPE STREQUAL "All"))
        eb_add_executable(${name}
            INCLUDE_PATH_VARS
                ${EB_TEMP_INCLUDE_PATH_VARS}
            INCLUDE_PATHS
                ${EB_TEMP_INCLUDE_PATHS}
                ${googletest_main_INCLUDE_PATHS}
            LIB_PATH_VARS
                ${EB_TEMP_LIB_PATH_VARS}
            LIB_VARS
                ${EB_TEMP_LIB_VARS}
            SOURCES
                ${Gtf_SOURCE_DIR}/test_system/src/main.cpp
                ${EB_TEMP_SOURCES}
            LIBS
                ${EB_TEMP_LIBS}
                googletest_main
                GtfCore
            DEFINITIONS
                ${EB_TEMP_DEFINITIONS}
            FLAGS
                ${EB_TEMP_FLAGS}
                ${googletest_FLAGS}
        )

        set_property (TARGET ${name} PROPERTY FOLDER "TF tests/tools")

        set(MODULE_INSTALL_DESTINTATION ${GTF_INSTALL_SUBFOLDER}bin)

        eb_install("Unittests"
            FILES
                ${EB_TEMP_INSTALL_FILES}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        eb_install("Unittests"
            DIRECTORY
                ${EB_TEMP_INSTALL_FOLDERS}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        postBuild_Executable_StripAndInstall(${name}
                                             "Unittests"
                                             "${MODULE_INSTALL_DESTINTATION}")
    endif()
endmacro()

macro(gtf_add_hmi_unit name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    gtf_add_include_path(${PROJECT_SOURCE_DIR}/include)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/include)
    gtf_add_include_path(${PROJECT_SOURCE_DIR}/interface)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/interface)


    eb_add_library(${name}
        STATIC
        INCLUDE_PATH_VARS
            ${EB_TEMP_INCLUDE_PATH_VARS}
        INCLUDE_PATHS
            ${PROJECT_BINARY_DIR}/src
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

    set_property (TARGET ${name} PROPERTY LINKER_LANGUAGE CXX)
    set_property (TARGET ${name} PROPERTY FOLDER "TF projects/hmi/units")

    gtf_install_include_folder(${PROJECT_SOURCE_DIR}/include)
    gtf_install_include_folder(${PROJECT_BINARY_DIR}/include)
    gtf_install_interface_folder(${PROJECT_SOURCE_DIR}/interface)
    gtf_install_interface_folder(${PROJECT_BINARY_DIR}/interface)
endmacro()

macro(gtf_check_unit_test_project_name name)
    string(REGEX MATCH "^(.*)UnitTest$" matched_name "${name}")
    if ("${matched_name}" STREQUAL "")
        message(FATAL_ERROR "Inivalid unit test project name: ${name}")
    endif()
endmacro()

macro(gtf_check_integration_test_project_name name)
    string(REGEX MATCH "^(.*)IntegrationTest$" matched_name "${name}")
    if ("${matched_name}" STREQUAL "")
        message(FATAL_ERROR "Inivalid integration test project name: ${name}")
    endif()
endmacro()

macro(gtf_add_hmi_unit_test name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_FILES;INSTALL_FOLDERS;INSTALL_DESTINATION"
         ""
         ${ARGN}
         )

    gtf_check_unit_test_project_name(${name})
    gtf_add_include_path(${PROJECT_SOURCE_DIR}/include)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/include)

    if((GTF_INSTALL_TYPE STREQUAL "Unittests") OR (GTF_INSTALL_TYPE STREQUAL "All"))
        eb_add_executable(${name}
            INCLUDE_PATH_VARS
                ${EB_TEMP_INCLUDE_PATH_VARS}
            INCLUDE_PATHS
                ${EB_TEMP_INCLUDE_PATHS}
                ${googletest_main_INCLUDE_PATHS}
            LIB_PATH_VARS
                ${EB_TEMP_LIB_PATH_VARS}
            LIB_VARS
                ${EB_TEMP_LIB_VARS}
            SOURCES
                ${Gtf_SOURCE_DIR}/test_system/src/main.cpp
                ${EB_TEMP_SOURCES}
            LIBS
                ${EB_TEMP_LIBS}
                googletest_main
                GtfCore
            DEFINITIONS
                ${EB_TEMP_DEFINITIONS}
            FLAGS
                ${EB_TEMP_FLAGS}
                ${googletest_FLAGS}
        )

        set_property (TARGET ${name} PROPERTY FOLDER "TF tests/hmi")

        set(MODULE_INSTALL_DESTINTATION ${GTF_INSTALL_SUBFOLDER}bin)

        eb_install("Unittests"
            FILES
                ${EB_TEMP_INSTALL_FILES}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        eb_install("Unittests"
            DIRECTORY
                ${EB_TEMP_INSTALL_FOLDERS}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        update_LinkerFlag_ForMapfile(${name})
        postBuild_Executable_StripAndInstall(${name}
                                             "Unittests"
                                             "${MODULE_INSTALL_DESTINTATION}")
    endif()

    gtf_install_include_folder(${PROJECT_SOURCE_DIR}/include)
    gtf_install_include_folder(${PROJECT_BINARY_DIR}/include)

endmacro()

macro(gtf_add_hmi_integration_test name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_FILES;INSTALL_FOLDERS;INSTALL_DESTINATION"
         ""
         ${ARGN}
         )

    gtf_check_integration_test_project_name(${name})

    if((CMAKE_BUILD_TYPE STREQUAL "Debug") AND (GTF_INSTALL_TYPE STREQUAL "All"))
        eb_add_executable(${name}
            INCLUDE_PATH_VARS
                ${EB_TEMP_INCLUDE_PATH_VARS}
            INCLUDE_PATHS
                ${EB_TEMP_INCLUDE_PATHS}
            LIB_PATH_VARS
                ${EB_TEMP_LIB_PATH_VARS}
            LIB_VARS
                ${EB_TEMP_LIB_VARS}
            SOURCES
                ${Gtf_SOURCE_DIR}/test_system/src/main.cpp
                ${EB_TEMP_SOURCES}
            LIBS
                ${EB_TEMP_LIBS}
                googletest_main
                TfTestSystem
                GtfCore
            DEFINITIONS
                ${EB_TEMP_DEFINITIONS}
            FLAGS
                ${EB_TEMP_FLAGS}
                ${googletest_FLAGS}
        )

        set_property (TARGET ${name} PROPERTY FOLDER "TF tests/integration")

        set(MODULE_INSTALL_DESTINTATION ${GTF_INSTALL_SUBFOLDER}bin)

        eb_install("Unittests"
            FILES
                ${EB_TEMP_INSTALL_FILES}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        eb_install("Unittests"
            DIRECTORY
                ${EB_TEMP_INSTALL_FOLDERS}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        update_LinkerFlag_ForMapfile(${name})
        postBuild_Executable_StripAndInstall(${name}
                                             "Unittests"
                                             "${MODULE_INSTALL_DESTINTATION}")
    endif()

endmacro()

macro(gtf_add_hmi_assembly name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    if(EB_PLATFORM_OS_FAMILY_WINDOWS)
        configure_file(${Gtf_SOURCE_DIR}/build_system/src/windows/dll_resource.rc.in ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.rc)
        configure_file(${Gtf_SOURCE_DIR}/build_system/src/windows/dll_resource.h.in ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.h)

        # copy the icon for the windows DLL
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Gtf_SOURCE_DIR}/build_system/resources/icons/eb32.ico ${PROJECT_BINARY_DIR}/include/gtf/${name}/eb32.ico)

        set(${name}_resources ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.rc
                              ${PROJECT_BINARY_DIR}/include/gtf/${name}/resource.h
                              ${PROJECT_BINARY_DIR}/include/gtf/${name}/eb32.ico)
    endif(EB_PLATFORM_OS_FAMILY_WINDOWS)

    eb_add_library(${name}
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
            ${${name}_resources}
            ${EB_TEMP_SOURCES}
            ${GTF_TRACY_CLIENT_DLL_SOURCES}
        LIBS
            ${EB_TEMP_LIBS}
            GtfCore
        DEFINITIONS
            ${EB_TEMP_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

    set_property (TARGET ${name} PROPERTY FOLDER "TF projects/hmi/assemblies")

    eb_install("SDK"
        TARGETS
            ${name}
        RUNTIME DESTINATION
            ${GTF_INSTALL_SUBFOLDER}bin
        LIBRARY DESTINATION
            ${GTF_INSTALL_SUBFOLDER}bin
    )

    update_LinkerFlag_ForMapfile(${name})
    postBuild_SharedLib_StripAndInstall(${name}
                                        "SDK"
                                        "${GTF_INSTALL_SUBFOLDER}bin")
endmacro()

macro(gtf_add_core_unit name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;SHARED_DEFINITIONS;STATIC_DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    gtf_add_include_path(${PROJECT_SOURCE_DIR}/include)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/include)
    gtf_add_include_path(${PROJECT_SOURCE_DIR}/interface)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/interface)

    if(EB_PLATFORM_OS_FAMILY_WINDOWS)
        set (GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER "windows")
    elseif(EB_PLATFORM_OS_FAMILY_POSIX)
        set (GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER "posix")
    endif()

    if(GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER)
        gtf_add_include_path(${PROJECT_SOURCE_DIR}/include/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_add_include_path(${PROJECT_BINARY_DIR}/include/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_add_include_path(${PROJECT_SOURCE_DIR}/interface/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_add_include_path(${PROJECT_BINARY_DIR}/interface/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
    endif()

    eb_add_convenience_library(GtfCore
        INCLUDE_PATH_VARS
            ${EB_TEMP_INCLUDE_PATH_VARS}
        INCLUDE_PATHS
            ${PROJECT_BINARY_DIR}/src
            ${EB_TEMP_INCLUDE_PATHS}
        LIB_PATH_VARS
            ${EB_TEMP_LIB_PATH_VARS}
        LIB_VARS
            ${EB_TEMP_LIB_VARS}
        SOURCES
            ${EB_TEMP_SOURCES}
        LIBS
            ${EB_TEMP_LIBS}
        SHARED_DEFINITIONS
            ${EB_TEMP_SHARED_DEFINITIONS}
        STATIC_DEFINITIONS
            ${EB_TEMP_STATIC_DEFINITIONS}
        FLAGS
            ${EB_TEMP_FLAGS}
    )

    gtf_install_include_folder(${PROJECT_SOURCE_DIR}/include)
    gtf_install_include_folder(${PROJECT_BINARY_DIR}/include)
    gtf_install_interface_folder(${PROJECT_SOURCE_DIR}/interface)
    gtf_install_interface_folder(${PROJECT_BINARY_DIR}/interface)

    if(GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER)
        gtf_install_include_folder(${PROJECT_SOURCE_DIR}/include/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_install_include_folder(${PROJECT_BINARY_DIR}/include/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_install_interface_folder(${PROJECT_SOURCE_DIR}/interface/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
        gtf_install_interface_folder(${PROJECT_BINARY_DIR}/interface/pd/${GTF_ADD_CORE_UNIT_PLATFORM_SUBFOLDER})
    endif()
endmacro()

macro(gtf_add_core_unit_test name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_FILES;INSTALL_FOLDERS;INSTALL_DESTINATION"
         ""
         ${ARGN}
         )

    gtf_check_unit_test_project_name(${name})
    gtf_add_include_path(${PROJECT_SOURCE_DIR}/include)
    gtf_add_include_path(${PROJECT_BINARY_DIR}/include)

    if((GTF_INSTALL_TYPE STREQUAL "Unittests") OR (GTF_INSTALL_TYPE STREQUAL "All"))
        eb_add_executable(${name}
            INCLUDE_PATH_VARS
                ${EB_TEMP_INCLUDE_PATH_VARS}
            INCLUDE_PATHS
                ${EB_TEMP_INCLUDE_PATHS}
                ${googletest_main_INCLUDE_PATHS}
            LIB_PATH_VARS
                ${EB_TEMP_LIB_PATH_VARS}
            LIB_VARS
                ${EB_TEMP_LIB_VARS}
            SOURCES
                ${Gtf_SOURCE_DIR}/test_system/src/main.cpp
                ${EB_TEMP_SOURCES}
            LIBS
                ${EB_TEMP_LIBS}
                googletest_main_static
                GtfCore_static
            DEFINITIONS
                ${EB_TEMP_DEFINITIONS}
                ${GTF_CONVENIENCE_LIB_GtfCore_RESULT_STATIC_DEFINITIONS}
            FLAGS
                ${EB_TEMP_FLAGS}
                ${googletest_FLAGS}
        )

        set_property (TARGET ${name} PROPERTY FOLDER "TF tests/core")

        set(MODULE_INSTALL_DESTINTATION ${GTF_INSTALL_SUBFOLDER}bin)

        eb_install("Unittests"
            FILES
                ${EB_TEMP_INSTALL_FILES}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        eb_install("Unittests"
            DIRECTORY
                ${EB_TEMP_INSTALL_FOLDERS}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        update_LinkerFlag_ForMapfile(${name})
        postBuild_Executable_StripAndInstall(${name}
                                             "Unittests"
                                             "${MODULE_INSTALL_DESTINTATION}")
    endif()

    gtf_install_include_folder(${PROJECT_SOURCE_DIR}/include)
    gtf_install_include_folder(${PROJECT_BINARY_DIR}/include)

endmacro()

macro(gtf_add_core_integration_test name)
    gtf_parse_arguments(EB_TEMP
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;LIBS;DEFINITIONS;FLAGS;INSTALL_FILES;INSTALL_FOLDERS;INSTALL_DESTINATION"
         ""
         ${ARGN}
         )

    gtf_check_integration_test_project_name(${name})

    if((CMAKE_BUILD_TYPE STREQUAL "Debug") AND (GTF_INSTALL_TYPE STREQUAL "All"))
        eb_add_executable(${name}
            INCLUDE_PATH_VARS
                ${EB_TEMP_INCLUDE_PATH_VARS}
            INCLUDE_PATHS
                ${EB_TEMP_INCLUDE_PATHS}
            LIB_PATH_VARS
                ${EB_TEMP_LIB_PATH_VARS}
            LIB_VARS
                ${EB_TEMP_LIB_VARS}
            SOURCES
                ${Gtf_SOURCE_DIR}/test_system/src/main.cpp
                ${EB_TEMP_SOURCES}
            LIBS
                ${EB_TEMP_LIBS}
                googletest_main
                TfTestSystem
                GtfCore
            DEFINITIONS
                ${EB_TEMP_DEFINITIONS}
            FLAGS
                ${EB_TEMP_FLAGS}
                ${googletest_FLAGS}
        )

        set_property (TARGET ${name} PROPERTY FOLDER "TF tests/integration")

        set(MODULE_INSTALL_DESTINTATION ${GTF_INSTALL_SUBFOLDER}bin)

        eb_install("Unittests"
            FILES
                ${EB_TEMP_INSTALL_FILES}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        eb_install("Unittests"
            DIRECTORY
                ${EB_TEMP_INSTALL_FOLDERS}
            DESTINATION
                ${MODULE_INSTALL_DESTINTATION}/${EB_TEMP_INSTALL_DESTINATION})

        update_LinkerFlag_ForMapfile(${name})
        postBuild_Executable_StripAndInstall(${name}
                                             "Unittests"
                                             "${MODULE_INSTALL_DESTINTATION}")
    endif()

endmacro()

macro(eb_set varname)
    set(${varname} ${ARGN} CACHE INTERNAL "" FORCE)
    list(REMOVE_DUPLICATES ${varname})
    set(${varname} ${${varname}} CACHE INTERNAL "" FORCE)
endmacro(eb_set)

###################################################################
#
#  eb_add_headers(name
#           [FILE1 ...] )
#
#  Adds an empty pseudo target, that ensures that header files are visible in
#  IDEs like visual studio.
#
#
macro(eb_add_headers name)
    gtf_parse_arguments(EB_ADD_HEADERS
         ""
         ""
         ${ARGN}
         )

    gtf_create_source_group(
              FOLDERS
                  ${CMAKE_SOURCE_DIR}
                  ${PROJECT_SOURCE_DIR}
                  ${PROJECT_BINARY_DIR}
              SOURCES
                  ${EB_ADD_HEADERS_DEFAULT_ARGS}
              )
    add_custom_target(${name}
        COMMENT "Pseudo Target ${name} to make headers visible"
        SOURCES ${EB_ADD_HEADERS_DEFAULT_ARGS} )

    set_property (GLOBAL APPEND PROPERTY EB_TARGETS ${name})

endmacro(eb_add_headers)


#####################################################################
# eb_configure_file(headername)
#
# headername must be a relative path,
# the generated header will be created in
#   ${PROJECT_BINARY_DIR}/${headername}
# provided that the input file is stored in
#   ${PROJECT_SOURCE_DIR}/${headername}.in
#
# Add ${PROJECT_BINARY_DIR}/path/to/header to your include paths
macro(eb_configure_file HEADER)
    if(IS_ABSOLUTE ${HEADER})
        message(ERROR "[eb_configure_file] Header file must be specified as relative path")
    endif()
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${HEADER}.in)
        message(ERROR "[eb_configure_file] Header template file ${PROJECT_SOURCE_DIR}/${HEADER}.in")
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/${HEADER}.in ${PROJECT_BINARY_DIR}/${HEADER})
endmacro(eb_configure_file)

##################################################################
#
# Doxygen variable(s)
set(DOXYGEN_PATTERNS "${CMAKE_CXX_SOURCE_FILE_EXTENSIONS};h;inl;hpp;hxx;h++")
set(GTF_DOXYGEN_SOURCES "" CACHE STRING "Files to include when generating doxygen" FORCE)

macro(eb_doxygen_add_file _file)
    # UseDoxygen module named it _DIRS but it works for files too
    get_filename_component(_file_ext "${_file}" EXT)
    get_filename_component(_abs_file "${_file}" ABSOLUTE)
    string(REPLACE "." "" _file_ext "${_file_ext}")
    list(FIND DOXYGEN_PATTERNS "${_file_ext}" EXT_INDEX)
    if(EXT_INDEX GREATER -1)
        list(FIND GTF_DOXYGEN_SOURCES "${_abs_file}" FILE_INDEX)
        if(FILE_INDEX EQUAL -1)
            set(GTF_DOXYGEN_SOURCES ${GTF_DOXYGEN_SOURCES};${_abs_file} CACHE  STRING "" FORCE)
        endif()
    endif()
endmacro(eb_doxygen_add_file)

macro(eb_doxygen_add_headers)
    gtf_parse_arguments(EB_ADD_DOXY_HDRS
        "FILES;DIRECTORY;DESTINATION;PROGRAMS;CONFIGURATIONS;COMPONENT;RENAME;RUNTIME;LIBRARY;ARCHIVE;TARGETS"
        "OPTIONAL"
         ${ARGN}
         )
    foreach(_directory ${EB_ADD_DOXY_HDRS_DIRECTORY})
        file(GLOB_RECURSE _all_files "${_directory}/*")
        foreach(_file ${_all_files})
            eb_doxygen_add_file("${_file}")
        endforeach()
    endforeach()

    foreach(_file ${EB_ADD_DOXY_HDRS_FILES})
        eb_doxygen_add_file("${_file}")
    endforeach()
endmacro(eb_doxygen_add_headers)
#####################################################################
# eb_install(install_configurations [install_args])
#
# calls the install command when GTF_INSTALL_TYPE is one of install_configurations
macro(eb_install INSTALL_CONFIGURATIONS)
    set(INSTALL_CONFIGURATIONS "${INSTALL_CONFIGURATIONS}") # WTF???
    foreach(install_config ${GTF_INSTALL_TYPE})
        list(FIND INSTALL_CONFIGURATIONS ${install_config} EB_INSTALL_DO_IT)
        if(EB_INSTALL_DO_IT GREATER -1 OR ${install_config} STREQUAL All)
            install(${ARGN})
            eb_doxygen_add_headers(${ARGN})
            break()
        endif()
    endforeach()
endmacro(eb_install)


#####################################################################
# eb_add_doxygen(install_configurations [install_args])
#
# adds files to doxygen generation when GTF_INSTALL_TYPE is one of install_configurations
macro(eb_add_doxygen INSTALL_CONFIGURATIONS)
endmacro(eb_add_doxygen)

macro(eb_add_doxygen_absolute INSTALL_CONFIGURATIONS)
endmacro(eb_add_doxygen_absolute INSTALL_CONFIGURATIONS)

macro(eb_replace_flags flags from to)
    #MESSAGE("modifying ${flags} from ${from} to ${to}")
    STRING(REPLACE "${from}" "${to}" Temp "${${flags}}")
    SET(${flags} ${Temp})
    #MESSAGE("flags ${flags} are now set to ${Temp}")
endmacro(eb_replace_flags)

#####################################################################
# eb_set_entry_point(target console_enabled)
#
# Configure the Win32 entry point and subsystem by linker flags
#
macro(eb_set_entry_point target console_enabled)
    if(EB_PLATFORM_OS_FAMILY_WINDOWS)
        get_property(
                TMP_FLAGS
            TARGET
                ${target}
            PROPERTY LINK_FLAGS
        )
        separate_arguments(TMP_FLAGS)
        list(APPEND TMP_FLAGS "/ENTRY:WinMainCRTStartup")
        list(REMOVE_DUPLICATES TMP_FLAGS)
        if (${console_enabled})
            list(REMOVE_ITEM TMP_FLAGS "/SUBSYSTEM:WINDOWS")
            list(APPEND TMP_FLAGS "/SUBSYSTEM:CONSOLE")
        else()
            list(APPEND TMP_FLAGS "/SUBSYSTEM:WINDOWS")
            list(REMOVE_ITEM TMP_FLAGS "/SUBSYSTEM:CONSOLE")
        endif()
        list(REMOVE_DUPLICATES TMP_FLAGS)
        eb_join_list(TMP_FLAGS "${TMP_FLAGS}")
        set_property(
            TARGET
                ${target}
            PROPERTY LINK_FLAGS
                ${TMP_FLAGS}
        )
    endif()
endmacro(eb_set_entry_point)

#####################################################################
# eb_write_generated_file_header()
#
# writes elektrobit header
#
macro(eb_write_generated_file_header OUTPUT_PATH)
    file(WRITE ${OUTPUT_PATH}
        "////////////////////////////////////////////////////////////////////////////////\n"
        "// Copyright (c) Elektrobit Automotive GmbH\n"
        "// Alle Rechte vorbehalten. All Rights Reserved.\n"
        "//\n"
        "// Information contained herein is subject to change without notice.\n"
        "// Elektrobit retains ownership and all other rights in the software and each\n"
        "// component thereof.\n"
        "// Any reproduction of the software or components thereof without the prior\n"
        "// written permission of Elektrobit is prohibited.\n"
        "////////////////////////////////////////////////////////////////////////////////\n"
        "\n"
        )
endmacro(eb_write_generated_file_header)

#####################################################################
# eb_write_configured_string_to_file(OUTPUT_PATH, WRITE_STRING)
#
# replaces variables in given string by their values and writes data
# to the output file
#
macro(eb_write_configured_string_to_file OUTPUT_PATH INPUT_STRING)
    STRING(CONFIGURE ${INPUT_STRING} OUTPUT_STRING @ONLY ESCAPE_QUOTES)
    file(APPEND ${OUTPUT_PATH} ${OUTPUT_STRING})
endmacro(eb_write_configured_string_to_file)

MACRO(eb_set_test_folder TARGET)
    set_property (TARGET ${TARGET} PROPERTY FOLDER "TF tests/legacy")
ENDMACRO()

MACRO(eb_add_third_party_library TARGET)
eb_add_library(${TARGET} ${ARGN})
set_property (TARGET ${TARGET} PROPERTY FOLDER "TF projects/third_party")
ENDMACRO()
