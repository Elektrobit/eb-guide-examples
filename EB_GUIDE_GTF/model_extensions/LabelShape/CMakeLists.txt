project("LabelShape")

gtf_add_sdk_example(
    SOURCES
        src/SineLabelShape.h
        src/SineLabelShape.cpp
        src/LabelShapeAdder.h
        src/LabelShapeAdder.cpp
        src/ExtendedDynamicFeatureCollection.h
        src/ExtendedDynamicFeatureCollection.cpp
        src/LabelShapeFeatureDesc.h
        src/LabelShapeFeatureDesc.cpp
        src/LabelShapePlugin.cpp
        src/PreparedStageImpl.h
        src/PreparedStageImpl.cpp
    )
