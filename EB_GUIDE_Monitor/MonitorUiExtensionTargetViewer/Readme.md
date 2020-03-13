# MonitorUiExtensionTargetViewer

Shows how to take and save a screenshot from the currently running EB GUIDE model.\
In this example, you use the following services:
* _IScreenshotService_ to take a screenshot from a running target framework
* _IScreenshotSavingUtility_ to save a screenshot
* _IPersistency_ interface to define launch settings for EB GUIDE Monitor
* _IConnectionService_ to check if EB GUIDE Monitor is connected to a running target framework

## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Monitor\\EB\_GUIDE\_Monitor\_examples.sln_ solution file.
2. Compile the _MonitorUiExtensionTargetViewer_ project.\
The _MonitorUiExtensionTargetViewer_ library file is created.

### Run

1. Copy the _MonitorUiExtensionTargetViewer.dll_ into _$GUIDE\_INSTALL\_PATH\\tools\\monitor\\lib\\ui\\_.
2. Start EB GUIDE Monitor.
3. To open the extension, select **Layout** in the menu and click **Monitor Target Viewer Extension**.
