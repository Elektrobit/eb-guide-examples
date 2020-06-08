project("DownscalingWidgetFeature")

gtf_add_sdk_example(
    SOURCES
        src/DownscalingFeatureDesc.h
        src/DownscalingFeatureDesc.cpp
        src/DownscalingFeature.h
        src/DownscalingFeature.cpp
        src/DownscalingFeaturePlugin.cpp
        src/PreparedStageImpl.h
        src/PreparedStageImpl.cpp
    )
