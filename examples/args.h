#include <stdio.h>
#include <unistd.h>

#include <libido.h>
#include "util.h"

static const struct
{
  enum libido_search_order order;
  const char *repr;
} order_strs[5] =
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
