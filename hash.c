#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"

/**
 * The hash table consists of slots. Each slot can be empty, deleted or occupied.
 * Occupied slots are "elements", the things users insert into the hash table and
 * work with.
 * 
 * Each slot has the following layout:
 * 
 *   | unified_hash_hash_t              |  The 32 or 64 bit hash for this slots value (0 == empty, UINT??_MAX == deleted)
 *   | hash_num_key_t or const char *   |  The original key for this slot
 *   | hash->value_size number of bytes |  Value bytes of the slot (size differs per hashmap)
 * 
 * Since the layout and field types depend on the hash there is no C struct representing
 * the slots. Instead we use macros to calculate the sizes and pointers to the fields.
 * 
 * We make sure that the key field is always as large as a pointer. On 32 bit systems
 * int32_t is used as integer key, on 64 bit systems int64_t. Thanks to that we don't need
 * to track the key size since it doesn't differ between integer and string keys. This
 * allows that we can get the key out of a slot without having to look at the hashmap
 * itself.
 */

// Define platform dependend types and functions
#if defined(__x86_64__) || defined(__ppc64__) || defined(_WIN64)
	#define UNIFIED_HASH_64BIT
	typedef uint64_t unified_hash_hash_t;
	
	static uint64_t int64_hash64(int64_t key);
	static uint64_t string_hash64(const char* key);
	
	#define int_hash(key)     int64_hash64(key)
	#define string_hash(key)  string_hash64(key)
#else
	typedef uint32_t unified_hash_hash_t;
	
	#define int_hash(key)     int32_hash32(key)
	#define string_hash(key)  string_hash32(key)
	
	static uint32_t int32_hash32(int32_t key);
	static uint32_t string_hash32(const char* key);
#endif

// Values of the hash_t `key_type` field
#define UNIFIED_HASH_NUMERIC_KEYS  0
#define UNIFIED_HASH_STRING_KEYS   1

// Special hash values
#define UNIFIED_HASH_SLOT_FREE     0
#define UNIFIED_HASH_SLOT_DELETED  UINT64_MAX

// Macros for slot access
#define slot_hash_size()     sizeof(unified_hash_hash_t)
#define slot_key_size()      sizeof(const char *)
#define slot_size(hash)      ( slot_hash_size() + slot_key_size() + hash->value_size )

#define slot_ptr(hash, index)       ( (void*)                ( (char*)hash->slots + slot_size(hash) * (index)   ) )
#define slot_hash_ptr(slot)         ( (unified_hash_hash_t*) ( slot                                             ) )
#define slot_key_ptr(slot, type)    ( (type*)                ( (char*)slot + slot_hash_size()                   ) )
#define slot_value_ptr(slot)        ( (void*)                ( (char*)slot + slot_hash_size() + slot_key_size() ) )

// Internal implementation functions that work for hash and dict (thus "unified hash")
static unified_hash_p unified_hash_new(size_t capacity, size_t value_size, uint8_t key_type);
static void           unified_hash_destroy(unified_hash_p hash);
static ssize_t        unified_hash_search(unified_hash_p hashmap, int64_t int_key, const char* string_key, uint64_t hash);

static void*          unified_hash_get_ptr(unified_hash_p hashmap, int64_t int_key, const char* string_key);
static void*          unified_hash_put_ptr(unified_hash_p hashmap, int64_t int_key, const char* string_key);
static void           unified_hash_remove(hash_p hashmap, int64_t int_key, const char* string_key);
static void           unified_hash_remove_elem(hash_p hashmap, void* element);
static bool           unified_hash_contains(hash_p hashmap, int64_t int_key, const char* string_key);

static void*          unified_hash_start(unified_hash_p hashmap);
static void*          unified_hash_next(unified_hash_p hashmap, void* element);
static void*          unified_hash_element_at_or_after_slot(unified_hash_p hash, void* slot);

static void           unified_hash_resize(unified_hash_p hash, size_t new_capacity);

// Prime functions
       size_t         snap_to_prime(size_t x);
static bool           is_prime(size_t x);


//
// Mapping from the hash or dict specific functions to the unified hash functions
//

hash_p  hash_new(size_t capacity, size_t value_size) { return unified_hash_new(capacity, value_size, UNIFIED_HASH_NUMERIC_KEYS); }
void    hash_destroy(hash_p hash)                    { unified_hash_destroy(hash); }
void    hash_resize(hash_p hash, size_t capacity)    { unified_hash_resize(hash, capacity); }

