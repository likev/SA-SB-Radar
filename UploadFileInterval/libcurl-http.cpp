#include <windows.h>

#include <stdio.h>
#include <iostream>
#include <string>


#define CURL_STATICLIB
#include "curl/curl.h"

void post_simple()
{
  /* get a curl handle */
  CURL *curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_PROXY, "172.18.172.251:80");
	  curl_easy_setopt(curl, CURLOPT_URL, "http://radaronline.sinaapp.com/post.php?fa=po");
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

}


size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    FILE *fptr = (FILE*)userp;
    fwrite(buffer, size, nmemb, fptr);

	//printf("nmemb:%d, size:%d",nmemb, size);
	return nmemb*size;
}

size_t write_console(void *buffer, size_t size, size_t count, void *userp) {
    
	std::string str((const char*)buffer, size*count);

	std::cout<<str;
	return count*size;
}

void post_file()
{
	const char* POSTURL =  "http://radaronline.sinaapp.com/post.php?fa=po";
	




	CURL *curl = curl_easy_init();

	//curl_easy_setopt(curl, CURLOPT_PROXY, "172.18.172.251:80");
	//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	//const char* FILENAME =  "a.txt";

	//FILE *fptr;

	//if ((fptr = fopen(FILENAME, "w")) == NULL) {//for receive response data
	//	fprintf(stderr, "fopen file error: %s\n", FILENAME);
	//	exit(1);
	//}
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);

	curl_httppost *formpost = 0, *lastptr  = 0;

	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, "radar-config.xml", CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_URL, POSTURL);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

	CURLcode res = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);

}

int main(void)
{	  /* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);
	
	post_file();
	
	curl_global_cleanup();
	return 0;
}



int APIENTRY WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance, 
            LPSTR    lpCmdLine, 
            int       cmdShow)
{
	curl_global_init(CURL_GLOBAL_ALL);
	
	post_file();
	
	curl_global_cleanup();
	return 0;
}