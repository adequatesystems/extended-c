
#include "_assert.h"
#include "extlib.h"

#include "exterrno.h"
#include <stdio.h>

int main()
{  /* check; operation and failures of SLLIST/SLNODE operation */
   SLLIST list = { 0 };
   SLNODE *np, *np_ins;

   /* FUNCTION TESTS */

   /* create and push nodes */
   ASSERT_NE((np = slnode_create(0)), NULL);
   ASSERT_EQ(slnode_push(np, &list), 0);
   ASSERT_NE((np = slnode_create(sizeof(int))), NULL);
   ASSERT_EQ(slnode_push(np, &list), 0);
   *((int *) np->data) = 1;
   ASSERT_NE((np = slnode_create(sizeof(int))), NULL);
   ASSERT_EQ(slnode_push(np, &list), 0);
   *((int *) np->data) = 2;
   ASSERT_NE((np = slnode_create(sizeof(int))), NULL);
   ASSERT_EQ(slnode_push(np, &list), 0);
   *((int *) np->data) = 3;
   ASSERT_NE((np = slnode_create(sizeof(int))), NULL);
   ASSERT_EQ(slnode_push(np, &list), 0);
   *((int *) np->data) = 4;
   /* list need only a head (latest push), and contain 5 nodes */
   ASSERT_NE(list.next, NULL);
   ASSERT_EQ(list.next, np);
   ASSERT_EQ(list.count, 5);

   /* CHECK RESULTING LIST [4, 3, 2, 1, NULL] */
   np = list.next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 4);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 3);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 2);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np->data, NULL);
   ASSERT_EQ(*((int *) np->data), 1);
   np = np->next;
   ASSERT_NE(np, NULL);
   ASSERT_EQ(np->data, NULL);
   np = np->next;
   ASSERT_EQ(np, NULL);

   /* remove all but the last node */
   for (np = list.next; np && np->next; np = list.next) {
      np_ins = slnode_pop(&list);
      ASSERT_NE(np_ins, NULL);
      slnode_destroy(np_ins);
   }

   /* CHECK RESULTING LIST [NULL] */
   np = list.next;
   ASSERT_NE(np, NULL);
   ASSERT_NE(np, NULL);
   ASSERT_EQ(np->data, NULL);
   np = np->next;
   ASSERT_EQ(np, NULL);

   /* pop node from list */
   np = slnode_pop(&list);
   ASSERT_EQ(list.next, NULL);

   /* FAILURE TESTS */

   /* bad slnode_push() */
   set_errno(0);
   ASSERT_NE(slnode_push(np, NULL), 0);
   ASSERT_EQ(errno, EINVAL);
   set_errno(0);
   ASSERT_NE(slnode_push(NULL, &list), 0);
   ASSERT_EQ(errno, EINVAL);

   /* bad slnode_pop() */
   set_errno(0);
   ASSERT_EQ(slnode_pop(&list), NULL);
   ASSERT_EQ(errno, ENOLINK);
   ASSERT_EQ(list.next, NULL);
   set_errno(0);
   ASSERT_EQ(slnode_pop(NULL), NULL);
   ASSERT_EQ(errno, EINVAL);

   /* bad slnode_create(): obscene memsize */
   ASSERT_EQ(slnode_create( ( 1LL << 62 ) ), NULL);

   /* cleanup */
   slnode_destroy(np);
}