void*   hash_get_ptr(hash_p hash, hash_key_t key)    { return unified_hash_get_ptr(hash, key, NULL); }
void*   hash_put_ptr(hash_p hash, hash_key_t key)    { return unified_hash_put_ptr(hash, key, NULL); }
void    hash_remove(hash_p hash, hash_key_t key)     { unified_hash_remove(hash, key, NULL); }
bool    hash_contains(hash_p hash, hash_key_t key)   { return unified_hash_contains(hash, key, NULL); }

hash_elem_t hash_start(hash_p hash)                            { return unified_hash_start(hash); }
hash_elem_t hash_next(hash_p hash, hash_elem_t element)        { return unified_hash_next(hash, element); }
hash_key_t  hash_key(hash_elem_t element)                      { return *slot_key_ptr(element, hash_key_t); }
void*       hash_value_ptr(hash_elem_t element)                { return slot_value_ptr(element); }
void        hash_remove_elem(hash_p hash, hash_elem_t element) { unified_hash_remove_elem(hash, element); }


dict_p  dict_new(size_t capacity, size_t value_size) { return unified_hash_new(capacity, value_size, UNIFIED_HASH_STRING_KEYS); }
void    dict_destroy(dict_p dict)                    { unified_hash_destroy(dict); }
void    dict_resize(dict_p dict, size_t capacity)    { unified_hash_resize(dict, capacity); }

void*   dict_get_ptr(dict_p dict, const char* key)    { return unified_hash_get_ptr(dict, 0, key); }
void*   dict_put_ptr(dict_p dict, const char* key)    { return unified_hash_put_ptr(dict, 0, key); }
void    dict_remove(dict_p dict, const char* key)     { unified_hash_remove(dict, 0, key); }
bool    dict_contains(dict_p dict, const char* key)   { return unified_hash_contains(dict, 0, key); }

dict_elem_t dict_start(dict_p dict)                            { return unified_hash_start(dict); }
dict_elem_t dict_next(dict_p dict, dict_elem_t element)        { return unified_hash_next(dict, element); }
const char* dict_key(dict_elem_t element)                      { return *slot_key_ptr(element, const char*); }
void*       dict_value_ptr(dict_elem_t element)                { return slot_value_ptr(element); }
void        dict_remove_elem(dict_p dict, dict_elem_t element) { unified_hash_remove_elem(dict, element); }


//
// Creation and destruction functions
//

static unified_hash_p unified_hash_new(size_t capacity, size_t value_size, uint8_t key_type){
	unified_hash_p hash = malloc(sizeof(unified_hash_t));
	
	if (hash == NULL)
		return NULL;
	
	hash->capacity = capacity;
	hash->length = 0;
	// slot_size() uses key_type and value_size, so assign them first
	hash->key_type = key_type;
	hash->value_size = value_size;
	hash->slots = calloc(hash->capacity, slot_size(hash));
	
	if (hash->slots == NULL){
		free(hash);
		return NULL;
	}
	
	return hash;
}

static void unified_hash_destroy(unified_hash_p hash){
	free(hash->slots);
	free(hash);
}


//
// Lookup, get and put functions
//

/**
 * Search the hashmap for the specified key.
 * 
 * Return value >= 0: The key has been found and it's index is returned.
 * Return value <= -1: The key was not found. The index of a free slot -1 is returned
 *   as a negative number. This is either the first free slot in the probing sequence
 *   or the first slot marked as deleted.
 */
static ssize_t unified_hash_search(unified_hash_p hashmap, hash_key_t int_key, const char* string_key, unified_hash_hash_t hash){
	unified_hash_hash_t hash_at_index;
	size_t index = hash % hashmap->capacity;
	size_t probe_offset = 1;
	ssize_t first_deleted_index = -1;
	
	while(true) {
		void* slot = slot_ptr(hashmap, index);
		hash_at_index = *slot_hash_ptr(slot);
		
		if (first_deleted_index == -1 && hash_at_index == UNIFIED_HASH_SLOT_DELETED) {
			first_deleted_index = index;
		} else if (hash_at_index == UNIFIED_HASH_SLOT_FREE) {
			if (first_deleted_index != -1)
				return -(first_deleted_index + 1);
			else
				return -(index + 1);
		} else if ( hash_at_index == hash ) {
			if (hashmap->key_type == UNIFIED_HASH_NUMERIC_KEYS) {
				if ( *slot_key_ptr(slot, hash_key_t) == int_key )
					return index;
			} else {
				if ( strcmp(*slot_key_ptr(slot, const char *), string_key) == 0 )
					return index;
			}
		}
		
		index = (index + probe_offset * probe_offset) % hashmap->capacity;
		probe_offset++;
	}
}

