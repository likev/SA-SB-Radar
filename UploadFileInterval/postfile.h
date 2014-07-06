#include "config.h"

#define CURL_STATICLIB
#include "curl/curl.h"

bool post_file(UploadInfo& upinfo);