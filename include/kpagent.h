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

#ifndef KP_AGENT_H
#define KP_AGENT_H

#include <stdbool.h>
#include <limits.h>

#include "kickpass.h"

#define KP_AGENT_SOCKET_ENV "KP_AGENT_SOCK"

enum kp_agent_msg_type {
	KP_MSG_STORE,
};

struct kp_unsafe {
	time_t timeout; /* timeout of the safe */
	char path[PATH_MAX]; /* name of the safe */
	char password[KP_PASSWORD_MAX_LEN + 1]; /* plain text password (null terminated) */
	char metadata[KP_METADATA_MAX_LEN + 1]; /* plain text metadata (null terminated) */
};

struct kp_store {
	time_t timeout; /* timeout of the safe */
	char path[PATH_MAX]; /* name of the safe */
	char * const password;      /* plain text password (null terminated) */
	char * const metadata;      /* plain text metadata (null terminated) */
};

kp_error_t kp_agent_socket(char *, bool, int *);

#endif /* KP_AGENT_H */
