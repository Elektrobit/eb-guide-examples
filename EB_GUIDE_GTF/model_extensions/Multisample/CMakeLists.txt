project("Multisample")

gtf_add_sdk_example(
    SOURCES
        src/MultisamplePlugin.cpp
        src/Multisample.h
        src/Multisample.cpp
        src/MultisampleCommand.h
        src/MultisampleCommand.cpp
        src/MultisampleRenderObject.h
        src/MultisampleRenderObject.cpp
        src/MultisampleStackFrame.h
        src/MultisampleStackFrame.cpp
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/GLES20Shader.h
        src/GLES3Shader.h
    )
