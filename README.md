Elastos Carrier Native SDK
==========================
|Travis CI|Circle| AppVeyor|
|:-:|:-:|:-:|
|[![Build Status](https://travis-ci.org/elastos/Elastos.NET.Carrier.Native.SDK.svg)](https://travis-ci.org/elastos/Elastos.NET.Carrier.Native.SDK)|[![CircleCI](https://circleci.com/gh/elastos/Elastos.NET.Carrier.Native.SDK.svg?style=svg)](https://circleci.com/gh/elastos/Elastos.NET.Carrier.Native.SDK)|[![Build status](https://ci.appveyor.com/api/projects/status/uqjjonmks6ujvi52?svg=true)](https://ci.appveyor.com/project/elastos/elastos-net-carrier-native-sdk)

## Summary

Elastos Carrier is a decentralized and encrypted peer-to-peer (P2P) communication framework that routes network traffic between virtual machines and Decentralized Applications (DApps).

The authentication process of peer nodes utilizes the Elastos Decentralized ID (DID) sidechain. （TODO）

## Build from source

**CMake** is used to build, test and package the Elastos Carrier project in an operating system as well as compiler-independent manner.

Certain knowledge of CMake is required.

At the time of this writing, The compilation of sources works on **macOS**, **Linux** (Ubuntu, Debian etc.) and **Windows**, and provides the option to cross-compile for target systems of **iOS**, **Android** and **RaspberryPi**.

### Ubuntu

#### 1. Brief introduction

On Ubuntu / Debian / Linux, besides the compilation for the host itself, cross-compilation is possible for the following targets:

* Android with architectures of **armv7a**, **arm64** and simulators of **x86/x86_64** are supported.
* RaspberryPi with architecture **armv7l** only.

#### 2. Install Pre-Requirements

To generate Makefiles by using **configure** or **cmake** and manage dependencies of the Carrier project, certain packages must be installed on the host before compilation.

Run the following commands to install the prerequisite utilities:

```shell
$ sudo apt-get update
$ sudo apt-get install -f build-essential autoconf automake autopoint libtool flex bison libncurses5-dev cmake
```

Download this repository using Git:
```shell
$ git clone https://github.com/elastos/Elastos.NET.Carrier.Native.SDK
```

#### 3. Build to run on host (Ubuntu / Debian / Linux)

To compile the project from source code for the target to run on Ubuntu / Debian / Linux, carry out the following steps:


Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir linux
$ cd linux
```

Generate the Makefile in the current directory:<br/>
Note: Please see custom options below.
```shell
$ cmake ../..
```
***
Optional (Generate the Makefile): To be able to build a distribution with a specific build type **Debug/Release**, as well as with customized install location of distributions, run the following commands:
```shell
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=YOUR-INSTALL-PATH ../..
```
***

Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```

***
**Run Elashell or Elatests**

Elashell is a fully functional, lightweight shell program that processes commands and returns the output to the terminal.
Through Elashell, users may connect to other carrier nodes and exchange messages.

Elatests is also a shell program, but with predefined commands, therefore no user interaction is necessary. The output for every command
is displayed in the terminal for a simple evaluation of test results.

To run elashell or elatests, first extract the distribution package created previously and enter the extracted folder.
Then, change directory to the 'bin' folder.
```shell
$ cd YOUR-DISTRIBUTION-PACKAGE-PATH/bin
```

Run Elashell:
```shell
$ ./elashell.sh
```

Or run Elatests:
```shell
$ ./elatests.sh
```

***

#### 4. Cross-compilation for Android Platform

With CMake, Elastos Carrier can be cross-compiled to run on Android as a target platform, while compilation is carried out on Ubuntu / Debian / Linux host.

**Prerequisite**: Android NDK 'android-ndk-r16b' or higher must be downloaded onto the host Linux based host.
Android NDKs (such as 'Linux 64-bit (x86)') can be downloaded from https://developer.android.com/ndk/downloads/ .
Please make sure to extract the downloaded NDK.

Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir android
$ cd android
```

To generate the required Makefile in the current directory, please make sure to first replace 'YOUR-TARGET-ARCHITECTURE'
and 'YOUR-ANDROID-NDK-HOME' with the correct option and path.

-DANDROID_ABI accepts the following target architecture options:
* armeabi-v7a
* arm64-v8a
* x86
* x86_64

Replace 'YOUR-ANDROID-NDK-HOME' with the path to the extracted NDK folder.

Run the command with the correct options described above:
```shell
$ cmake -DANDROID_ABI=YOUR-TARGET-ARCHITECTURE -DANDROID_NDK_HOME=YOUR-ANDROID-NDK-HOME -DCMAKE_TOOLCHAIN_FILE=../../cmake/AndroidToolchain.cmake ../..

```

Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```


#### 5. Cross-compilation for Raspberry Pi

With CMake, Elastos Carrier can be cross-compiled to run on Raspberry Pi (Raspbian OS) as a target platform, while compilation is carried out on Ubuntu / Debian / Linux host.

**Prerequisite**: The Raspberry Pi Toolchain must be downloaded onto the host Linux based host.
```shell
$ git clone https://github.com/raspberrypi/tools
```


Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir rpi
$ cd rpi
```


To generate the required Makefile in the current directory, please make sure to first replace 'YOUR-RASPBERRYPI-TOOLCHAIN-HOME' with the correct path to the previously downloaded Raspberry Pi Toolchain folder.

Run the command with the correct option described above:
```shell
$ cmake -DRPI_TOOLCHAIN_HOME=YOUR-RASPBERRYPI-TOOLCHAIN-HOME -DCMAKE_TOOLCHAIN_FILE=../../cmake/RPiToolchain.cmake ../..

```


Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```



### macOS

#### 1. Brief introduction

On macOS, besides the compilation for the host itself, cross-compilation is possible for the following targets:

* Android with architectures of **armv7a**, **arm64** and simulators of **x86/x86_64** are supported.
* iOS platforms to run on **iPhone-arm64** and **iPhoneSimulator-x86_64**.


#### 2. Install Pre-Requirements

packages must be installed on the host before compilation.

The following packages related to **configure** and **cmake** must be installed on the host before compilation either by installation through the package manager **homebrew** or by building from source:

Note: Homebrew can be downloaded from https://brew.sh/ .


Install packages with Homebrew:
```
$ brew install autoconf automake libtool shtool pkg-config gettext cmake
```

Please note that **homebrew** has an issue with linking **gettext**. If you have an issue with the execution of **autopoint**, fix it by running:

```shell
$ brew link --force gettext
```

Download this repository using Git:
```shell
$ git clone https://github.com/elastos/Elastos.NET.Carrier.Native.SDK
```

#### 3. Build to Run on MacOS

To compile the project from source code for the target to run on MacOS, carry out the following steps:

Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir macos
$ cd macos
```

Generate the Makefile in the current directory:<br/>
Note: Please see custom options below.
```shell
$ cmake ../..
```
***
Optional (Generate the Makefile): To be able to build a distribution with a specific build type **Debug/Release**, as well as with customized install location of distributions, run the following commands:
```shell
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=YOUR-INSTALL-PATH ../..
```
***

Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```

***
**Run Elashell or Elatests**

Elashell is a fully functional, lightweight shell program that processes commands and returns the output to the terminal.
Through Elashell, users may connect to other carrier nodes and exchange messages.

Elatests is also a shell program, but with predefined commands, therefore no user interaction is necessary. The output for every command
is displayed in the terminal for a simple evaluation of test results.

To run elashell or elatests, first extract the distribution package created previously and enter the extracted folder.
Then, change directory to the 'bin' folder.
```shell
$ cd YOUR-DISTRIBUTION-PACKAGE-PATH/bin
```

Run Elashell:
```shell
$ ./elashell.sh
```

Or run Elatests:
```shell
$ ./elatests.sh
```

***

#### 4. Cross-compilation for Android Platform

Elastos Carrier cmake system also supports to build cross-compilation for android platform on macOS with android NDK toolchains of same requirement of minimum API level **21**.

With CMake, Elastos Carrier can be cross-compiled to run on Android as a target platform, while compilation is carried out on MacOS host.

**Prerequisite**: Android NDK 'android-ndk-r16b' or higher must be downloaded onto the host Linux based host.
Android NDKs (such as 'http://android-ndk-r18b-darwin-x86_64' for MacOS Host) can be downloaded from https://developer.android.com/ndk/downloads/ .
Please make sure to extract the downloaded NDK.

Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir android
$ cd android
```

To generate the required Makefile in the current directory, please make sure to first replace 'YOUR-TARGET-ARCHITECTURE'
and 'YOUR-ANDROID-NDK-HOME' with the correct option and path.

-DANDROID_ABI accepts the following target architecture options:
* armeabi-v7a
* arm64-v8a
* x86
* x86_64

Replace 'YOUR-ANDROID-NDK-HOME' with the path to the extracted NDK folder.

Run the command with the correct options described above:
```shell
$ cmake -DANDROID_ABI=YOUR-TARGET-ARCHITECTURE -DANDROID_NDK_HOME=YOUR-ANDROID-NDK-HOME -DCMAKE_TOOLCHAIN_FILE=../../cmake/AndroidToolchain.cmake ../..

```

Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```


#### 5. Cross-compilation for iOS Platform

With CMake, Elastos Carrier can be cross-compiled to run on iOS as a target platform, while compilation is carried out on a MacOS host with XCode.

**Prerequisite**: MacOS version must be **9.0** or higher.


Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir ios
$ cd ios
```

To generate the required Makefile in the current directory, please make sure to first replace
'YOUR-IOS-PLATFORM' with the correct option.

-DIOS_PLATFORM accepts the following target architecture options:
* iphoneos
* iphonesimulator

Replace 'YOUR-IOS-PLATFORM' with the path to the extracted NDK folder.

Run the command with the correct options described above:
```shell
$ cmake -DIOS_PLATFORM=YOUR-IOS-PLATFORM -DCMAKE_TOOLCHAIN_FILE=../../cmake/iOSToolchain.cmake ../..

```

Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ make
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ make install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ make dist
```


### Windows

#### 1. Brief introduction

With CMake, Elastos Carrier can be cross-compiled to run only on Windows as target platform, while compilation is carried out on a Windows host. Both 32-bit and 64-bit target versions are supported.

**Prerequisite**: Visual Studio IDE is required. The Community version can be downloaded at https://visualstudio.microsoft.com/downloads/ for free.

To build for a 32-bit target , select `x86 Native Tools Command Console` to run building commands, otherwise, select `x64 Native Tools Command Console` for a 64-bit target.

#### 2. Build

To compile the project from source code for the target to run on Windows, carry out the following steps:


Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of the Carrier project.

```shell
$ cd YOUR-PATH/Elastos.NET.Carrier.Native.SDK
```

Enter the 'build' folder.
```shell
$ cd build
```

Create a new folder with the target platform name, then change directory.
```shell
$ mkdir win
$ cd win
```

Generate the Makefile in the current directory:
```shell
$ cmake -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX=outputs ..\..
```


Build the program: <br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ nmake
```



Install the program: <br/>
Note: If "make install" fails due to missing permissions, use "sudo make install" instead.
```shell
$ nmake install
```


Create distribution package: <br/>
Note: If "make dist" fails due to missing permissions, use "sudo make dist" instead.
```
$ nmake dist
```

***
**Run Elashell or Elatests**

Elashell is a fully functional, lightweight shell program that processes commands and returns the output to the terminal.
Through Elashell, users may connect to other carrier nodes and exchange messages.

Elatests is also a shell program, but with predefined commands, therefore no user interaction is necessary. The output for every command
is displayed in the terminal for a simple evaluation of test results.

To run elashell or elatests, first extract the distribution package created previously and enter the extracted folder.
Then, change directory to the 'bin' folder.
```shell
$ cd YOUR-DISTRIBUTION-PACKAGE-PATH/bin
```

Run Elashell:
```shell
$ ./elashell.sh
```

Or run Elatests:
```shell
$ ./elatests.sh
```

***



## Build API Docs

Currently, the API documentation can only be built on **Linux** hosts. MacOS has a bug issue with python, which would cause build process failure.

### Ubuntu

#### 1. Install Pre-Requirements

```shell
$ sudo apt-get update
$ sudo apt-get install doxygen python-sphinx graphviz
$ curl -L -o /tmp/get-pip.py https://bootstrap.pypa.io/get-pip.py
$ sudo python /tmp/get-pip.py
$ sudo pip install breathe
```

#### 2. Build

Change to the directory where the build for any target has been previously executed. For example,
if the target was Linux, the folder structure would be similar to:
```shell
cd /YOUR-PATH/Elastos.NET.Carrier.Native.SDK/build/linux
```

Run the following command:<br/>
Note: If "make" fails due to missing permissions, use "sudo make" instead.
```shell
$ cmake -DENABLE_DOCS=ON ../..
$ make
```

The generated API documentation will be created in the new **/docs** directory on the same directory level.
Change to the docs folder:
```shell
cd docs/html
```
Open the index.html file in a browser from the terminal:
```shell
xdg-open index.html
```

## Contribution

We welcome contributions to the Elastos Carrier Project.

## Acknowledgments

A sincere thank you to all teams and projects that we rely on directly or indirectly.

## License
This project is licensed under the terms of the [MIT license](https://github.com/elastos/Elastos.NET.Carrier.Native.SDK/blob/master/LICENSE).
