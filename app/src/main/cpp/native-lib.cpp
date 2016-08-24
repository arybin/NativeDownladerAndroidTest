#include <jni.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>

#include <chrono>
#include <pthread.h>
#include <vector>

#include "downloader/HTTPResponse.h"
#include "downloader/URL.h"
#include "downloader/DNSResolver.h"
#include "downloader/Client.h"


extern "C" {

//prototype the function first
void *print(void *);

std::string ConvertJString(JNIEnv *env, jstring str) {
    jboolean isCopy;
    const jsize len = env->GetStringLength(str);
    const char *strChars = env->GetStringUTFChars(str, &isCopy);
    std::string result(strChars, len);
    //don't need memory leaks everywhere
    env->ReleaseStringUTFChars(str, strChars);
    return result;
}

std::string startDownloading(std::string &url, std::string &filePath) {
    HTTPResponse httpResponse;
    URL u(url);
    DNSResolver dnsResolver(u.host());

    int port = u.port();
    std::string host = dnsResolver.getHost();
    std::string fileName(u.file());

    Client client(port, host);

    client.constructRequest("HEAD", u.host(), u.path(), "", "");
    std::string requestBody = client.getRequest();
    client.sendRequest();
    std::string response = client.readResponse();
    httpResponse.parse(response);
    std::string totalLength = httpResponse.header(string("Content-Length"));

    const int numberOfThreads = 2;
    client.constructThreads(numberOfThreads, totalLength, u.host(), u.path());

    std::vector<pthread_t> threads;

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_t t;
        pthread_create(&t, NULL, &print, (void *) &client.myData.at(i));
        threads.emplace_back(t);
    }

    filePath.append(fileName);

    std::string *ret;
    std::ofstream file(filePath.c_str());
    void **r = (void **) (&ret);
    for (int i = 0; i < threads.size(); i++) {
        pthread_join(threads.at(i), r);
        if (ret != 0) {
            file << *ret;

        }
    }

    file.close();
    return filePath;
}

jstring
Java_com_example_andreirybin_nativeapptest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jstring url,
        jstring locationOnDevice
) {
    std::string cpp_url = ConvertJString(env, url);
    std::string cpp_location = ConvertJString(env, locationOnDevice);
    std::string fileLocation = startDownloading(cpp_url, cpp_location);
    return env->NewStringUTF(fileLocation.c_str());
}

//thread routine
void *print(void *ptr) {
    Client::thdata_ *data;
    data = (Client::thdata_ *) ptr;

    Client c(data->port, data->ip);
    c.constructRequest("GET", data->host, data->path, data->str_start,
                       data->str_end);
    string req = c.getRequest();
    c.sendRequest();
    string *downloaded = new string(c.download(data->starting_point, data->ending_point));

    return (void *) downloaded;
}

jstring
Java_com_example_andreirybin_nativeapptest_asyncrequests_NativeAsyncNetworkRequest_downloadLocationFromNative(
        JNIEnv *env, jobject instance, jstring url_, jstring location_) {

    std::string cpp_url = ConvertJString(env, url_);
    std::string cpp_location = ConvertJString(env, location_);
    std::string fileLocation = startDownloading(cpp_url, cpp_location);

    return env->NewStringUTF(fileLocation.c_str());
}
}
