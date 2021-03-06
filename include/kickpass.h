/*
 * Copyright (c) 2015 Paul Fariello <paul@fariello.eu>
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

#ifndef KP_KICKPASS_H
#define KP_KICKPASS_H

#include <stddef.h>
#include <limits.h>

#include "error.h"
#include "kickpass_config.h"

#define KP_PASSWORD_MAX_LEN 4096

struct kp_ctx {
	char ws_path[PATH_MAX];
	char * const password;
	struct {
		unsigned long long opslimit;
		size_t memlimit;
	} cfg;
};

kp_error_t kp_init(struct kp_ctx *);
kp_error_t kp_load(struct kp_ctx *);
kp_error_t kp_fini(struct kp_ctx *);
kp_error_t kp_init_workspace(struct kp_ctx *);

#endif /* KP_KICKPASS_H */
