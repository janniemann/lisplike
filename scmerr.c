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

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scmerr.h"

/* this is the first part of the one line error message. */
static const char *const scmerr_list[] = {
  "error-000: ",                            /* SCMERR_SYSCALL */
  "error-001: undefined error: ",           /* SCMERR_UNDEFINED */
  "error-002: bad encoding: ",              /* SCMERR_BAD_ENCODING */
  "error-003: integer literal overflow: ",  /* SCMERR_OVERFLOW */
  "error-004: unknown type: ",              /* SCMERR_UNKNOWN_TYPE */
  "error-005: unknown escape sequence: ",   /* SCMERR_UNKNOWN_ESCAPE */
  "error-006: internal reader error: ",     /* SCMERR_INTERNAL_READER */
  "error-007: premature end-of-file: ",     /* SCMERR_PREMATURE_EOF */
};

_Noreturn void
scmerr(enum scmerr_no no, const char *fmt, ...)
{
  const char *errstr;
  va_list ap;

  /* SCMERR_SYSCALL uses errno */
  if (no == SCMERR_SYSCALL) {
    errstr = (const char *)strerror(errno);
    (void)fputs(scmerr_list[no], stderr);
    (void)fputs(errstr, stderr);
    (void)fputs(": ", stderr);
  } else {
    (void)fputs(scmerr_list[no], stderr);
  }

  /* formatted output, if any */
  if (fmt != NULL) {
    va_start(ap, fmt);
    (void)vfprintf(stderr, fmt, ap);
    va_end(ap);
  }

  /* line break */
  (void)fputc('\n', stderr);

  /* exit failure */
  exit(EXIT_FAILURE);
}
