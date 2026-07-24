#include <iostream>
#include <string>
#include <climits>
#include <vector>
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
			if(item == nullptr)
				continue;
			std::string codeType = item->GetCodeType();
			std::vector<std::pair<std::string, std::string>> resultMap;
			resultMap.push_back({"Code Type", codeType});
			if (codeType == "AAMVA_DL_ID")
			{
				// For full field list and details, please refer to the documentation:
				// https://www.dynamsoft.com/code-parser/docs/core/code-types/aamva-dl-id.html?lang=cplusplus#aamva_dl_id-fields

				const char* licenseNumber = item->GetFieldValue("licenseNumber");
				if(licenseNumber && item->GetFieldValidationStatus("licenseNumber") != VS_FAILED)
					resultMap.push_back({"License Number", licenseNumber});
				
				const char* vehicleClass = item->GetFieldValue("vehicleClass");
				if(vehicleClass && item->GetFieldValidationStatus("vehicleClass") != VS_FAILED)
					resultMap.push_back({"Vehicle Class", vehicleClass});
				
				const char* fullName = item->GetFieldValue("fullName");
				if (fullName && item->GetFieldValidationStatus("fullName") != VS_FAILED)
				{
					resultMap.push_back({"Full Name", fullName});
				}
				else
				{
					const char* givenName = item->GetFieldValue("givenName");
					if (givenName && strlen(givenName) > 0 && item->GetFieldValidationStatus("givenName") != VS_FAILED)
					{
						string fullName = givenName;
						const char* lastName = item->GetFieldValue("lastName");
						if (lastName && strlen(lastName) > 0 && item->GetFieldValidationStatus("lastName") != VS_FAILED)
						{
							fullName += " ";
							fullName += lastName;
						}
						resultMap.push_back({ "Full Name", fullName });
					}
					else
					{
						const char* firstName = item->GetFieldValue("firstName");
						const char* middleName = item->GetFieldValue("middleName");
						const char* lastName = item->GetFieldValue("lastName");
						string fullName;
						if (firstName && item->GetFieldValidationStatus("firstName") != VS_FAILED)
							fullName += firstName;
						if (middleName && strlen(middleName) > 0 && item->GetFieldValidationStatus("middleName") != VS_FAILED)
						{
							if (!fullName.empty())
								fullName += " ";
							fullName += middleName;
						}
						if (lastName && strlen(lastName) > 0 && item->GetFieldValidationStatus("lastName") != VS_FAILED)
						{
							if (!fullName.empty())
								fullName += " ";
							fullName += lastName;
						}
						if (!fullName.empty())
							resultMap.push_back({ "Full Name", fullName });
					}
				}
				
				const char* sex = item->GetFieldValue("sex");
				if(sex && item->GetFieldValidationStatus("sex") != VS_FAILED)
					resultMap.push_back({"Sex", sex});

				const char* expirationDate = item->GetFieldValue("expirationDate");
				if(expirationDate && item->GetFieldValidationStatus("expirationDate") != VS_FAILED)
					resultMap.push_back({"Expiration Date", expirationDate});
			}
			else if (codeType == "AAMVA_DL_ID_WITH_MAG_STRIPE")
			{
				// For full field list and details, please refer to the documentation:
				// https://www.dynamsoft.com/code-parser/docs/core/code-types/aamva-dl-id.html?lang=cplusplus#aamva_dl_id_with_mag_stripe-fields

				const char* licenseNumber = item->GetFieldValue("DLorID_Number");
				if(licenseNumber && item->GetFieldValidationStatus("DLorID_Number") != VS_FAILED)
					resultMap.push_back({"License Number", licenseNumber});
				
				const char* isoIIN = item->GetFieldValue("ISOIIN");
				if(isoIIN && item->GetFieldValidationStatus("ISOIIN") != VS_FAILED)
					resultMap.push_back({"ISO IIN", isoIIN});
				
				const char* name = item->GetFieldValue("name");
				if(name && item->GetFieldValidationStatus("name") != VS_FAILED)
					resultMap.push_back({"Name", name});
				
				const char* sex = item->GetFieldValue("sex");
				if(sex && item->GetFieldValidationStatus("sex") != VS_FAILED)
					resultMap.push_back({"Sex", sex});
				
				const char* expirationDate = item->GetFieldValue("expirationDate");
				if(expirationDate && item->GetFieldValidationStatus("expirationDate") != VS_FAILED)
					resultMap.push_back({"Expiration Date", expirationDate});
			}
			else if (codeType == "SOUTH_AFRICA_DL")
			{
				// For full field list and details, please refer to the documentation:
				// https://www.dynamsoft.com/code-parser/docs/core/code-types/za-dl.html?lang=cplusplus#fields
				const char* idNumber = item->GetFieldValue("idNumber");
				if(idNumber && item->GetFieldValidationStatus("idNumber") != VS_FAILED)
					resultMap.push_back({"ID Number", idNumber});

				const char* licenseNumber = item->GetFieldValue("licenseNumber");
				if(licenseNumber && item->GetFieldValidationStatus("licenseNumber") != VS_FAILED)
					resultMap.push_back({"License Number", licenseNumber});
				const char* vehicleLicense = item->GetFieldValue("vehicleLicense");
				if(vehicleLicense && item->GetFieldValidationStatus("vehicleLicense") != VS_FAILED)
					resultMap.push_back({"Vehicle License", vehicleLicense});

				const char* surname = item->GetFieldValue("surname");
				const char* initials = item->GetFieldValue("initials");
				string fullName;
				if (surname && item->GetFieldValidationStatus("surname") != VS_FAILED)
					fullName += surname;
				if (initials && item->GetFieldValidationStatus("initials") != VS_FAILED)
				{
					if (!fullName.empty())
						fullName += " ";
					fullName += initials;
				}
				if (!fullName.empty())
					resultMap.push_back({"Name", fullName});

				const char* gender = item->GetFieldValue("gender");
				if(gender && item->GetFieldValidationStatus("gender") != VS_FAILED)
					resultMap.push_back({"Gender", gender});
				const char* licenseValidityTo = item->GetFieldValue("licenseValidityTo");
				if(licenseValidityTo && item->GetFieldValidationStatus("licenseValidityTo") != VS_FAILED)
					resultMap.push_back({"License Valid To", licenseValidityTo});
			}
			else
			{
				cout << "Unsupported code type: " << codeType << endl;
				continue;
			}

			std::cout << "Parsed Information:" << std::endl;
			for (auto& pair : resultMap)
			{
				std::cout << "\t" << pair.first << ": " << pair.second << std::endl;
			}
			std::cout << std::endl;
		}
	}

	std::cout << std::endl;
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
			CCapturedResultArray* resultArray = cvRouter->CaptureMultiPages(imgPath.c_str(),"ReadDriversLicense");
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
					cout << "Capture failed: ErrorCode: " << result->GetErrorCode() << ", ErrorString: " << result->GetErrorString() << endl;
				}

				int pageNumber = i + 1;

				// It is usually necessary to determine 'ImageTagType' based on the original image tag.
				// Since imageFile is used, it is directly converted to 'const dynamsoft::basic_structures::CFileImageTag *'.
				const CFileImageTag *tag = dynamic_cast<const CFileImageTag *>(result->GetOriginalImageTag());
				if(tag != nullptr)
				{
					pageNumber = tag->GetPageNumber() + 1;
				}

				CParsedResult* dcpResult = result->GetParsedResult();
				if (dcpResult == NULL || dcpResult->GetItemsCount() == 0)
				{
					cout << "No parsed results in page " << pageNumber << "." << endl;
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