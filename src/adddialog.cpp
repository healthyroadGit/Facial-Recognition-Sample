#include "adddialog.h"
#include <qstring.h>
#include <vector>
#include <qmessagebox.h>
#include <sstream>
#include <iomanip>
#include <qpainter.h>
#include "Utils.h"

AddDialog::AddDialog(HrDLib* hrdx, void* imgx, QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	hrd = hrdx;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	timer->start(33);
	img = imgx;
	ui.lblInfoPic->setVisible(false);
}

AddDialog::~AddDialog()
{	
}

void AddDialog::on_btnCreate_clicked(){
	//Method used to finish an enrollment and save it's images into the folder
	hrd->finishEnrollment();	
	QMessageBox::information(this, "Information", "User created!", QMessageBox::Ok);
	created = true;
	this->reject();
}
void AddDialog::on_btnAutoTrain_clicked(){
	//In this method we will create a user and train him (taking 5 photos)
	name = ui.textNewName->toPlainText().toStdString();
	if (numPics == 0){
		//We start a neew enrollment simply by calling this function and sending the new person's name
		hrd->startNewEnrollment(name);
	}
	picsToTake = 5; //while we have picsToTake, the method takePhoto will be called
	timerPic = new QTimer(this);
	connect(timerPic, SIGNAL(timeout()), this, SLOT(takePhoto()));
	clock = std::clock();
	timerPic->start(33);
	ui.lblInfoPic->setVisible(true);
	ui.btnCreate->setEnabled(false);
	//ui.btnAddPhoto->setEnabled(false);
	ui.btnAutoTrain->setEnabled(false);
	ui.lblInfo->setText("Training User...");
	training = true;
	refreshWindow();
}


void AddDialog::on_textNewName_textChanged(){
	QString txt = ui.textNewName->toPlainText();
	if (txt.trimmed().isEmpty()){
		ui.btnAutoTrain->setEnabled(false);
	}
	else{
		if (numPics > 0){
			hrd->renameEnrollment(name, txt.toStdString());
		}
		ui.btnAutoTrain->setEnabled(true);
	}
}

void AddDialog::showImg(){
	//cals the tracking method and sets the hasFace variable
	int ret = hrd->track(false);
	if (ret == 0){
		hasFace = true;
	}
	else{
		hasFace = false;
	}
	QImage qimg = QImage((uchar*)img, 640, 480, QImage::Format::Format_RGB888);
	qimg = qimg.scaled(320, 240);
	qimg = qimg.rgbSwapped();
	qimg = qimg.mirrored(true, false);
	pix.convertFromImage(qimg);
	ui.lblTrack->setPixmap(pix);
	refreshWindow();
}

void AddDialog::refreshWindow(){
	if (numPics >= 5){
		ui.btnCreate->setEnabled(true);
	}
	ui.lblPicsNum->setText(QString::number(numPics));	
	Utils::paintCheck(ui.lblFace, hasFace);
}

void AddDialog::takePhoto(){
	//This function is only called when the auto training is active 
	clock_t t= std::clock();
	double time = double(t - clock);
	double timeLeft = 2000;
	if (picsToTake == 5){
		timeLeft = 4000;
	}
	timeLeft -= time;
	timeLeft /= 1000;
	if (timeLeft < 0)timeLeft = 0;
	stringstream ss;
	ss << fixed << setprecision(1) << timeLeft;
	ss.str();
	//std::string str = std::string("Taking picture in %.1f seconds", timeLeft);
	std::string str= "Taking picture in " + ss.str() + " seconds";
	ui.lblInfoPic->setText(QString::fromStdString(str));

	if (timeLeft == 0 && hasFace){
		//When the time to take  the next photo reaches zero and we have a face, we can add it to the enrollment
		//We don't need to send anything because the image is already in the hrd
		//we send the "false" because went don't want the image to be shown
		hrd->addImageEnrollment(false);
		picsToTake--;
		numPics++;
		clock = std::clock();
		if (picsToTake == 0){
			disconnect(timerPic, SIGNAL(timeout()), this, SLOT(takePhoto()));
			ui.lblInfoPic->setVisible(false);
			if (numPics >= 5){
				ui.btnCreate->setEnabled(true);
			}
			//ui.btnAddPhoto->setEnabled(true);
			ui.btnAutoTrain->setEnabled(true);
			ui.lblInfo->setText("");
			if (training){
				ui.lblInfo->setText(QString("Training complete!"/*\n You may now create a user."*/));
			}
			else{
				ui.lblInfo->setText("Picture taken!");
			}
			delete timerPic;
		}
	}
}

void AddDialog::reject(){
	if (!created){
		QMessageBox::StandardButton resBtn = QMessageBox::Yes;
		resBtn = QMessageBox::question(this, "Add User",
			tr("Are you sure you want to close?\n"),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
		if (resBtn != QMessageBox::Yes) {
			return;
		}
		//To cancel the new enrollment we need to call this function
		hrd->cancelEnrollment();
	}
	delete timer;
	QDialog::reject();
}

