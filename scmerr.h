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

#ifndef _SCMERR_H
#define _SCMERR_H

enum scmerr_no {
  SCMERR_SYSCALL = 0,
  SCMERR_UNDEFINED,
  SCMERR_BAD_ENCODING,
  SCMERR_OVERFLOW,
  SCMERR_UNKNOWN_TYPE,
  SCMERR_UNKNOWN_ESCAPE,
  SCMERR_INTERNAL_READER,
  SCMERR_PREMATURE_EOF,
};

/* prints an error message and exits with failure */
_Noreturn void scmerr(enum scmerr_no, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

#endif
