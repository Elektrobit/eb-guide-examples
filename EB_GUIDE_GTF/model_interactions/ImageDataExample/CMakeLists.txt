project("ImageDataExample")

gtf_add_sdk_example(
    SOURCES
        src/ImageDataPlugin.cpp
        src/Mandelbrot2MemImageConverter.h
        src/Mandelbrot2MemImageConverter.cpp
        src/MandelbrotImage.h
        src/MandelbrotImage.cpp
        src/MandelbrotSet.h
        src/MandelbrotSet.cpp
        src/RunStageImpl.cpp
        src/RunStageImpl.h
    )
