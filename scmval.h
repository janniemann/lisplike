/*
 * Copyright (c) 2018, 2019 Jan Niemann <jan.niemann@beet5.de>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _SCMVAL_H
#define _SCMVAL_H

// type for tagged pointers

/*

struct _scmval:

 +---------------------------------+     ->  +---------------------------------+
 | pointer to struct _scmval  data |    /    | pointer to struct _scmval  data |
 +---------------------------------+   /     +---------------------------------+
 | pointer to struct _scmval  next |  -      | pointer to struct _scmval  next |  ---->  SCMVAL_NIL 
 +---------------------------------+         +---------------------------------+


scmval is a pointer
-> casting to (intptr_t) is safe: used for pointer tagging, untagging, and pointer predicates

scmval is always tagged

*/

typedef struct _scmval *scmval;

struct _scmval {
  scmval data;
  scmval next;
};


/*

we need at least 8 byte alignment from malloc() and ... in order to tag a pointer with type information.
note that amd64 pointer are only 48 bit wide and sign extend from bit 47.

  66665555555555444444444433333333332222222222111111111100000 00 000
  32109876543210987654321098765432109876543210987654321098765 43 210
 +-----------------------------------------------------------+--+---+
 | 59bit signed integer                                      |00|000|
 +-----------------------------------------------------------+--+---+
 | nil                                                       |01|000|
 +-----------------------------------------------------------+--+---+
 | true                                                      |10|000|
 +-----------------------------------------------------------+--+---+
 | false                                                     |11|000|
 +-----------------------------------------------------------+--+---+
 | symbol, char *                                               |001|
 +-----------------------------------------------------------+--+---+
 | string, char *                                               |010|
 +-----------------------------------------------------------+--+---+
 | list, struct _scmval *                                       |011|
 +-----------------------------------------------------------+--+---+
 | currently unused                                             |100|
 +-----------------------------------------------------------+--+---+
 | currently unused                                             |101|
 +-----------------------------------------------------------+--+---+
 | currently unused                                             |110|
 +-----------------------------------------------------------+--+---+
 | SCMVAL_EOF                                                   |111|
 +-----------------------------------------------------------+--+---+


 */

/* type predicates */
#define SCMVAL_IS_INTEGER(x)    (((intptr_t)(x) & 0x1f) == 0x00)
#define SCMVAL_IS_NIL(x)        (((intptr_t)(x) & 0x1f) == 0x08)
#define SCMVAL_IS_TRUE(x)       (((intptr_t)(x) & 0x1f) == 0x10)
#define SCMVAL_IS_FALSE(x)      (((intptr_t)(x) & 0x1f) == 0x18)
#define SCMVAL_IS_SYMBOL(x)     (((intptr_t)(x) & 0x07) == 0x01)
#define SCMVAL_IS_STRING(x)     (((intptr_t)(x) & 0x07) == 0x02)
#define SCMVAL_IS_LIST(x)       (((intptr_t)(x) & 0x07) == 0x03)
#define SCMVAL_IS_EOF(x)        ((intptr_t)(x) == -1)

/* conversion to c native types */
#define SCMVAL_TO_C_INT(v)        ( (intptr_t)     (((intptr_t)(v) & ~0x07)>>5) )
#define SCMVAL_TO_C_STR(v)        ( (const char *) ((intptr_t)(v) & ~0x07) )
#define SCMVAL_TO_LIST(v)         ( (scmval)       ((intptr_t)(v) & ~0x07) )

/* tag and cast */
#define SCMVAL_MAKE_INTEGER(v)    ( (scmval) ((intptr_t)((v)<<5) | 0x00))
#define SCMVAL_MAKE_SYMBOL(v)     ( (scmval) ((intptr_t)(v) | 0x01))
#define SCMVAL_MAKE_STRING(v)     ( (scmval) ((intptr_t)(v) | 0x02))
#define SCMVAL_MAKE_LIST(v)       ( (scmval) ((intptr_t)(v) | 0x03))

#define SCMVAL_NIL                  (scmval)0x08
#define SCMVAL_TRUE                 (scmval)0x10
#define SCMVAL_FALSE                (scmval)0x18
#define SCMVAL_EOF                  (scmval)-1

// for 59bit integers the max integer is represented by the lowest 58 bits set
// ranges from +288230376151711743 to -288230376151711744
#define SCMVAL_INT_MAX          0x03ffffffffffffffL
#define SCMVAL_INT_MIN          (-SCMVAL_INT_MAX - 1L)

// allocate a cons cell
inline scmval
scmval_cons(scmval data, scmval next) {
  scmval pair = (scmval) scmmem_alloc(1, sizeof(struct _scmval));
  pair->data = data;
  pair->next = next;
  return pair;
}

#endif
