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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scmerr.h"
#include "scmmem.h"
#include "scmval.h"
#include "scmrdr.h"
#include "scmprt.h"
#include "scmevl.h"

#ifdef NO_EVAL
#define scmevl(v) (v)
#endif

/* static prototypes */
static _Noreturn void usage(void);
static void repl(scmrdr *rdr);

static _Noreturn void
usage(void)
{
  fputs("synopsis:\n"
	"  scm [ - | -c form | file ] ...\n"
	"\n"
	"    -          reads from standard input.\n"
	"    -c form    reads from the string form.\n"
	"    file       reads from the file.\n"
	"\n", stderr);

  exit(EXIT_FAILURE);
}

static void
repl(scmrdr *rdr)
{
  scmval v;

  for(;;) {
    v = scmevl(scmrdr_read(rdr));
    if (SCMVAL_EOF == v) {
      break;
    }
    scmprt_print(v);
  }
}

int
main(int argc, char **argv)
{
  int i;
  scmrdr *rdr;

  if (1 == argc) {
    usage();
  }

  for (i=1; i<argc; i++) {
    if (!strcmp("-", argv[i])) {
      rdr = scmrdr_open_stdin();
    } else if (!strcmp("-c", argv[i])) {
      i++;
      if (i == argc) {
	usage();
      }
      rdr = scmrdr_open_buffer(argv[i], strlen(argv[i]) + 1);
    } else {
      rdr = scmrdr_open_file(argv[i]);
    }
    repl(rdr);
 
    scmrdr_close(rdr);
  }
}
