# HealthyRoad - Facial Recognition Sample
HealthyRoad® 2015


This is a sample created with the objective of demonstrating the functionalities of our SDK in the facial recognition area.

Compiled executables can be found in the HealthyRoad Developer Zone [(click here)](http://www.healthyroad.pt/devcenter/en/index.php).


## Prerequisites
You'll need to install:

- Qt 5.4.x
- CMake 2.8.11+
- OpenCV 2.4.x
- HealthyRoad SDK


## Building
To build, you'll need to run these commands in the FacialRecognitionSample directory:

**Linux:**

	$ mkdir build
	$ cd build
	$ cmake -D HRDSDK_DIR="<path to the sdk>" -D OpenCV_DIR="<opencv directory>/opencv/build" ..
	$ make


**Windows (for MS Visual Studio):**

	$ mkdir build
	$ cd build
	$ cmake -G "Visual Studio 12 2013" -D HRDSDK_DIR="<path to the sdk>" -D OpenCV_DIR="<opencv directory>\opencv\build" ..
	$ start FacialRecognitionSample.sln


If CMake complains about Qt5Widgets, that means there is no environment variable set to the Qt directory, and you'll need to add this parameter to the cmake command:

`
-D CMAKE_PREFIX_PATH="<path to qt>"
`

where the path leads to the directory containing Qt's /bin directory. 


## Running

To run the generated executable, you'll need to place the following files and directories in the same directory as the executable:

- config (from the HealthyRoad SDK)
- shared (from the HealthyRoad SDK)
- plugins (from the HealthyRoad SDK) (not needed on Linux)
- platforms (from the HealthyRoad SDK)
- Resources (available here in the /src directory)
- properties.txt file
- (If running on Windows) the .dll files from the HealthyRoad SDK


## Using the application

- The first time the application is ran, you will be taken through a series of steps in order to be able to test the sample:
Camera selection, camera validation and user creation are key for the sample to function, so make sure you do all of those steps.
- Once that is done, you can test the recognition process, and add more users to comprove it's accuracy.

## Having trouble being recognized?

Check out the website for more information on how to train correctly