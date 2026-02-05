#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <memory>
#include <type_traits>
#include <utility>

#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
#include "../../Include/DynamsoftIdentityUtility.h"

using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dlr;
using namespace dynamsoft::dlr::intermediate_results;
using namespace dynamsoft::dcp;
using namespace dynamsoft::license;
using namespace dynamsoft::basic_structures;
using namespace dynamsoft::utility;
using namespace dynamsoft::ddn;
using namespace dynamsoft::ddn::intermediate_results;
using namespace dynamsoft::id_utility;

// -----------------------------------------------------------------------------
// Windows-specific library linking.
// If you integrate this sample into your own build system (e.g., CMake),
// you can remove these pragmas and set linker inputs in your project instead.
// -----------------------------------------------------------------------------
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftIdentityUtilityx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftIdentityUtilityx86.lib")
#endif
#endif
namespace detail
{
	template <typename...>
	struct voider { typedef void type; };

	template <typename... Ts>
	using void_t = typename voider<Ts...>::type;

	template <typename Tp, typename = void>
	struct has_retain : std::false_type {};

	template <typename Tp>
	struct has_retain<Tp, void_t<decltype(std::declval<Tp&>().Retain())>> : std::true_type {};

	template <typename Tp, typename = void>
	struct has_release : std::false_type {};

	template <typename Tp>
	struct has_release<Tp, void_t<decltype(std::declval<Tp&>().Release())>> : std::true_type {};
}
struct ReleaseDeleter
{
	template <typename Tp, typename std::enable_if<detail::has_release<Tp>::value, int>::type = 0>
	void operator()(Tp* p) const
	{
		p->Release();
	}
};

template <typename Tp>
using RetainPtr = std::unique_ptr<Tp, ReleaseDeleter>;

template <typename Tp, typename std::enable_if<detail::has_retain<Tp>::value, int>::type = 0>
static RetainPtr<Tp> Retain(Tp* p) noexcept(noexcept(p->Retain()))
{
	if (p != nullptr)
		p->Retain();
	return RetainPtr<Tp>(p);
}

// -----------------------------------------------------------------------------
// MyIntermediateResultReceiver
//
// This receiver listens to intermediate results generated during the capture pipeline.
// We save several "units" keyed by original image hash id.
//
// Why store intermediate results?
// - We retain the useful intermediate results for subsequent use in the FindPrecisePortraitZone function.
//
// Note about hashId:
// - We provides a stable hash id per original image.
// - We can use it as the key to correlate intermediate units belonging to the same image.
// -----------------------------------------------------------------------------
class MyIntermediateResultReceiver : public CIntermediateResultReceiver
{
private:
	// Per-image cache of intermediate units required by FindPrecisePortraitZone().
	struct Data
	{
		RetainPtr<CScaledColourImageUnit> ptrScaledColourImageUnit;
		RetainPtr<CLocalizedTextLinesUnit> ptrLocalizedTextLinesUnit;
		RetainPtr<CRecognizedTextLinesUnit> ptrRecognizedTextLinesUnit;
		RetainPtr<CDetectedQuadsUnit> ptrDetectedQuadsUnit;
		RetainPtr<CDeskewedImageUnit> ptrDeskewedImageUnit;
	};

public:
	void OnScaledColourImageUnitReceived(CScaledColourImageUnit* pResult, const IntermediateResultExtraInfo* info) override
	{
		const char* hashId = pResult->GetOriginalImageHashId();
		shared_ptr<Data> data = GetData(hashId);
		data->ptrScaledColourImageUnit = Retain(pResult);
	}

	void OnLocalizedTextLinesReceived(CLocalizedTextLinesUnit* pResult, const IntermediateResultExtraInfo* info) override
	{
		if (info->isSectionLevelResult)
		{
			const char* hashId = pResult->GetOriginalImageHashId();
			shared_ptr<Data> data = GetData(hashId);
			data->ptrLocalizedTextLinesUnit = Retain(pResult);
		}
	}

