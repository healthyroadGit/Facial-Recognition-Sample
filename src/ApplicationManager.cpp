#include "ApplicationManager.h"
#include "instalationdialog.h"
#include "ui_loadingscreen.h"
#include <QMessageBox>

bool ApplicationManager::FIRSTTIME = false;
std::string ApplicationManager::CAMERA = "";
float ApplicationManager::THRESHOLD = 0.8;

ApplicationManager::ApplicationManager(QApplication* a){
	application = a;	
}


ApplicationManager::~ApplicationManager()
{
}
void ApplicationManager::start(){
	//LOGO DISPLAY
	QDialog dialog;
	Ui::Dialog ui;
	ui.setupUi(&dialog);
	ui.hrIconLabel->setPixmap(QPixmap(":Sample/Resources/appIcon.png"));
	dialog.setWindowIcon(QIcon(":Sample/Resources/appIcon.ico"));
	dialog.show();
	QApplication::processEvents();

	//VARIABLE INITIALIZATION
	bool firstTime;
	std::string camera;
	float threshold;
	if (Utils::readPropertiesFile(firstTime, camera, threshold)){
		FIRSTTIME = firstTime;
		CAMERA = camera;
		THRESHOLD = threshold;
	}
	else{
		//error reading file or wrong parameters
		std::cout << "Exiting..." << std::endl;
		exit(0);
	}

	//SDK VARIABLE CREATION
	//In order to create use the Facial Recognition SDk you need to create the HrDLib variable.
	HrDLib hrd("./config", "./config/hrd_publicKey.txt", true);
		
	// Now that the sdk variable is created we need to tell him what camera are we going to use, \
	and to do so, we do one of the following:
	int license = hrd.getLicenseType();
	if (license != 2 && license != 3){		
		QMessageBox::critical(NULL, "Error!", "Invalid license.\n Please purchase a license to use this software.", QMessageBox::Close);
		return;
	}

	
	if (isdigit(camera[0])){
		//To use a USB camera or a webcam, we need to select what device is going to be used.
		//We use the device read in the properties file,\
		but for simplicity use 0, as it uses the first camera available in the computer.
		//It's also needed a pointer to the image so we can access it at any time (img).
		int device = atoi(&camera[0]);
		hrd.easySetImgFromDevice(device, img);
	}
	else{
		//To use an IP camera we need to give it's address (in this case is the string read from the properties), \
		 and the size of the image we want to receive (640 * 480 in this case)
		hrd.easySetImgFromIPCam(camera, 640, 480, img);

	}
	//closing logo display after the sdk is created
	dialog.close();

	//For the first run of the application
	if (FIRSTTIME){
		InstalationDialog w(&hrd, img);
		w.setModal(true);
		int ret = w.exec();
		if (!ret){
			//installation was not completed successfully
			ApplicationManager::FIRSTTIME = false;
			Utils::updatePropertiesFile(ApplicationManager::FIRSTTIME, CAMERA, Utils::ftos(THRESHOLD));
		}
	}

	//APPLICATION MAIN WINDOW CREATION
	if (!FIRSTTIME){
		Sample w(&hrd, img);
		w.show();
		application->exec();
	}
}
