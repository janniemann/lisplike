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

#ifndef _SCMMEM_H
#define _SCMMEM_H

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define _SCMMEM_MUL_NO_OVERFLOW ((size_t)1 << (sizeof(size_t) * 4))

inline void *
scmmem_alloc(size_t nmemb, size_t size)
{
  void *p;

  if ((nmemb >= _SCMMEM_MUL_NO_OVERFLOW || size >= _SCMMEM_MUL_NO_OVERFLOW) &&
      nmemb > 0 && SIZE_MAX / nmemb < size) {
    errno = ENOMEM;
    scmerr(SCMERR_SYSCALL, "scmmem_alloc(%zu, %zu)", nmemb, size);
  }

  if (NULL == (p = malloc(nmemb * size))) {
    scmerr(SCMERR_SYSCALL, "scmmem_alloc(%zu, %zu)", nmemb, size);
  }

  return p;
}

inline void *
scmmem_realloc(void *ptr, size_t nmemb, size_t size)
{
  void *nptr;

  if ((nmemb >= _SCMMEM_MUL_NO_OVERFLOW || size >= _SCMMEM_MUL_NO_OVERFLOW) &&
      nmemb > 0 && SIZE_MAX / nmemb < size) {
    errno = ENOMEM;
    scmerr(SCMERR_SYSCALL, "scmmem_realloc(%p, %zu, %zu)", ptr, nmemb, size);
  }

  if (NULL == (nptr = realloc(ptr, nmemb * size))) {
    scmerr(SCMERR_SYSCALL, "scmmem_realloc(%p, %zu, %zu)", ptr, nmemb, size);
  }

  return nptr;
}

inline char *
scmmem_strdup(const char *s)
{
  char *new_s;
  size_t len = strlen(s) + 1;

  new_s = scmmem_alloc(1, len);
  (void)memcpy(new_s, s, len);

  return new_s;
}


inline void
scmmem_free(void **ptr)
{
  if (NULL == *ptr) {
    scmerr(SCMERR_SYSCALL, "scmmem_free(%p) contains %p (NULL)", ptr, *ptr);
  }
  free(*ptr);
  *ptr = NULL;
}

#endif
