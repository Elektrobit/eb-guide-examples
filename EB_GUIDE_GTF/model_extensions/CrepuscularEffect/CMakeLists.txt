project("CrepuscularEffect")

gtf_add_sdk_example(
    SOURCES
        src/CrepuscularRenderer.h
        src/CrepuscularRenderer.cpp
        src/CrepuscularPlugin.cpp
        src/PreparedStageImpl.h
        src/PreparedStageImpl.cpp
        src/LoadedStageImpl.h
        src/LoadedStageImpl.cpp
        src/OpenGLES3Shader.h
    )
