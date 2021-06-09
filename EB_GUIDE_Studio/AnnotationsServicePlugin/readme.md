
# AnnotationsServicePlugin

Shows how to customize the user interface of EB GUIDE Studio.\
In this example, you create customized:
* Model elements
* Services

## Instructions

### Prerequisite
* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup
1. In Visual Studio, open the _EB\_GUIDE\_Studio\\EB\_GUIDE\_Studio\_examples.sln_ solution file.
2. Compile the _AnnotationsServicePlugin_ project.\
The _AnnotationsServicePlugin.dll_ library file is created.
3. Compile the _AnnotationsUiPlugin_ project.\
The _AnnotationsUiPlugin.dll_ library file is created. 

### Run
1. Copy the _AnnotationsServicePlugin.dll_ into _$GUIDE_INSTALL_PATH\\studio\\lib\\core\\_.
2. Copy the _AnnotationsUiPlugin.dll_ into _$GUIDE_INSTALL_PATH\\studio\\lib\\ui\\_.
3. Start EB GUIDE Studio.
4. Create a new EB GUIDE project or load an existing project.
5. Select *Layout* in the menu bar and click the entry for the Annotations component.\
The _Annotations_ component is added to the user interface of EB GUIDE Studio.

# Note
Model elements have to be available when the model is loaded. If the model elements are 
defined in UI components and the model is loaded via the command line, loading of the model
will fail.
