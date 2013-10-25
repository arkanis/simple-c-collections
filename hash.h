#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>


/**

"Hash" is a hash table with int64_t or int32_t keys (depending on the platform).
"Dict" is a hash table with string keys (const char *).

TODO:

- rename all internal hash variables to hashmap
- update documentation to use the name "hashmap"
- resize in public API? if so would need to take care of the load factor because the user should not know about it.

*/

typedef struct {
	size_t length, capacity;
	uint32_t value_size, key_type;
	void* slots;
} unified_hash_t, *unified_hash_p, *hash_p, *dict_p;
typedef void *hash_elem_t, *dict_elem_t;

#if defined(__x86_64__) || defined(__ppc64__) || defined(_WIN64)
	typedef int64_t hash_key_t;
#else
	typedef int32_t hash_key_t;
#endif


#define hash_of(type)              hash_new(5, sizeof(type))
#define hash_with(capacity, type)  hash_new(capacity, sizeof(type))
hash_p  hash_new(size_t capacity, size_t value_size);
void    hash_destroy(hash_p hash);
void    hash_resize(hash_p hash, size_t capacity);

#define hash_put(hash, key, type, value)  ( *((type*)hash_put_ptr(hash, key)) = (value) )
#define hash_get(hash, key, type)         ( *((type*)hash_get_ptr(hash, key)) )
void*   hash_get_ptr(hash_p hash, hash_key_t key);
void*   hash_put_ptr(hash_p hash, hash_key_t key);
void    hash_remove(hash_p hash, hash_key_t key);
bool    hash_contains(hash_p hash, hash_key_t key);

hash_elem_t hash_start(hash_p hash);
hash_elem_t hash_next(hash_p hash, hash_elem_t element);
hash_key_t  hash_key(hash_elem_t element);
#define     hash_value(element, type)     ( *((type*)hash_value_ptr(element)) )
void*       hash_value_ptr(hash_elem_t element);
void        hash_remove_elem(hash_p hash, hash_elem_t element);


#define dict_of(type)              dict_new(5, sizeof(type))
#define dict_with(capacity, type)  dict_new(capacity, sizeof(type))
dict_p  dict_new(size_t capacity, size_t value_size);
void    dict_destroy(dict_p dict);
void    dict_resize(dict_p dict, size_t capacity);

#define dict_put(dict, key, type, value)  ( *((type*)dict_put_ptr(dict, key)) = (value) )
#define dict_get(dict, key, type)         ( *((type*)dict_get_ptr(dict, key)) )
void*   dict_get_ptr(dict_p dict, const char* key);
void*   dict_put_ptr(dict_p dict, const char* key);
void    dict_remove(dict_p dict, const char* key);
bool    dict_contains(dict_p dict, const char* key);

dict_elem_t dict_start(dict_p dict);
dict_elem_t dict_next(dict_p dict, dict_elem_t element);
const char* dict_key(dict_elem_t element);
#define     dict_value(element, type)     ( *((type*)dict_value_ptr(element)) )
void*       dict_value_ptr(dict_elem_t element);
void        dict_remove_elem(dict_p dict, dict_elem_t element);