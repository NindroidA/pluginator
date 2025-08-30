#include "../include/http_client.hpp"
#include "../include/lang.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

struct DownloadData {
    ofstream* file;
    function<void(double)> progressCallback;
};

static size_t WriteToFile(void* contents, size_t size, size_t nmemb, void* userp) {
    ofstream* file = static_cast<ofstream*>(userp);
    size_t totalSize = size * nmemb;
    
    if (file && file->is_open()) {
        file->write(static_cast<const char*>(contents), totalSize);
        return file->good() ? totalSize : 0;
    }
    
    return 0;
}

// progress callback with fancy styling
static int FancyProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal <= 0) return 0;
    
    static auto startTime = chrono::steady_clock::now();
    
    double progress = (double)dlnow / (double)dltotal * 100.0;
    
    // only update every 1% or at completion
    static int lastPercent = -1;
    int currentPercent = (int)progress;
    
    if (currentPercent != lastPercent || progress >= 100.0) {
        lastPercent = currentPercent;
        
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count() / 1000.0;
        
        // calculate download speed
        double speed = 0.0;
        if (elapsed > 0) {
            speed = (double)dlnow / elapsed; // bytes per second
        }
        
        // create the progress bar using utils
        string progressBar = Utils::createProgressBar(progress, 40);
        
        // print the fancy shmancy progress line
        printf("\r  %s%s%s %s%5.1f%%%s %s",
            Colors::BLUE.c_str(),
            progressBar.c_str(),
            Colors::NC.c_str(),
            Colors::GREEN.c_str(),
            progress,
            Colors::NC.c_str(),
            Utils::formatBytes((int64_t)dlnow).c_str()
        );
        
        if (dltotal > 0) {
            printf("/%s", Utils::formatBytes((int64_t)dltotal).c_str());
        }
        
        if (speed > 0) {
            printf(" %s%s%s",
                Colors::YELLOW.c_str(),
                Utils::formatSpeed(speed).c_str(),
                Colors::NC.c_str()
            );
        }
        
        if (elapsed > 0) {
            printf(" %s", Utils::formatDuration(elapsed).c_str());
        }
        
        fflush(stdout);
    }
    
    return 0;
}

HttpClient::HttpClient() : userAgent("Pluginator"), timeoutSeconds(500) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient() {
    curl_global_cleanup();
}

size_t HttpClient::WriteCallback(void* contents, size_t size, size_t nmemb, string* res) {
    size_t totalSize = size * nmemb;
    res -> append((char*) contents, totalSize);
    return totalSize;
}

/**
 * setUserAgent
 * @brief function to set the user agent
 * @param a string reference to the name of the user agent we want to have set
 */
void HttpClient::setUserAgent(const string& a) { userAgent = a; }

/**
 * setTimeout
 * @brief function to set the http timeout
 * @param s int timeout in seconds
 */
void HttpClient::setTimeout(int s) { timeoutSeconds = s; }

/**
 * get
 * @brief actual GET http function
 * @param url string reference to the url
 * @param headers map reference of two strings containing the necessary headers
 * @returns the http response 
 */
HttpResponse HttpClient::get(const string& url, const map<string, string>& headers) {
    HttpResponse res;
    CURL* curl = curl_easy_init();

    // make sure curl is initialized
    if (!curl) {
        res.error = "Failed to initialize CURL";
        return res;
    }

    // set basic options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res.body);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // set headers
    struct curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        string headerStr = header.first + ": " + header.second;
        headerList = curl_slist_append(headerList, headerStr.c_str());
    }
    if (headerList) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }

    // perform request
    CURLcode r = curl_easy_perform(curl);

    if (r != CURLE_OK) {
        res.error = curl_easy_strerror(r);
        res.success = false;
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.statusCode);
        res.success = (res.statusCode >= 200 && res.statusCode < 300);

        if (!res.success) {
            res.error = "HTTP " + to_string(res.statusCode);
        }
    }

    // cleanup
    if (headerList) {
        curl_slist_free_all(headerList);
    }
    curl_easy_cleanup(curl);

    return res;
}

/**
 * downloadFile
 * @brief function to download a file
 * @param url string reference to the download url
 * @param outPath string reference to the path we wanna drop the file
 * @param progressCallback void function that takes in a double parameter that keeps track of the download progress
 */
bool HttpClient::downloadFile(const string& url, const string& outPath, function<void(double)> progressCallback) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    ofstream file(outPath, ios::binary);
    if (!file.is_open()) {
        curl_easy_cleanup(curl);
        return false;
    }

    // set options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    
    // timeout settings
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1024L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
    
    // always show progress for downloads - ignore the callback parameter for safety
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, FancyProgressCallback);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // perform download
    CURLcode res = curl_easy_perform(curl);
    
    // always print newline after progress
    printf("\n");
    
    file.close();

    bool success = (res == CURLE_OK);
    
    if (success) {
        long statusCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
        success = (statusCode >= 200 && statusCode < 300);
    }

    if (!success) {
        remove(outPath.c_str());
    }

    curl_easy_cleanup(curl);
    return success;
}

/**
 * urlEncode
 * @brief url encoding function (ngl i honestly don't 100% know what it does but it do)
 * @param str string reference to SOMETHIN lmao
 */
string HttpClient::urlEncode(const string& str) {
    CURL* curl = curl_easy_init();
    if (!curl) return str;

    char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
    string result(encoded);
    curl_free(encoded);
    curl_easy_cleanup(curl);

    return result;
}