#include "sample.h"
#include "adddialog.h"
#include "viewdialog.h"
#include "settings.h"
#include <QMessageBox>
#include <QIcon>
#include <vector>
#include "ApplicationManager.h"
#include "validate.h"

Sample::Sample(HrDLib* hrdx,void* imgx, QWidget *parent)
: QMainWindow(parent), ui(new Ui::SampleClass())
{
	ui->setupUi(this);

	//acess to the sdk and the image variables
	hrd = hrdx;
	img = imgx;

	//timer to continuously get and show the image and to further analyse it
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	timer->start(33);	
	initialize();
	checkVariables();
}

Sample::~Sample()
{

}

void Sample::on_actionAdd_triggered(){
	//closes this window and pops the Add Users window
	disconnect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	AddDialog a(hrd, img);
	leaveFunction();
	a.setModal(true);
	this->close();
	a.exec();
	this->show();
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	checkVariables();
}

void Sample::on_actionView_triggered(){
	//closes this window and pops the Users View window
	disconnect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	ViewDialog v(hrd, img);
	v.setModal(true);
	leaveFunction();
	this->close();
	v.exec();
	this->show();
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	checkVariables();

}

void Sample::on_actionSettings_triggered(){
	//closes this window and pops the Setting window
	disconnect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	Settings s(hrd, &img);	
	s.setModal(true);
	leaveFunction();
	this->close();
	s.exec();
	this->show();
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	checkVariables();
}

void Sample::on_btnRecog_clicked(){
	//the recogOn variable will be set to true of false (depending on it's previous state)\
	if it's true, the image will be analysed in the showImg method.
	recogOn = !recogOn;
	if (recogOn){
		ui->btnRecog->setEnabled(true);
		ui->btnRecogLiv->setEnabled(false);
		ui->btnRecog->setText(QString("Stop"));
	}
	else{
		ui->btnRecog->setEnabled(true);
		ui->btnRecogLiv->setEnabled(true);
		ui->btnRecog->setText(QString("Recognize"));
		ui->lblInfo->setText("");
	}
}
void Sample::on_btnRecogLiv_clicked(){
	//the recogLiveOn variable will be set to true of false (depending on it's previous state)\
	if it's true, the image will be analysed in the showImg method.
	recogLiveOn = !recogLiveOn;
	if (recogLiveOn){
		ui->btnRecog->setEnabled(false);
		ui->btnRecogLiv->setEnabled(true);
		ui->btnRecogLiv->setText(QString("Stop"));
	}
	else{
		ui->btnRecog->setEnabled(true);
		ui->btnRecogLiv->setEnabled(true);
		ui->btnRecogLiv->setText(QString("Recognize with \n Liveness"));
		ui->lblInfo->setText("");
	}

}


void Sample::showImg(){
	//Every 33 miliseconds, this function will be called (timer defined in the constructor)
	//This function will get the image from the camera and analyse it further if a face is found
	//So we need to tell the SDK read the images, searching for a face, and tracking it
	//The return of this next function "track()" tells us if was detected a face(returns 0), or not(returns -1)
	int ret = hrd->track(false);
	//transformation to a QImage
	QImage qimg = QImage((uchar*)img, 640, 480, QImage::Format::Format_RGB888);
	//Color channel switch (because opencv works in BGR and QImage works with RGB)
	qimg = qimg.rgbSwapped();
	//horizontal flip on the image
	qimg = qimg.mirrored(true, false);
	//conversion of the image to a pixmap
	pix.convertFromImage(qimg);
	//now we can set the pixmap of the label as the image
	ui->lblImage->setPixmap(pix);
	//in case the recognition on the recognition with livenss is on
	if (recogOn || recogLiveOn){
		if (ret != 0){
			//lost face, restarting variables
			recog = false;
			alive = false;
			recogPerson = "";
			ui->lblInfo->setStyleSheet("QLabel { color: red}");
			ui->lblInfo->setText(QString::fromStdString("Face not found!"));
		}
		//in case of existing a face in the image
		else{
			if (!recog) {
				std::string person;
				//Now that we have a face in the image and we want to recognize a person, we need to call the function "instantRecognize()"
				//To do so, we send a string to receive the person name in case someone is recognized and also the threshold.
				//The threshold works like a confidence level, the higher, the more confident in the person recognized, but also less likely to recognize a person
				int rec = hrd->instantRecognize(person, ApplicationManager::THRESHOLD);
				if (rec == 0){
					if (person == ""){
						//person not recognized
					}
					else{
						recogPerson = person;
						recog = true;
					}
				}
				else{
					//no face
					recog = false;
					alive = false;
					recogPerson = "";
				}
			}
			if (!alive){
				//To get a persons livesness confirmation we need to call the following funciton only sending a bool variable
				hrd->getLiveness(alive);
			}

			if ((recogOn && recog) || (recogLiveOn && recog && alive)){
				ui->lblInfo->setStyleSheet("QLabel { color: green}");
				ui->lblInfo->setText(QString::fromStdString("Person recognized: " + recogPerson));

				leaveFunction();
			}
			else{
				if (recog){
					//found person but not liveness yet
					ui->lblInfo->setStyleSheet("QLabel { color: orange}");
					ui->lblInfo->setText(QString::fromStdString("Person recognized: " + recogPerson + ". Waiting for liveness..."));
				}
				else{
					ui->lblInfo->setStyleSheet("QLabel { color: red}");
					ui->lblInfo->setText(QString::fromStdString("Recognizing..."));
				}
				//else  if person is alive but not recognized

			}
		}

	}

}

void Sample::checkVariables(){
	//We can check how many peopple we have added in our system, to do so, we call the function "getPeopleEnrolled()" \
	that takes a string vector as an argument and returns it with the names of the people existing on the system
	std::vector<std::string> ppl;
	hrd->getPeopleEnrolled(ppl);
	if (ppl.empty()){
		ui->actionView->setEnabled(false);
		ui->actionView->setToolTip("No user added yet!");
		ui->btnRecog->setEnabled(false);
		ui->btnRecogLiv->setEnabled(false);
	}
	else{
		ui->actionView->setEnabled(true);
		ui->actionView->setToolTip("users here");
		ui->btnRecog->setEnabled(true);
		ui->btnRecogLiv->setEnabled(true);
	}
}

void Sample::leaveFunction(){
	ui->btnRecog->setText(QString("Recognize"));
	ui->btnRecogLiv->setText(QString("Recognize with \n Liveness"));
	ui->btnRecog->setEnabled(true);
	ui->btnRecogLiv->setEnabled(true);
	recogOn = false;
	recogLiveOn = false;
	recog = false;
	alive = false;
	recogPerson = "";
}

void Sample::initialize(){
	ui->actionAdd->setShortcut(QKeySequence(tr("Ctrl+N")));
	ui->actionView->setShortcut(QKeySequence(tr("Ctrl+E")));
	ui->btnRecogLiv->setText(QString("Recognize with \n Liveness"));

	QIcon icon("appIcon.ico");
	setWindowIcon(icon);
}
