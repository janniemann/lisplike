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

#include <errno.h>   /* errno */
#include <stdlib.h>      /* exit, malloc, realloc, free, NULL */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "scmerr.h"
#include "scmmem.h"
#include "scmval.h"
#include "scmprt.h"

void
scmprt_print(scmval v)
{
  if (SCMVAL_IS_INTEGER(v)) {
    printf("%li\n", SCMVAL_TO_C_INT(v));
  }
  else if (SCMVAL_IS_NIL(v)) {
    printf("nil ()\n");
  }
  else if (SCMVAL_IS_TRUE(v)) {
    printf("true #t\n");
  }
  else if (SCMVAL_IS_FALSE(v)) {
    printf("false #f\n");
  }
  else if (SCMVAL_IS_STRING(v)) {
    printf("\"%s\"\n", SCMVAL_TO_C_STR(v));
  }
  else if (SCMVAL_IS_SYMBOL(v)) {
    printf("%s\n", SCMVAL_TO_C_STR(v));
  }
  else if (SCMVAL_IS_LIST(v)) {
    printf("(\n");
    for (; v != SCMVAL_NIL; v = SCMVAL_TO_LIST(v)->next) {
      scmprt_print(SCMVAL_TO_LIST(v)->data);
    }
    printf(")\n");
  }
  else if (SCMVAL_IS_EOF(v)) {
    ;
  }
  else {
    scmerr(SCMERR_UNKNOWN_TYPE, NULL);
  }
}
