project("DependencyResolverExample")

gtf_add_sdk_example(
    SOURCES
        src/DependencyResolvingPlugin.cpp
        src/RunStageImpl.cpp
        src/RunStageImpl.h
        src/TemperatureConverter.h
        src/TemperatureConverterToFahrenheit.h
        src/TemperatureConverterToFahrenheit.cpp
        src/TemperatureConverterToKelvin.h
        src/TemperatureConverterToKelvin.cpp
        src/Tracing.h
    )
