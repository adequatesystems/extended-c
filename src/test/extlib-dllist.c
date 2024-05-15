
#include "_assert.h"
#include "extlib.h"

#include "exterrno.h"
#include <stdio.h>

int main()
{  /* check; operation and failures of DLLIST/DLNODE operation */
   DLLIST list = { 0 };
   DLLIST list2 = { 0 };
   DLLIST list_nolast = { 0 };
   DLLIST list_nonext = { 0 };
   DLNODE *np, *np_ins;

   /* INIT */
   np = dlnode_create(0);
   list_nolast.next = np;

   /* FUNCTION TESTS */

   /* create and append nodes */
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_append(np, &list2), 0);
   *((int *) np->data) = 5;
   ASSERT_NE((np = dlnode_create(0)), NULL);
   ASSERT_EQ(dlnode_append(np, &list), 0);
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_append(np, &list), 0);
   *((int *) np->data) = 1;
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_append(np, &list), 0);
   *((int *) np->data) = 2;
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_append(np, &list), 0);
   *((int *) np->data) = 3;
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_append(np, &list), 0);
   *((int *) np->data) = 4;
   /* list must have a head and tail, and contain 5 nodes */
   ASSERT_NE(list.next, NULL);
   ASSERT_NE(list.last, NULL);
   ASSERT_EQ(list.count, 5);

   /* perform node "insertions" at head, middle and end */
   np_ins = list.next;
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_insert(np, np_ins, &list), 0);
   *((int *) np->data) = 5;
   ASSERT_NE((np_ins = np_ins->next), NULL);
   ASSERT_NE((np_ins = np_ins->next), NULL);
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_insert(np, np_ins, &list), 0);
   *((int *) np->data) = 6;
   np_ins = list.last;
   ASSERT_NE((np = dlnode_create(sizeof(int))), NULL);
   ASSERT_EQ(dlnode_insert(np, np_ins, &list), 0);
   *((int *) np->data) = 7;

   /* CHECK RESULTING LIST [5, NULL, 1, 6, 2, 3, 7, 4] */
   np = list.next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 5);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_EQ(np->data, NULL);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 1);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 6);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 2);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 3);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 7);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 4);

   /* remove all but the first linked node */
   for (np = list.next->next; np; np = np_ins) {
      np_ins = np->next;
      ASSERT_EQ(dlnode_remove(np, &list), 0);
      dlnode_destroy(np);
   }

   /* link lists together, should result in [ 5, 5 ] */
   ASSERT_EQ(dllist_append(&list2, &list), 0);
   np = list.next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 5);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 5);
   /* swap list back */
   ASSERT_EQ(dllist_append(&list, &list2), 0);
   np = list2.next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 5);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 5);

   /* FAILURE TESTS */
   np = dlnode_create(0);
   list_nolast.next = np;
   list_nonext.last = np;
   list_nolast.count = list_nonext.count = 1;

   /* bad dllist_append():
    * src.last == NULL, src.next == NULL, dst.last == NULL */
   set_errno(0);
   ASSERT_NE(dllist_append(&list_nolast, &list2), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dllist_append(&list_nonext, &list2), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dllist_append(&list2, &list_nolast), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dllist_append(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(dllist_append(&list2, NULL), 0);
   ASSERT_EQ(errno, EINVAL);
   list.next = NULL;

   /* bad dlnode_append(): list.last == NULL && list.next != NULL */
   set_errno(0);
   ASSERT_NE(dlnode_append(np, &list_nolast), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dlnode_append(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(dlnode_append(np, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* bad dlnode_create(): obscene memsize */
   ASSERT_EQ(dlnode_create( ( 1LL << 62 ) ), NULL);

   /* bad dlnode_remove():
    * node != listp->last && node->next == NULL
    * node != listp->next && node->prev == NULL */
   set_errno(0);
   ASSERT_NE(dlnode_remove(np, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   np->next = np;
   set_errno(0);
   ASSERT_NE(dlnode_remove(np, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dlnode_remove(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(dlnode_remove(np, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* bad dlnode_insert(): currp != NULL && currp->prev == NULL */
   np_ins = list2.last;
   np = np_ins->prev;
   np_ins->prev = NULL;
   set_errno(0);
   ASSERT_NE(dlnode_insert(np, np_ins, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(dlnode_insert(NULL, np_ins, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(dlnode_insert(np, np_ins, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* cleanup*/
   dlnode_destroy(list2.next);
   dlnode_destroy(list2.last);
}
