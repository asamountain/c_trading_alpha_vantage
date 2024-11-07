#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_URL "https://www.alphavantage.co/query"
#define API_KEY "MSTU"

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

void make_request(const char *symbol, const char *api_key, const char *function, const char *interval) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[256];
    snprintf(url, sizeof(url), "%s?function=%s&symbol=%s&interval=%s&apikey=%s", API_URL, function, symbol, interval, api_key);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("%s\n", s.ptr);
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    free(s.ptr);
}

int main() {
    const char *symbol = "MSTU";  // Replace with the desired stock symbol
    const char *api_key = API_KEY;
    const char *function = "TIME_SERIES_INTRADAY";
    const char *interval = "15min"; // The following values are supported: 1min, 5min, 15min, 30min, 60min
    make_request(symbol, api_key, function, interval);
    return 0;
}

