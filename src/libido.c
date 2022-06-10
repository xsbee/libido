#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <libido.h>

static const char *libido_search_order_string[] =
{
  "created_at_unix",
  "views",
  "likes",
  "released_at_unix",
  "title_sortable"
};

struct libido_memory {
  void  *base;
  size_t capacity;
  size_t length;
};

struct libido_context {
  CURL *c;
  
  struct {
    struct {
      struct curl_slist *headers;
      struct libido_memory res;
    } search;
    
    json_tokener *tok;
  } cache;
};

static
void search_request_to_json
(
  json_object *json,
  struct libido_search_request req
) 
{
  json_object *blacklist_json = json_object_new_array();
  json_object *brands_json = json_object_new_array();
  json_object *tags_json = json_object_new_array();
  
  json_object_object_add(json, "blacklist", blacklist_json);
  json_object_object_add(json, "brands", brands_json);
  json_object_object_add(json, "tags", tags_json);
  json_object_object_add(json, "order_by", json_object_new_string(libido_search_order_string[req.order]));
  json_object_object_add(json, "ordering", json_object_new_string(req.ascending ? "asc" : "desc"));
  json_object_object_add(json, "tags_mode", json_object_new_string(req.tag_and ? "AND" : "OR"));
  json_object_object_add(json, "page", json_object_new_uint64(req.page_no));
  json_object_object_add(json, "search_text", json_object_new_string(req.query ? req.query : ""));
  
  if (req.blacklist)
    for (; *req.blacklist != NULL; ++req.blacklist)
      json_object_array_add(blacklist_json, json_object_new_string(*req.blacklist));
  
  if (req.brands)
    for (; *req.brands != NULL; ++req.brands)
      json_object_array_add(brands_json, json_object_new_string(*req.brands));
  
  if (req.tags)
    for (; *req.tags != NULL; ++req.tags)
      json_object_array_add(tags_json, json_object_new_string(*req.tags));
}

static
void search_hits_from_json
(
  struct libido_search_hit **res,
  json_object *hits
)
{
  struct libido_search_hit *hit, *hit_prev;
  size_t num_hits;
  
  hit = NULL;
  hit_prev = NULL;
  num_hits = json_object_array_length (hits);
 
  for (size_t i = 0; i < num_hits; ++i)
  {
    json_object *titles, *tags;
    json_object *hit_json = json_object_array_get_idx (hits, i);
    size_t num_titles, num_tags;
   
    if (hit == NULL)
    {
      hit = malloc (sizeof (struct libido_search_hit));
      if (!hit)
        goto drop;
      
      if (i == 0)
        *res = hit;
      
      hit->next = NULL; // terminator
     
      if (hit_prev)
         hit_prev->next = hit; // linking
      
      hit->titles = NULL; // realloc()
      hit->tags = NULL; // .. compatiblity.
    }
    
    hit->id = json_object_get_uint64(json_object_object_get(hit_json, "id"));
    hit->name = json_object_get_string(json_object_object_get(hit_json, "name"));
    hit->slug = json_object_get_string(json_object_object_get(hit_json, "slug"));
    hit->description = json_object_get_string(json_object_object_get(hit_json, "description"));
    hit->views = json_object_get_uint64(json_object_object_get(hit_json, "views"));
    hit->poster_url = json_object_get_string(json_object_object_get(hit_json, "poster_url"));
    hit->cover_url = json_object_get_string(json_object_object_get(hit_json, "cover_url"));
    hit->brand = json_object_get_string(json_object_object_get(hit_json, "brand"));
    hit->brand_id = json_object_get_uint64(json_object_object_get(hit_json, "brand_id"));
    hit->duration = json_object_get_uint64(json_object_object_get(hit_json, "duration_in_ms"));
    hit->is_censored = json_object_get_boolean(json_object_object_get(hit_json, "is_censored"));
    hit->likes = json_object_get_uint64(json_object_object_get(hit_json, "likes"));
    hit->dislikes = json_object_get_uint64(json_object_object_get(hit_json, "dislikes"));
    hit->downloads = json_object_get_uint64(json_object_object_get(hit_json, "downloads"));
    hit->monthly_rank = json_object_get_uint64(json_object_object_get(hit_json, "monthly_rank"));
    hit->uploaded_at = json_object_get_uint64(json_object_object_get(hit_json, "created_at"));
    hit->released_at = json_object_get_uint64(json_object_object_get(hit_json, "released_at"));
    
    titles = json_object_object_get(hit_json, "titles");
    num_titles = json_object_array_length(titles);
    
    tags = json_object_object_get(hit_json, "tags");
    num_tags = json_object_array_length(tags);
    
    // allocate a slot more for the terminator (execv style).
    hit->titles = realloc(hit->titles, sizeof (const char*) * (num_titles + 1));
    hit->tags = realloc(hit->tags, sizeof (const char*) * (num_tags + 1));
    
    // TODO: warn the user of a memory shortage, as a result of which
    // results are truncated, this is however a rare condition.
    if (!hit->titles || !hit->tags)
      goto drop;
    
    for (size_t j = 0; j < num_titles; ++j)
      hit->titles[j] = json_object_get_string(json_object_array_get_idx (titles, j));
    hit->titles[num_titles] = NULL;
    
    for (size_t j = 0; j < num_tags; ++j)
      hit->tags[j] = json_object_get_string(json_object_array_get_idx (tags, j));
    hit->tags[num_tags] = NULL;
    
    hit_prev = hit;
    hit = hit->next;
    continue;
drop:
    if (hit)
      free (hit);
    break;
  }
}

