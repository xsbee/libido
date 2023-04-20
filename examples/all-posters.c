#include <stdio.h>
#include <string.h>

#include <curl/curl.h>

#include <libido.h>

int main() {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  struct libido_context *ctx;
  struct libido_search_request req = {0};
  struct libido_search_response res;
  enum libido_error err;
  
  ctx = libido_new();
  if (!ctx)
    goto cleanup;
  
  req.order = LIBIDO_ORDER_BY_RELEASE_DATE;

  for (;;) {
    err = libido_search (ctx, req, &res);
    
    if (err != LIBIDO_ERROR_NOTHING)
      goto cleanup;

    // if no results were found don't proceed
    if (!res.num_pages)
      break;

    struct libido_search_hit *hit = res.hits;
    
    for(; hit != NULL; hit = hit->next)
      puts (hit->poster_url);

    // if there are any pages left, proceed with loop
    if (!(++req.page_no < res.num_pages))
      break;
  }

cleanup:
  curl_global_cleanup();
}
