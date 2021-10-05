# Overview

BAlertAthenaCLI creates an LSL stream and streams data over LSL from a B-Alert X10 or X24 system by Advanced Brain Monitoring (http://www.b-alert.com/).

It is based on the [App-BAlert LSL app](https://github.com/labstreaminglayer/App-BAlert/releases) but does not require Qt (somewhat simplifying the build process), and is designed to be run from the command line. 

It requires the [Athena SDK](https://www.advancedbrainmonitoring.com/products/b-alert-software#section-b-alert-live) from ABM. It does not required the [BAlert-Live](https://www.advancedbrainmonitoring.com/products/b-alert-software#section-b-alert-live) application to run, as it connects directly to the device. However, it does require the BAlert-Live application to be installed as certain configuration files from BAlert-Live are required. 

# Installation 

  * Unzip the LSLBAlertAthenaCLI release .zip into a folder. It will include the .exe executable as well as the LSL dll (`lsl.dll`). 
  * Copy the contents of the `bin` folder of the ABM Athena SDK into the same folder. 
  * Copy the entire `Config` folder from the installed BAlert-Live app into the same folder (as a subfolder named `Config`).

# Usage
  * Make sure that the device is turned on and properly connected.
  * Run the LSLBAlertAthenaCLI.exe application. A console window will open and a descriptive message will appear. 
  * The application will automatically connect to the device and create an LSL stream with the name "BAlert" and type "EEG".

# Build Instructions

**Dependencies**:

* [liblsl i386 release v 1.14.0](https://github.com/sccn/liblsl/releases/tag/v1.14.0) -- download and extract. (Note: Only tested with liblsl 1.14; may not work with 1.15 or higher.)
* ABM BAlert Athena SDK.
* The Windows 10 SDK 10.0.20348.0 or higher for C++ 17 compatibility ([see here](https://docs.microsoft.com/en-us/cpp/overview/install-c17-support?view=msvc-160)). This may be installable as a VisualStudio component or separately. 

1. Clone or download this repository.
2. Copy/move the ABM Athena SDK to a folder named `SDK` that is parallel to the root folder of this repository. 
3. Move the liblsl folder (`liblsl-1.14.0-Win_i386`) so that it is parallel to the root folder of this repository. (So you should have a folder containing three folders: this repo, the SDK, and liblsl)
4. Double-click (or open) the `BAlertAthena.sln` to open the project in Visual Studio 2019.
5. Under Project, Properties, C/C++, Language, set the C++ Language Standard property to `ISO C++17 Standard`.
6. Select `Release` and `x86` from the target dropdowns.
7. Build. The executable will be in the `Release` folder. 
8. Copy the `lsl.dll` file from the `liblsl-1.14.0-Win_i386\bin` folder into the `Release` folder. 
9. Copy all of the dlls from the `SDK\bin` folder into the `Release` folder. 
10. Copy the `Config` folder from the BAlert-Live app (as indicated under Installation above) to your `Release` folder (so that it is a subfolder called `Config` in the folder where your executable is located).
