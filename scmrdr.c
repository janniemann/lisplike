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
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>      /* exit, malloc, realloc, free, NULL */
#include <stdint.h> /**/
#include <string.h>


#include "scmerr.h"
#include "scmmem.h"
#include "scmval.h"
#include "scmspl.h"
#include "scmprt.h"
#include "scmrdr.h"

// Implementation limits:
#define _SCMRDR_BUFFERSIZE 256



// types of byte reader
enum _scmrdr_type {
  SCMRDR_TYPE_BUFFER,
  SCMRDR_TYPE_FILE,
  SCMRDR_TYPE_STDIN
};


// may be returned by byte_reader_read or byte_reader_peek
#define SCMRDR_EOF -1

// if peek has this value, a real read must be performed
#define SCMRDR_PEEK_INVALID -2




struct _scmrdr {
  enum _scmrdr_type type;
  char *name;
  int peek;
  int line;
  int pos;
  FILE *stream;
};


// initialize a reader from a buffer in memory
scmrdr *
scmrdr_open_buffer(char *buffer, size_t size)
{
  scmrdr *rdr = (scmrdr *) scmmem_alloc(1, sizeof(struct _scmrdr));
  rdr->type = SCMRDR_TYPE_BUFFER;
  // 8 Bytes + 18 ('0x' + 16 nibbles buffer) + 20 
  rdr->name = (char *) scmmem_alloc(50, sizeof(char));
  snprintf(rdr->name, 50, "<mem:%p:%zu>", buffer, size);
  rdr->peek = SCMRDR_PEEK_INVALID;
  rdr->line = 1;
  rdr->pos = 0;
  if (NULL == (rdr->stream = fmemopen(buffer, size, "r"))) {
    scmerr(SCMERR_SYSCALL, "scmrdr_open_buffer(\"%p\", \"%zu\")", buffer, size);
  }
  return rdr;
}

// initialize a reader from a file
scmrdr *
scmrdr_open_file(char *file)
{
  scmrdr *rdr = (scmrdr *) scmmem_alloc(1, sizeof(struct _scmrdr));
  rdr->type = SCMRDR_TYPE_FILE;
  rdr->name = scmmem_strdup(file);
  /*  if (-1 == asprintf(&rdr->name, "%s", file)) {
    scmerr(SCMERR_SYSCALL, "asprintf");
    }*/
  rdr->peek = SCMRDR_PEEK_INVALID;
  rdr->line = 1;
  rdr->pos = 0;
  if (NULL == (rdr->stream = fopen(file, "r"))) {
    scmerr(SCMERR_SYSCALL, "scmrdr_open_file(\"%s\", \"r\")", file);
  }
  return rdr;
}

// initialize a reader from standard input
scmrdr *
scmrdr_open_stdin(void)
{
  scmrdr *rdr = (scmrdr *) scmmem_alloc(1, sizeof(struct _scmrdr));
  rdr->type = SCMRDR_TYPE_STDIN;
  rdr->name = "<stdin>";
  /*  if (-1 == asprintf(&rdr->name, "<stdin>")) {
    scmerr(SCMERR_SYSCALL, "asprintf");
    }*/
  rdr->peek = SCMRDR_PEEK_INVALID;
  rdr->line = 1;
  rdr->pos = 0;
  rdr->stream = stdin;
  return rdr;
}

// destroy the reader
void
scmrdr_close(scmrdr *rdr)
{

  switch (rdr->type) {
  case SCMRDR_TYPE_STDIN:
    break;
  case SCMRDR_TYPE_FILE:
  case SCMRDR_TYPE_BUFFER:
    if (fclose(rdr->stream)) {
      scmerr(SCMERR_SYSCALL, "fclose(\"%s\")", rdr->name);
    }
    scmmem_free((void **) &(rdr->name));
    break;
  }

  scmmem_free((void **) &rdr);
}




