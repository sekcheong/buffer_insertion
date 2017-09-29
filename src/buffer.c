/* Copyright (c) ACM/SIGDA
   Prepared by Geert Janssen, geert@us.ibm.com
*/

/* ------------------------------------------------------------------------ */
/* INCLUDES								    */
/* ------------------------------------------------------------------------ */

#include <math.h>
#include "parser.h"

/* ------------------------------------------------------------------------ */
/* LOCAL DEFINES                                                            */
/* ------------------------------------------------------------------------ */

/* Define this to resort to a simple-minded solution approach. */
#define _CHICKEN

/* Accuracy in comparing two floating point numbers for equality. */
#define EPS		1e-5
#define EQUAL(a,b)	(fabs((a) - (b)) < EPS)

#define min(a,b)	((a) < (b) ? (a) : (b))
#define max(a,b)	((a) > (b) ? (a) : (b))

#define OFLAG(x)	((x)->flag)
#define OPAIR(x)	((x)->p)
#define OTIME(x)	PTIME(OPAIR(x))
#define OLOAD(x)	PLOAD(OPAIR(x))
#define ONEXT(x)	((x)->next)

#define PTIME(x)	((x).T)
#define PLOAD(x)	((x).L)

#define VISIT_MASK	0x1
#define VISIT(x)	(FLAG(x) |  VISIT_FLAG)
#define UNVISIT(x)	(FLAG(x) & ~VISIT_FLAG)
#define VISITED(x)	(!!(FLAG(x) & VISIT_FLAG))

#define OPTS_GET(x)	((Options) T_DATA(x))
#define OPTS_SET(x,o)	(T_DATA(x) = (x))

/* ------------------------------------------------------------------------ */
/* LOCAL TYPE DEFINITIONS                                                   */
/* ------------------------------------------------------------------------ */

typedef unsigned int	Bool;
typedef unsigned int	Nat;
typedef double		Time;
typedef double		Capacitance;
typedef double		Resistance;
typedef double		Length;

/* An option pair. */
typedef struct Pair_S {
  Time T;			/* required arrival time */
  Capacitance L;		/* overall capacitive load */
} Pair;

/* A list of option pairs. */
typedef struct Options_S *Options;
struct Options_S {
  /*Nat flag;*/			/* as yet unused */
  Pair p;			/* the element pair */
  Options next;			/* rest of list or NULL */
};

/* ------------------------------------------------------------------------ */
/* VARIABLES		                                                    */
/* ------------------------------------------------------------------------ */
/* ADDED FOR ECE756 HWK */
/* Boolean variable added to add a bunch of debug messages for merge/filter */
static Bool my_debug = 0;
/* END ADDED FOR ECE756 HWK */

static Bool debug = 0;

/* Per unit wire length resistance and capacitance values: */
static Resistance	R_unit =  0.1; /* [Ohm/m] */
static Capacitance	C_unit =  0.2; /* [F/m] */

/* Buffer parameters: */
static Resistance	R_buf  = 10.0; /* Ohm */
static Capacitance	C_buf  =  4.0; /* femto Farad */
static Time		D_buf  =  2.0; /* nano sec */

/* ------------------------------------------------------------------------ */
/* FUNCTION DEFINITIONS                                                     */
/* ------------------------------------------------------------------------ */

/* Returns the pair (T,L). */
static Pair
pair_mk(Time T, Capacitance L)
{
  Pair p;

  PTIME(p) = T;
  PLOAD(p) = L;
  return p;
}

static void
pair_show(FILE *fp, Pair p)
{
  fprintf(fp, "(%.2f, %.2f)", PTIME(p), PLOAD(p));
}

static Options
option_mk(Pair p)
{
  Options o = malloc(sizeof(*o));

  OPAIR(o) = p;
  ONEXT(o) = NULL;
  return o;
}

static void
option_free(Options o)
{
  if (o)
    free(o);
}

#if 0
static Options
options_copy(Options o)
{
  Options Z, *tail = &Z;

  while (o) {
    *tail = option_mk(OPAIR(o));
    tail = &ONEXT(*tail);
    o = ONEXT(o);
  }
  return Z;
}

