# TagFilterPlugin

Shows how to modify the user interface of EB GUIDE Studio.\
With this example, you create a customized tag filter for the Datapool component.

## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Studio\\EB\_GUIDE\_Studio\_examples.sln_ solution file.
2. Compile the _TagFilterPlugin_ project.\
The _TagFilterPlugin.dll_ library file is created. 

### Run

1. Copy the _TagFilterPlugin.dll_ into _$GUIDE\_INSTALL\_PATH\\studio\\lib\\ui\\_.
2. Start EB GUIDE Studio.
3. Create an EB GUIDE project.
4. In the Datapool component, click into the tag filter box.
5. Enter "Language support", "Skin support", or "No variant support".
   The filter results display the custom tag filters.