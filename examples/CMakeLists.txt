project (libido-examples)

add_executable (libido-query libido-query.c util.c)
add_executable (all-posters EXCLUDE_FROM_ALL all-posters.c)

target_link_libraries (libido-query ido)
