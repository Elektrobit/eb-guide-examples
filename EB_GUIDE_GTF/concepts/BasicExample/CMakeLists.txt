project("BasicExample")

gtf_add_sdk_example(
    SOURCES
        src/BasicPlugin.cpp
        src/RunStageImpl.h
        src/RunStageImpl.cpp
        src/Tracing.h
    )
