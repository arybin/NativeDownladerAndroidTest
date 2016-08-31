//
// Created by Andrei Rybin on 8/31/16.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
#ifndef NATIVEAPPTEST_CURLDOWNLOADER_H
#define NATIVEAPPTEST_CURLDOWNLOADER_H

#include <stdio.h>
#include <string>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class CurlDownloader{

private:
    std::string url_;
    std::string localPath_;
    std::string absolutePath_;

    static size_t write_data(void * ptr, size_t size, size_t nmemb, void * stream) {
        size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
        return written;
    }


public:
    CurlDownloader(std::string & url, std::string & localPath);
    std::string download();
};



#endif //NATIVEAPPTEST_CURLDOWNLOADER_H

#pragma clang diagnostic pop