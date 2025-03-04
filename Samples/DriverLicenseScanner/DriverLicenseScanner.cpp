#include <iostream>
#include <string>
#include <climits>
#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::dcp;
using namespace dynamsoft::license;
using namespace dynamsoft::basic_structures;
using namespace dynamsoft::utility;

// The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class DriverLicenseResult
{
public:
	string codeType;
	string versionNumber;
	string licenseNumber;
	string vehicleClass;
	string fullName;
	string lastName;
	string givenName;
	string gender;
	string birthDate;
	string issuedDate;
	string expirationDate;

	DriverLicenseResult FromParsedResultItem(const CParsedResultItem *item)
	{
		codeType = item->GetCodeType();

		if (codeType != "AAMVA_DL_ID" && codeType != "AAMVA_DL_ID_WITH_MAG_STRIPE" && codeType != "SOUTH_AFRICA_DL")
			return *this;

		if (item->GetFieldValidationStatus("licenseNumber") != VS_FAILED && item->GetFieldValue("licenseNumber") != NULL)
		{
			licenseNumber = item->GetFieldValue("licenseNumber");
		}
		if (item->GetFieldValidationStatus("AAMVAVersionNumber") != VS_FAILED && item->GetFieldValue("AAMVAVersionNumber") != NULL)
		{
			versionNumber = item->GetFieldValue("AAMVAVersionNumber");
		}
		if (item->GetFieldValidationStatus("vehicleClass") != VS_FAILED && item->GetFieldValue("vehicleClass") != NULL)
		{
			vehicleClass = item->GetFieldValue("vehicleClass");
		}
		if (item->GetFieldValidationStatus("lastName") != VS_FAILED && item->GetFieldValue("lastName") != NULL)
		{
			lastName = item->GetFieldValue("lastName");
		}
		if (item->GetFieldValidationStatus("surName") != VS_FAILED && item->GetFieldValue("surName") != NULL)
		{
			lastName = item->GetFieldValue("surName");
		}
		if (item->GetFieldValidationStatus("givenName") != VS_FAILED && item->GetFieldValue("givenName") != NULL)
		{
			givenName = item->GetFieldValue("givenName");
		}
		if (item->GetFieldValidationStatus("fullName") != VS_FAILED && item->GetFieldValue("fullName") != NULL)
		{
			fullName = item->GetFieldValue("fullName");
		}

		if (item->GetFieldValidationStatus("sex") != VS_FAILED && item->GetFieldValue("sex") != NULL)
		{
			gender = item->GetFieldValue("sex");
		}

		if (item->GetFieldValidationStatus("gender") != VS_FAILED && item->GetFieldValue("gender") != NULL)
		{
			gender = item->GetFieldValue("gender");
		}

		if (item->GetFieldValidationStatus("birthDate") != VS_FAILED && item->GetFieldValue("birthDate") != NULL)
		{
			birthDate = item->GetFieldValue("birthDate");
		}
		if (item->GetFieldValidationStatus("issuedDate") != VS_FAILED && item->GetFieldValue("issuedDate") != NULL)
		{
			issuedDate = item->GetFieldValue("issuedDate");
		}
		if (item->GetFieldValidationStatus("expirationDate") != VS_FAILED && item->GetFieldValue("expirationDate") != NULL)
		{
			expirationDate = item->GetFieldValue("expirationDate");
		}

		if (fullName == "")
		{
			fullName = lastName + givenName;
		}

		return *this;
	}

	string ToString()
	{
		string msg = "Parsed Information:\n";
		msg += "\tCode Type: " + codeType + "\n";
		msg += "\tLicense Number: " + licenseNumber + "\n";
		msg += "\tVehicle Class: " + vehicleClass + "\n";
		msg += "\tLast Name: " + lastName + "\n";
		msg += "\tGiven Name: " + givenName + "\n";
		msg += "\tFull Name: " + fullName + "\n";
		msg += "\tGender: " + gender + "\n";
		msg += "\tDate of Birth: " + birthDate + "\n";
		msg += "\tIssued Date: " + issuedDate + "\n";
		msg += "\tExpiration Date: " + expirationDate + "\n";

		return msg;
	}
};


void PrintResult(CParsedResult *pResult)
{
	const CFileImageTag *tag = dynamic_cast<const CFileImageTag *>(pResult->GetOriginalImageTag());

	cout << "File: " << tag->GetFilePath() << endl;
	cout << "Page: " << tag->GetPageNumber() << endl;

	if (pResult->GetErrorCode() != EC_OK && pResult->GetErrorCode() != EC_UNSUPPORTED_JSON_KEY_WARNING)
	{
		cout << "Error: " << pResult->GetErrorString() << endl;
	}
	else
	{
		int lCount = pResult->GetItemsCount();
		cout << "Parsed " << lCount << " driver licenses" << endl;
		for (int i = 0; i < lCount; i++)
		{
			const CParsedResultItem *item = pResult->GetItem(i);

			DriverLicenseResult result;
			result.FromParsedResultItem(item);
			cout << result.ToString() << endl;
		}
	}

	cout << endl;
}

int main()
{
	int errorCode = 0;
	char error[512];

	cout << "*****************************************" << endl;
	cout << "Welcome to Dynamsoft DriverLicense Sample" << endl;
	cout << "*****************************************" << endl;

	// 1. Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples&package=c_cpp
	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << error << endl;
		cout << "Press Enter to quit..." << endl;
		cin.ignore();
		return 0;
	}
	else
	{
		// 2. Create an instance of CCaptureVisionRouter.
		CCaptureVisionRouter *router = new CCaptureVisionRouter;

		// 3. Set input image source.
		string imgPath;

		while (1)
		{
			cout << endl
					<< ">> Input your image full path:" << endl
					<<">> 'Enter' for sample image or 'Q'/'q' to quit"<< endl;

			getline(cin, imgPath);

			if (imgPath == "q" || imgPath == "Q")
			{
				break;
			}

			if (imgPath.empty())
				imgPath = "../../Images/driver-license-sample.jpg";
				
			if (imgPath.length() >= 2 && imgPath.front() == '"' && imgPath.back() == '"')
				imgPath = imgPath.substr(1, imgPath.length() - 2);
			// 4. Capture.
			CCapturedResult* result = router->Capture(imgPath.c_str(),"ReadDriversLicense");
			if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
			{
				cout << "Capture warning: Warning Code: " << result->GetErrorCode() << ", Warning String: " << result->GetErrorString() << endl;
			}
			else if (result->GetErrorCode() != ErrorCode::EC_OK)
			{
				cout << "Capture failed: ErrorCode: " << result->GetErrorCode() << ", ErrorString: " << result->GetErrorString() << endl;
			}
			CParsedResult* dcpResult = result->GetParsedResult();
			if (dcpResult == NULL || dcpResult->GetItemsCount() == 0)
			{
				cout<<"No parsed results."<<endl;
			}
			else
			{
				PrintResult(dcpResult);
			}
			//5. Release the parsed result.
			if (dcpResult)
				dcpResult->Release();
			//6. Release the capture result.
			if (result)
				result->Release();
		}

		// 7. Release the allocated memory.
		delete router, router = NULL;
	}

	return 0;
}