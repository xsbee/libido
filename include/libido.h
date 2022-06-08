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

struct libido_search_hit {
  /* ID to refer this entry by. */
  uint64_t id;
  /* Main title of the entry. */
  const char *name;
  /* Alternative titles (if exist). */
  const char *const *titles;
  /* Slug to refer this entry by. */
  const char *slug;
  /* Description of the content. */
  const char *description;
  /* List of associated tags. */
  const char *const *tags;
  /* Number of (non-unique) viewers. */
  uint64_t views;
  /* TODO: not sure about its purpose. */
  // uint64_t interests;
  /* URL to poster (image). */
  const char *poster_url;
  /* URL to cover (image). */
  const char *cover_url;
  /* Name of the producing brand. */
  const char *brand;
  /* ID of the producing brand. */
  uint64_t brand_id;
  /* Duration of the content in (ms). */
  uint64_t duration;
  /* 
     If the content is censored.
     See: https://en.wikipedia.org/wiki/Pornography_in_Japan#Censorship_laws
  */
  bool is_censored;
  /* TODO: apparently this property is null. */
  // uint64_t rating;
  /* Number of likes (by registered users) gained. */
  uint64_t likes;
  /* Number of dislikes (by registered users) gained. */
  uint64_t dislikes;
  /* Number of (non-unique) downloads. */
  uint64_t downloads;
  /* Rank in monthly selection. */
  uint64_t monthly_rank;
  /* UNIX timestamp of upload. */
  uint64_t uploaded_at;
  /* UNIX timestamp of release. */
  uint64_t released_at;
  
  struct libido_search_hit *next;
};

struct libido_search_response {
  /* Current page number. */
  uint64_t page_no;
  /* Total number of pages. */
  uint64_t num_pages;
  /* Number of results per page. */
  uint64_t num_hits_per_page;
  /* Number of hits for this page. */
  uint64_t num_hits;
  /* Linked list of results. */
  struct libido_search_hit *hits;  
};