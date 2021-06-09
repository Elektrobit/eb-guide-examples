# MonitorRemoteViewPlugin

Shows how to take a single screenshot or several screenshots in a specified interval.

## Instructions

### Prerequisites

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup Visual Studio

1. In Visual Studio, open the _EB\_GUIDE\_Monitor\\EB\_GUIDE\_Monitor\_examples.sln_ solution file.
2. Compile the _MonitorRemoteViewPlugin_ project.\
The _MonitorRemoteViewPlugin_ library file is created.

### Setup EB GUIDE Studio
To be able to take screenshots, the remote framebuffer needs to be enabled. 
1. In EB GUIDE Studio, open your EB GUIDE project.
2. Select **Configure** > **Profiles** > **Simulation** and select **enableRemoteFrameBuffer**. 

### Run

1. Copy the  _MonitorRemoteViewPlugin.dll_ into  _$GUIDE\_INSTALL\_PATH\\tools\\monitor\\lib\\ui\\_.
2. Run Simulation  in EB GUIDE Studio, or launch EB GUIDE Monitor from your default installation path.
3. From the **Layout** menu, select **Model Viewer**.
4. To get a screenshot from the corresponding scene configured in EB GUIDE Studio, set the **Scene ID**.
5. To set an interval for how quickly the screenshots are taken and updated, define the **Frames per second**.
* Click **Refresh** to take a single screenshot from the model specified with the scene ID and display it in the **Model Viewer** component.
* Click **Auto Refresh** to take constant screenshots at the specified interval in 'Frames per second'\(FPS\) . Note that FPS is only estimated.
* Click **Save** to open a dialog to save the current screenshot as _.png_ file. 
* When **Auto Refresh** is enabled, you can click the image and touch events are sent to EB GUIDE GTF on the click position coordinates.
