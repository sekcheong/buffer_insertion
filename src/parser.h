/* Copyright (c) 2003 ACM/SIGDA
   Prepared by Geert Janssen, geert@us.ibm.com
*/

#ifndef PARSER_H
#define PARSER_H

#include "tree.h"

#if defined __cplusplus
extern "C" {
#endif

/* Syntax for specifying a fanout tree.
   The input consists of a single tree specification.
   White-space (blank, tab, newline) has no semantic meaning and may be used
   in any quantity to enhance the readability. The same holds for comments
   that start with the hash character '#' and extend to the end of the line.

        Input         : Tree .
        Tree          : Leaf
                      | "(" Id Wire_Length Tree Tree ")" .
        Leaf          : "<" Id Wire_Length Required_Time Load ">" .
        Wired_Length  : Float_Number .
        Required_Time : Float_Number .
        Load          : Float_Number .
        Id            : Identifier .

   Note: Float_Numbers are C language floating point numbers; Identifier
         denotes any text starting with a non-blank character till the next
	 blank character or end-of-file.
*/

/* Reads a fanout tree from stdin and constructs a tree data structure
   using the functions and macros defined in the file `tree.h'.
   The accepted input format is defined above.
   Aborts (with exit(1)) whenever a syntax error occurs.

   The top-level production rule is:

        Input : Tree .
*/
Tree parse(void);

#ifdef __cplusplus
}
#endif

#endif /* PARSER_H */
