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
#include <string.h>
#include <stdint.h>

#include "scmerr.h"      /* scmerr */
#include "scmmem.h"
#include "scmval.h"
#include "scmspl.h"


typedef struct string_pool {
  struct string_pool * next;
  char *cstr;
} string_pool;

static string_pool *sp = NULL;

static string_pool * _string_pool_cons(const char *cstr, string_pool *old_head);

string_pool *
_string_pool_cons(const char *cstr, string_pool *old_head) {
  string_pool *_pair = (string_pool *)scmmem_alloc(1, sizeof (string_pool));
  _pair->cstr = scmmem_strdup(cstr);
  _pair->next = old_head;
  return _pair;
}

scmval
scmspl_intern_string(const char *cstr)
{
  string_pool *_sp = sp;

  for (;_sp; _sp=_sp->next) {
    if (!strcmp(cstr, _sp->cstr)) {
      return SCMVAL_MAKE_STRING(_sp->cstr);
    }
  }

  sp = _string_pool_cons(cstr, sp);
  return SCMVAL_MAKE_STRING(sp->cstr);
}



scmval
scmspl_intern_symbol(const char *cstr)
{
  string_pool *_sp = sp;

  if (!strcmp("false", cstr))
    return SCMVAL_FALSE;
  if (!strcmp("true", cstr))
    return SCMVAL_TRUE;
  if (!strcmp("nil", cstr))
    return SCMVAL_NIL;

  for (;_sp; _sp=_sp->next) {
    if (!strcmp(cstr, _sp->cstr)) {
      return SCMVAL_MAKE_SYMBOL(_sp->cstr);
    }
  }

  sp = _string_pool_cons(cstr, sp);
  return SCMVAL_MAKE_SYMBOL(sp->cstr);
}
