//
// Created by Andrei Rybin on 8/9/16.
//

#ifndef NATIVEAPPTEST_NETWORKINGINTERFACE_H
#define NATIVEAPPTEST_NETWORKINGINTERFACE_H


#include <stdio.h>
#include <string>

#include <mutex>
#include <future>
#include <thread>
#include <vector>
#include <cmath>
#include <iostream>
#include <ostream>
#include <chrono>
#include <memory>

#include "downloader/HTTPResponse.h"
#include "downloader/URL.h"
#include "downloader/DNSResolver.h"
#include "downloader/Client.h"



class NetworkingInterface{
private:
    std::string url_;
    std::string locationOnDevice_;
    std::mutex iomutex;

    std::string workerFunction(int i, Client::thdata_ data);

public:
    NetworkingInterface(std::string  & url, std::string & location);
    ~NetworkingInterface();
    std::string download();

};


#endif //NATIVEAPPTEST_NETWORKINGINTERFACE_H
