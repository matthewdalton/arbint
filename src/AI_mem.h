/*
 * Memory management wrappers
 */
#ifndef _AI_MEM_H_
#define _AI_MEM_H_

void *AI_malloc(size_t size);

void AI_free(void *mem);

void *AI_memcpy(void *dest, void const *src, size_t size);

void *AI_memset(void *dest, int val, size_t len);

#endif /* _AI_MEM_H_ */
