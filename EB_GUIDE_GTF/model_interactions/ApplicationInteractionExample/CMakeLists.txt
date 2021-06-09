project("ApplicationInteractionExample")

gtf_add_sdk_example(
    SOURCES
        src/ApplicationInteractionPlugin.cpp
        src/Mandelbrot2MemImageConverter.h
        src/Mandelbrot2MemImageConverter.cpp
        src/MandelbrotImage.h
        src/MandelbrotImage.cpp
        src/MandelbrotSet.h
        src/MandelbrotSet.cpp
        src/RunStageImpl.h
        src/RunStageImpl.cpp
    )
