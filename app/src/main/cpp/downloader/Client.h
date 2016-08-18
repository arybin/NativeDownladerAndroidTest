/*
 * Client.h
 *
 *  Created on: Sep 12, 2012
 *      Author: andrei
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <inttypes.h>

#include "HTTPRequest.h"
using namespace std;

class Client {
private:
	struct sockaddr_in server;
	const char *ipaddress;
	int option, s, port;
	long nwritten, nleft;
	string line;
	char *buf;// = (char *) calloc(1024, sizeof(char));
	const char *ptr;
	string URL;
	string request;
	HTTPRequest h_request;

	void fillMyData(uint64_t starting_point, uint64_t total_int_length,
			uint64_t constant_increase,string host,string path);

public:
	// structure to hold data passed to a thread
	typedef struct thdata_ {
	    uint64_t starting_point;
	    uint64_t ending_point;
	    string str_start;
	    string str_end;
	    string host;
	    string path;
	    //just added these lines
	    int port;
	    string ip;
	} thdata;

	vector<thdata> myData;

	Client(int p,string host);
	virtual ~Client();
	void createSocket();
	void sendRequest();
	string readResponse();
	string download(uint64_t start,uint64_t end);
	void constructRequest(string requestType,string host,string path,string begBytes,string endBytes);
	void printRequest();

	void constructThreads(int number_of_threads,string total_length,string host,string path);
	string getRequest();

};

#endif /* CLIENT_H_ */
