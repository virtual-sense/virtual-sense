#ifndef __heap_h
#define __heap_h

#include "common/types.h"

#include "config.h"

#define HEAP_REFSTACKSIZE 4

/**
 * Heap chunk types. Darjeeling keeps a heap where Java objects and non-Java objects are mixed on the same heap.
 * This enumeration indicates the types of built-in objects.
 */
// CHUNKID_FREE acts as a marker. Everything above CHUNKID_FREE is a built-in object and is never garbage collected
// Please don't change the order of things here
enum chunk_id
{
	CHUNKID_FREE=0,
	CHUNKID_INVALID=1,

	CHUNKID_MONITOR_BLOCK=2,
	CHUNKID_VM=3,
	CHUNKID_FRAME=4,
	CHUNKID_THREAD=5,

	CHUNKID_MANAGED_START=6,

	CHUNKID_INFUSION=6,
	CHUNKID_REFARRAY=7,
	CHUNKID_INTARRAY=8,

	CHUNKID_JAVA_START=9

};

typedef struct _heap_chunk heap_chunk;

/**
 *
 */
enum
{
	TCM_WHITE = 0,
	TCM_GRAY = 1,
	TCM_BLACK = 2,
};

struct _heap_chunk
{
	uint16_t color:2;
	uint16_t size:14;
	uint16_t shift;
	uint8_t id;
#ifdef ALIGN_16
	uint8_t PADDING;
#endif
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

/* system ibernation function declaration */
/**
 * Get heap base pointer
 */
void *dj_mem_get_base_pointer();

/**
 * Get heap left_pointer from base
 */
uint16_t dj_mem_get_left_pointer();

/**
 * Get heap rigth_pointer from base
 */
uint16_t dj_mem_get_right_pointer();

/**
 * Get heap panic_exceptio_object_pointer from base
 */
uint16_t dj_mem_get_panic_exception_object_pointer();

/**
 * Get heap refStackPointer
 */
uint8_t dj_mem_get_ref_stack();

/**
 * Get heap left_pointer from base
 */
void dj_mem_set_left_pointer(uint16_t left_p);

/**
 * Set heap rigth_pointer from base
 */
void dj_mem_set_right_pointer(uint16_t right_p);

/**
 * Set heap panic_exceptio_object_pointer from base
 */
void dj_mem_set_panic_exception_object_pointer(uint16_t panic_exc);
/**
 * Set heap refStackPointer
 */
void dj_mem_set_ref_stack(uint8_t ref_stack);

/* END system ibernation declaration */

void dj_mem_init(void *mem_pointer, uint16_t mem_size);
void * dj_mem_alloc(uint16_t size, runtime_id_t id);
void dj_mem_free(void *ptr);

void dj_mem_setPanicExceptionObject(dj_object *obj);
dj_object * dj_mem_getPanicExceptionObject();

void dj_mem_pushCompactionUpdateStack(ref_t ref);
ref_t dj_mem_popCompactionUpdateStack();

int dj_mem_countChunks(runtime_id_t id);
void dj_mem_gc();

void * dj_mem_getPointer();

void dj_mem_shiftRuntimeIDs(runtime_id_t start, uint16_t range);

uint16_t dj_mem_getFree();
uint16_t dj_mem_getSize();
#ifdef DARJEELING_DEBUG
#if 0
void dj_mem_dump();
#endif
#endif

#ifdef DARJEELING_DEBUG_CHECK_HEAP_SANITY
void dj_check_sanity();
#endif

static inline void * dj_mem_getData(heap_chunk * chunk)
{
	return (void*)(chunk) + sizeof(heap_chunk);
}

static inline runtime_id_t dj_mem_getChunkId(void *ptr)
{
	return ((heap_chunk*)((size_t)ptr-sizeof(heap_chunk)))->id;
}

static inline void dj_mem_setChunkId(void *ptr, uint8_t id)
{
	((heap_chunk*)((size_t)ptr-sizeof(heap_chunk)))->id = id;
}

static inline uint16_t dj_mem_getChunkSize(void *ptr)
{
	return ((heap_chunk*)((size_t)ptr-sizeof(heap_chunk)))->size;
}

static inline uint16_t dj_mem_getChunkShift(void *ptr)
{
	return ((heap_chunk*)((size_t)ptr-sizeof(heap_chunk)))->shift;
}

static inline void * dj_mem_getUpdatedPointer(void * ptr)
{
	return (ptr==0)?0:(ptr - dj_mem_getChunkShift(ptr));
}

static inline ref_t dj_mem_getUpdatedReference(ref_t ref)
{
	return (ref==0)?0:(ref - dj_mem_getChunkShift(REF_TO_VOIDP(ref)));
}

static inline void dj_mem_setChunkColor(void *ptr, int color)
{
	((heap_chunk*)(ptr-sizeof(heap_chunk)))->color = color;
}

static inline int dj_mem_getChunkColor(void *ptr)
{
	return ((heap_chunk*)(ptr-sizeof(heap_chunk)))->color;
}

static inline void dj_mem_setRefGrayIfWhite(ref_t ref)
{
	if (ref==nullref) return;
	heap_chunk * chunk = ((heap_chunk*)(REF_TO_VOIDP(ref)-sizeof(heap_chunk)));
	if (chunk->color==TCM_WHITE) chunk->color=TCM_GRAY;
}

static inline void dj_mem_setRefColor(ref_t ref, int color)
{
	if (ref==nullref) return;
	dj_mem_setChunkColor(REF_TO_VOIDP(ref), color);
	// ((heap_chunk*)(REF_TO_VOIDP(ref)-sizeof(heap_chunk)))->color = color;
}

#endif
