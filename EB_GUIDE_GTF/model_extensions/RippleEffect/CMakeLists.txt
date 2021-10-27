project("RippleEffect")

gtf_add_sdk_example(
    SOURCES
        src/RippleEffectPlugin.cpp
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/RippleEffect.cpp
        src/RippleEffect.h
        src/GLES3Shader.h
        src/GLES20Shader.h
    )
