/* Copyright (c) 2003 ACM/SIGDA
   Prepared by Geert Janssen, geert@us.ibm.com
*/

#ifndef TREE_H
#define TREE_H

/* ------------------------------------------------------------------------ */
/* INCLUDES								    */
/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <alloca.h>

#if defined __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* DEFINES								    */
/* ------------------------------------------------------------------------ */

/* Tree node field access macros: */
#define T_LEAF(x)		((x)->leaf)
#define T_NAME(x)		((x)->id)
#define T_DATA(x)		((x)->data)
#define T_WIRE(x)		((x)->wire)
/* Next are only valid for leaf nodes: */
#define T_TIME(x)		((x)->u.l.time)
#define T_LOAD(x)		((x)->u.l.load)
/* Next are only valid for internal nodes: */
#define T_SUB1(x)		((x)->u.i.sub1)
#define T_SUB2(x)		((x)->u.i.sub2)

/* ------------------------------------------------------------------------ */
/* TYPE DEFINITIONS							    */
/* ------------------------------------------------------------------------ */

/* Data structure for fanout trees:
   A binary tree with internal nodes that represent signal branching points,
   and leaf nodes that represent sinks.
*/
typedef struct Node_S *Tree;
struct Node_S {
  int leaf;			/* 1 for leaf node, 0 otherwise */
  const char *id;		/* identification string */
  double wire;			/* length of wire to (virtual) parent node */
  union {
    struct {
      double time;		/* sink required arrival time */
      double load;		/* sink capacitive load */
    } l;
    struct {
      Tree sub1;		/* internal node first subtree */
      Tree sub2;		/* internal node second subtree */
    } i;
  } u;
  void *data;			/* node application data */
};

/* ------------------------------------------------------------------------ */
/* FUNCTION PROTOTYPES							    */
/* ------------------------------------------------------------------------ */

/* Returns a leaf node containing the data supplied. */
Tree tree_mk_leaf(const char *id, double wire, double time, double load);

/* Returns a freshly created internal tree node with as children
   `sub1' and `sub2' and the additionally supplied data.
*/
Tree tree_mk_inode(const char *id, double wire, Tree sub1, Tree sub2);

#ifdef __cplusplus
}
#endif

#endif /* TREE_H */
