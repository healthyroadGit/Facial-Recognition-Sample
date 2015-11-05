#ifndef VALIDATE_H
#define VALIDATE_H

#include <QDialog>
#include "ui_validate.h"
#include "HrDLib.h"
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <opencv2/opencv.hpp>

/**This class is used to validade the camera attending to quality, distance, resolution, etc...*/
class Validate : public QDialog
{
	Q_OBJECT

public:
	Validate(HrDLib* hrd, void* img, QWidget *parent = 0);
	~Validate();

private:
	Ui::Validate ui;
	HrDLib* hrd;
	void* img;
	QPixmap pix;
	QTimer* timer;
	QTime valwaitcam;
	cv::CascadeClassifier HNS;
	cv::CascadeClassifier FaceCasc;
	cv::CascadeClassifier lefteyeCasc;
	cv::CascadeClassifier righteyeCasc;
	cv::Rect hns, facerect;
	bool validateResolution = false;
	bool validatePerson = false;
	bool validateFace = false;
	bool validateDistance = false;
	bool validateEyes = false;
	bool validateQuality = false;
	bool validate = false;
	bool lefteye = false;
	bool righteye = false;

	void setCheck(QPixmap& map, bool on);
	

public slots:
	void showImg();
	void on_btnCancel_clicked();
};

#endif // VALIDATE_H
