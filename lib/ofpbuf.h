/*
 * Copyright (c) 2008, 2009 Nicira Networks.
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

#ifndef OFPBUF_H
#define OFPBUF_H 1

#include <stddef.h>

/* Buffer for holding arbitrary data.  An ofpbuf is automatically reallocated
 * as necessary if it grows too large for the available memory. */
struct ofpbuf {
    void *base;                 /* First byte of area malloc()'d area. */
    size_t allocated;           /* Number of bytes allocated. */

    void *data;                 /* First byte actually in use. */
    size_t size;                /* Number of bytes in use. */

    void *l2;                   /* Link-level header. */
    void *l3;                   /* Network-level header. */
    void *l4;                   /* Transport-level header. */
    void *l7;                   /* Application data. */

    struct ofpbuf *next;        /* Next in a list of ofpbufs. */
    void *private;              /* Private pointer for use by owner. */
};

void ofpbuf_use(struct ofpbuf *, void *, size_t);

void ofpbuf_init(struct ofpbuf *, size_t);
void ofpbuf_uninit(struct ofpbuf *);
void ofpbuf_reinit(struct ofpbuf *, size_t);

struct ofpbuf *ofpbuf_new(size_t);
struct ofpbuf *ofpbuf_clone(const struct ofpbuf *);
struct ofpbuf *ofpbuf_clone_data(const void *, size_t);
void ofpbuf_delete(struct ofpbuf *);

void *ofpbuf_at(const struct ofpbuf *, size_t offset, size_t size);
void *ofpbuf_at_assert(const struct ofpbuf *, size_t offset, size_t size);
void *ofpbuf_tail(const struct ofpbuf *);
void *ofpbuf_end(const struct ofpbuf *);

void *ofpbuf_put_uninit(struct ofpbuf *, size_t);
void *ofpbuf_put_zeros(struct ofpbuf *, size_t);
void *ofpbuf_put(struct ofpbuf *, const void *, size_t);
void ofpbuf_reserve(struct ofpbuf *, size_t);
void *ofpbuf_push_uninit(struct ofpbuf *b, size_t);
void *ofpbuf_push(struct ofpbuf *b, const void *, size_t);

size_t ofpbuf_headroom(struct ofpbuf *);
size_t ofpbuf_tailroom(struct ofpbuf *);
void ofpbuf_prealloc_headroom(struct ofpbuf *, size_t);
void ofpbuf_prealloc_tailroom(struct ofpbuf *, size_t);
void ofpbuf_trim(struct ofpbuf *);

void ofpbuf_clear(struct ofpbuf *);
void *ofpbuf_pull(struct ofpbuf *, size_t);
void *ofpbuf_try_pull(struct ofpbuf *, size_t);

#endif /* ofpbuf.h */
