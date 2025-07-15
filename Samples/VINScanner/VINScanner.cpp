#include <iostream>
#include <string>
#include <climits>

#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
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

int main()
{
	int errorCode = 0;
	char error[512];

	cout << "*******************************" << endl;
	cout << "Welcome to Dynamsoft VIN Sample" << endl;
	cout << "*******************************" << endl;

	// 1. Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples&package=c_cpp
	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << error << endl;
		cout << "Press Enter to quit..." << endl;
		cin.ignore();
		return 0;
	}
	else
	{

		// 2. Create an instance of CCaptureVisionRouter.
		CCaptureVisionRouter *cvRouter = new CCaptureVisionRouter;

		// 3. Set input image source
		string imgPath;
		string templateName;

		while (1)
		{
			cout << endl
					<< ">> Step 1: Input your image full path (or 'Q'/'q' to quit):" << endl;
			getline(cin, imgPath);

			if (imgPath == "q" || imgPath == "Q")
			{
				break;
			}
			if (imgPath.length() >= 2 && imgPath.front() == '"' && imgPath.back() == '"')
				imgPath = imgPath.substr(1, imgPath.length() - 2);

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

			// 4. Capture.
			CCapturedResultArray* resultArray = cvRouter->CaptureMultiPages(imgPath.c_str(), templateName.c_str());
			int count = resultArray->GetResultsCount();
			for (int i = 0; i < count; ++i)
			{
				const CCapturedResult* result = resultArray->GetResult(i);

				if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
				{
					cout << "Capture warning: Warning Code: " << result->GetErrorCode() << ", Warning String: " << result->GetErrorString() << endl;
				}
				else if (result->GetErrorCode() != ErrorCode::EC_OK)
				{
					cout << "Capture failed: Error Code: " << result->GetErrorCode() << ", Error String: " << result->GetErrorString() << endl;
				}
				CParsedResult* dcpResult = result->GetParsedResult();
				if (dcpResult == NULL || dcpResult->GetItemsCount() == 0)
				{
					cout << "No parsed results in page " << i << "." << endl;
				}
				else
				{
					PrintResult(dcpResult);
				}
				//5. Release the parsed result.
				if (dcpResult)
					dcpResult->Release();
			}
			//6. Release the capture result.
			if (resultArray)
				resultArray->Release();
		}

		// 7. Release the allocated memory.
		delete cvRouter, cvRouter = NULL;
	}
	return 0;
}