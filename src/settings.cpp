#include "settings.h"
#include "ApplicationManager.h"
#include <sstream>
#include "Utils.h"
#include <opencv2\opencv.hpp>
#include "validate.h"
#include "Utils.h"

#include <dshow.h>
#pragma comment(lib, "strmiids")

Settings::Settings(HrDLib* hrdx, void** imgx, bool firstTimex, QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	hrd = hrdx;
	img = imgx;
	firstTime = firstTimex;
	initailize();
}

Settings::~Settings()
{

}
HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
	// Create the System Device Enumerator.
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the category.
		hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
		}
		pDevEnum->Release();
	}
	return hr;
}

QStringList DisplayDeviceInformation(IEnumMoniker *pEnum)
{
	QStringList device;
	IMoniker *pMoniker = NULL;
	//device.push_back("---");
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;
		}

		VARIANT var;
		VariantInit(&var);

		// Get description or friendly name.
		hr = pPropBag->Read(L"Description", &var, 0);
		if (FAILED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &var, 0);
		}
		if (SUCCEEDED(hr))
		{
			std::wstring ws(var.bstrVal, SysStringLen(var.bstrVal));
			std::string str(ws.begin(), ws.end());
			device.push_back(QString::fromStdString(str));
			//printf("%S\n", var.bstrVal);
			VariantClear(&var);
		}

		hr = pPropBag->Write(L"FriendlyName", &var);

		pPropBag->Release();
		pMoniker->Release();
	}
	return device;
}


void Settings::on_btnSave_clicked(){
	int index = ui.cbxDevices->currentIndex();
	std::string text = ui.textCamera->toPlainText().toStdString();
	std::string camera = "";
	if (index != device.size() - 1){
		hrd->easySetImgFromDevice(index, *img);
		camera = std::to_string(index);
	}
	else{
		camera = text;
		hrd->easySetImgFromIPCam(text, 640, 480, *img);
	}
	
	ApplicationManager::CAMERA = camera;
	ApplicationManager::THRESHOLD = stof(ui.textConf->toPlainText().toStdString()) / 100.0;
	std::ostringstream ss;
	ss << ApplicationManager::THRESHOLD;
	std::string thresh(ss.str());

	if (firstTime){
		Utils::updatePropertiesFile(ApplicationManager::FIRSTTIME, camera, Utils::ftos(ApplicationManager::THRESHOLD));
		accept();
		return;
	}

	Validate vc(hrd, *img);
	vc.setModal(true);
	if (vc.exec()){
		Utils::updatePropertiesFile(ApplicationManager::FIRSTTIME, camera, thresh);
		accept();
	}
	else{
		ApplicationManager::CAMERA = startCamera;
		ApplicationManager::THRESHOLD = std::stof(startTreshold) / 100.0;

		if (isdigit(startCamera[0])){
			hrd->easySetImgFromDevice(atoi(&startCamera[0]), *img);
		}
		else{
			hrd->easySetImgFromIPCam(startCamera, 640, 480, *img);
		}
	}
}

void Settings::on_btnDefault_clicked(){
	ui.textCamera->setText("");
	ui.cbxDevices->setCurrentIndex(0);
	ui.textConf->setText("80");
}

void Settings::on_btnCancel_clicked(){
	reject();
}


void Settings::on_cbxDevices_currentIndexChanged(int index){
	if (index == device.size() - 1){
		ui.textCamera->setVisible(true);
		//ui.textCamera->setText(QString(""));
	}
	else{
		ui.textCamera->setVisible(false);
		//ui.textCamera->setText(QString(""+ui.cbxDevices->currentIndex()));
	}
}

void Settings::on_textConf_textChanged(){
	std::string threshold = ui.textConf->toPlainText().toStdString();
	bool isFloat = Utils::isFloat(threshold);
	ui.btnSave->setEnabled(isFloat); 
	if (isFloat){
		float f = stof(threshold);
		ui.btnSave->setEnabled(f > 0 && f <= 100);
	}
	//checkSave();
}

void Settings::checkSave(){
	std::cout << startCamera << "\n";
	std::cout << ui.textCamera->toPlainText().toStdString() << "\n";
	bool a = startCamera != ui.textCamera->toPlainText().toStdString();
	std::cout << a << "\n";
	bool b = startTreshold != ui.textConf->toPlainText().toStdString();
	std::cout << b << "\n";
	ui.btnSave->setEnabled(a || b);
}

void Settings::initailize(){
	IEnumMoniker *pEnum;
	HRESULT hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
	if (SUCCEEDED(hr))
	{
		device = DisplayDeviceInformation(pEnum);
		pEnum->Release();
		device.append("IP Camera");
		ui.cbxDevices->addItems(device);
	}

	
	ui.btnSave->setText(QString::fromStdString("Validate\nand save"));
	startCamera = ApplicationManager::CAMERA;
	if (startCamera == "")startCamera = "0";
	if (isdigit(startCamera[0]) && startCamera.length() == 1){
		ui.textCamera->setVisible(false);
		int max = device.size() - 1;
		int index = atoi(&startCamera[0]);
		if (index < max){
			//printf("true");
			ui.cbxDevices->setCurrentIndex(index);
		}
		else{
			ui.cbxDevices->setCurrentIndex(0);
			//printf("false    ");
			//printf("%d     %d", index, max);
		}

	}
	else{
		ui.textCamera->setVisible(true);
		ui.textCamera->setText(QString::fromStdString(startCamera));
		ui.cbxDevices->setCurrentIndex(device.size() - 1);
	}		
	startTreshold = Utils::ftos(ApplicationManager::THRESHOLD * 100.0);
	ui.textConf->setText(QString::fromStdString(startTreshold));
		
	if (firstTime){
		ui.btnDefault->setVisible(false);
		ui.label_2->setVisible(false);
		ui.textConf->setVisible(false);
		ui.btnSave->setText(QString::fromStdString("Save"));
		this->setWindowTitle("Camera Selection");
	}	
	
}

