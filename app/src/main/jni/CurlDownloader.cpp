//
// Created by Andrei Rybin on 8/31/16.
//

#include "CurlDownloader.h"

CurlDownloader::CurlDownloader(std::string & url, std::string & localPath) {
    url_ = url;
    localPath_ = localPath;
}

//just return the path where the file was written
std::string CurlDownloader::download() {

    std::string fileName;
    std::string::size_type lastPosition = url_.find_last_of("/");
    if(lastPosition != std::string::npos) {
        fileName = url_.substr(lastPosition + 1, url_.size() -1);
    }

    absolutePath_ = localPath_ + fileName;

    FILE *pagefile;
    //std::ofstream file(absoluteFile.c_str());

    CURL * curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());

        /* Switch on full protocol/debug output while testing */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* disable progress meter, set to 0L to enable and disable debug output */
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

#ifdef SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        pagefile = fopen(absolutePath_.c_str(), "wb");
        if(pagefile) {
            //write the page body to this file handle
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
            //actually get the file
            curl_easy_perform(curl);
            fclose(pagefile);
        } else {
            absolutePath_ = "error writing to file";
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();




    return absolutePath_;
}