static void* unified_hash_get_ptr(unified_hash_p hashmap, hash_key_t int_key, const char* string_key){
	unified_hash_hash_t hash = (hashmap->key_type == UNIFIED_HASH_NUMERIC_KEYS) ? int_hash(int_key) : string_hash(string_key);
	ssize_t index = unified_hash_search(hashmap, int_key, string_key, hash);
	if (index < 0)
		return NULL;
	return slot_value_ptr(slot_ptr(hashmap, index));
}

static void* unified_hash_put_ptr(unified_hash_p hashmap, hash_key_t int_key, const char* string_key){
	if (hashmap->length + 1 > hashmap->capacity * 0.75)
		unified_hash_resize(hashmap, snap_to_prime(hashmap->capacity * 2));
	
	unified_hash_hash_t hash = (hashmap->key_type == UNIFIED_HASH_NUMERIC_KEYS) ? int_hash(int_key) : string_hash(string_key);
	ssize_t index = unified_hash_search(hashmap, int_key, string_key, hash);
	void* slot = NULL;
	
	if (index < 0) {
		// Key wasn't found. The return value is -(next_free_index + 1).
		index = -index - 1;
		slot = slot_ptr(hashmap, index);
		
		*slot_hash_ptr(slot) = hash;
		if (hashmap->key_type == UNIFIED_HASH_NUMERIC_KEYS)
			*slot_key_ptr(slot, hash_key_t) = int_key;
		else
			*slot_key_ptr(slot, const char *) = string_key;
		hashmap->length++;
	} else {
		slot = slot_ptr(hashmap, index);
	}
	
	return slot_value_ptr(slot);
}

void unified_hash_remove(hash_p hashmap, hash_key_t int_key, const char* string_key){
	unified_hash_hash_t hash = (hashmap->key_type == UNIFIED_HASH_NUMERIC_KEYS) ? int_hash(int_key) : string_hash(string_key);
	ssize_t index = unified_hash_search(hashmap, int_key, string_key, hash);
	
	if (index < 0)
		return;
	
	void* slot = slot_ptr(hashmap, index);
	*slot_hash_ptr(slot) = UNIFIED_HASH_SLOT_DELETED;
	hashmap->length--;
	
	if (hashmap->length < hashmap->capacity * 0.2)
		unified_hash_resize(hashmap, snap_to_prime(hashmap->capacity / 2));
}

void unified_hash_remove_elem(hash_p hashmap, void* element){
	*slot_hash_ptr(element) = UNIFIED_HASH_SLOT_DELETED;
	hashmap->length--;
}

bool unified_hash_contains(hash_p hashmap, hash_key_t int_key, const char* string_key){
	return (unified_hash_get_ptr(hashmap, int_key, string_key) != NULL);
}


//
// Iterator functions
//

void* unified_hash_start(unified_hash_p hashmap){
	return unified_hash_element_at_or_after_slot(hashmap, hashmap->slots);
}

void* unified_hash_next(unified_hash_p hashmap, void* element){
	return unified_hash_element_at_or_after_slot(hashmap, (char*)element + slot_size(hashmap));
}

/**
 * Starts at the memory address of `slot` and scans for the next element (an slot
 * that is not free or deleted).
 * 
 * Returns NULL if there is no element at or after `slot`.
 */
static void* unified_hash_element_at_or_after_slot(unified_hash_p hash, void* slot){
	for(char* ptr = slot; ptr < (char*)hash->slots + slot_size(hash) * hash->capacity; ptr += slot_size(hash)) {
		uint64_t slot_hash = *slot_hash_ptr(ptr);
		if ( slot_hash != UNIFIED_HASH_SLOT_FREE && slot_hash != UNIFIED_HASH_SLOT_DELETED )
			return ptr;
	}
	
	return NULL;
}



