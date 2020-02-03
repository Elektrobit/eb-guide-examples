project("ModelLauncher")

gtf_add_sdk_example(
    SOURCES
        src/PreparedStageImpl.cpp
        src/PreparedStageImpl.h
        src/LoadedStageImpl.cpp
        src/LoadedStageImpl.h
        src/ModelLauncherPlugin.cpp
        src/ModelLauncherDescriptorProvider.cpp
        src/ModelLauncherDescriptorProvider.h
        src/ModelLauncherNames.h
    )
