#ifndef SAMPLE_H
#define SAMPLE_H

#include <QtWidgets/QMainWindow>
#include "ui_sample.h"
#include "HrDLib.h"
#include <QTimer>
#include <ctime>
#include <vector>

class Sample : public QMainWindow
{
	Q_OBJECT

public:
	Sample(HrDLib* hrd, void* imgx, QWidget *parent = 0);
	~Sample();

private:
	Ui::SampleClass *ui;
	HrDLib *hrd;
	void* img;
	QPixmap pix;
	QTimer* timer;
	bool recogOn = false;
	bool recogLiveOn = false;
	bool recog = false;
	bool alive = false;
	std::string recogPerson = "";

	void leaveFunction();	
	void checkVariables();
	void initialize();

public slots:
	void on_actionAdd_triggered();
	void on_actionView_triggered();
	void on_actionSettings_triggered();
	void on_btnRecog_clicked();
	void on_btnRecogLiv_clicked();
	void showImg();
};

#endif // SAMPLE_H
