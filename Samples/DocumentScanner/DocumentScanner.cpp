#include <iostream>
#include <string>

#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"

using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::ddn;
using namespace dynamsoft::license;
using namespace dynamsoft::utility;

// The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#endif
#endif

int main()
{
	int errorCode = 0;
	char error[512];

	// 1.Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=ddn&utm_source=samples&package=c_cpp
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);
	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << error << endl;
	}
	else
	{
		// 2.Create an instance of CCaptureVisionRouter.
		CCaptureVisionRouter *cvRouter = new CCaptureVisionRouter;

		// 3. Set input image source.
		string imageFile;

		while (1)
		{
			cout << endl
					<< ">> Input your image full path:" << endl
					<<">> 'Enter' for sample image or 'Q'/'q' to quit"<< endl;
			getline(cin, imageFile);

			if (imageFile == "q" || imageFile == "Q")
			{
				break;
			}

			if(imageFile.empty())
				imageFile = "../../Images/document-sample.jpg";

			if (imageFile.length() >= 2 && imageFile.front() == '"' && imageFile.back() == '"')
				imageFile = imageFile.substr(1, imageFile.length() - 2);

			// 4. Capture.
			CCapturedResult *result = cvRouter->Capture(imageFile.c_str(), CPresetTemplate::PT_DETECT_AND_NORMALIZE_DOCUMENT);

			cout << "File: " << imageFile << endl;
			if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
			{
				cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << endl;
			}
			else if (result->GetErrorCode() != ErrorCode::EC_OK)
			{
				cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
			}

			CProcessedDocumentResult *processedDocumentResult = result->GetProcessedDocumentResult();
			if (processedDocumentResult == nullptr || processedDocumentResult->GetDeskewedImageResultItemsCount() == 0)
			{
				cout << "No document found." << endl;
			}
			else
			{
				int count = processedDocumentResult->GetDeskewedImageResultItemsCount();
				cout << "Deskewed " << count << " documents" << endl;
				for (int i = 0; i < count; i++)
				{
					const CDeskewedImageResultItem *deskewedImage = processedDocumentResult->GetDeskewedImageResultItem(i);
					string outPath = "deskewedResult_";
					outPath += to_string(i) + ".png";

					CImageIO io;

					// 5.Save normalized image to file.
					errorCode = io.SaveToFile(deskewedImage->GetImageData(), outPath.c_str());
					if (errorCode == 0)
					{
						cout << "Document " << i << " file: " << outPath << endl;
					}
				}
			}
			// 6. Release the allocated memory.
			if (processedDocumentResult)
				processedDocumentResult->Release();
			if (result)
				result->Release();
		}

		delete cvRouter, cvRouter = NULL;
	}

	return 0;
}