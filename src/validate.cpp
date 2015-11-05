#include "validate.h"
#include <QMessageBox>
#include "Utils.h"

/**This class is used to validade the camera attending to quality, distance, resolution, etc...*/
Validate::Validate(HrDLib* hrdx, void* imgx, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	hrd = hrdx;
	img = imgx;
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(showImg()));
	timer->start(33);
	valwaitcam.start();

	HNS.load("config/haarcascade_headandshoulders.xml");
	FaceCasc.load("config/haarcascade_frontalface_alt2.xml");
	lefteyeCasc.load("config/haarcascade_mcs_lefteye.xml");
	righteyeCasc.load("config/haarcascade_mcs_righteye.xml");
}

Validate::~Validate()
{
	delete timer;
}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

short GetSharpness(cv::Mat data)
{
	cv::Mat out;
	Laplacian(data, out, CV_8UC1, 3);
	short maxLap = -32767;
	float media = 0;
	int x = 0;
	for (int i = 0; i< out.total(); i++)
	{
		media += out.at<uchar>(i);
		if (out.at<uchar>(i) > maxLap){ maxLap = out.at<uchar>(i); x = i; }
	}
	media /= out.total();

	return media;
}

void detectFace(cv::Mat &yo, cv::Rect &face, cv::CascadeClassifier faceCascade){
	vector<cv::Rect> rect;
	float dim = max(yo.rows, yo.cols);
	int min = dim / 5;
	faceCascade.detectMultiScale(yo, rect, 1.2, 1, 0 | cv::CASCADE_FIND_BIGGEST_OBJECT | cv::CASCADE_SCALE_IMAGE, cv::Size(min, min));
	if (rect.size() > 0){
		face = rect[0];
	}
}

void detectHnS(cv::Mat &yo, cv::Rect &HNS, cv::CascadeClassifier HnSDetector){
	vector<cv::Rect> rect;
	float dim = max(yo.rows, yo.cols);
	int min = dim / 3;
	HnSDetector.detectMultiScale(yo, rect, 1.1, 9, 0 | cv::CASCADE_FIND_BIGGEST_OBJECT | cv::CASCADE_SCALE_IMAGE, cv::Size(min, min));
	if (rect.size() > 0)
		HNS = rect[0];
}

void detectEye(cv::Mat const &eyemat, cv::Rect &eye, cv::CascadeClassifier eyecascade){
	vector<cv::Rect> rect;
	float dim = max(eyemat.rows, eyemat.cols);
	//int min = dim / 5;
	eyecascade.detectMultiScale(eyemat, rect, 1.1, 20, 0 | cv::CASCADE_FIND_BIGGEST_OBJECT | cv::CASCADE_SCALE_IMAGE);
	if (rect.size() > 0){
		eye = rect[0];
	}
}

void Validate::showImg(){
	hrd->track(false);
	cv::Mat image(480, 640, 16, img);
	cv::Mat imgval = image.clone();
	if (imgval.cols>0  && imgval.rows>0){
		cv::Mat gray;
		if (imgval.channels() == 1)gray = imgval.clone();
		else{
			if ((gray.rows != imgval.rows) || (gray.cols != imgval.cols))
				gray.create(imgval.rows, imgval.cols, CV_8U);
			cv::cvtColor(imgval, gray, CV_BGR2GRAY);
		}
		facerect = cv::Rect();
		hns = cv::Rect();
		lefteye = false;
		righteye = false;
		if (valwaitcam.elapsed() > 2000){
			if (image.rows >= 480 && image.cols >= 640){
				validateResolution = true;
			}
			else
			{
				validateResolution = false;
			}
			detectHnS(image, hns, HNS);
			if (hns.height > 0){
				validatePerson = true;
				//imshow("Person Detected", imval(hns));
			}
			else
			{
				//std::cout << "nao detetou person" << std::endl;
				validatePerson = false;
			}
			//std::cout << "face rect height: " << facerect.height << std::endl;
			detectFace(image, facerect, FaceCasc);
			if (facerect.height > 0){
				validateFace = true;
				//imshow("Face Detected", imval(facerect));
			}
			else
			{
				//std::cout << "nao detetou cara" << std::endl;
				validateFace = false;
			}

			if (facerect.width > 0.2*image.cols){
				if (hns.width > image.cols / 3){
					//cout << "\t\tOK" << endl;
					validateDistance = true;
				}
				else
				{
					validateDistance = false;
				}
			}


			detectFace(image, facerect, FaceCasc);
			if (facerect.height > 0){
				if (!lefteye){
					cv::Rect eye;
					cv::Rect LeyeRoi = cv::Rect(facerect.x + 0.1*facerect.width, facerect.y + 0.2*facerect.height, 0.4*facerect.width, 0.4*facerect.height);
					detectEye(image(LeyeRoi), eye, lefteyeCasc);
					//rectangle(imgval, eye, Scalar(255));
					if (eye.height > 0){
						lefteye = true;
					}
				}
				if (!righteye){
					cv::Rect eye;
					cv::Rect ReyeRoi = cv::Rect(facerect.x + 0.1*facerect.width, facerect.y + 0.2*facerect.height, 0.4*facerect.width, 0.4*facerect.height);
					detectEye(image(ReyeRoi), eye, righteyeCasc);
					//rectangle(imgval, eye, Scalar(255));
					if (eye.height > 0){
						righteye = true;
					}
				}
			}

			if (lefteye && righteye)
			{
				validateEyes = true;
			}
			else{
				validateEyes = false;
			}
			//bool ok = false;
			//while (!ok){


			detectFace(image, facerect, FaceCasc);
			if (facerect.height > 0){
				int quality = GetSharpness(gray(facerect));

				//std::cout << endl << "quality = " << quality << std::endl;
				if (quality > 5){
					validateQuality = true;
					//ok = true;
				}
				else {
					validateQuality = false;
				}
			}
			Utils::paintCheck(ui.lblResolution, validateResolution);
			Utils::paintCheck(ui.lblPerson, validatePerson);
			Utils::paintCheck(ui.lblFace, validateFace);
			Utils::paintCheck(ui.lblDistance, validateDistance);
			Utils::paintCheck(ui.lblEyes, validateEyes);
			Utils::paintCheck(ui.lblQuality, validateQuality);
		}
		QImage qimg = QImage((uchar*)img, 640, 480, QImage::Format::Format_RGB888);
		qimg = qimg.scaled(448, 336);
		qimg = qimg.rgbSwapped();
		qimg = qimg.mirrored(true, false);
		pix.convertFromImage(qimg);
		ui.lblImage->setPixmap(pix);
		if (validateResolution && validatePerson && validateFace && validateDistance && validateEyes && validateQuality){
			QMessageBox::StandardButton reply;
			reply = QMessageBox::information(this, "HealthySecurity", tr("Setup Validated"),
				QMessageBox::Ok);
			if (reply == QMessageBox::Ok){
				//valtimer.stop();
				accept();
				;
			}
		}
	}
	
}

void Validate::on_btnCancel_clicked(){
	reject();
}
