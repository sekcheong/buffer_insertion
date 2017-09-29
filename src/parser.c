/* Copyright (c) 2003 ACM/SIGDA
   Prepared by Geert Janssen, geert@us.ibm.com
*/

/* ------------------------------------------------------------------------ */
/* INCLUDES                                                                 */
/* ------------------------------------------------------------------------ */

#include "parser.h"

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

/* Considerations:
   - In case of syntax error, give appropriate message and abort; no recovery.
   - When parsing is successful, tree will be valid.
   - Parse routines mirror structure of grammar rules.
   - Reading a character takes care of skipping white-space.
*/

/* Give syntax error message and abort program. */
static void
fatal(char *mes, ...)
{
  va_list ap;

  fprintf(stderr, "Syntax error: ");
  va_start(ap, mes);
  vfprintf(stderr, mes, ap);
  fprintf(stderr, ".\n");
  va_end(ap);
  exit(1);
}

/* --------------------------------------------------------------------- */
/* The Lexer                                                             */
/* --------------------------------------------------------------------- */

/* Reads a character (first skips any white-space). */
static int
readc(void)
{
  int c;

 restart:
  /* Check for end-of-file condition: */
  if (feof(stdin)) return EOF;

  /* Skip any whitespace: */
  do c = getchar(); while (isspace(c));

  /* Skip any comment-till-end-of-line: */
  if (c == '#') {
    while ((c = getchar()) != '\n' && c != EOF)
      ;
    goto restart;
  }
  return c;
}

/* Reads an identifier (first skips any white-space). */
static const char *
read_ident(void)
{
  char s[BUFSIZ];

  if (fscanf(stdin, "%s", s) != 1)
    fatal("identifier expected");
  return (const char *) strcpy(malloc(strlen(s) + 1), s);
}

/* Reads a C-style floating point number (first skips any white-space). */
static double
read_number(void)
{
  double n;

  if (fscanf(stdin, "%lf", &n) != 1)
    fatal("floating-point number expected");
  return n;
}

/* --------------------------------------------------------------------- */
/* The Parser                                                            */
/* --------------------------------------------------------------------- */

/* Leaf : "<" Id Wire_Length Required_Time Load ">" . */
static Tree
P_Leaf(void)
{
  int c;
  const char *id;
  double wl, rt, cl;

  if ((c = readc()) != '<')
    fatal("'<' expected");

  id = read_ident();
  wl = read_number();
  rt = read_number();
  cl = read_number();

  if ((c = readc()) != '>')
    fatal("'>' expected");

  return tree_mk_leaf(id, wl, rt, cl);
}

/* Tree          : Leaf
                 | "(" Id Wire_Length Tree Tree ")" .
*/
static Tree
P_Tree(void)
{
  Tree t1, t2;
  int c;
  const char *id;
  double wl;

  if ((c = readc()) != '(') {
    ungetc(c, stdin);
    return P_Leaf();
  }

  id = read_ident();
  wl = read_number();

  t1 = P_Tree();
  t2 = P_Tree();

  if (readc() != ')')
    fatal("')' expected");

  return tree_mk_inode(id, wl, t1, t2);
}

/* Input : Tree */
Tree
parse(void)
{
  return P_Tree();
}