static Nat
options_len(Options o)
{
  Nat len;

  for (len = 0; o; o = ONEXT(o), len++)
    ;
  return len;
}
#endif

static Options
options_last(Options o)
{
  while (o) {
    Options next = ONEXT(o);

    if (!next)
      return o;
    o = next;
  }
  return NULL;
}

static void
options_free(Options o)
{
  while (o) {
    Options next = ONEXT(o);
    option_free(o);
    o = next;
  }
}

static void
options_show(FILE *fp, Options o, const char *msg)
{
  fprintf(fp, "%s", msg);
  while (o) {
    Options next;

    pair_show(fp, OPAIR(o));
    next = ONEXT(o);
    if (next)
      fprintf(fp, ", ");
    o = next;
  }
  fprintf(fp, "\n");
}

static void
node_options_show(FILE *fp, Options o, Tree k)
{
  char buf[64];

  sprintf(buf, "%s: ", T_NAME(k));
  options_show(stdout, o, buf);
}

/* ------------------------------------------------------------------------ */

/* Returns (singleton) options list for sink node k. */
static Options
options_sink(Tree k)
{
  Options Z;

  Z = option_mk(pair_mk(T_TIME(k), T_LOAD(k)));
  if (debug)
    options_show(stdout, Z, "Sink:\n");
  return Z;
}


/************************************************/
/* (FILLED BY YOU) */

/* Filter out inferior pairs from the Options list.*/
static Options options_filter(Options Z, Nat len){

  return Z; //replace this line
}

/************************************************/

/* Add wire segment of length l.
   Update rules:

   T = T - R * C / 2.0 - R * L
   L = L + C

   Returns possibly modified options list Z.
*/
static Options
options_add_wire(Options Z, Length l)
{
  Resistance  R = R_unit * l;
  Capacitance C = C_unit * l;
  Options o;
  Nat len;

  for (o = Z, len = 0; o; o = ONEXT(o), len++) {
    OTIME(o) -= R * (C / 2.0 + OLOAD(o));
    OLOAD(o) += C;
  }
  if (debug)
    options_show(stdout, Z, "Added wire:\n");

  return options_filter(Z, len);
}

/* Add buffer option.
   Update rules:

   T = T - Dbuf - Rbuf * L
   L = Cbuf

   Returns possibly modified options list Z.
*/
static Options
options_add_buffer(Options Z)
{
  Options *tail, o;
  Nat len;
  Time Tmax;

#ifdef CHICKEN
  /* For each option, calculate new option when buffer is added, and
     insert that new option right after the original one.
     Of course this disrupts the order and also might introduce redundant
     options, therefore must explicitly sort/filter the list.
  */
  for (o = Z, len = 0; o; o = ONEXT(o), len++) {
    Time    T = OTIME(o) - D_buf - R_buf * OLOAD(o);
    Options n = option_mk(pair_mk(T, C_buf));

    /* Insert n after o: */
    ONEXT(n) = ONEXT(o);
    ONEXT(o) = n;
    o = n;
    len++;
  }
  if (debug)
    options_show(stdout, Z, "Added buffer:\n");

  Z = options_filter(Z, len);
#else
  /* Determine Tmax: */

  /* Get reference time by adding buffer to first option element: */
  o = Z;
  Tmax = OTIME(o) - D_buf - R_buf * OLOAD(o);
  /* Compare against rest of elements if any: */
  o = ONEXT(o);
  len = 1;
  while (o) {
    /* Get effect of adding buffer to this option element: */
    Time T = OTIME(o) - D_buf - R_buf * OLOAD(o);

    if (T > Tmax)
      Tmax = T;
    o = ONEXT(o);
    len++;
  }
  /* Note: Tmax <= OTIME(o) for some o elem Z. */

  /* Find location to (possibly) insert buffer option (Tmax,C_buf): */
  tail = &Z;
  o = *tail;
  while (OTIME(o) < Tmax) {
    tail = &ONEXT(o);
    o = *tail;
  }
  /* Here: must have Tmax <= OTIME(o) */

  if (EQUAL(OTIME(o), Tmax)) {
    /* Prune the existing option or the buffer option: */
    if (C_buf < OLOAD(o))
      /* This element o becomes the buffer option: */
      OLOAD(o) = C_buf;
    /* else discard buffer option. */
  }
  else { /* Here: Tmax < OTIME(o) */
    if (C_buf < OLOAD(o)) {
      /* Insert buffer option before element o: */
      *tail = option_mk(pair_mk(Tmax, C_buf));
      ONEXT(*tail) = o;
    }
    /* else discard buffer option. */
  }
  if (debug)
    options_show(stdout, Z, "Added buffer:\n");
#endif
  return Z;
}



