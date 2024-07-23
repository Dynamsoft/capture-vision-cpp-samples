#include <iostream>
#include <string>
#include <climits>

#include "../../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../../Include/DynamsoftUtility.h"
using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dlr;
using namespace dynamsoft::dcp;
using namespace dynamsoft::license;
using namespace dynamsoft::basic_structures;
using namespace dynamsoft::utility;

// The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class VINResult
{
public:
	string codeType;
	string WMI;
	string region;
	string VDS;
	string VIS;
	string modelYear;
	string plantCode;

	string vinString;

	VINResult FromParsedResultItem(const CParsedResultItem *item)
	{
		codeType = item->GetCodeType();

		if (codeType != "VIN")
			return *this;

		if (item->GetFieldValue("vinString") != NULL)
		{
			vinString = item->GetFieldValue("vinString");
			if (item->GetFieldValidationStatus("vinString") == VS_FAILED)
			{
				vinString += ", Validation Failed";
			}
		}

		if (item->GetFieldValidationStatus("WMI") != VS_FAILED && item->GetFieldValue("WMI") != NULL)
		{
			WMI = item->GetFieldValue("WMI");
		}
		if (item->GetFieldValidationStatus("region") != VS_FAILED && item->GetFieldValue("region") != NULL)
		{
			region = item->GetFieldValue("region");
		}
		if (item->GetFieldValidationStatus("VDS") != VS_FAILED && item->GetFieldValue("VDS") != NULL)
		{
			VDS = item->GetFieldValue("VDS");
		}
		if (item->GetFieldValidationStatus("VIS") != VS_FAILED && item->GetFieldValue("VIS") != NULL)
		{
			VIS = item->GetFieldValue("VIS");
		}
		if (item->GetFieldValidationStatus("modelYear") != VS_FAILED && item->GetFieldValue("modelYear") != NULL)
		{
			modelYear = item->GetFieldValue("modelYear");
		}
		if (item->GetFieldValidationStatus("plantCode") != VS_FAILED && item->GetFieldValue("plantCode") != NULL)
		{
			plantCode = item->GetFieldValue("plantCode");
		}

		return *this;
	}

	string ToString()
	{
		string msg = "VIN String: " + vinString + "\n";
		msg += "Parsed Information:\n";
		msg += "\tWMI: " + WMI + "\n";
		msg += "\tRegion: " + region + "\n";
		msg += "\tVDS: " + VDS + "\n";
		msg += "\tVIS: " + VIS + "\n";
		msg += "\tModelYear: " + modelYear + "\n";
		msg += "\tPlantCode: " + plantCode + "\n";

		return msg;
	}
};

class MyImageSourceStateListener : public CImageSourceStateListener
{
private:
	CCaptureVisionRouter *m_router;

public:
	MyImageSourceStateListener(CCaptureVisionRouter *router)
	{
		m_router = router;
	}

	virtual void OnImageSourceStateReceived(ImageSourceState state)
	{
		if (state == ISS_EXHAUSTED)
			m_router->StopCapturing();
	}
};

class MyResultReceiver : public CCapturedResultReceiver
{
public:
	virtual void OnParsedResultsReceived(CParsedResult *pResult)
	{
		const CFileImageTag *tag = dynamic_cast<const CFileImageTag *>(pResult->GetOriginalImageTag());

		cout << "File: " << tag->GetFilePath() << endl;
		cout << "Page: " << tag->GetPageNumber() << endl;

		if (pResult->GetErrorCode() != EC_OK)
		{
			cout << "Error: " << pResult->GetErrorString() << endl;
		}
		else
		{
			int lCount = pResult->GetItemsCount();
			cout << "Parsed " << lCount << " VIN codes" << endl;
			for (int i = 0; i < lCount; i++)
			{
				const CParsedResultItem *item = pResult->GetItem(i);

				VINResult result;
				result.FromParsedResultItem(item);
				cout << result.ToString() << endl;
			}
		}

		cout << endl;
	}
};

int main()
{
	int errorcode = 0;
	char error[512];

	cout << "*******************************" << endl;
	cout << "Welcome to Dynamsoft VIN Sample" << endl;
	cout << "*******************************" << endl;

	// 1. Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=cvs&utm_source=samples&package=c_cpp
	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
	errorcode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	if (errorcode != ErrorCode::EC_OK && errorcode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorcode << ", ErrorString: " << error << endl;
		cout << "Press Enter to quit..." << endl;
		cin.ignore();
		return 0;
	}
	else
	{

		// 2. Create an instance of CCaptureVisionRouter.
		CCaptureVisionRouter *router = new CCaptureVisionRouter;

		// 3. Initialize the settings customized for VIN
		errorcode = router->InitSettingsFromFile("VIN.json", error, 512);
		if (errorcode != ErrorCode::EC_OK)
		{
			cout << "VIN template initialization: " << error << endl;
			delete router, router = NULL;
			cout << "Press Enter to quit..." << endl;
			cin.ignore();
			return 0;
		}
		else
		{

			// 4. Set input image source
			CDirectoryFetcher *dirFetcher = new CDirectoryFetcher;
			router->SetInput(dirFetcher);

			// 5. Add image source state listener
			CImageSourceStateListener *listener = new MyImageSourceStateListener(router);
			router->AddImageSourceStateListener(listener);

			// 6. Add captured result receiver
			CCapturedResultReceiver *recv = new MyResultReceiver;
			router->AddResultReceiver(recv);

			string imgPath;
			string templateName;

			while (1)
			{
				cout << endl
					 << ">> Step 1: Input your image directory full path (or 'Q'/'q' to quit):" << endl;
				getline(cin, imgPath);

				if (imgPath == "q" || imgPath == "Q")
				{
					break;
				}

				errorcode = dirFetcher->SetDirectory(imgPath.c_str());
				if(errorcode != ErrorCode::EC_OK)
				{
					cout << "SetDirectory failed: ErrorCode: " << errorcode << ", ErrorString: " << DC_GetErrorString(errorcode) << endl;
					continue;
				}
				int iNum = 0;
				do
				{
					cout << endl
						 << ">> Step 2: Choose a Mode Number:" << endl;
					cout << "   1. Read VIN from Barcode" << endl;
					cout << "   2. Read VIN from Text" << endl;
					if (!(cin >> iNum))
					{
						cin.clear();
						cin.ignore(INT_MAX, '\n');
					}
				} while (iNum < 1 || iNum > 2);

				if (iNum == 1)
					templateName = "ReadVINBarcode";
				else if (iNum == 2)
					templateName = "ReadVINText";
				cin.ignore(INT_MAX, '\n');

				// 7. Start capturing
				errorcode = router->StartCapturing(templateName.c_str(), true, error, 512);
				if (errorcode != ErrorCode::EC_OK)
				{
					cout << "StartCapturing failed: ErrorCode: " << errorcode << ", ErrorString: " << error << endl;
				}
			}

			// 8. Release the allocated memory.
			delete router, router = NULL;
			delete dirFetcher, dirFetcher = NULL;
			delete listener, listener = NULL;
			delete recv, recv = NULL;
		}
	}
	return 0;
}