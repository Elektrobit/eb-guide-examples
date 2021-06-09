project("GaussianBlur")

gtf_add_sdk_example(
    SOURCES
        src/GaussianBlurPlugin.cpp
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/GaussianBlur.cpp
        src/GaussianBlur.h
        src/GaussianBlurRenderObject.h
        src/GaussianBlurRenderObject.cpp
        src/GaussianBlurStackFrame.h
        src/GaussianBlurStackFrame.cpp
        src/GaussianBlurCommand.h
        src/GaussianBlurCommand.cpp
        src/GLES3Shader.h
        src/GLES20Shader.h
    )
