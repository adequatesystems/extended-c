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

/**
 * @struct DLNODE Doubly-linked node struct.
 * @property DLNODE::next Pointer to next Doubly-linked node.
 * @property DLNODE::prev Pointer to previous Doubly-linked node.
 * @property DLNODE::data Pointer to node data.
*/
typedef struct doubly_linked_node {
   struct doubly_linked_node *next;
   struct doubly_linked_node *prev;
   void *data;
} DLNODE;

/**
 * @struct DLLIST Doubly-linked list struct.
 * @property DLLIST::next Pointer to next node (list head).
 * @property DLLIST::last Pointer to last node (list tail).
 * @property DLLIST::count Number of nodes in list.
*/
typedef struct doubly_linked_list {
   struct doubly_linked_node *next;
   struct doubly_linked_node *last;
   int count;
} DLLIST;

/**
 * @struct SLNODE Singly-linked node struct.
 * @property SLNODE::next Pointer to next Singly-linked node.
 * @property SLNODE::data Pointer to node data.
*/
typedef struct singly_linked_node {
   struct singly_linked_node *next;
   void *data;
} SLNODE;

/**
 * @struct SLLIST Singly-linked list struct.
 * @property SLLIST::next Pointer to next node (list head).
 * @property SLLIST::count Number of nodes in list.
*/
typedef struct singly_linked_list {
   struct singly_linked_node *next;
   int count;
} SLLIST;

/* C/C++ compatible function prototypes for extthread.c */
#ifdef __cplusplus
extern "C" {
#endif

/* 64-bit guard */
#ifdef HAS_64BIT
   void put64_x64(void *buff, const void *value);

#endif

void put64_x86(void *buff, const void *val);

word16 get16(const void *value);
void put16(void *buff, word16 value);
word32 get32(const void *value);
void put32(void *buff, word32 value);
void put64(void *buff, const void *value);
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

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
