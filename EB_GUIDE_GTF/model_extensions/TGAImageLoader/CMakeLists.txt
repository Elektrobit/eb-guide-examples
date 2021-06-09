project("TGAImageLoader")

gtf_add_sdk_example(
    SOURCES
        src/TgaImageLoaderPlugin.cpp
        src/InitializedStageImpl.cpp
        src/InitializedStageImpl.h
        src/DecoderImpl.cpp
        src/DecoderImpl.h
        src/Tga.cpp
        src/Tga.h
    )
