/*
 * Copyright (c) 2008 Nicira Networks.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <config.h>
#include "dynamic-string.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "timeval.h"
#include "util.h"

void
ds_init(struct ds *ds)
{
    ds->string = NULL;
    ds->length = 0;
    ds->allocated = 0;
}

void
ds_clear(struct ds *ds) 
{
    ds->length = 0;
}

void
ds_truncate(struct ds *ds, size_t new_length)
{
    if (ds->length > new_length) {
        ds->length = new_length;
        ds->string[new_length] = '\0';
    }
}

void
ds_reserve(struct ds *ds, size_t min_length)
{
    if (min_length > ds->allocated || !ds->string) {
        ds->allocated += MAX(min_length, ds->allocated);
        ds->allocated = MAX(8, ds->allocated);
        ds->string = xrealloc(ds->string, ds->allocated + 1);
    }
}

char *
ds_put_uninit(struct ds *ds, size_t n)
{
    ds_reserve(ds, ds->length + n);
    ds->length += n;
    ds->string[ds->length] = '\0';
    return &ds->string[ds->length - n];
}

void
ds_put_char(struct ds *ds, char c)
{
    *ds_put_uninit(ds, 1) = c;
}

void
ds_put_char_multiple(struct ds *ds, char c, size_t n)
{
    memset(ds_put_uninit(ds, n), c, n);
}

void
ds_put_buffer(struct ds *ds, const char *s, size_t n)
{
    memcpy(ds_put_uninit(ds, n), s, n);
}

void
ds_put_cstr(struct ds *ds, const char *s)
{
    size_t s_len = strlen(s);
    memcpy(ds_put_uninit(ds, s_len), s, s_len);
}

void
ds_put_format(struct ds *ds, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    ds_put_format_valist(ds, format, args);
    va_end(args);
}

void
ds_put_format_valist(struct ds *ds, const char *format, va_list args_)
{
    va_list args;
    size_t available;
    int needed;

    va_copy(args, args_);
    available = ds->string ? ds->allocated - ds->length + 1 : 0;
    needed = vsnprintf(&ds->string[ds->length], available, format, args);
    va_end(args);

    if (needed < available) {
        ds->length += needed;
    } else {
        size_t available;

        ds_reserve(ds, ds->length + needed);

        va_copy(args, args_);
        available = ds->allocated - ds->length + 1;
        needed = vsnprintf(&ds->string[ds->length], available, format, args);
        va_end(args);

        assert(needed < available);
        ds->length += needed;
    }
}

void
ds_put_printable(struct ds *ds, const char *s, size_t n) 
{
    ds_reserve(ds, ds->length + n);
    while (n-- > 0) {
        unsigned char c = *s++;
        if (c < 0x20 || c > 0x7e || c == '\\' || c == '"') {
            ds_put_format(ds, "\\%03o", (int) c);
        } else {
            ds_put_char(ds, c);
        }
    }
}

void
ds_put_strftime(struct ds *ds, const char *template, const struct tm *tm)
{
    if (!tm) {
        time_t now = time_now();
        tm = localtime(&now);
    }
    for (;;) {
        size_t avail = ds->string ? ds->allocated - ds->length + 1 : 0;
        size_t used = strftime(&ds->string[ds->length], avail, template, tm);
        if (used) {
            ds->length += used;
            return;
        }
        ds_reserve(ds, ds->length + (avail < 32 ? 64 : 2 * avail)); 
    }
}

int
ds_get_line(struct ds *ds, FILE *file)
{
    ds_clear(ds);
    for (;;) {
        int c = getc(file);
        if (c == EOF) {
            return ds->length ? 0 : EOF;
        } else if (c == '\n') {
            return 0;
        } else {
            ds_put_char(ds, c);
        }
    }
}

char *
ds_cstr(struct ds *ds)
{
    if (!ds->string) {
        ds_reserve(ds, 0);
    }
    ds->string[ds->length] = '\0';
    return ds->string;
}

void
ds_destroy(struct ds *ds)
{
    free(ds->string);
}

/* Writes the 'size' bytes in 'buf' to 'string' as hex bytes arranged 16 per
 * line.  Numeric offsets are also included, starting at 'ofs' for the first
 * byte in 'buf'.  If 'ascii' is true then the corresponding ASCII characters
 * are also rendered alongside. */
void
ds_put_hex_dump(struct ds *ds, const void *buf_, size_t size,
                uintptr_t ofs, bool ascii)
{
  const uint8_t *buf = buf_;
  const size_t per_line = 16; /* Maximum bytes per line. */

  while (size > 0)
    {
      size_t start, end, n;
      size_t i;

      /* Number of bytes on this line. */
      start = ofs % per_line;
      end = per_line;
      if (end - start > size)
        end = start + size;
      n = end - start;

      /* Print line. */
      ds_put_format(ds, "%08jx  ", (uintmax_t) ROUND_DOWN(ofs, per_line));
      for (i = 0; i < start; i++)
        ds_put_format(ds, "   ");
      for (; i < end; i++)
        ds_put_format(ds, "%02hhx%c",
                buf[i - start], i == per_line / 2 - 1? '-' : ' ');
      if (ascii)
        {
          for (; i < per_line; i++)
            ds_put_format(ds, "   ");
          ds_put_format(ds, "|");
          for (i = 0; i < start; i++)
            ds_put_format(ds, " ");
          for (; i < end; i++) {
              int c = buf[i - start];
              ds_put_char(ds, c >= 32 && c < 127 ? c : '.');
          }
          for (; i < per_line; i++)
            ds_put_format(ds, " ");
          ds_put_format(ds, "|");
        }
      ds_put_format(ds, "\n");

      ofs += n;
      buf += n;
      size -= n;
    }
}

int
ds_last(const struct ds *ds)
{
    return ds->length > 0 ? (unsigned char) ds->string[ds->length - 1] : EOF;
}

void
ds_chomp(struct ds *ds, int c)
{
    if (ds->length > 0 && ds->string[ds->length - 1] == (char) c) {
        ds->string[--ds->length] = '\0';
    }
}
