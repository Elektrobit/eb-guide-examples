project("TriangleWidget")

gtf_add_sdk_example(
    SOURCES
        src/TrianglePlugin.cpp
        src/TriangleDesc.h
        src/TriangleDesc.cpp
        src/TriangleRenderer.cpp
        src/TriangleRenderer.h
        src/PreparedStageImpl.h
        src/PreparedStageImpl.cpp
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/GLES20Shader.h
        src/GLES3Shader.h
    )
