/*
 * DNSResolver.cpp
 *
 *  Created on: Sep 12, 2012
 *      Author: andrei
 */

#include "DNSResolver.h"
//http://long.ccaba.upc.edu/long/045Guidelines/eva/ipv6.html#daytimeClient4
DNSResolver::DNSResolver(string url_to_resolve) {
	host = url_to_resolve;

}

DNSResolver::~DNSResolver() {
}
//resolves the host into a string
string DNSResolver::getHost() {

	struct hostent *hostEntry;

	//cout<<host<<endl;

	hostEntry = gethostbyname(host.c_str());
		if (!hostEntry) {
			cout << "No such host name: " << host << endl;
			return NULL;
		}

	return inet_ntoa(*(struct in_addr *)hostEntry->h_addr_list[0]);
}


