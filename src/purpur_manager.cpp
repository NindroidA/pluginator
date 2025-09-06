#include "../include/purpur_manager.hpp"
#include "../include/json_helper.hpp"
#include "../include/lang.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

PurpurManager::PurpurManager(Logger& log) : logger(log) {
    httpClient.setUserAgent("Pluginator");
    httpClient.setTimeout(30);
}

PurpurInfo PurpurManager::checkPurpurUpdate() {
    PurpurInfo info;
    
    logger.log(LANG("purpur.checking"));
    
    // get latest purpur version
    HttpResponse versionResponse = httpClient.get("https://api.purpurmc.org/v2/purpur");
    
    if (!versionResponse.success) {
        info.error = LANGF("purpur.fetch_failed", versionResponse.error);
        return info;
    }
    
    // extract current version
    info.latestVersion = JsonHelper::extractNestedValue(versionResponse.body, "metadata.current");
    
    if (info.latestVersion.empty()) {
        info.error = LANG("purpur.parse_failed");
        return info;
    }
    
    logger.log(LANGF("purpur.latest_version", info.latestVersion));
    
    // get latest build info for this version
    string buildUrl = "https://api.purpurmc.org/v2/purpur/" + info.latestVersion + "/latest";
    HttpResponse buildResponse = httpClient.get(buildUrl);
    
    if (!buildResponse.success) {
        info.error = LANGF("purpur.build.fetch_failed", buildResponse.error);
        return info;
    }
    
    info.latestBuild = JsonHelper::extractValue(buildResponse.body, "build");
    info.buildResult = JsonHelper::extractValue(buildResponse.body, "result");
    
    if (info.latestBuild.empty()) {
        info.error = LANG("purpur.build.parse_failed");
        return info;
    }
    
    logger.log(LANGF("purpur.latest_build", info.latestBuild));
    logger.log(LANGF("purpur.build_result", info.buildResult));
    
    // check if build was successful
    if (info.buildResult == "FAILURE") {
        info.error = LANG("purpur.build_failed");
        return info;
    }
    
    // construct download URL
    info.downloadUrl = "https://api.purpurmc.org/v2/purpur/" + info.latestVersion + "/latest/download";
    info.hasUpdate = true; // will determine if update is needed
    
    return info;
}


bool PurpurManager::downloadPurpur(const string& version, const string& outputPath) {
    cout << endl;
    logger.log(LANGF("purpur.downloading_version", version));
    logger.log(LANGF("purpur.download_path", outputPath));
    logger.log(LANG("purpur.download_wait_message"));
    cout << endl;
    
    string url = "https://api.purpurmc.org/v2/purpur/" + version + "/latest/download";
    
    // the fancy progress bar will be shown by HttpClient
    bool success = httpClient.downloadFile(url, outputPath, nullptr);
    
    cout << endl; // extra spacing after progress bar
    
    if (success) {
        // show file size after successful download
        ifstream file(outputPath, ios::binary | ios::ate);
        if (file.is_open()) {
            size_t fileSize = file.tellg();
            file.close();
            
            logger.success(LANGF("purpur.download_success_with_size", 
                                version, 
                                Utils::formatBytes((int64_t)fileSize)));
        } else {
            logger.success(LANGF("purpur.download_success", version));
        }
    } else {
        logger.err(LANGF("purpur.download_failed", version));
    }
    
    return success;
}

void PurpurManager::showPurpurStatus() {
    PurpurInfo info = checkPurpurUpdate();
    string buildRes;
    
    cout << endl << Colors::BLUE << LANG("purpur.status.run") << Colors::NC << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    
    if (!info.error.empty()) {
        cout << Colors::RED << "Error: " << info.error << Colors::NC << endl;
        return;
    }
    
    if (info.buildResult == "SUCCESS") {
        buildRes = Colors::GREEN.c_str() + info.buildResult + Colors::NC.c_str();
    } else {
        buildRes = Colors::YELLOW.c_str() + info.buildResult + Colors::NC.c_str();
    }

    cout << LANGF("purpur.status.summary", info.latestVersion, info.latestBuild, buildRes);
    
    cout << LANGF("purpur.status.url", info.downloadUrl) << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
}