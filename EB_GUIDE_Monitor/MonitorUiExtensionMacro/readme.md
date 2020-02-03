 # MonitorUiExtensionMacro

Shows how to track EB GUIDE Monitor sent messages, and convert them into an EB GUIDE Monitor script.

In this example you use the following services:
* _IMacroService_ to discover macro recorders and to start a macro recording session
* _IMacroRecordSession_ to start or stop the macro recording
* _IMacro_ to record the macro that can be serialized

The following classes are used to convert the sent messages to script:
* _SentMessageRecorder_ listens to sent messages and stores them as _IMacroSteps_
* _IMacroToScriptBuilder_ converts stored messages to a script
* _MessageMacro_ (implements _IMacro_) uses implementation of _IMacroToScriptBuilder_ to convert the stored message _IMacroSteps_ to an EB GUIDE Monitor script, it can also serialize the script


## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Monitor\_examples.sln_ solution file.
2. Compile the _MonitorUiExtensionMacro_ project.
The _MonitorUiExtensionMacro.dll_ library file is created.

### Run

1. Copy the _MonitorUiExtensionMacro.dll_ into _$GUIDE\_INSTALL\_PATH\\monitor__\\lib\\ui\\_ folder.
2. Start EB GUIDE Monitor.
3. To open the extension, select **Layout** in the menu and click **Macro Recorder**.\
The UI of the message recorder opens.
4. Click _Run_ to start the macro recording.\
Now you can change values of datapool items or trigger events.
5. CLick _Build_ to serialize the recorded messages to a script file.
6. Click _Stop_ to stop the recorder.
