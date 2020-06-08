# ValidationPlugin

Shows how to write a custom validator to add entries to the **Problems** component in EB GUIDE Studio.

## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Studio\\EB\_GUIDE\_Studio\_examples.sln_ solution file. 
2. Compile the _ValidationPlugin_ project.\
The _ValidationPlugin_ library file is created.

### Run

1. Copy the _ValidationPlugin.dll_ into _$GUIDE\_INSTALL\_PATH\\studio\\lib\\core\\_.
2. Start EB GUIDE Studio.
3. Create a new EB GUIDE project or load an existing project.
4. Add a datapool item with language support. For more information on how to add language support to an EB GUIDE project, see the EB GUIDE Studio User guide.
5. To validate the project, press ``F6``.
