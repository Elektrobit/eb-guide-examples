# TGAImageLoaderPlugin

Shows how to write a custom plugin supporting the preview and drag-and-drop functionality of TGA image resources in EB GUIDE Studio.

## Instructions

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of this extension example is locally available

### Setup

1. In Visual Studio, open the _EB\_GUIDE\_Studio\\EB\_GUIDE\_Studio\_examples.sln_ solution file. 
2. Compile the _TGAImageLoaderPlugin_ project.\
The _TGAImageLoaderPlugin_ library file is created.

### Run

1. Copy the _TGAImageLoaderPlugin.dll_ into _$GUIDE\_INSTALL\_PATH\\studio\\lib\\ui\\_.
2. Start EB GUIDE Studio.
3. Create a new EB GUIDE project or load an existing project.
4. Add a TGA image into the resource folder of the model.\
The TGA image is now available in the __Assets__ component.
For  more information on how to enable an EB GUIDE model to use and render TGA images inside a view, see [EB GUIDE GTF example](../../EB_GUIDE_GTF/model_extensions/TGAImageLoader/readme.md).
5. Once you enabled the EB GUIDE model to work with the TGA images, you can do one of the following:
    * From the __Assets__ component, drag a TGA image to the View.
    * Add an Image widget to the View. In the __Properties__ component, from the __image__ drop-down list box select the TGA image .