	void OnRecognizedTextLinesReceived(CRecognizedTextLinesUnit* pResult, const IntermediateResultExtraInfo* info) override
	{
		if (info->isSectionLevelResult)
		{
			const char* hashId = pResult->GetOriginalImageHashId();
			shared_ptr<Data> data = GetData(hashId);
			data->ptrRecognizedTextLinesUnit = Retain(pResult);
		}
	}

	void OnDetectedQuadsReceived(CDetectedQuadsUnit* pResult, const IntermediateResultExtraInfo* info) override
	{
		if (!info->isSectionLevelResult)
		{
			const char* hashId = pResult->GetOriginalImageHashId();
			shared_ptr<Data> data = GetData(hashId);
			data->ptrDetectedQuadsUnit = Retain(pResult);
		}
	}

	void OnDeskewedImageReceived(CDeskewedImageUnit* pResult, const IntermediateResultExtraInfo* info) override
	{
		if (info->isSectionLevelResult)
		{
			const char* hashId = pResult->GetOriginalImageHashId();
			shared_ptr<Data> data = GetData(hashId);
			data->ptrDeskewedImageUnit = Retain(pResult);
		}
	}

	// Compute the precise portrait zone for the given image hash id.
	//
	// This uses the Identity Utility component:
	// - CIdentityProcessor::FindPrecisePortraitZone(...)
	//
	// Prerequisites:
	// - The intermediate results required by FindPrecisePortraitZone must have been received.
	int GetPrecisePortraitZone(const char* hashId, CQuadrilateral& quad)
	{
		auto it = m_dataMap.find(hashId);
		if (it == m_dataMap.end())
			return -1;
		shared_ptr<Data> data = it->second;
		CIdentityProcessor idProcessor;
		int errorCode = idProcessor.FindPortraitZone(
			data->ptrScaledColourImageUnit.get(),
			data->ptrLocalizedTextLinesUnit.get(),
			data->ptrRecognizedTextLinesUnit.get(),
			data->ptrDetectedQuadsUnit.get(),
			data->ptrDeskewedImageUnit.get(),
			quad);

		return errorCode;
	}
private:
	std::shared_ptr<Data> GetData(const char* hashId)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		std::shared_ptr<Data>& ret = m_dataMap[hashId];
		if (!ret)
		{
			ret = std::make_shared<Data>();
		}
		return ret;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Data>> m_dataMap;
	std::mutex m_mutex;
};

