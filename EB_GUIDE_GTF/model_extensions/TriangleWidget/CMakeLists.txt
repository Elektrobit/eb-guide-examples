project("TriangleWidget")

gtf_add_sdk_example(
    SOURCES
        src/TrianglePlugin.cpp
        src/TriangleRenderer.cpp
        src/TriangleRenderer.h
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/GLES20Shader.h
        src/GLES3Shader.h
    )
