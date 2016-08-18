//
// Created by Andrei Rybin on 8/9/16.
//

#include "NetworkingInterface.h"
#include <functional>


NetworkingInterface::NetworkingInterface(std::string & url, std::string & location) {
    url_ = url;
    locationOnDevice_ = location;
}

NetworkingInterface::~NetworkingInterface() {
    //default empty destructor, nothing allocated on the heap though
}

std::string NetworkingInterface::download() {
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    HTTPResponse httpResponse;
    URL u(url_);
    DNSResolver dnsResolver(u.host());

    int port = u.port();
    std::string host = dnsResolver.getHost();
    std::string fileName(u.file());

    Client client(port, host);

    client.constructRequest("HEAD", u.host(), u.path(), "", "");
    client.sendRequest();
    std::string response = client.readResponse();
    httpResponse.parse(response);
    std::string totalLength = httpResponse.header(string("Content-Length"));
    std::cout<<"total length: "<< totalLength;

    const int numberOfThreads = 1;
    client.constructThreads(numberOfThreads, totalLength, u.host(), u.path());

    std::cout << "\nmain thread id=>" << std::this_thread::get_id();
    std::future<std::string> future;
    //std::vector<std::future<std::string>> futureVec;


    for (int i = 0; i < numberOfThreads; ++i) {
        auto clientPart = client.myData.at(i);
        //auto f = std::async(std::launch::async, &NetworkingInterface::workerFunction, this, i, clientPart);
        //futureVec.push_back(async(std::launch::async, &NetworkingInterface::workerFunction,this, i, clientPart));
    }

    std::string fileStoredLocation = locationOnDevice_ + fileName;
    ofstream file(locationOnDevice_.c_str());


//    for (auto& fut : futureVec) {
//        std::lock_guard<std::mutex> lg(iomutex);
////        auto x = fut.get();
////        file << x;
//    }

    std::cout << "\ndone\n";


    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << duration<<std::endl;

    return locationOnDevice_;

}

std::string NetworkingInterface::workerFunction(int i, Client::thdata_ data) {
    std::lock_guard<std::mutex> lg(iomutex);
    Client c(data.port, data.ip);

    c.constructRequest("GET", data.host, data.path, data.str_start, data.str_end);
    string req = c.getRequest();
    c.sendRequest();
    string downloaded = c.download(data.starting_point, data.ending_point);
    return downloaded;
}