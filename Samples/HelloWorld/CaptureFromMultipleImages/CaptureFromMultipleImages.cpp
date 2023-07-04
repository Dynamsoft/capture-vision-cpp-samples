#include<iostream>
#include<string>
#include "../../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::dlr;
using namespace dynamsoft::ddn;
using namespace dynamsoft::license;
using namespace dynamsoft::basic_structures;
using namespace dynamsoft::utility;

// The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class MyImageSourceStateListener : public CImageSourceStateListener
{
private:
	CCaptureVisionRouter* m_router;

public:
	MyImageSourceStateListener(CCaptureVisionRouter* router) {
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
	virtual void OnCapturedResultReceived(const CCapturedResult* pResult)
	{
		const CFileImageTag *tag = dynamic_cast<const CFileImageTag*>(pResult->GetSourceImageTag());

		cout << "File: " << tag->GetFilePath() << endl;
		cout << "Page: " << tag->GetPageNumber() << endl;

		if (pResult->GetErrorCode() != EC_OK)
		{
			cout << "Error: " << pResult->GetErrorString() << endl;
		}
		else
		{
			int lCount = pResult->GetCount();
			cout << "Captured " << lCount << " items" << endl;
			for (int i = 0; i < lCount; i++) {
				const CCapturedResultItem* item = pResult->GetItem(i);

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
					int errorcode = manager.SaveToFile(normalizedImage->GetImageData(), outPath.c_str());
					if (errorcode == 0) {
						cout << ">>Item " << i << ": " << "NormalizedImage," << outPath << endl;
					}
				}
			}
		}

		cout << endl;
	}
};

int main()
{
	int errorcode = 0;
	char error[512];
	string imgPath;

	// 1.Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	errorcode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	cout << "License initialization: " << errorcode << "," << error << endl;

	// 2.Create an instance of CCaptureVisionRouter.
	CCaptureVisionRouter *router = new CCaptureVisionRouter;

	cout << endl << ">> Input your image directory full path:" << endl;
	cin >> imgPath;

	// 3.Set input image source
	CDirectoryFetcher *dirFetcher = new CDirectoryFetcher;
	// Replace it with your image directory
	dirFetcher->SetDirectory(imgPath.c_str());

	router->SetInput(dirFetcher);

	// 4. Add image source state listener
	CImageSourceStateListener *listener = new MyImageSourceStateListener(router);
	router->AddImageSourceStateListener(listener);

	// 5. Add captured result receiver
	CCapturedResultReceiver *recv = new MyResultReceiver;
	router->AddResultReceiver(recv);

	// 6. Start capturing
	router->StartCapturing(CPresetTemplate::PT_DEFAULT, true);

	// 7. Release the allocated memory.
	delete router, router = NULL;
	delete dirFetcher, dirFetcher = NULL;
	delete listener, listener = NULL;
	delete recv, recv = NULL;
	
	return 0;
}