#include <stdint.h>
#include <stdbool.h>

enum libido_search_order {
  LIBIDO_ORDER_BY_UPLOAD_DATE,
  LIBIDO_ORDER_BY_VIEWS,
  LIBIDO_ORDER_BY_LIKES,
  LIBIDO_ORDER_BY_RELEASE_DATE,
  LIBIDO_ORDER_BY_TITLE,
  LIBIDO_ORDER_BY_DEFAULT = 0,
};

/**
 * Search query crietion request structure.
 */
struct libido_search_request {
  /* Textual search query for titles. */
  const char *query;
  /* Page number of result set. */
  uint64_t page_no;
  /* Property to order searches by. */
  enum libido_search_order order; 
  /* Blacklist of tags. */
  const char *const *blacklist;
  /* Production brands to look for. */
  const char *const *brands;
  /* Whitelist of tags. */
  const char *const *tags;
  /* If to order ascendingly. */
  bool ascending;
  /* If to match all of or one of the tags. */
  bool tag_and;
};