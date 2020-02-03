# MonitorUiExtensionDatapool

Shows how to add a list of all datapool items in a separate component in EB GUIDE Monitor.
In this example, you use the following services:
* _IModelService_ to list all datapool items from the model
* _IDatapoolService_ to register for datapool updates and to read the datapool list items
* _IConnectionService_ to check if EB GUIDE Monitor is connected to a running target framework

## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _studio\_examples.sln_ solution file.
2. Compile the _MonitorUiExtensionEvents_ project.
The _MonitorUiExtensionEvents.dll_ library file is created.

### Run

1. Copy the _MonitorUiExtensionDatapool.dll_ into _$GUIDE\_INSTALL\_PATH\\monitor__\\lib\\ui\\_ folder.
2. Start EB GUIDE Monitor.
3. To open the extension, select _Layout_ in the menu and click _Monitor Extension_.
