#ifndef INSTALATIONDIALOG_H
#define INSTALATIONDIALOG_H

#include <QDialog>
#include "ui_instalationdialog.h"
#include "HrDLib.h"

class InstalationDialog : public QDialog
{
	Q_OBJECT

public:
	InstalationDialog(HrDLib*  hrd, void* img, QWidget *parent = 0);
	~InstalationDialog();

private:
	Ui::InstalationDialog ui;
	HrDLib* hrd;
	void* img;

	bool cameraSelection = false;
	bool cameraValidation = false;
	bool userCreation = false;

public slots:
	void on_btnBack_clicked();
	void on_btnNext_clicked();

	void initialize();
	void updateLights();

};

#endif // INSTALATIONDIALOG_H
