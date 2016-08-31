#include <jni.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>

#include <chrono>
#include <pthread.h>
#include <vector>

#include "CurlDownloader.h"


extern "C" {

//prototype the function first

const char * ConvertJString(JNIEnv *env, jstring str) {
    jboolean isCopy;
    const jsize len = env->GetStringLength(str);
    const char *strChars = env->GetStringUTFChars(str, &isCopy);
    std::string result(strChars, len);
    //don't need memory leaks everywhere
    env->ReleaseStringUTFChars(str, strChars);
    const char * cResult = result.c_str();
    return cResult;
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
    return env->NewStringUTF("hello main callee");
}


jstring
Java_com_example_andreirybin_nativeapptest_asyncrequests_NativeAsyncNetworkRequest_downloadLocationFromNative(
        JNIEnv *env, jobject instance, jstring url_, jstring location_) {

    std::string cpp_url = ConvertJString(env, url_);
    std::string cpp_location = ConvertJString(env, location_);
    CurlDownloader downloader(cpp_url, cpp_location);
    std::string filePath = downloader.download();
    return env->NewStringUTF(filePath.c_str());
}
}
