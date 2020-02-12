# CommandLinePlugin

Shows how to create your own command line option.\
In this example, the number of events of a model on project load is printed via a command line option. 

# Instructions


### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Studio\_examples.sln_ solution file.
2. Compile the _CommandLinePlugin_ project.\
The _CommandLinePlugin_ library file is created. 

### Run

* Copy the _CommandLinePlugin.dll_ into _$GUIDE_INSTALL_PATH\\studio\\lib\\core\\_.
* Run the EB GUIDE Studio command line with the custom option:
```
Studio.Console.exe -x -c validation_log.txt -o <project file>
```
