project("CoreStageExample")

gtf_add_sdk_example(
    SOURCES
        src/EarlyStage.cpp
        src/EarlyStage.h
        src/EarlyStageProvider.cpp
        src/EarlyStageProvider.h
        src/StagePlugin.cpp
        src/PrepareStageImpl.cpp
        src/PrepareStageImpl.h
        src/LateStage.cpp
        src/LateStage.h
        src/LateStageProvider.cpp
        src/LateStageProvider.h
        src/Tracing.h
    )
