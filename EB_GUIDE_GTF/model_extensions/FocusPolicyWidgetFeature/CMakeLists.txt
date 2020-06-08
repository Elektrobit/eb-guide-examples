project("FocusPolicyWidgetFeature")

gtf_add_sdk_example(
    SOURCES
        src/FocusPolicyWidgetFeatureDesc.h
        src/FocusPolicyWidgetFeatureDesc.cpp
        src/FocusPolicyWidgetFeature.h
        src/FocusPolicyWidgetFeature.cpp
        src/FocusPolicyWidgetFeaturePlugin.cpp
        src/RandomFocusFeature.cpp
        src/RandomFocusFeature.h
        src/PreparedStageImpl.h
        src/PreparedStageImpl.cpp
    )
