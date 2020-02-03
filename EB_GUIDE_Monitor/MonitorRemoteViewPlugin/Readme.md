# MonitorRemoteViewPlugin

Shows how to take a single screenshot or several screenshots in a specified interval.

## Instructions

### Prerequisites

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup Visual Studio

1. In Visual Studio, open the studio_examples.sln solution file.
2. Compile the _MonitorRemoteViewPlugin_ project.
The _MonitorRemoteViewPlugin.dll_ library file is created.

### Setup EB GUIDE Studio
To be able to take screenshots, the remote framebuffer needs to be enabled. 
1. In EB GUIDE Studio, open your EB GUIDE project.
2. Select *Configure* > *Settings* > *Simulation* and select *enableRemoteFrameBuffer*. 
* ``Scene ID``: Gets the screenshot from the corresponding scene configured in EB GUIDE Studio.
* ``Frames per second``: Interval for how quickly the screenshots are taken and updated.

### Run

1. Copy the  _MonitorRemoteViewPlugin.dll_ into  _$GUIDE\_INSTALL\_PATH\\tools\\monitor\\lib\\ui\\_.
2. Run Simulation  in EB GUIDE Studio, or launch EB GUIDE Monitor from your default installation path.
3. From the ``Layout`` menu, select ``Model Viewer``.
4. Clicking ``Refresh`` takes a single screenshot from the model specified with the scene ID and displays it in the **Model Viewer** component.
5. Clicking ``Auto Refresh`` takes constant screenshots at the specified interval in 'Frames per second'\(FPS\) . Note that FPS is only estimated.
6. Clicking ``Save`` opens a dialog to save the current screenshot as _.png_ file. 
7. When ``Auto Refresh`` is enabled, you can click the image and touch events are sent to EB GUIDE GTF on the click position coordinates.

### Known issues

* After saving the screenshot, ``Auto refresh`` stops.\
Workaround: Re-enable ``Auto refresh``.