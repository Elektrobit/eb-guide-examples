
# AnnotationsPlugin

Shows how to customize the user interface of EB GUIDE Studio.\
In this example, you create customized: 
* Docking components
* View models and views
* Reactions on selection changes
* Services
* Model elements

## Instructions

### Prerequisite
* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup
1. In Visual Studio, open the _EB\_GUIDE\_Studio\_examples.sln_ solution file.
2. Compile the _AnnotationsPlugin_ project.\
The _AnnotationsPlugin.dll_ library file is created. 

### Run
1. Copy the _AnnotationsPlugin.dll_ into _$GUIDE_INSTALL_PATH\\studio\\lib\\ui\\_.
2. Start EB GUIDE Studio.
3. Create a new EB GUIDE project or load an existing project.
4. Select *Layout* in the menu bar and click the entry for the Annotations component.\
The _Annotations_ component is added to the user interface of EB GUIDE Studio.
