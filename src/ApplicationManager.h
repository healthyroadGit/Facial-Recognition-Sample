#pragma once
#include "sample.h"
#include <QtWidgets/QApplication>
#include "HrDLib.h"
#include <QDebug>
#include <QMessageBox>
#include <string>
#include "Utils.h"

class ApplicationManager
{
public:
	static bool FIRSTTIME;
	static std::string CAMERA;
	static float THRESHOLD;

	ApplicationManager(QApplication* a);
	~ApplicationManager();

	QApplication* application;
	void* img;

	void start();
};

