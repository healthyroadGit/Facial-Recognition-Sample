#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "ui_settings.h"
#include "HrDLib.h"

class Settings : public QDialog
{
	Q_OBJECT

public:
	Settings(HrDLib* hrd, void** img, bool firstTime = false, QWidget *parent = 0);
	~Settings();

private:
	Ui::Settings ui;
	HrDLib* hrd;
	QStringList device;
	void** img;
	std::string startCamera;
	std::string startTreshold;
	bool firstTime;


public slots:
	void on_btnSave_clicked();
	void on_btnDefault_clicked();
	void on_btnCancel_clicked();
	void on_cbxDevices_currentIndexChanged(int index);
	void on_textConf_textChanged();

	void initailize();
	void checkSave();
};

#endif // SETTINGS_H
