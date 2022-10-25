/**
 * @file extlib.h
 * @brief Extended general utilities support.
 * @details Provides extended support for general utilities.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_UTILITIES_H
#define EXTENDED_UTILITIES_H


#include "extint.h"
#include <stdlib.h>

#ifdef _WIN32
   /* compatibility layer definitions for file mapping */
   #define MAP_FILE        0x00
   #define MAP_SHARED      0x01
   #define MAP_PRIVATE     0x02
   #define MAP_TYPE        0x0f
   #define MAP_FIXED       0x10
   #define MAP_ANONYMOUS   0x20
   #define MAP_ANON        MAP_ANONYMOUS
   #define MAP_FAILED      ((void *) -1)
   #define PROT_NONE       0x00
   #define PROT_READ       0x01
   #define PROT_WRITE      0x02
   #define PROT_EXEC       0x04

/* end Windows */
#else
   #include <sys/mman.h>  /* for mmap() et al */

/* end UNIX-like */
#endif

/**
 * Linked node struct.
*/
typedef struct linkednode_t {
   struct linkednode_t *next; /**< pointer to next node (if in list) */
   struct linkednode_t *prev; /**< pointer to previous node (if in list) */
   void *data;                /**< pointer to data for node */
} LinkedNode;

/**
 * Linked list struct.
*/
typedef struct linkedlist_t {
   struct linkednode_t *next; /**< pointer to next node (head) in list */
   struct linkednode_t *last; /**< pointer to last node (tail) in list */
   int count;                 /**< count of nodes in list */
} LinkedList;

/* C/C++ compatible function prototypes for extthread.c */
#ifdef __cplusplus
extern "C" {
#endif

/* 64-bit guard */
#ifdef HAS_64BIT
   void put64_x64(void *buff, void *val);

#endif

void put64_x86(void *buff, void *val);

word16 get16(void *buff);
void put16(void *buff, word16 val);
word32 get32(void *buff);
void put32(void *buff, word32 val);
void put64(void *buff, void *val);
void srand16fast(word32 x);
word32 get_rand16fast(void);
void srand16(word32 x, word32 y, word32 z);
void get_rand16(word32 *x, word32 *y, word32 *z);
word32 rand16fast(void);
word32 rand16(void);
word32 rand32(void);
void srand32(unsigned long long x);
void shuffle(void *list, size_t size, size_t count);
void shufflenz(void *list, size_t size, size_t count);

void *bsearch_len(const void *key, size_t len,
   const void *ptr, size_t count, size_t size);
int filesort(const char *filename, size_t size, size_t bufsz,
   int (*comp)(const void *, const void *));
int link_list_append(LinkedList *srcp, LinkedList *dstp);
int link_node_append(LinkedNode *nodep, LinkedList *listp);
LinkedNode *link_node_create(size_t datasz);
void link_node_destroy(LinkedNode *lnp);
int link_node_insert
   (LinkedNode *nodep, LinkedNode *currp, LinkedList *listp);
int link_node_remove(LinkedNode *nodep, LinkedList *listp);

#ifdef _WIN32

void *mmap(void *addr, size_t len, int prot, int flags, int fd, size_t off);
int munmap(void *addr, size_t length);

/* end Windows */
#endif

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
