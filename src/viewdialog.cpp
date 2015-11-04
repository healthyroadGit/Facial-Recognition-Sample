#include "viewdialog.h"
#include <QMessageBox>

ViewDialog::ViewDialog(HrDLib* hrdx, void* imgx, QWidget *parent)
	: QDialog(parent)
{
	hrd = hrdx;
	img = imgx;
	ui.setupUi(this);
	updateList();
}

ViewDialog::~ViewDialog()
{

}

void ViewDialog::on_btnRemove_clicked(){
	if (!ui.listWidgetPpl->size().isEmpty()){
		const QString s = ui.listWidgetPpl->currentItem()->text();
		hrd->deleteUser(s.toStdString());
		updateList();
		QMessageBox::information(this, "Information", "User removed!", QMessageBox::Ok);
	}
}


void ViewDialog::updateList(){
	std::vector<std::string> ppl;
	hrd->getPeopleEnrolled(ppl);
	ui.listWidgetPpl->clear();
	if (!ppl.empty()){
		for (const std::string& p : ppl){
			ui.listWidgetPpl->addItem(QString::fromStdString(p));
		}
		ui.btnRemove->setEnabled(true);
		ui.btnRemove->setToolTip("");
	}
	else{
		ui.btnRemove->setEnabled(false);
		ui.btnRemove->setToolTip("You have no more users!");
	}
}

