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

#ifndef IN_BAND_H
#define IN_BAND_H 1

#include "flow.h"

struct dpif;
struct in_band;
struct ofproto;
struct rconn;
struct secchan;
struct settings;
struct switch_status;

int in_band_create(struct ofproto *, struct dpif *, struct switch_status *,
                   struct rconn *controller, struct in_band **);
void in_band_destroy(struct in_band *);
void in_band_run(struct in_band *);
void in_band_wait(struct in_band *);
void in_band_flushed(struct in_band *);

#endif /* in-band.h */
