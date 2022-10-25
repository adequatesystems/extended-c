
#include "_assert.h"
#include "extlib.h"

#include "exterrno.h"
#include <stdio.h>

int main()
{  /* check; operation and failures of LinkedList/Node operation */
   LinkedList list = { 0 };
   LinkedList list2 = { 0 };
   LinkedList list_nolast = { 0 };
   LinkedList list_nonext = { 0 };
   LinkedNode *lnp, *lnp_ins;

   /* INIT */
   lnp = link_node_create(0);
   list_nolast.next = lnp;

   /* FUNCTION TESTS */

   /* create and append nodes */
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_append(lnp, &list2), 0);
   *((int *) lnp->data) = 5;
   ASSERT_NE((lnp = link_node_create(0)), NULL);
   ASSERT_EQ(link_node_append(lnp, &list), 0);
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_append(lnp, &list), 0);
   *((int *) lnp->data) = 1;
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_append(lnp, &list), 0);
   *((int *) lnp->data) = 2;
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_append(lnp, &list), 0);
   *((int *) lnp->data) = 3;
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_append(lnp, &list), 0);
   *((int *) lnp->data) = 4;
   /* list must have a head and tail, and contain 5 nodes */
   ASSERT_NE(list.next, NULL);
   ASSERT_NE(list.last, NULL);
   ASSERT_EQ(list.count, 5);

   /* perform node "insertions" at head, middle and end */
   lnp_ins = list.next;
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_insert(lnp, lnp_ins, &list), 0);
   *((int *) lnp->data) = 5;
   ASSERT_NE((lnp_ins = lnp_ins->next), NULL);
   ASSERT_NE((lnp_ins = lnp_ins->next), NULL);
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_insert(lnp, lnp_ins, &list), 0);
   *((int *) lnp->data) = 6;
   lnp_ins = list.last;
   ASSERT_NE((lnp = link_node_create(sizeof(int))), NULL);
   ASSERT_EQ(link_node_insert(lnp, lnp_ins, &list), 0);
   *((int *) lnp->data) = 7;

   /* CHECK RESULTING LIST [5, NULL, 1, 6, 2, 3, 7, 4] */
   lnp = list.next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 5);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_EQ(lnp->data, NULL);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 1);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 6);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 2);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 3);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 7);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 4);

   /* remove all but the first linked node */
   for (lnp = list.next->next; lnp; lnp = lnp_ins) {
      lnp_ins = lnp->next;
      ASSERT_EQ(link_node_remove(lnp, &list), 0);
      link_node_destroy(lnp);
   }

   /* link lists together, should result in [ 5, 5 ] */
   ASSERT_EQ(link_list_append(&list2, &list), 0);
   lnp = list.next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 5);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 5);
   /* swap list back */
   ASSERT_EQ(link_list_append(&list, &list2), 0);
   lnp = list2.next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 5);
   lnp = lnp->next;
   ASSERT_NE(lnp, NULL);
   ASSERT_NE(lnp->data, NULL);
   ASSERT_EQ(*((int *) lnp->data), 5);

   /* FAILURE TESTS */
   lnp = link_node_create(0);
   list_nolast.next = lnp;
   list_nonext.last = lnp;
   list_nolast.count = list_nonext.count = 1;

   /* bad link_list_append():
    * src.last == NULL, src.next == NULL, dst.last == NULL */
   set_errno(0);
   ASSERT_NE(link_list_append(&list_nolast, &list2), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_list_append(&list_nonext, &list2), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_list_append(&list2, &list_nolast), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_list_append(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(link_list_append(&list2, NULL), 0);
   ASSERT_EQ(errno, EINVAL);
   list.next = NULL;

   /* bad link_node_append(): list.last == NULL && list.next != NULL */
   set_errno(0);
   ASSERT_NE(link_node_append(lnp, &list_nolast), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_node_append(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(link_node_append(lnp, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* bad link_node_create(): obscene memsize */
   ASSERT_EQ(link_node_create( ( 1LL << 62 ) ), NULL);

   /* bad link_node_remove():
    * node != listp->last && node->next == NULL
    * node != listp->next && node->prev == NULL */
   set_errno(0);
   ASSERT_NE(link_node_remove(lnp, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   lnp->next = lnp;
   set_errno(0);
   ASSERT_NE(link_node_remove(lnp, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_node_remove(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(link_node_remove(lnp, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* bad link_node_insert(): currp != NULL && currp->prev == NULL */
   lnp_ins = list2.last;
   lnp = lnp_ins->prev;
   lnp_ins->prev = NULL;
   set_errno(0);
   ASSERT_NE(link_node_insert(lnp, lnp_ins, &list), 0);
   ASSERT_EQ(errno, ENOLINK);
   set_errno(0);
   ASSERT_NE(link_node_insert(NULL, lnp_ins, &list), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(link_node_insert(lnp, lnp_ins, NULL), 0);
   ASSERT_EQ(errno, EINVAL);

   /* cleanup*/
   link_node_destroy(list2.next);
   link_node_destroy(list2.last);
}
