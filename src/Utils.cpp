#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <QPainter>
#include <QPainter>
#include <clocale>

namespace Utils{

	bool readPropertiesFile(bool& firstTime, std::string& camera, float& threshold, std::string file){
		std::string line;
		std::ifstream myfile(file);
		setlocale(LC_NUMERIC, "C");
		int sub=0;
		#ifdef WIN32
			sub=1;
		#endif
		
		#ifdef __unix__
			sub=2;
		#endif

		if (myfile.is_open())
		{
			firstTime = true;
			camera = "";
			threshold = 0;
			while (getline(myfile, line))
			{
				int pos = line.find(':');
				if (pos != -1){
					//exist
					std::string type = line.substr(0, pos);
	
					std::string value = line.substr(pos + 1, line.length()-type.length()-sub);
					std::cout << type <<"\n";
					std::cout << value <<"\n";
					if (type == "firstTime"){
						if (value == "yes"){
							firstTime = true;
						}	
						else{
							if (value == "no"){
								firstTime = false;
							}
							else{

								return false;
							}
						}

					}
					else{
						if (type == "camera"){
							camera = value;
						}
						else{
							if (type == "threshold"){
								try{

									threshold = std::stof(value);
									if(threshold >1 || threshold < 0) return false;
									
								}
								catch(...){
									return false;
								}
							}
						}
					}
				}
			}
					std::cout << "camera: "<<camera<<std::endl;
			std::cout << "thresh: " << threshold << std::endl;
			std::cout << "first: " << firstTime << std::endl;
			myfile.close();
		}

		else{
			std::cout << "Unable to open file :(";
			return false; 
		}
		return true;
	}

	bool updatePropertiesFile(bool firstTime, std::string const & camera, std::string const & threshold, std::string file){
		std::ofstream myfile(file);
		if (myfile.is_open())
		{
			//printf("saved as camera %s", camera);
			std::string ft = "no";
			if (firstTime)ft = "yes";
			myfile << "firstTime:" + ft + "\n";
			myfile << "camera:" + camera + "\n";
			myfile << "threshold:" + threshold + "\n";
		}
		else{
			std::cout << "Unable to open file ):";
			return false;
		}
		return true;
	}

	void paintCheck(QLabel* label, bool on){
		QPixmap myPix(20, 20);
		QPainter painter(&myPix);
		if (on){
			painter.setBrush(Qt::green);
		}
		else{
			painter.setBrush(Qt::red);
		}
		painter.drawEllipse(0, 0, 19, 19);
		label->setPixmap(myPix);
	}

	bool isFloat(std::string myString) {
		std::istringstream iss(myString);
		float f;
		iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
		// Check the entire string was consumed and if either failbit or badbit is set
		return iss.eof() && !iss.fail();
	}

	std::string ftos(float f){
		std::ostringstream ss;
		ss << f;
		std::string s(ss.str());
		return s;
	}


}
