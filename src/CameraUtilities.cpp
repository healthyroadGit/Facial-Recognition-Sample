#include "CameraUtilities.h"
#include <iostream>
#ifdef WIN32
	#include <dshow.h>
	#pragma comment(lib, "strmiids")


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

	std::vector<std::string> DisplayDeviceInformation(IEnumMoniker *pEnum)
	{
		std::vector<std::string> devices;
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
				devices.push_back(str);
				VariantClear(&var);
			}

			hr = pPropBag->Write(L"FriendlyName", &var);

			pPropBag->Release();
			pMoniker->Release();
		}
		return devices;
	}
#endif

#ifdef __unix__
	#include <stdio.h>
	#include <unistd.h>
	#include <fcntl.h>
	
	std::vector<std::string> getDevices()
	{
		std::vector<std::string> devices;
		int fd;

	    	int i = 0;
	    	std::string baseString = "/sys/class/video4linux/video";

	    	while(true)
	    	{
	    		auto newString = baseString + std::to_string(i) + "/name";

	   		fd = open(newString.c_str(), O_RDONLY);
		   	if(fd == -1)
		   	{
		       		break;
		   	}
			char buffer[200];
			int result = read(fd, &buffer, 200);
			if(result > 0)
			{
				devices.push_back({buffer, (unsigned int)result-1});
			}		
			close(fd);
	    		++i;
	    	}
	
	   	return devices;
	}


#endif

namespace cameraUtils
{

	std::vector<std::string> getCameras()
	{
		std::vector<std::string> devices;
#ifdef WIN32
		IEnumMoniker *pEnum;
		HRESULT hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
		devices = DisplayDeviceInformation(pEnum);
		pEnum->Release();
#endif
		
#ifdef __unix__
		devices = getDevices();
#endif
		return devices;

	}
}