static
bool search_response_from_json
(
  struct libido_search_response *res,
  json_object *json
)
{
  // `hits' object is, apparently, a string of JSON.
  json_object *hits_json = json_tokener_parse (
    json_object_get_string (json_object_object_get (json, "hits")));
  
  if (!hits_json)
    return false;
  
  res->private = hits_json;
  res->page_no = json_object_get_uint64(json_object_object_get(json, "page"));
  res->num_pages = json_object_get_uint64(json_object_object_get(json, "nbPages"));
  res->num_hits = json_object_get_uint64(json_object_object_get(json, "nbHits"));
  res->num_hits_per_page = json_object_get_uint64(json_object_object_get(json, "hitsPerPage"));
   
  if (!res->num_hits)
    res->hits = NULL;
  
  search_hits_from_json (&res->hits, hits_json);
    
  return true;
}

static
size_t write_to_buffer
(
  void  *data,
  size_t size, 
  size_t nmemb, 
  void  *userp
)
{
  struct libido_memory *mem = userp;
  void *new_base;
  size_t written, offset;
  
  written = size * nmemb;
  offset = mem->length;
  mem->length += written;
  
  if (mem->capacity < mem->length)
  {
    new_base = realloc (mem->base, mem->length);
    
    if (!new_base)
      return 0; // OOM!
    
    mem->base = new_base;
    mem->capacity = mem->length;
  }
  
  memcpy(mem->base + offset, data, written);
  return written;
}

struct libido_context* libido_new() {
  struct libido_context *ctx;
  
  ctx = malloc (sizeof (struct libido_context));
  if (!ctx)
    return NULL;
  
  ctx->c = curl_easy_init();
  if (!ctx->c)
    goto drop;
  
  // initialise cache and stuff.
  ctx->cache.search.headers = curl_slist_append (NULL, "Content-Type: application/json");
  ctx->cache.search.res = (struct libido_memory) {NULL, 0, 0};
  ctx->cache.tok = json_tokener_new ();

  return ctx;
drop:
  if (ctx)
    free (ctx);
  return NULL;
}

enum libido_error libido_search (
  struct libido_context *ctx,
  struct libido_search_request req,
  struct libido_search_response *res
)
{
  enum libido_error err;
  int ret;
  CURLcode cres;
  
  json_object *req_json, *res_json = NULL;
  
  req_json = json_object_new_object ();
  search_request_to_json (req_json, req);
  
  ctx->cache.search.res.length = 0; // reset to reuse.
  
  curl_easy_setopt (ctx->c, CURLOPT_URL, "https://search.htv-services.com/");
  curl_easy_setopt (ctx->c, CURLOPT_HTTPHEADER, ctx->cache.search.headers);
  curl_easy_setopt (ctx->c, CURLOPT_POSTFIELDS,
    json_object_to_json_string_ext (req_json, JSON_C_TO_STRING_PLAIN));
  curl_easy_setopt (ctx->c, CURLOPT_WRITEFUNCTION, write_to_buffer);
  curl_easy_setopt (ctx->c, CURLOPT_FAILONERROR, 1); // HTTP 4xx
  curl_easy_setopt (ctx->c, CURLOPT_WRITEDATA, (void*) &ctx->cache.search.res);
   
  cres = curl_easy_perform (ctx->c);
  
  json_object_put (req_json);
  
  if (cres != CURLE_OK)
  {
    err = LIBIDO_ERROR_NETWORK;
    goto drop;
  }
    
  res_json = json_tokener_parse_ex (
    ctx->cache.tok,
    ctx->cache.search.res.base,
    ctx->cache.search.res.length);
  
  if (!res_json)
  {
    json_tokener_reset (ctx->cache.tok);
    
    err = LIBIDO_ERROR_JSON;
    goto drop;
  }
  
  ret = search_response_from_json (res, res_json);
  if (!ret)
  {
    err = LIBIDO_ERROR_JSON;
    goto drop;
  }
  
  return LIBIDO_ERROR_NOTHING;
drop:
  if (res_json)
    json_object_put (res_json);
  
  return err;
}

void libido_search_response_drop
(
  struct libido_search_response *res
)
{
  struct libido_search_hit *hit = res->hits;
  struct libido_search_hit *hit_old;
  
  while (hit != NULL)
  {
    free (hit->titles);
    free (hit->tags);
    
    hit_old = hit;
    hit = hit->next;
    
    free (hit_old);
  }
  
  json_object_put (res->private);
}

void libido_drop (struct libido_context* ctx)
{
  curl_easy_cleanup (ctx->c);
  curl_slist_free_all (ctx->cache.search.headers);
  json_tokener_free (ctx->cache.tok);
  
  free (ctx);
}
