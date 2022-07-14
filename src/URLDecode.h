#pragma once

#include <curl/curl.h>
#include <string>

CURL* curl;

std::string URLDecode(std::string arg) {
	return curl_easy_unescape(curl, arg.c_str(), 0, NULL);
}

void URLDecodeInit() {
	curl = curl_easy_init();
}