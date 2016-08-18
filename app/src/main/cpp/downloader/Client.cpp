/*
 * Client.cpp
 *
 *  Created on: Sep 12, 2012
 *      Author: andrei
 */

#include "Client.h"

Client::Client(int p, string host) {
	port = p;
	ipaddress = host.c_str();
	buf = (char *) calloc(1024, sizeof(char));

	createSocket();
}

Client::~Client() {
}

void Client::createSocket() {
	// setup socket address structure
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (!inet_aton(ipaddress, &server.sin_addr)) {
		printf("inet_addr() conversion error\n");
		exit(-1);
	}

	// create socket
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		exit(-1);
	}

	// connect to server
	if (connect(s, (const struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect");
		exit(-1);
	}

}

void Client::sendRequest() {

	ptr = request.c_str();
	nleft = request.length();
	// write the data to the server
	while (nleft) {
		if ((nwritten = send(s, ptr, nleft, 0)) < 0) {
			if (errno == EINTR) {
				// the socket call was interrupted -- try again
				nwritten = 0;
			} else {
				// an error occurred, so break out
				perror("write");
				break;
			}
		} else if (nwritten == 0) {
			cout << "the socket is closed" << endl;
			// the socket is closed
			break;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
}

string Client::readResponse() {
	// read the response

	buf = (char *) calloc(1024, sizeof(char));
	memset(buf, 0, 1024);
	string downloaded;

	while (true) {
		long nread = recv(s, buf, 1024, 0);
		downloaded.append(buf, nread);
		if (downloaded.at(downloaded.length() - 1) == 10
				&& downloaded.at(downloaded.length() - 3) == 10) {

			break;

		}

	}
	return downloaded;
}

string Client::download(uint64_t start, uint64_t end) {

	uint64_t amount_to_receive = 0;
	uint64_t total_amount = end - start;
	char * tmp = new char[total_amount + 1];

    string downloaded;// = new string();

	while (amount_to_receive < total_amount) {
		uint64_t nread = (uint64_t) recv(s, tmp, total_amount, 0);
		if (nread < 0) {
			if (errno == EINTR)
				// the socket call was interrupted -- try again
				continue;
			else
				// an error occurred, so break out
				return 0;
		} else if (nread == 0) {
			// the socket is closed
			cout << "the socket is closed" << endl;
			return 0;

		}

		amount_to_receive += nread;

 		downloaded.append(tmp,nread);

	}
	delete[] tmp;

	string temp;
    // check for "\r\n\r\n"
	uint64_t i = 0;
	while (true) {
		if (downloaded.at(i) == 13 &&
				downloaded.at(i+1)==10 &&
				downloaded.at(i + 2) == 13 &&
				downloaded.at(i+3)==10) {
			// carriage returns followed by new lines
			i += 4;
			break;
		}
		i++;
	}
    //only need the part without the headers, AKA actual data
    std::string generated = downloaded.substr(i, downloaded.length() +1 - i);
	return generated;
}

void Client::constructRequest(string requestType, string host, string path,
		string begBytes, string endBytes) {
	h_request = HTTPRequest();
	h_request.uri(path);
	h_request.version("HTTP/1.1");
	h_request.header("Host", host);
	if (requestType.compare("HEAD") == 0) {
		h_request.method("HEAD");
	} else if (requestType.compare("GET") == 0) {
		h_request.method("GET");
		h_request.header("Range", "bytes=" + begBytes + "-" + endBytes); //put in manually for now
	}

	request = h_request.str();
}

void Client::printRequest() {
	if (!request.empty()) {
		cout << request << endl;
	}
}

void Client::fillMyData(uint64_t starting_point, uint64_t total_int_length,
		uint64_t constant_increase, string h, string p) {

	while (starting_point < total_int_length) {
		thdata data;
		stringstream ss;
		if (starting_point == 0) {
			data.starting_point = starting_point;
			ss << starting_point;
			data.str_start = ss.str();
			ss.str("");
			starting_point += constant_increase;
			data.ending_point = starting_point;
			ss << starting_point;
			data.str_end = ss.str();
			ss.str("");

			data.host += h;
			data.path += p;
			data.ip += ipaddress;
			data.port = port;
			myData.push_back(data);
		} else {
			starting_point++;
			ss.str("");
			data.starting_point = starting_point;
			ss << starting_point;
			data.str_start = ss.str();
			ss.str("");
			starting_point += constant_increase;
			//in case where after adding a constant will make it bigger
			if (starting_point >= total_int_length) {
				data.ending_point = total_int_length;
				ss << total_int_length;
				data.str_end = ss.str();
				ss.str("");
				//in case where there's still room for addition
			} else {
				data.ending_point = starting_point;
				ss << starting_point;
				data.str_end = ss.str();
				ss.str("");
			}
			data.host += h;
			data.path += p;
			data.ip += ipaddress;
			data.port = port;
			myData.push_back(data);
		}
	}
}

void Client::constructThreads(int number_of_threads, string total_length,
		string host, string path) {

	//total length to download
	uint64_t total_int_length = atoi(total_length.c_str());
	//constant by which increase each time 0-100,101-201;
	uint64_t constant_increase = total_int_length / number_of_threads;
	//a constant always 0
	uint64_t starting_point = 0;

	fillMyData(starting_point, total_int_length, constant_increase, host, path);

}

string Client::getRequest() {
	return request;
}
