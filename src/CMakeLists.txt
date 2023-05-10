add_library (ido libido.c)

target_include_directories (ido PRIVATE "${JSONC_INCLUDE_DIRS}")
target_include_directories (ido PUBLIC "${CURL_INCLUDE_DIRS}")
target_link_libraries (ido PUBLIC "${CURL_LIBRARIES}" "${JSONC_LIBRARIES}")
