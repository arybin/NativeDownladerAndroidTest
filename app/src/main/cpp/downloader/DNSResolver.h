/*
 * DNSResolver.h
 *
 *  Created on: Sep 12, 2012
 *      Author: andrei
 */

#ifndef DNSRESOLVER_H_
#define DNSRESOLVER_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

class DNSResolver {
private:
	string host;


public:
	DNSResolver(string url_to_resolve);
	virtual ~DNSResolver();
	string getHost();

};

#endif /* DNSRESOLVER_H_ */
