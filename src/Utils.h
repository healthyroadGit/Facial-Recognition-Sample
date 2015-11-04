#pragma once
#include <string>
#include <QLabel>

namespace Utils
{
	bool readPropertiesFile(bool& firstTime, std::string& camera, float& threshold, std::string file="properties.txt");
	bool updatePropertiesFile(bool firstTime, std::string& camera, std::string& threshold, std::string file = "properties.txt");
	void paintCheck(QLabel* label, bool on);
	bool isFloat(std::string myString);
	std::string ftos(float f);
};

