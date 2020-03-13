# EB GUIDE GTF extension and application examples

With EB GUIDE you can use extensions and applications to customize the visual appeareance and behavior of your EB GUIDE model. A collection of ready-to-use source code examples shows you how to create your own EB GUIDE GTF extensions and applications.

## Instructions

---
**NOTE: Preconfigured Visual Studio solution files**

All EB GUIDE GTF examples are provided with Visual Studio 2017 solution and project files. All Visual Studio solution files are preconfigured to use required files from the EB GUIDE default installation folder. If you have installed EB GUIDE in a custom folder, adapt the values of **Additional include directories** and **Additional library directories** in the Visual Studio project file.

---

### Prerequisite

* Visual Studio 2017 version 15.9 or later is installed
* EB GUIDE Studio is installed
* The source code of the extension example is locally available


### Building the EB GUIDE GTF examples with Visual Studio

1. In Visual Studio, open the solution file of the example. 
2. Run the build.\
If EB GUIDE Studio is not installed in the default path, adapt the paths accordingly in your Visual Studio project.
3. Copy the resulting _.dll_ files into the resource subfolder of the EB GUIDE project. Hereby you overwrite the existing _.dll_ files.\
The extension is now available in your EB GUIDE model.

### Building the EB GUIDE GTF examples with CMake generated Visual Studio solution file
With CMake 3.7.1 you can generate the Visual Studio solution file for the EB GUIDE examples. Afterwards, you can open the generated solution file with Visual Studio and build the _.dll_ files of all EB GUIDE examples at once.\
For information on how to use CMake, see <https://cmake.org/runningcmake>.

1. Start **cmake-gui** of CMake 3.7.1.
2. Set **Where is the source code** to the root folder of the EB GUIDE GTF examples.
3. Set **Where to build the binaries** to a dedicated working directory with write permissions, e.g. _C:\\Users\\...\\Desktop\\eb\_guide\_sdk\_examples\_build_.
4. Click **Configure**.\
Select the generator for **Visual Studio 15 2017 Win64** and agree on creating missing folders.
5. CMake will report an error in the configuration process.\
To solve the error, set the value of the CMake variable _CMAKE\_INSTALL\_PREFIX_ to a dedicated install folder with write permissions, e.g. _C:\\Users\\...\\Desktop\\eb\_guide\_sdk\_examples\_install_.
6. Set the value of the CMake variable _GTF\_TARGET\_SDK\_ROOT_ to the folder _GUIDE\_INSTALL\_PATH\\platform\\win64_.
7. Click **Configure**.\
The CMake error is solved.
8. Click **Generate**.\
Your Visual Studio solution for the EB GUIDE examples is now generated and you can close **cmake-gui**.
9. Start Visual Studio and open the _sdk\_examples.sln_ from the folder you configured previously for **Where to build the binaries** in the **cmake-gui**.
10. In the **Solution Explorer** of Visual Studio right-click **INSTALL**. Select **Build** from the context menu to build and install all EB GUIDE GTF example plug-ins.\
All EB GUIDE GTF example plug-ins are now available in the folder you configured previously for _CMAKE\_INSTALL\_PREFIX_ in the **cmake-gui**.
11. Copy the resulting _.dll_ files into the resource subfolder of the EB GUIDE projects. Hereby you overwrite the existing _.dll_ files.\
Your plug-ins are now available in your EB GUIDE models.


### Using the EB GUIDE GTF extension examples

1. Select an extension example and navigate to the folder _model_.
2. Double-click the EB GUIDE project file.\
Your extension is now available in your EB GUIDE model.

### Migrating EB GUIDE projects with custom widgets
If your EB GUIDE project contains custom widgets and you want to update your EB GUIDE software, you first need to build the custom widgets with the newest EB GUIDE GTF version. Afterwards, you can migrate the EB GUIDE Studio projects.\
The extension examples provided on https://www.elektrobit.com/ebguide/examples/, are compatible with the
newest EB GUIDE Studio version.

---
**NOTE: Check release notes**

Before you start the migration, check the EB GUIDE release notes for further migration instructions.

---

1. Build the custom widgets with the new EB GUIDE GTF version.
2. Copy the library files into the resource folder of your EB GUIDE project.
3. Migrate the EB GUIDE project by opening it with the new EB GUIDE Studio version and confirm the migration prompt.\
The EB GUIDE project is migrated to the new EB GUIDE Studio version.
