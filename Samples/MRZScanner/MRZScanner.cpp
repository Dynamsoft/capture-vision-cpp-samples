#include <iostream>
#include <string>
#include <vector>
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
#pragma comment(lib, "../../Distributables/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../Distributables/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Distributables/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class MRZResult
{
public:
	string docId;
	string docType;
	string nationality;
	string issuer;
	string dateOfBirth;
	string dateOfExpiry;
	string gender;
	string surname;
	string givenname;

	vector<string> rawText;

	MRZResult FromParsedResultItem(const CParsedResultItem *item)
	{
		docType = item->GetCodeType();

		if (docType == "MRTD_TD3_PASSPORT")
		{
			if (item->GetFieldValidationStatus("passportNumber") != VS_FAILED && item->GetFieldValue("passportNumber") != NULL)
			{
				docId = item->GetFieldValue("passportNumber");
			}
		}
		else if (item->GetFieldValidationStatus("documentNumber") != VS_FAILED && item->GetFieldValue("documentNumber") != NULL)
		{
			docId = item->GetFieldValue("documentNumber");
		}

		string line;
		if (item->GetFieldValue("line1") != NULL)
		{
			line = item->GetFieldValue("line1");
			if (item->GetFieldValidationStatus("line1") == VS_FAILED)
			{
				line += ", Validation Failed";
			}
			rawText.push_back(line);
		}

		if (item->GetFieldValue("line2") != NULL)
		{
			line = item->GetFieldValue("line2");
			if (item->GetFieldValidationStatus("line2") == VS_FAILED)
			{
				line += ", Validation Failed";
			}
			rawText.push_back(line);
		}

		if (item->GetFieldValue("line3") != NULL)
		{
			line = item->GetFieldValue("line3");
			if (item->GetFieldValidationStatus("line3") == VS_FAILED)
			{
				line += ", Validation Failed";
			}
			rawText.push_back(line);
		}

		if (item->GetFieldValidationStatus("nationality") != VS_FAILED && item->GetFieldValue("nationality") != NULL)
		{
			nationality = item->GetFieldValue("nationality");
		}
		if (item->GetFieldValidationStatus("issuingState") != VS_FAILED && item->GetFieldValue("issuingState") != NULL)
		{
			issuer = item->GetFieldValue("issuingState");
		}
		if (item->GetFieldValidationStatus("dateOfBirth") != VS_FAILED && item->GetFieldValue("dateOfBirth") != NULL)
		{
			dateOfBirth = item->GetFieldValue("dateOfBirth");
		}
		if (item->GetFieldValidationStatus("dateOfExpiry") != VS_FAILED && item->GetFieldValue("dateOfExpiry") != NULL)
		{
			dateOfExpiry = item->GetFieldValue("dateOfExpiry");
		}
		if (item->GetFieldValidationStatus("sex") != VS_FAILED && item->GetFieldValue("sex") != NULL)
		{
			gender = item->GetFieldValue("sex");
		}
		if (item->GetFieldValidationStatus("primaryIdentifier") != VS_FAILED && item->GetFieldValue("primaryIdentifier") != NULL)
		{
			surname = item->GetFieldValue("primaryIdentifier");
		}
		if (item->GetFieldValidationStatus("secondaryIdentifier") != VS_FAILED && item->GetFieldValue("secondaryIdentifier") != NULL)
		{
			givenname = item->GetFieldValue("secondaryIdentifier");
		}

		return *this;
	}

	string ToString()
	{
		string msg = "Raw Text:\n";
		for (size_t idx = 0; idx < rawText.size(); ++idx)
		{
			msg += "\tLine " + to_string(idx + 1) + ": " + rawText[idx] + "\n";
		}
		msg += "Parsed Information:\n";
		msg += "\tDocument Type: " + docType + "\n";
		msg += "\tDocument ID: " + docId + "\n";
		msg += "\tSurname: " + surname + "\n";
		msg += "\tGiven Name: " + givenname + "\n";
		msg += "\tNationality: " + nationality + "\n";
		msg += "\tIssuing Country or Organization: " + issuer + "\n";
		msg += "\tGender: " + gender + "\n";
		msg += "\tDate of Birth(YYMMDD): " + dateOfBirth + "\n";
		msg += "\tExpiration Date(YYMMDD): " + dateOfExpiry + "\n";

		return msg;
	}
};

void PrintResult(CParsedResult *pResult)
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
		cout << "Parsed " << lCount << " MRZ Zones" << endl;
		for (int i = 0; i < lCount; i++)
		{
			const CParsedResultItem *item = pResult->GetItem(i);

			MRZResult result;
			result.FromParsedResultItem(item);
			cout << result.ToString() << endl;
		}
	}

	cout << endl;
}

int main()
{
	int errorcode = 0;
	char error[512];

	cout << "*******************************" << endl;
	cout << "Welcome to Dynamsoft MRZ Sample" << endl;
	cout << "*******************************" << endl;

	// 1. Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples&package=c_cpp
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

		// 3. Set input image source
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

			if(imgPath.empty())
				imgPath = "../../Images/passport-sample.jpg";
				
			if (imgPath.length() >= 2 && imgPath.front() == '"' && imgPath.back() == '"')
				imgPath = imgPath.substr(1, imgPath.length() - 2);

			// 4. Capture.
			CCapturedResult* result = router->Capture(imgPath.c_str(), "ReadPassportAndId");
			if (result->GetErrorCode() != ErrorCode::EC_OK)
			{
				cout << "Capture failed: ErrorCode: " << result->GetErrorCode() << ", ErrorString: " << result->GetErrorString() << endl;
			}
			else
			{
				CParsedResult* dcpResult = result->GetParsedResult();
				if (dcpResult == NULL || dcpResult->GetItemsCount() == 0)
				{
					cout << "No parsed results." << endl;
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
			if (result)
				result->Release();
		}
	}
	return 0;
}