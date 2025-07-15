#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <regex>

#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"

using namespace dynamsoft;
using namespace dynamsoft::cvr;
using namespace dynamsoft::license;
using namespace dynamsoft::dcp;
using namespace dynamsoft::dbr;
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCodeParserx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCodeParserx86.lib")
#endif
#endif

bool isAI(const std::string& x)
{
    static const std::regex pattern(R"(\d+|(\d+n))");
    return std::regex_match(x, pattern);
}

std::map<std::string, std::pair<std::string, std::string>> groupByFirstLayerAndFindInfo(const std::vector<std::string>& paths) 
{
    std::map<std::string, std::pair<std::string, std::string>> grouped;
    for (const auto& path : paths) {
        size_t pos = path.find('.');
        std::string firstLayer = (pos == std::string::npos) ? path : path.substr(0, pos);
        size_t posEnd = path.rfind('.');
        std::string lastLayer = (posEnd == std::string::npos) ? path : path.substr(posEnd + 1);
        if (lastLayer == firstLayer + "AI")
            grouped[firstLayer].first = lastLayer;
        if (lastLayer == firstLayer + "Data")
            grouped[firstLayer].second = lastLayer;
    }
    return grouped;
}
class GS1AIResult
{
public:
    GS1AIResult(const CParsedResultItem* result) :m_pRes(result) {}
    std::string toString()
    {
        std::string ret;
        int fieldsCount = m_pRes->GetFieldCount();
        std::vector<std::string> fieldNames;
        for (int i = 0; i < fieldsCount; ++i)
        {
            const char* name = m_pRes->GetFieldName(i);
            if (name)
                fieldNames.emplace_back(name);
        }
        std::map<std::string, std::pair<std::string, std::string>> nameMap = groupByFirstLayerAndFindInfo(fieldNames);
        for (auto& kv : nameMap)
        {
            if (isAI(kv.first))
            {
                const char* ai = m_pRes->GetFieldValue(kv.second.first.c_str());
                const char* value = m_pRes->GetFieldValue(kv.second.second.c_str());
                ret.append("AI: ");
                ret.append(kv.first);
                ret.append(" (" + (ai ? std::string(ai) : std::string("")) + ")");
                ret.append(", Value: " + (value ? std::string(value) : std::string("")) + "\n");
            }
        }
        if (!ret.empty())
            ret.insert(0, "Parsed GS1 Application Identifiers (AIs):\n");
        return ret;
    }
private:
    const CParsedResultItem* m_pRes{ nullptr };
};
int main() {
    std::cout << "***********************************************************" << std::endl;
    std::cout << "    Welcome to Dynamsoft Capture Vision - GS1 AI Sample" << std::endl;
    std::cout << "***********************************************************" << std::endl;

    int errorCode;
    char errorMessage[1024];

    // Initialize license.
    // The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
    // You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples&package=c_cpp
    errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMessage, 1024);
    if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING) {
        std::cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorMessage << std::endl;
        return -1;
    }

    CCaptureVisionRouter* cvRouter = new CCaptureVisionRouter();

    std::string templatePath = "../../CustomTemplates/ReadGS1AIBarcode.json";
    errorCode = cvRouter->InitSettingsFromFile(templatePath.c_str(), errorMessage, 1024);
    if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING) {
        std::cout << "Init template failed: " << errorCode << ", " << errorMessage << std::endl;
    }
    else
    {
        while (true)
        {
            std::cout << std::endl;
            std::cout << ">> Input your image full path:" << std::endl;
            std::cout << ">> 'Enter' for sample image or 'Q'/'q' to quit" << std::endl;
            std::string imagePath;
            std::getline(std::cin, imagePath);

            if (imagePath == "q" || imagePath == "Q")
                break;

            if (imagePath.empty())
                imagePath = "../../Images/gs1-ai-sample.png";

            if (imagePath.length() >= 2 && imagePath.front() == '"' && imagePath.back() == '"')
                imagePath = imagePath.substr(1, imagePath.length() - 2);

            if (!std::ifstream(imagePath).good())
            {
                std::cout << "The image path does not exist." << std::endl;
                continue;
            }

            CCapturedResultArray* resultArray = cvRouter->CaptureMultiPages(imagePath.c_str(), "ReadGS1AIBarcode");
            int count = resultArray->GetResultsCount();
            bool noResultPrint = true;
            for (int i = 0; i < count; ++i)
            {
                const CCapturedResult* result = resultArray->GetResult(i);

                if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
                    std::cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << std::endl;
                else if (result->GetErrorCode() != ErrorCode::EC_OK)
                    std::cout << "Error: " << result->GetErrorCode() << ", " << result->GetErrorString() << std::endl;

                CParsedResult* parsedResult = result->GetParsedResult();
                if (parsedResult)
                {
                    int itemCount = parsedResult->GetItemsCount();
                    for (int j = 0; j < itemCount; ++j)
                    {
                        const CParsedResultItem* item = parsedResult->GetItem(j);
                        const CCapturedResultItem* referenceItem = item->GetReferenceItem();
                        if (referenceItem && referenceItem->GetType() == CapturedResultItemType::CRIT_BARCODE)
                        {
                            const CBarcodeResultItem* barcodeItem = static_cast<const CBarcodeResultItem*>(referenceItem);
                            std::cout << "Barcode result: " << barcodeItem->GetText() << std::endl;
                        }
                        GS1AIResult gs1Result(item);
                        std::cout << gs1Result.toString();
                    }
                    if (noResultPrint)
                        noResultPrint = false;
                    parsedResult->Release();
                }
            }

            if (noResultPrint)
            {
                std::cout << "No GS1 Application Identifiers (AIs) barcode found in the image." << std::endl;
            }
            resultArray->Release();
        }
    }
    delete cvRouter, cvRouter = nullptr;
    return 0;
}