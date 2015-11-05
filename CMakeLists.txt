cmake_minimum_required(VERSION 2.8.11)

project(FacialRecognitionSample)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library
find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
QT5_WRAP_UI(UIS_HDRS
	src/adddialog.ui
	src/instalationdialog.ui
	src/sample.ui
	src/settings.ui
	src/validate.ui
	src/viewdialog.ui
	src/loadingscreen.ui
)
QT5_ADD_RESOURCES(resources src/sample.qrc)
message("Binary directory ${FacialRecognitionSample_BINARY_DIR}")

set( EXECUTABLE_OUTPUT_PATH 
     ${FacialRecognitionSample_BINARY_DIR}/bin CACHE PATH
     "Output directory for the FacialRecognitionSample executable" )


set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3" )


set(sample_sources
	src/main.cpp 
	src/adddialog.h src/ApplicationManager.h src/instalationdialog.h src/sample.h src/settings.h src/Utils.h src/validate.h src/viewdialog.h
	src/adddialog.cpp src/ApplicationManager.cpp src/instalationdialog.cpp src/sample.cpp src/settings.cpp src/Utils.cpp src/validate.cpp src/viewdialog.cpp

	src/CameraUtilities.h src/CameraUtilities.cpp
)

add_executable(FacialRecognitionSample ${sample_sources} ${UIS_HDRS} ${resources})
qt5_use_modules(FacialRecognitionSample Widgets)
include_directories(${HRDSDK_DIR}/include)
if(WIN32)
	target_link_libraries(FacialRecognitionSample  ${HRDSDK_DIR}/lib/HrDLib.lib ${OpenCV_LIBS})
endif(WIN32)
if(UNIX)
	target_link_libraries(FacialRecognitionSample  ${HRDSDK_DIR}/lib/libHrDLib.so ${OpenCV_LIBS})
endif(UNIX)

