project("ScriptFunction")

gtf_add_sdk_example(
    SOURCES
        src/PreparedStageImpl.cpp
        src/PreparedStageImpl.h
        src/LoadedStageImpl.cpp
        src/LoadedStageImpl.h
        src/ScriptFunctionPlugin.cpp
        src/ScriptFunctionDescriptorProvider.cpp
        src/ScriptFunctionDescriptorProvider.h
        src/ScriptFunctionNames.h
    )