static int _scmrdr_getc(scmrdr *rdr);
static int _scmrdr_read(scmrdr *rdr);
static int _scmrdr_peek(scmrdr *rdr);


static int
_scmrdr_getc(scmrdr *rdr)
{
  int c;

  c = getc(rdr->stream);
  if ((EOF == c) && (ferror(rdr->stream))) {
    scmerr(SCMERR_SYSCALL, "%s", rdr->name);
  }

  return c;
}


// read from reader
static int
_scmrdr_read(scmrdr *rdr)
{
  int c;

  // if not peeked already, read from stream, else return peek
  if (SCMRDR_PEEK_INVALID == rdr->peek) { 
    c = _scmrdr_getc(rdr);
  } else {
    c = rdr->peek;
    rdr->peek = SCMRDR_PEEK_INVALID;
  }

  // advance position and line
  switch (c) {
  case '\n':
    rdr->line++;
    rdr->pos = 0;
    break;
  case '\t':
    rdr->pos = rdr->pos + 8 - (rdr->pos % 8);
    break;
  case EOF:
    break;
  default:
    rdr->pos++;
  }

  return c;
}


// peek at the next byte (or -1 for EOF) from the byte_reader
static int
_scmrdr_peek(scmrdr *rdr)
{
  if (SCMRDR_PEEK_INVALID == rdr->peek) {
    rdr->peek = _scmrdr_getc(rdr);
  }
  return rdr->peek;
}



static scmval _scmrdr_read_integer(scmrdr *rdr, int stash);
static scmval _scmrdr_read_string(scmrdr *rdr);
static scmval _scmrdr_read_symbol(scmrdr *rdr, int stash);
static scmval _scmrdr_read_list(scmrdr *rdr);


/*
 * read an integer.
 *
 *
 */
static scmval
_scmrdr_read_integer(scmrdr *rdr, int stash)
{
  int start_pos = rdr->pos;
  intptr_t num = 0;
  int peek;
  int is_negative = 0;
  
  // adjust position if stash, derive sign from stash
  switch (stash) {
  case '-':
    is_negative = 1;
    /* FALLTHROUGH */
  case '+':
    start_pos--;
    break;
  case EOF:
    stash='+';
    break;
  default:
    scmerr(SCMERR_INTERNAL_READER, "%s:%i:%s", __FILE__, __LINE__, __func__);
  }

  peek = _scmrdr_peek(rdr);
  while (('0' <= peek) && (peek <= '9')) {
    num = (10*num) + peek - '0';
    if (((!is_negative) && (num > SCMVAL_INT_MAX)) ||
	((is_negative) && (-num < SCMVAL_INT_MIN))) {
      scmerr(SCMERR_OVERFLOW, "%s:%i:%i", rdr->name, rdr->line, start_pos);
    }
    (void) _scmrdr_read(rdr);
    peek = _scmrdr_peek(rdr);
  }

  if (is_negative) {
    num = -num;
  }

  //  fprintf(stderr, "%li\n", num);

  return SCMVAL_MAKE_INTEGER(num);
}


static scmval
_scmrdr_read_string(scmrdr *rdr)
{
  int peek;
  char buffer[_SCMRDR_BUFFERSIZE];
  int idx = 0;

  // XXX buffer overflow
  peek = _scmrdr_peek(rdr);
  while (peek != '"') {
    if (EOF == peek) {
      scmerr(SCMERR_PREMATURE_EOF, "%s:%i:%i", rdr->name, rdr->line, rdr->pos);
    }
    if ('\\' == peek) {
      // escape sequences: \\ \" \n \t
      _scmrdr_read(rdr);
      peek = _scmrdr_peek(rdr);
      switch (peek) {
      case '\\':
	buffer[idx++] = '\\';
	_scmrdr_read(rdr);
	break;
      case 'n':
	buffer[idx++] = '\n';
	_scmrdr_read(rdr);
	break;
      case 't':
	buffer[idx++] = '\t';
	_scmrdr_read(rdr);
	break;
      case '"':
	buffer[idx++] = '"';
	_scmrdr_read(rdr);
	break;
      default:
	scmerr(SCMERR_UNKNOWN_ESCAPE, "%s:%i:%i", rdr->name, rdr->line, rdr->pos);
      }
    } else {
      buffer[idx++] = peek;
      _scmrdr_read(rdr);
    }
    peek = _scmrdr_peek(rdr);
  }

  // skip over last " and zero terminate buffer
  _scmrdr_read(rdr);
  buffer[idx++] = '\0';

  return scmspl_intern_string(buffer);
}



