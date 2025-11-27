#include "starmap/utils/HttpClient.h"
#include <curl/curl.h>
#include <stdexcept>
#include <memory>

namespace starmap {
namespace utils {

// Callback per scrivere i dati ricevuti
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpClient::Impl {
public:
    Impl() : timeout_(30L) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
        if (!curl_) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    ~Impl() {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
        curl_global_cleanup();
    }

    std::string performRequest(const std::string& url, 
                              const std::string& method,
                              const std::string& postData,
                              const std::map<std::string, std::string>& headers) {
        std::string response;
        
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout_);
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        
        // Headers personalizzati
        struct curl_slist* headerList = nullptr;
        for (const auto& [key, value] : headers) {
            std::string header = key + ": " + value;
            headerList = curl_slist_append(headerList, header.c_str());
        }
        
        if (headerList) {
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headerList);
        }
        
        // POST data
        if (method == "POST" && !postData.empty()) {
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, postData.c_str());
        }
        
        CURLcode res = curl_easy_perform(curl_);
        
        if (headerList) {
            curl_slist_free_all(headerList);
        }
        
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL error: ") + 
                                   curl_easy_strerror(res));
        }
        
        long httpCode = 0;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &httpCode);
        
        if (httpCode >= 400) {
            throw std::runtime_error("HTTP error: " + std::to_string(httpCode));
        }
        
        return response;
    }

    void setTimeout(long seconds) {
        timeout_ = seconds;
    }

private:
    CURL* curl_;
    long timeout_;
};

HttpClient::HttpClient() : pImpl_(new Impl()) {}

HttpClient::~HttpClient() {
    delete pImpl_;
}

std::string HttpClient::get(const std::string& url, 
                           const std::map<std::string, std::string>& headers) {
    return pImpl_->performRequest(url, "GET", "", headers);
}

std::string HttpClient::post(const std::string& url, 
                            const std::string& data,
                            const std::map<std::string, std::string>& headers) {
    return pImpl_->performRequest(url, "POST", data, headers);
}

void HttpClient::setTimeout(long seconds) {
    pImpl_->setTimeout(seconds);
}

} // namespace utils
} // namespace starmap
