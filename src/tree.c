/* Copyright (c) 2003 ACM/SIGDA
   Prepared by Geert Janssen, geert@us.ibm.com
*/

/* ------------------------------------------------------------------------ */
/* INCLUDES                                                                 */
/* ------------------------------------------------------------------------ */

#include "tree.h"

/* ------------------------------------------------------------------------ */
/* LOCAL DEFINES                                                            */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* LOCAL TYPE DEFINITIONS                                                   */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* LOCAL VARIABLES                                                          */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* FUNCTION DEFINITIONS                                                     */
/* ------------------------------------------------------------------------ */

Tree
tree_mk_leaf(const char *id, double wire, double time, double load)
{
  Tree t = malloc(sizeof(*t));

  if (!t) {
    printf("[tree_mk]: memory allocation failed.\n");
    exit(1);
  }

  T_LEAF(t) = 1;
  T_NAME(t) = id;
  T_WIRE(t) = wire;
  T_TIME(t) = time;
  T_LOAD(t) = load;
  T_DATA(t) = NULL;

  return t;
}

Tree
tree_mk_inode(const char *id, double wire, Tree sub1, Tree sub2)
{
  Tree t = malloc(sizeof(*t));

  if (!t) {
    printf("[tree_mk]: memory allocation failed.\n");
    exit(1);
  }

  T_LEAF(t) = 0;
  T_NAME(t) = id;
  T_WIRE(t) = wire;
  T_SUB1(t) = sub1;
  T_SUB2(t) = sub2;
  T_DATA(t) = NULL;

  return t;
}
