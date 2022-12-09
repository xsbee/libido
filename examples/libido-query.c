#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>

#include "args.h"
#include "util.h"

static struct
{
  enum libido_search_order order;
  const char *repr;
}
const order_strs[5] =
{
  {LIBIDO_ORDER_BY_UPLOAD_DATE, "upload"},
  {LIBIDO_ORDER_BY_VIEWS, "views"},
  {LIBIDO_ORDER_BY_LIKES, "likes"},
  {LIBIDO_ORDER_BY_RELEASE_DATE, "release"},
  {LIBIDO_ORDER_BY_TITLE, "title"}
};

static
void print_usage (const char *name)
{
  fprintf (stderr, "%s [-aA] [-b brands ...] [-B blacklist ...] [-t tags ...] [-o order] [query]\n\n", name);
  fputs (
    "-a    enable ascending order, desecending otherwise\n"
    "-A    enable tag exclusive mode, otherwise union\n"
    "-b    union of brandlist, otherwise empty\n"
    "-B    exclusive blacklist of tags, otherwise empty\n"
    "-t    list of tags, otherwise empty\n"
    "-o    property to order results by (can be\n"
    "      upload, views, likes, release or title),\n"
    "      otherwise libido default\n"
    "-n    number of results to show\n",
    stderr);
}

static
int search_req_from_args
(
  struct libido_search_request *req,
  size_t *numres,
  int argc,
  char *const *argv
)
{
  int opt;
  int err = 0;
  
  req->ascending = false;
  req->tag_and = true;
  req->blacklist = NULL;
  req->brands = NULL;
  req->tags = NULL;
  req->query = NULL;
  req->order = LIBIDO_ORDER_BY_DEFAULT;

  *numres = -1; // indefinite
  
  while ((opt = getopt (argc, argv, ":haAB:b:t:o:n:")) != -1)
    switch (opt)
    {
      case 'h':
        print_usage (argv [0]);
        
        ++err; // dirty hack
        break;
      case 'a': req->ascending = true;
                break;
      case 'A': req->tag_and = false;
                break;
      case 'B': split_string (&req->blacklist, optarg, ",");
                break;
      case 'b': split_string (&req->brands, optarg, ",");
                break;
      case 't': split_string (&req->tags, optarg, ",");
                break;
      case 'o': // long-live linear search!
        for (size_t i = 0; i < 5; ++i)
          if (strcmp (optarg, order_strs[i].repr) == 0)
          {
            req->order = order_strs[i].order;
            ++err;
            break;
          }
        
        --err;
        if (err)
          fprintf (stderr, "invalid order %s\n", optarg);
      break;
      case 'n':
        err += !sscanf(optarg, "%zu", numres);
        break;
      case ':':
        fprintf (stderr, "option -%c requires an operand\n", opt);
        --err;
      break;
      case '?':
        fprintf (stderr, "unrecognized option -%c\n", opt);
        --err;
      break;
    }
  
  argv += optind;
  argc -= optind;
  
  if (argc > 0)
    req->query = argv[0];
  
  return err;
}

int main (int argc, char **argv)
{
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

  req.page_no = 0;
  n_res = 0;

  do {
    idoerr = libido_search(ido, req, &res);
    if (idoerr != LIBIDO_ERROR_NOTHING) {
      fprintf(stderr, "fatal: libido: libido_search() failed: %d", idoerr);
      goto drop;
    }

    struct libido_search_hit *hit = res.hits;
    for (; hit != NULL; hit = hit->next) {
      if (n_res >= n_res_max)
        goto drop;

      puts("---");
      printf("%s (%ld)\n", hit->name, hit->id);
      printf("\n%s\n\n", hit->description);

      // NOTE: apparently, this property is inconsistent.
      if (hit->duration)
        printf("  duration %ld:%ld:%ld\n", hit->duration / 3600000,
               hit->duration / 60000 % 60, hit->duration / 1000 % 60);

      printf("  by %s (%ld)\n", hit->brand, hit->brand_id);
      printf("  viewed by %ld otakus\n", hit->views);
      printf("  %ld likes %ld dislikes\n", hit->likes, hit->dislikes);
      printf("  uncensored? %s\n", hit->is_censored ? "alas!" : "oh yeah!");

      printf("  has\n    ");
      for (const char **tag = hit->tags; *tag != NULL; ++tag)
        printf("%s  ", *tag);
      putchar('\n');

      puts("  aka");
      for (const char **title = hit->titles; *title != NULL; ++title)
        printf("    %s\n", *title);

      printf("  cover %s\n", hit->cover_url);
      printf("  poster %s\n", hit->poster_url);
      printf("  ranking %ld\n", hit->monthly_rank);

      printf("  released at %s",
             asctime(localtime(&(const time_t){hit->released_at})));
      printf("  uploaded at %s",
             asctime(localtime(&(const time_t){hit->uploaded_at})));

      printf("  downloaded %ld times\n", hit->downloads);
      printf("  url https://hanime.tv/videos/hentai/%s\n", hit->slug);
      puts("---");

      ++n_res;
    }

    ++req.page_no;
  } while (req.page_no < res.num_pages);

drop:
  if (ido)
    libido_drop (ido);
  
  curl_global_cleanup();
}