/* (FILLED BY YOU) */
inline static Options
options_merge(Options o1, Options o2)
{
  /*Find merged capacitive loading and RAT of the combined solution
  Note you can make use of the function option_mk here to allocate new solution*/
  Time T;
  Capacitance L;
  /*
  if (OTIME(o1) < OTIME(o2))
    T = OTIME(o1);
  else
    T = OTIME(o2);
  */
  if (my_debug) {
    options_show(stderr, o1, "options_merge o1:");  
    options_show(stderr, o2, "options_merge o2:");  
  }
  
  T = min(OTIME(o1), OTIME(o2));
  L = OLOAD(o1) + OLOAD(o2);

  if(my_debug) fprintf(stderr, "options_merge output: (%2f, %2f)\n\n", T, L);

  return option_mk(pair_mk(T, L));
} 

/*
   Cartesian product of option sets with on-the-fly pruning.
   Update rules:

   T = min(T1, T2)
   L = L1 + L2

   Returns freshly created options list Z.
*/
static Options
options_combine(Options Z1, Options Z2)
{
  Options Z, o1, o2;
  Options *tail;
  Nat len=0;
  /* Combine every option element from Z1 with every option element from
     Z2. The merge operation is symmetric so this approach is sufficient
     to generate all possible combinations. The merge results are stored
     in a new list Z.
     Of course this list need not be ordered and very likely contains
     redundant options, therefore must explicitly sort/filter the list.
  */

  tail=&Z;
  for (o1 = Z1; o1; o1 = ONEXT(o1))
    for (o2 = Z2; o2; o2 = ONEXT(o2)) {
      /* Merge o1 and o2 */
        *tail = options_merge(o1, o2); 
        tail=&ONEXT(*tail);
        len++;
    }
  if (debug) options_show(stdout, Z, "test");

  /*   Z=*tail;*/
  Z = options_filter(Z, len); 

  /* Delete original lists: */
  options_free(Z1);
  options_free(Z2);
  return Z;
}
/************************************************/

/* It is assumed that option lists are < sorted w.r.t. both time and load
   values. For any two elements (a1,b1) and (a2,b2) in the list we have
   that if (a1,b1) appears before (a2,b2) then a1 < a2 and b1 < b2.
*/

/* Lukas P.P.P. van Ginneken algorithm for optimal buffer insertion in
   RC-tree.
*/
static Options
bottom_up(Tree k, Bool no_buf)
{
 Options Z, Z1, Z2;

  if (T_LEAF(k))
    Z = options_sink(k);
  else {
    Z1 = bottom_up(T_SUB1(k), no_buf);
    Z2 = bottom_up(T_SUB2(k), no_buf);
    
    /****************************/
    /** IMPLMENT THIS FUNCTION **/

    Z  = options_combine(Z1, Z2);

    /***************************/
  }
  Z = options_add_wire(Z, T_WIRE(k));
  if (!no_buf)
    Z = options_add_buffer(Z);
  if (debug)
    node_options_show(stdout, Z, k);
  return Z;
}

int
main(int argc, char *argv[])
{
  Tree t;

  if (argc > 1 && argv[1])
    if (!freopen(argv[1], "r", stdin)) {
      fprintf(stderr, "Cannot open file `%s' for reading.\n", argv[1]);
      return EXIT_FAILURE;
    }

  t = parse();

  pair_show(stdout, OPAIR(bottom_up(t, 1)));
  fprintf(stdout, "\n");
  pair_show(stdout, OPAIR(options_last(bottom_up(t, 0))));
  fprintf(stdout, "\n");

  return EXIT_SUCCESS;
}
