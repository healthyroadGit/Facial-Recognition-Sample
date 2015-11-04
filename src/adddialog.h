#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include "ui_adddialog.h"
#include "HrDLib.h"
#include <QTimer>
#include <ctime>

class AddDialog : public QDialog
{
	Q_OBJECT

public:
	AddDialog(HrDLib* hrd, void* img, QWidget *parent = 0);
	~AddDialog();

private:
	Ui::AddDialog ui;
	
	HrDLib* hrd;
	QPixmap pix;
	QTimer* timer;
	QTimer* timerPic;
	void* img;
	clock_t clock;
	int numPics = 0;
	std::string name = "";
	int picsToTake;
	bool hasFace = false;
	bool created = false;
	bool training = false;

	void reject();

public slots:
	void on_btnCreate_clicked();
	void on_btnAutoTrain_clicked();
	void on_textNewName_textChanged();

	void showImg();
	void refreshWindow();
	void takePhoto();

};

#endif // ADDDIALOG_H