static void unified_hash_resize(unified_hash_p hash, size_t new_capacity){
	// Just in case: avoid to make the hashmap smaller than it can be
	if (new_capacity < hash->length)
		return;
	
	// Create a new empty hash map with the new capacity
	unified_hash_t new_hash;
	new_hash.capacity = new_capacity;
	new_hash.length = 0;
	new_hash.value_size = hash->value_size;
	new_hash.key_type = hash->key_type;
	new_hash.slots = calloc(new_hash.capacity, slot_size(hash));
	
	// Failed to allocate memory for new hash map, leave the original untouched
	if (new_hash.slots == NULL)
		return;
	
	for(void* elem = unified_hash_start(hash); elem != NULL; elem = unified_hash_next(hash, elem)){
		void* old_value_ptr = slot_value_ptr(elem);
		void* new_value_ptr = unified_hash_put_ptr(&new_hash, *slot_key_ptr(elem, int64_t), *slot_key_ptr(elem, const char *));
		memcpy(new_value_ptr, old_value_ptr, hash->value_size);
	}
	
	free(hash->slots);
	*hash = new_hash;
}


//
// Hashing functions
// 
// The 32 and 64 bit integer hashing functions use the MurmurHash3.
// 
//   https://code.google.com/p/smhasher/wiki/MurmurHash3
// 
// For strings the djb2 hashing function is used. The 64 bit function is
// a bit weak and could use some more thinking.
// 
//   http://stackoverflow.com/questions/7666509/hash-function-for-string
//   http://www.cse.yorku.ca/~oz/hash.html
//   http://stackoverflow.com/questions/8334836/convert-djb-hash-to-64-bit
// 
// The hash table uses the hash 0 and UINT64_MAX (or UINT32_MAX) as special
// values to signal empty or deleted slots. Therefore the hash functions don't
// generate these values. The price for this is that the values of 1 and
// UINT64_MAX - 1 (or UINT32_MAX - 1) have twice the probability to appear.
// Not really sure if this is bad or not.
// 

#if defined(UNIFIED_HASH_64BIT)

	static uint64_t int64_hash64(int64_t key){
		uint64_t h = key;
		
		h ^= h >> 33;
		h *= 0xff51afd7ed558ccd;
		h ^= h >> 33;
		h *= 0xc4ceb9fe1a85ec53;
		h ^= h >> 33;
		
		if (h == 0)
			h = 1;
		else if (h == UINT64_MAX)
			h = UINT64_MAX - 1;
		
		return h;
	}
	
	static uint64_t string_hash64(const char* key){
		uint64_t hash = 5381;
		int c;
		
		while ( (c = *key++) != '\0' )
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		
		if (hash == 0)
			hash = 1;
		else if (hash == UINT64_MAX)
			hash = UINT64_MAX - 1;
		
		return hash;
	}

#else

	static uint32_t int32_hash32(int32_t key){
		uint32_t h = key;
		
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		
		if (h == 0)
			h = 1;
		else if (h == UINT32_MAX)
			h = UINT32_MAX - 1;
		
		return h;
	}
	
	static uint32_t string_hash32(const char* key){
		uint32_t hash = 5381;
		int c;
		
		while ( (c = *key++) != '\0' )
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		
		if (hash == 0)
			hash = 1;
		else if (hash == UINT32_MAX)
			hash = UINT32_MAX - 1;
		
		return hash;
	}

#endif


//
// Prime number functions.
// Not quite optimized prime number finding algo taken from libc++ implementation
// posted by Howard Hinnant at http://stackoverflow.com/a/5694432
//

size_t snap_to_prime(size_t x){
	// First search the precomputed list. The list contains the next primes after 2^x (starting with x = 2).
	size_t primes[] = {5, 11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759, 411527, 823117, 1646237, 3292489, 6584983};
	for(size_t i = 0; i < sizeof(primes) / sizeof(size_t); i++){
		if (primes[i] >= x)
			return primes[i];
	}
	
	// No match in precomputed list, compute next larger prime.
	// Make x odd by setting the least significant bit (there are no even primes).
	x |= 1;
	
	// Loop odd numbers until we have a prime
	while( !is_prime(x) )
		x += 2;
	
	return x;
}

static bool is_prime(size_t x){
	for(size_t i = 3; true; i += 2){
		size_t q = x / i;
		if (q < i)
			return true;
		if (x % i == 0)
			return false;
	}
	
	return true;
}