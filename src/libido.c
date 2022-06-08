#include <stdlib.h>

#include <curl/curl.h>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <libido.h>

static const char* libido_search_order_string[] = 
{
  "created_at_unix",
  "views",
  "likes",
  "released_at_unix",
  "title_sortable"
};

static const char* libido_user_agent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.61 Safari/537.36";

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
  json_object_object_add(json, "search_text", json_object_new_string(req.query));
  
  for (; *req.blacklist != NULL; ++req.blacklist)
    json_object_array_add(blacklist_json, json_object_new_string(*req.blacklist));
  
  for (; *req.brands != NULL; ++req.brands)
    json_object_array_add(brands_json, json_object_new_string(*req.brands));
  
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
  struct libido_search_hit *hit;
  struct libido_search_hit *hit_prev;
  size_t num_hits;

  num_hits = json_object_array_length (hits);
  if (!num_hits)
    return;

  // num_hits must be >1, if we reach here.
  hit = *res = malloc (sizeof (struct libido_search_hit));

  // nothing previous this exists, it is the foremost.
  hit_prev = hit;

  for (size_t i = 0; i < num_hits; ++i)
  {
    json_object *titles, *tags;
    json_object *hit_json = json_object_array_get_idx (hits, i);
    size_t num_titles, num_tags;

    // allocate if did not already.
    if (hit == NULL)
      hit = malloc (sizeof (struct libido_search_hit));   
    hit->next = NULL;

    hit->id = json_object_get_uint64(json_object_object_get(hit_json, "id"));    
    hit->name = json_object_get_string(json_object_object_get(hit_json, "name"));
    hit->slug = json_object_get_string(json_object_object_get(hit_json, "slug"));
    hit->description = json_object_get_string(json_object_object_get(hit_json, "description"));
    hit->views = json_object_get_uint64(json_object_object_get(hit_json, "views"));
    hit->poster_url = json_object_get_string(json_object_object_get(hit_json, "poster_url"));
    hit->cover_url = json_object_get_string(json_object_object_get(hit_json, "cover_url"));
    hit->brand = json_object_get_string(json_object_object_get(hit_json, "brand"));
    hit->brand_id = json_object_get_uint64(json_object_object_get(hit_json, "brand_id"));
    hit->duration = json_object_get_boolean(json_object_object_get(hit_json, "duration_in_ms"));
    hit->is_censored = json_object_get_uint64(json_object_object_get(hit_json, "is_censored"));
    hit->likes = json_object_get_uint64(json_object_object_get(hit_json, "likes"));
    hit->dislikes = json_object_get_uint64(json_object_object_get(hit_json, "dislikes"));
    hit->downloads = json_object_get_uint64(json_object_object_get(hit_json, "downloads"));
    hit->monthly_rank = json_object_get_uint64(json_object_object_get(hit_json, "monthly_rank"));
    hit->uploaded_at = json_object_get_uint64(json_object_object_get(hit_json, "uploaded_at"));
    hit->released_at = json_object_get_uint64(json_object_object_get(hit_json, "released_at"));
    
    titles = json_object_object_get(hit_json, "titles");
    tags = json_object_object_get(hit_json, "tags");
    
    num_titles = json_object_array_length(titles);
    num_tags = json_object_array_length(tags);
    
    // allocate a slot more for the terminator (execv style).
    hit->titles = malloc(sizeof (const char*) * (num_titles + 1));  
    hit->tags = malloc(sizeof (const char*) * (num_tags + 1));
      
    for (size_t j = 0; j < num_titles; ++j)
      hit->titles[j] = json_object_get_string(json_object_array_get_idx (titles, j));
    hit->titles[num_titles] = NULL;
    
    for (size_t j = 0; j < num_tags; ++j)
      hit->tags[j] = json_object_get_string(json_object_array_get_idx (tags, j));
    hit->tags[num_tags] = NULL;
    
    // advance to next element.
    hit_prev = hit;
    hit = hit->next;
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
  json_object *hits_json = json_tokener_parse(
    json_object_get_string(json_object_object_get(json, "hits")));
  
  if (!hits_json)
    return false;
  
  res->page_no = json_object_get_uint64(json_object_object_get(json, "page"));
  res->num_pages = json_object_get_uint64(json_object_object_get(json, "nbPages"));
  res->num_hits = json_object_get_uint64(json_object_object_get(json, "nbHits"));
  res->num_hits_per_page = json_object_get_uint64(json_object_object_get(json, "hitsPerPage"));
   
  if (!res->num_hits)
    res->hits = NULL;
  
  search_hits_from_json(&res->hits, hits_json);
  
  return true;
}
