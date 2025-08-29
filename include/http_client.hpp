#pragma once
#include <string>
#include <map>
#include <functional>

using namespace std;

struct HttpResponse {
    long statusCode;
    string body;
    string error;
    bool success;
    
    HttpResponse() : statusCode(0), success(false) {}
};

class HttpClient {
    string userAgent;
    int timeoutSeconds;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* response);
    
public:
    HttpClient();
    ~HttpClient();
    
    // config
    void setUserAgent(const string& agent);
    void setTimeout(int seconds);
    
    // HTTP methods
    HttpResponse get(const string& url, const map<string, string>& headers = {});
    //HttpResponse post(const string& url, const string& data, const map<string, string>& headers = {});
    
    // utils
    bool downloadFile(const string& url, const string& outputPath, 
                     function<void(double)> progressCallback = nullptr);
    
    // url encoding
    static string urlEncode(const string& str);
};