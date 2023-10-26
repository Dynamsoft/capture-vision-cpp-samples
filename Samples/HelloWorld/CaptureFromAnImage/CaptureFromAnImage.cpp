#include<iostream>
#include<string>

#include "../../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dlr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::ddn;
using namespace dynamsoft::license;
using namespace dynamsoft::utility;

// The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

int main()
{
	int errorcode = 0;
	char error[512];

	// 1.Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	errorcode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	cout << "License initialization: " << errorcode << "," << error << endl;

	// 2.Create an instance of CCaptureVisionRouter.
	CCaptureVisionRouter *router = new CCaptureVisionRouter;

	// 3.Perform capture jobs(decoding barcodes/recognizing text lines/normalizing images) on an image
	string imageFile = "../../../Images/dcv-sample-image.png";
	CCapturedResult* result = router->Capture(imageFile.c_str(), CPresetTemplate::PT_DEFAULT);

	cout << "File: " << imageFile << endl;

	// 4.Outputs the result.
	if (result->GetErrorCode() != 0) {
		cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
	}

	/*
	* There can be multiple types of result items per image.
	*/
	int count = result->GetItemsCount();
	cout << "Captured " << count << " items" << endl;
	for (int i = 0; i < count; i++) {
		const CCapturedResultItem* item = result->GetItem(i);

		CapturedResultItemType type = item->GetType();
		if (type == CapturedResultItemType::CRIT_BARCODE) {
			const CBarcodeResultItem* barcode = dynamic_cast<const CBarcodeResultItem*>(item);

			// Output the decoded barcode text.
			cout << ">>Item " << i << ": " << "Barcode," << barcode->GetText() << endl;
		}
		else if (type == CapturedResultItemType::CRIT_TEXT_LINE) {
			const CTextLineResultItem* textLine = dynamic_cast<const CTextLineResultItem*>(item);

			// Output the recognized text line.
			cout << ">>Item " << i << ": " << "TextLine," << textLine->GetText() << endl;
		}
		else if (type == CapturedResultItemType::CRIT_NORMALIZED_IMAGE) {
			const CNormalizedImageResultItem* normalizedImage = dynamic_cast<const CNormalizedImageResultItem*>(item);

			string outPath = "normalizedResult_";
			outPath += to_string(i) + ".png";

			CImageManager manager;

			// Save normalized iamge to file.
			errorcode = manager.SaveToFile(normalizedImage->GetImageData(), outPath.c_str());
			if (errorcode == 0) {
				cout << ">>Item " << i << ": " << "NormalizedImage," << outPath << endl;
			}
		}
	}

	// 5. Release the allocated memory.
	delete router, router = NULL;
	delete result, result = NULL;
	
	return 0;
}