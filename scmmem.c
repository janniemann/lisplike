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

// MACOS: #include <sys/errno.h>   /* errno */

#include <errno.h>   /* errno */
#include <stdint.h>  /* SIZE_MAX */
#include <stdlib.h>      /* exit, malloc, realloc, free, NULL */
#include <string.h>      /* strlen */

#include "scmerr.h"      /* scmerr */
#include "scmmem.h"

/* heap memory */
extern void *scmmem_alloc(size_t nmemb, size_t size);
extern void *scmmem_realloc(void *ptr, size_t nmemb, size_t size);
extern char *scmmem_strdup(const char *s);
extern void scmmem_free(void **ptr);
