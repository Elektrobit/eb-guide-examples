# MonitorUiExtensionEvents

Shows how to add a list of all events in a separate component in EB GUIDE Monitor.\
In this example you use the following services:
* _IModelService_ to list all events from the model
* _IEventService_ to fire an event
* _IConnectionService_ to check if EB GUIDE Monitor is connected to a running target framework


## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Monitor\\EB\_GUIDE\_Monitor\_examples.sln_ solution file.
2. Compile the _MonitorUiExtensionEvents_ project.\
The _MonitorUiExtensionEvents_ library file is created.

### Run

1. Copy the _MonitorUiExtensionEvents.dll_ into  _$GUIDE\_INSTALL\_PATH\\tools\\monitor\\lib\\ui\\_.
2. Start EB GUIDE Monitor.
3. To open the extension, select **Layout** in the menu and click **Monitor Events Extension**.
