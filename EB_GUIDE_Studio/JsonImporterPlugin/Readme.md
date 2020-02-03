# JsonImporterPlugin


Shows how to modify the user interface of EB GUIDE Studio.\
With this example you:
* Create a customized import menu for the menu bar 
* Read and import model data from a _.json_ file
* Update the model through changing the _.json_ file
* Create a toast notification to show import information

## Instructions

### Preparation

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _studio\_examples.sln_ solution file.
2. Compile the _JsonImporterPlugin_ project.\
The _JsonImporterPlugin_ library file is created. 

### Run

1. Copy the _JsonImporterPlugin.dll_ into _$GUIDE_INSTALL_PATH\\studio\\lib\\ui\\_.
2. Start EB GUIDE Studio.
3. Create a new EB GUIDE project or load an existing project.
4. Select the import item from the menu bar.
5. Select and open the _ExampleModel.json_ file from _\\JsonImporterPlugin\\Resources\\_.
