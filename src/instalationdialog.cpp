#include "instalationdialog.h"
#include "validate.h"
#include "settings.h"
#include "adddialog.h"
#include "Utils.h"
#include "ApplicationManager.h"

InstalationDialog::InstalationDialog(HrDLib*  hrdx, void* imgx, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	hrd = hrdx;
	img = imgx;
	initialize();
}

InstalationDialog::~InstalationDialog()
{

}

void InstalationDialog::on_btnBack_clicked(){
	if (userCreation){
		userCreation = false;
	}
	else{
		if (cameraValidation){
			cameraValidation = false;
		}
		else{
			if (cameraSelection){
				cameraSelection = false;
			}
		}
	}
	updateLights();
}

void InstalationDialog::on_btnNext_clicked(){
	//state 1
	if (!cameraSelection){
		Settings s(hrd, &img, true);
		s.setModal(true);
		cameraSelection = s.exec();
	}
	else{
		//state 2
		if (!cameraValidation){
			Validate vc(hrd, img);
			vc.setModal(true);
			if (vc.exec()){
				cameraValidation = true;
			}
			else{
				cameraSelection = false;
			}
		}

		else{
			//state 3
			if (!userCreation){
				AddDialog a(hrd, img);
				a.setModal(true);
				a.exec();
				std::vector<std::string> ppl;
				hrd->getPeopleEnrolled(ppl);
				if (!ppl.empty()){
					userCreation = true;
				}
			}
			else{
			//final state
				ApplicationManager::FIRSTTIME = false;
				this->reject();
			}
		}
	}
	updateLights();

}

void InstalationDialog::updateLights(){
	if (!cameraSelection){
		ui.btnBack->setVisible(false);
	}
	else{
		ui.btnBack->setVisible(true);
	}
	if (userCreation){
		ui.btnNext->setText(QString("Finish"));
	}
	else{
		ui.btnNext->setText(QString("Next"));
	}
	Utils::paintCheck(ui.lblSelection, cameraSelection);
	Utils::paintCheck(ui.lblValidation, cameraValidation);
	Utils::paintCheck(ui.lblCreation, userCreation);
}

void InstalationDialog::initialize(){
	ui.btnBack->setVisible(false);
	updateLights();
}