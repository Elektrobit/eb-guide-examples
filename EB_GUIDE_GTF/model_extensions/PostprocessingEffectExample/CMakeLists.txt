project("PostprocessingEffectExample")

gtf_add_sdk_example(
    SOURCES
        src/PostprocessingEffectPlugin.cpp
        src/GLES20Shader.h
        src/GLES3Shader.h
        src/PostprocessingExampleEffectPasses.cpp
        src/PostprocessingExampleEffectPasses.h
        src/PostprocessingExampleEffectFactory.cpp
        src/PostprocessingExampleEffectFactory.h
	)
