#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>

#include "args.h"
#include "util.h"

int main(int argc, char **argv) {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  struct libido_context *ido;
  enum libido_error idoerr;
  int reti;
  
  ido = libido_new ();
  if (!ido)
  {
    fputs ("fatal: libido: libido_new() failed", stderr);
    goto drop;
  }
    
  struct libido_search_request req;
  struct libido_search_response res;
  size_t num_disp_res, res_idx;
  
  reti = search_req_from_args (&req, &num_disp_res, argc, argv);
  if (reti != 0)
  {
    if (reti < 0)
    {
      putchar ('\n');
      print_usage (argv[0]);
    }
    goto drop;
  }
  
  req.page_no = 0;
  res_idx = 0;

fetch_results:
  idoerr = libido_search (ido, req, &res);
  if (idoerr != LIBIDO_ERROR_NOTHING)
  {
    fprintf (stderr, "fatal: libido: libido_search() failed: %d", idoerr);
    goto drop;
  }

  struct libido_search_hit *hit = res.hits;
  for (; hit != NULL; hit = hit->next)
  {
    if (res_idx >= num_disp_res)
      goto drop;

    puts ("---");
    printf ("%s (%ld)\n", hit->name, hit->id);
    printf ("\n%s\n\n", hit->description);

    // NOTE: apparently, this property is inconsistent.
    if (hit->duration)
      printf ("  duration %ld:%ld:%ld\n", 
        hit->duration / 3600000,
        hit->duration / 60000 % 60,
        hit->duration / 1000 % 60);
    
    printf ("  by %s (%ld)\n", hit->brand, hit->brand_id);
    printf ("  viewed by %ld otakus\n", hit->views);
    printf ("  %ld likes %ld dislikes\n", hit->likes, hit->dislikes);
    printf ("  uncensored? %s\n", hit->is_censored ? "alas!" : "oh yeah!");
    
    printf("  has\n    ");
    for (const char **tag = hit->tags; *tag != NULL; ++tag)
      printf ("%s  ", *tag);
    putchar ('\n');
     
    puts ("  aka");
    for (const char **title = hit->titles; *title != NULL; ++title)
      printf ("    %s\n", *title);
    
    printf ("  cover %s\n", hit->cover_url);
    printf ("  poster %s\n", hit->poster_url);
    printf ("  ranking %ld\n", hit->monthly_rank);
    
    printf ("  released at %s", asctime (localtime (&(const time_t) {hit->released_at})));
    printf ("  uploaded at %s", asctime (localtime (&(const time_t) {hit->uploaded_at})));
    
    printf ("  downloaded %ld times\n", hit->downloads);
    printf ("  url https://hanime.tv/videos/hentai/%s\n", hit->slug);
    puts ("---");
  }
  
  if (req.page_no < res.num_pages)
  {
    ++req.page_no;
    goto fetch_results;
  }
drop:
  if (ido)
    libido_drop (ido);
  
  curl_global_cleanup();
}
