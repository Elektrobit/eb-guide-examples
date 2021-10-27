project("BloomEffect")

gtf_add_sdk_example(
    SOURCES
        src/BloomEffectPlugin.cpp
        src/BloomEffectRenderer.cpp
        src/BloomEffectRenderer.h
        src/LoadedStageImpl.cpp
        src/LoadedStageImpl.h
        src/GLES3Shader.h
    )
