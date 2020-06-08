project("BezierCurveExample")

gtf_add_sdk_example(
    SOURCES
        src/BezierCurve.h
        src/BezierFunction.h
        src/BezierCurveDescriptorProvider.cpp
        src/BezierCurveDescriptorProvider.h
        src/BezierCurvePlugin.cpp
        src/Curve.h
        src/DelayedFixedCurve.cpp
        src/DelayedFixedCurve.h     
        src/PreparedStageImpl.cpp
        src/PreparedStageImpl.h
        src/Properties.cpp
        src/Properties.h
        src/Tracing.h

    )