static scmval
_scmrdr_read_symbol(scmrdr *rdr, int stashed)
{
  int peek;
  char buffer[_SCMRDR_BUFFERSIZE];
  int idx = 0;

  if (EOF != stashed) {
    buffer[idx++] = stashed;
  }

  peek = _scmrdr_peek(rdr);

  // XXX buffer overflow
  while (!isspace(peek) && ('(' != peek) && (')' != peek) && (EOF != peek)) {
    buffer[idx++] = peek;
    (void) _scmrdr_read(rdr);
    peek = _scmrdr_peek(rdr);
  }
  buffer[idx++] = '\0';

  return scmspl_intern_symbol(buffer);
}





/* superheftig general ... */
static scmval
_scmrdr_read_list(scmrdr *rdr)
{
  int c;
  struct _scmval *v_start = SCMVAL_NIL;
  struct _scmval *v_cur = NULL;
  scmval v_tmp = NULL; 

  for (;;) {
    while (isspace(_scmrdr_peek(rdr))) {
      (void) _scmrdr_read(rdr);
    }
    c = _scmrdr_peek(rdr);
    if (-1 == c) {
      // XXX abort
      abort();
    }
    if (')' == c) {
      (void) _scmrdr_read(rdr);
      if (SCMVAL_NIL == v_start)
	return SCMVAL_NIL;
      else
	break;
    } else {
      v_tmp = scmval_cons(scmrdr_read(rdr), SCMVAL_NIL);
      if (SCMVAL_NIL == v_start) {
	v_start = v_tmp;
	v_cur = v_start;
      } else {
	v_cur->next = SCMVAL_MAKE_LIST(v_tmp);
	v_cur = v_tmp;
      }
    }
  }

  return SCMVAL_MAKE_LIST(v_start);
}


scmval
scmrdr_read(scmrdr *rdr)
{
  int c;

  while (isspace(_scmrdr_peek(rdr))) {
    (void) _scmrdr_read(rdr);
  }

  c = _scmrdr_peek(rdr);

  if (EOF == c) {
    return SCMVAL_EOF;
  }
  if ('+' == c) {
    (void) _scmrdr_read(rdr);
    c = _scmrdr_peek(rdr);
    if (('0' <= c) && ('9' >= c)) {
      return _scmrdr_read_integer(rdr, '+');
    } else {
      return _scmrdr_read_symbol(rdr, '+');
    }
  }
  if ('-' == c) {
    (void) _scmrdr_read(rdr);
    c = _scmrdr_peek(rdr);
    if (('0' <= c) && ('9' >= c)) {
      return _scmrdr_read_integer(rdr, '-');
    } else {
      return _scmrdr_read_symbol(rdr, '-');
    }
  }
  if (('0' <= c) && ('9' >= c)) {
    return _scmrdr_read_integer(rdr, -1);
  }
  if ('"' == c) {
    (void) _scmrdr_read(rdr);
    return _scmrdr_read_string(rdr);
  }
  if ('(' == c) {
    (void) _scmrdr_read(rdr);
    return _scmrdr_read_list(rdr);
  }
  // XXX was, wenn ')'

  return _scmrdr_read_symbol(rdr, EOF);
}
