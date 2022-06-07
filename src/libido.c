#include <curl/curl.h>

#include <json-c/json_object.h>

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
  json_object_object_add(json, "order_by", 
    json_object_new_string(libido_search_order_string[req.order]));
  json_object_object_add(json, "ordering",
    json_object_new_string(req.ascending ? "asc" : "desc"));
  json_object_object_add(json, "tags_mode",
    json_object_new_string(req.tag_and ? "AND" : "OR"));
  json_object_object_add(json, "page",
    json_object_new_uint64(req.page_no));
  json_object_object_add(json, "search_text",
    json_object_new_string(req.query));
  
  for (; *req.blacklist != NULL; ++req.blacklist)
    json_object_array_add(blacklist_json, json_object_new_string(*req.blacklist));
  
  for (; *req.brands != NULL; ++req.brands)
    json_object_array_add(brands_json, json_object_new_string(*req.brands));
  
  for (; *req.tags != NULL; ++req.tags)
    json_object_array_add(tags_json, json_object_new_string(*req.tags));
}