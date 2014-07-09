#include "postfile.h"
#include "config.h"

#define CURL_STATICLIB
#include "curl/curl.h"

#include <windows.h>

#include <stdio.h>
#include <iostream>
#include <string>


size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	FILE *fptr = (FILE*)userp;
	fwrite(buffer, size, nmemb, fptr);

	//printf("nmemb:%d, size:%d",nmemb, size);
	return nmemb*size;
}

size_t write_console(void *buffer, size_t size, size_t count, void *userp) {

	std::string str((const char*)buffer, size*count);

	applog << str;
	return count*size;
}

bool post_file(UploadInfo& upinfo)
{
	std::string POSTURL = getUploadUrl(); //"http://radaronline.sinaapp.com/post.php?fa=po";

	CURL *curl = curl_easy_init();

	std::string proxy = getProxy();

	if (proxy != "0") curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_console);

	//const char* FILENAME =  "a.txt";

	//FILE *fptr;

	//if ((fptr = fopen(FILENAME, "w")) == NULL) {//for receive response data
	//	fprintf(stderr, "fopen file error: %s\n", FILENAME);
	//	exit(1);
	//}
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);

	curl_httppost *formpost = 0, *lastptr = 0;



	for (auto it = upinfo.post_form.begin(); it != upinfo.post_form.end(); it++)
	{
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, it->first.c_str(), CURLFORM_PTRCONTENTS, it->second.c_str(), CURLFORM_END);
	}

	for (auto it = upinfo.rfiles.begin(); it != upinfo.rfiles.end(); it++)
	{
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "rfiles[]", CURLFORM_FILE, it->c_str(), CURLFORM_END);
	}

	curl_easy_setopt(curl, CURLOPT_URL, POSTURL.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

	std::string errors(CURL_ERROR_SIZE, ' ');
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &*errors.begin());

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		applog << "post error occur: " << errors << '\n';
		//std::cout <<" curl_easy_strerror:"<< curl_easy_strerror(res) << '\n';
		return false;
	}

	curl_easy_cleanup(curl);

	return true;

}