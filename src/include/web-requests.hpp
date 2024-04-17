#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#else
#include <curl/curl.h>

CURLcode res;

#endif

#include <events.hpp>

enum REQUEST_TYPE{
    GET,
    POST
};

/// @brief a simple container that contains meta information about a particular web request
struct WebRequestClient{
    const char* url;
    REQUEST_TYPE type;
    Event successEvent;
    Event failedEvent;
};

/// @brief creates a WebRequest Client and made it ready to be used
struct WebRequestClient CreateWebRequestClient(const char* url, REQUEST_TYPE type){
    WebRequestClient client;
    client.url = url;
    client.type = type;
    InitEvent(&(client.successEvent));
    InitEvent(&(client.failedEvent));
    return client;
}

void InitWebRequestService(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

/// @brief add a handler to handle success event
/// @param client 
/// @param successEventHandler 
void AddWebRequestSuccessEventHandler(struct WebRequestClient& client, EventHandler successEventHandler){
    AddEventHandler(client.successEvent, successEventHandler);
}

/// @brief add a handler to handle failed event
/// @param client
/// @param failedEventHandler
void AddWebRequestFailedEventHandler(struct WebRequestClient& client, EventHandler failedEventHandler){
    AddEventHandler(client.failedEvent, failedEventHandler);
}

static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    std::string* buffer = reinterpret_cast<std::string*>(userdata);
    buffer->append(ptr, realsize);
    return realsize;
}

/// @brief Initilaize the underlying dependencies using the data in the client
/// @return whether if client object has been initialized successfully or not.
std::string SendWebRequest(WebRequestClient& client) {
    CURL* curl;
    CURLcode res;
    std::string buffer;

    curl = curl_easy_init();
    if (!curl) {
        printf("cURL initialization failed.\n");
        return "";
    }

    curl_easy_setopt(curl, CURLOPT_URL, client.url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("cURL request failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return "";
    }

    EmitEvent(client.successEvent, buffer.c_str());
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return buffer;
}