class DCPResultProcessor
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
	bool bIsPassport{ false };
	vector<string> rawText;

	// Read fields from parsed result item; keep raw MRZ lines and validated fields.
	// If a field's validation status is VS_FAILED, it indicates checksum/format validation failed.
	DCPResultProcessor FromParsedResultItem(const CParsedResultItem* item)
	{
		docType = item->GetCodeType();

		if (docType == "MRTD_TD3_PASSPORT")
		{
			if (item->GetFieldValidationStatus("passportNumber") != VS_FAILED && item->GetFieldValue("passportNumber") != NULL)
			{
				docId = item->GetFieldValue("passportNumber");
			}
			bIsPassport = true;
		}
		else if (item->GetFieldValidationStatus("documentNumber") != VS_FAILED && item->GetFieldValue("documentNumber") != NULL)
		{
			docId = item->GetFieldValue("documentNumber");
		}

		// Raw MRZ lines
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

		// Parsed fields (use only those that are not VS_FAILED)
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

	// Format parsed information for displaying in console.
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

const CImageData* GetOriginalImage(const CCapturedResult* pResult)
{
	int count = pResult->GetItemsCount();
	for (int i = 0; i < count; ++i)
	{
		const CCapturedResultItem* item = pResult->GetItem(i);
		if (item && item->GetType() == CRIT_ORIGINAL_IMAGE)
		{
			const CImageData* ret = static_cast<const COriginalImageResultItem*>(item)->GetImageData();
			return ret;
		}
	}
	return nullptr;
}

struct PathInfo {
	std::string directory;
	std::string filename;
	std::string stem;
	std::string extension;
};

PathInfo parse_path(const std::string& path)
{
	PathInfo info;

	size_t pos_sep = path.find_last_of("/\\");
	std::string name =
		(pos_sep == std::string::npos) ? path : path.substr(pos_sep + 1);
	std::string directory =
		(pos_sep == std::string::npos) ? "" : path.substr(0, pos_sep);

	info.filename = name;
	info.directory = directory;

	size_t pos_dot = name.find_last_of('.');
	if (pos_dot != std::string::npos && pos_dot != 0) {
		info.stem = name.substr(0, pos_dot);
		info.extension = name.substr(pos_dot + 1);
	}
	else {
		info.stem = name;
		info.extension.clear();
	}

	return info;
}

void SavePrecisePortrait(const PathInfo& pathInfo, MyIntermediateResultReceiver& irr, const char* hashId, const CImageData* originalImageData, int pageNumber)
{
	CQuadrilateral quad;
	int errorCode = irr.GetPrecisePortraitZone(hashId, quad);
	if(errorCode != ErrorCode::EC_OK)
		return;
	if (!originalImageData)
		return;
	CImageProcessor imgProcessor;
	CImageData* croppedImage = imgProcessor.CropAndDeskewImage(originalImageData, quad, 0, 0, 0, &errorCode);
	if (errorCode != ErrorCode::EC_OK)
	{
		std::cout << "crop image failed, error code : " << errorCode << std::endl;
		return;
	}
	string outputPath = pathInfo.stem + "_" + to_string(pageNumber) + "_portrait.png";
	CImageIO imgIo;
	errorCode = imgIo.SaveToFile(croppedImage, outputPath.c_str());
	if(errorCode == ErrorCode::EC_OK)
		std::cout << "Precise Portrait file: " << outputPath << std::endl;
	else
		std::cout << "Save precise portrait failed, error code : " << errorCode << std::endl;
	if (croppedImage)
		delete croppedImage;
}
void SaveProcessedDocumentResult(const PathInfo& pathInfo, const CCapturedResult* result, int pageNumber)
{
	CProcessedDocumentResult* pDocResult = result->GetProcessedDocumentResult();
	if (pDocResult == nullptr || pDocResult->GetEnhancedImageResultItemsCount() == 0)
	{
		std::cout << "Page-" << pageNumber << " No processed document result found." << std::endl;
		if(pDocResult)
			pDocResult->Release();
		return;
	}
	int count = pDocResult->GetEnhancedImageResultItemsCount();
	if(count > 0)
	{
		string outputPath = pathInfo.stem + "_" + to_string(pageNumber) + "_document.png";
		CImageIO imgIo;
		const CImageData* enhancedImage = pDocResult->GetEnhancedImageResultItem(0)->GetImageData();
		if (enhancedImage)
		{
			int errorCode = imgIo.SaveToFile(enhancedImage, outputPath.c_str());
			if (errorCode == ErrorCode::EC_OK)
				std::cout << "Document " << 1 << " file: " << outputPath << std::endl;
			else
				std::cout << "Save processed document failed, error code : " << errorCode << std::endl;
		}
	}
	if (pDocResult)
		pDocResult->Release();
}

void ProcessResult(const PathInfo& pathInfo, const CCapturedResult *result,MyIntermediateResultReceiver& irr, int printIndex)
{
	if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
	{
		cout << "Capture warning: Warning Code: " << result->GetErrorCode() << ", Warning String: " << result->GetErrorString() << endl;
	}
	else if (result->GetErrorCode() != ErrorCode::EC_OK)
	{
		cout << "Capture failed: Error Code: " << result->GetErrorCode() << ", Error String: " << result->GetErrorString() << endl;
	}

	int pageNumber = printIndex + 1;

	const CFileImageTag* tag = dynamic_cast<const CFileImageTag*>(result->GetOriginalImageTag());
	if (tag != nullptr)
	{
		pageNumber = tag->GetPageNumber() + 1;
		cout << "File: " << tag->GetFilePath() << endl;
		cout << "Page: " << pageNumber << endl;
	}

	CParsedResult* dcpResult = result->GetParsedResult();
	if (dcpResult == NULL || dcpResult->GetItemsCount() == 0)
	{
		cout << "No parsed results in page " << pageNumber << "." << endl;
		if (dcpResult)
			dcpResult->Release();
		return;
	}
	const char* hashId = result->GetOriginalImageHashId();
	bool bIsPassport = false;
	if (dcpResult->GetErrorCode() != EC_OK && dcpResult->GetErrorCode() != EC_UNSUPPORTED_JSON_KEY_WARNING)
	{
		cout << "Error: " << dcpResult->GetErrorString() << endl;
	}
	else
	{
		int lCount = dcpResult->GetItemsCount();
		cout << "Parsed " << lCount << " MRZ Zones" << endl;
		for (int i = 0; i < lCount; i++)
		{
			const CParsedResultItem *item = dcpResult->GetItem(i);

			DCPResultProcessor dcpResultProcessor;
			cout << dcpResultProcessor.FromParsedResultItem(item).ToString() << endl;
			if(!bIsPassport)
				bIsPassport = dcpResultProcessor.bIsPassport;
		}
	}
	if (dcpResult)
		dcpResult->Release();
	if (bIsPassport)
	{
		const CImageData* originalImage = GetOriginalImage(result);
		SavePrecisePortrait(pathInfo, irr, hashId, originalImage, pageNumber);
		SaveProcessedDocumentResult(pathInfo, result, pageNumber);
	}
	cout << endl;
	
}

int main()
{
	int errorCode = 0;
	char error[512];

	cout << "*******************************" << endl;
	cout << "Welcome to Dynamsoft MRZ Sample" << endl;
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

		CIntermediateResultManager* irm = cvRouter->GetIntermediateResultManager();
		MyIntermediateResultReceiver* myReceiver = nullptr;

		// 3. Set input image source
		string imgPath;

		while (1)
		{
			// Re-create receiver for each run, so cached intermediate data does not mix
			// between different input files.
			if (myReceiver)
			{
				irm->RemoveResultReceiver(myReceiver);
				delete myReceiver, myReceiver = nullptr;
			}
			//4. Add intermediate result receiver.
			myReceiver = new MyIntermediateResultReceiver();
			irm->AddResultReceiver(myReceiver);

			//5. Get input image path from console.
			cout << endl
				<< ">> Input your image full path:" << endl
				<< ">> 'Enter' for sample image or 'Q'/'q' to quit" << endl;

			getline(cin, imgPath);

			if (imgPath == "q" || imgPath == "Q")
			{
				break;
			}

			if(imgPath.empty())
				imgPath = "../../Images/passport-sample.jpg";

			if (imgPath.length() >= 2 && imgPath.front() == '"' && imgPath.back() == '"')
				imgPath = imgPath.substr(1, imgPath.length() - 2);

			PathInfo inputFileInfo = parse_path(imgPath);
			// 6. Capture.
			CCapturedResultArray* resultArray = cvRouter->CaptureMultiPages(imgPath.c_str(), "ReadPassportAndId");
			int count = resultArray->GetResultsCount();
			// 7. Process the capture results.
			for (int i = 0; i < count; ++i)
			{
				const CCapturedResult* result = resultArray->GetResult(i);
				ProcessResult(inputFileInfo, result, *myReceiver, i);
			}
			// 8. Release the capture result.
			if (resultArray)
				resultArray->Release();
		}
		// 9. Release intermediate result receiver.
		if(myReceiver)
			delete myReceiver, myReceiver = nullptr;
		// 10. Release the allocated memory.
		delete cvRouter, cvRouter = NULL;
	}
	return 0;
}