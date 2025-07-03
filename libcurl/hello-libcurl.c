#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <curl/curl.h>

struct buffer {
    unsigned char* data;
    size_t size;
    size_t capacity;
};

void buffer_free(struct buffer* buffer)
{
    if (buffer->data) {
        free(buffer->data);
    }
    buffer->data = NULL;
    buffer->size = buffer->capacity = 0;
}

CURL* init_curl_or_exit(void);
void print_curl_info(void);

size_t res_body_write_callback(void* chunk, size_t size, size_t nmemb, void* userdata)
{
    struct buffer* buffer = userdata;
    const size_t chunk_size = size * nmemb;

    if (buffer->size + chunk_size > buffer->capacity) {
        static const size_t grow_factor = 8;

        const size_t new_capacity = (buffer->capacity + chunk_size) * grow_factor;
        buffer->data = realloc(buffer->data, new_capacity);
        if (buffer->data == NULL) {
            fprintf(stderr, "error: out of memory\n");
            return 0; // or should we just exit(1) instead?!
        }
        buffer->capacity = new_capacity;
    }

    memcpy(buffer->data + buffer->size, chunk, chunk_size);
    buffer->size += chunk_size;

    return chunk_size;
}

size_t res_header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    (void) userdata;

    // https://curl.se/libcurl/c/CURLOPT_HEADERFUNCTION.html
    // buffer points to the delivered data, and the size of that data is nitems; size is always 1.
    // Do not assume that the header line is null-terminated!
    const size_t buffer_size = size * nitems;

    fwrite(buffer, sizeof(buffer[0]), buffer_size, stderr);

    return buffer_size;
}

int main()
{
    CURL* curl = init_curl_or_exit();
    print_curl_info();

    const char* url = "https://jsonplaceholder.typicode.com/users/1";
    struct buffer res_body = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // default: 0L (disabled)
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res_body_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, res_header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);

    printf("GET %s\n", url);
    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        fprintf(stderr, "error: GET \"%s\" failed: %s\n", url, curl_easy_strerror(rc));
        goto err_buffer_free;
    }

    fprintf(stderr, "info: request is done. got response\n");

    long res_code = 0;
    rc = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
    assert(rc == CURLE_OK);

    fprintf(stderr, "info: status_code=%ld bytes=%zu\n", res_code, res_body.size);
    printf("%.*s", (int) res_body.size, res_body.data);

    buffer_free(&res_body);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;

err_buffer_free:
    buffer_free(&res_body);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    exit(1);
}

CURL* init_curl_or_exit(void)
{
    // From curl_easy_init(3) - Linux man page:
    //
    // > If you did not already call curl_global_init(3), curl_easy_init(3) does it automatically.
    // > This may be lethal in multi-threaded cases, since curl_global_init(3) is not thread-safe,
    // > and it may result in resource problems because there is no corresponding cleanup.
    // > You are strongly advised to not allow this automatic behaviour, by calling curl_global_init(3) yourself properly
    //
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        fprintf(stderr, "error: curl: global init failed.\n");
        exit(1);
    }

    CURL* curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "error: curl: easy init failed.\n");
        curl_global_cleanup();
        exit(1);
    }
  
    return curl;
}

void print_curl_info(void)
{
    const curl_version_info_data* curl_version = curl_version_info(CURLVERSION_NOW);
    
    fprintf(stderr, "info: CURL Version: %s\n", curl_version->version);
    fprintf(stderr, "info: CURL Host Version: %s\n", curl_version->host);
    fprintf(stderr, "info: CURL OpenSSL Version: %s\n", curl_version->ssl_version);
    fprintf(stderr, "info: CURL LibZ Version: %s\n", curl_version->libz_version);
    
    if (curl_version->age >= CURLVERSION_SECOND) {
        fprintf(stderr, "info: CURL Ares Version: %s\n", curl_version->ares);
    }
    
    if (curl_version->age >= CURLVERSION_THIRD) {
        fprintf(stderr, "info: CURL LibIdn Version: %s\n", curl_version->libidn);
    }
//    if (curl_version->age >= CURLVERSION_FOURTH) fprintf(stderr, "info: CURL LibIconv Version: %s\n", curl_version->libiconv);
}

