#include <curl/curl.h>
#include "HttpGet.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	std::vector<uint8_t> *pData = static_cast<std::vector<uint8_t>*>(userp);
	if (!pData)
		return 0;

	size_t realSize = size * nmemb;
	uint8_t *buffer = static_cast<uint8_t*>(contents);

	pData->insert(
		pData->end(),
		buffer,
		buffer + realSize);

	return realSize;
}

bool HttpGet(const std::string &strURL, std::vector<uint8_t> &data)
{
	CURL *curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_URL, strURL.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, static_cast<void*>(&data));
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "AnyAgent/1.0");

	CURLcode res = curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);

	if (res != CURLE_OK)
		return false;

	return true;
}
