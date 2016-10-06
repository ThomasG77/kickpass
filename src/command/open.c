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

#include <sys/stat.h>
#include <sys/queue.h>

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "kickpass.h"

#include "command.h"
#include "open.h"
#include "prompt.h"
#include "safe.h"
#include "log.h"
#include "kpagent.h"

static kp_error_t open_safe(struct kp_ctx *ctx, int argc, char **argv);

struct kp_cmd kp_cmd_open = {
	.main  = open_safe,
	.usage = NULL,
	.opts  = "open <safe>",
	.desc  = "Open a password safe and load it in kickpass agent",
	/* TODO agent = true */
};

kp_error_t
open_safe(struct kp_ctx *ctx, int argc, char **argv)
{
	kp_error_t ret;
	struct kp_safe safe;
	struct kp_unsafe unsafe;

	if (argc - optind != 1) {
		ret = KP_EINPUT;
		kp_warn(ret, "missing safe name");
		return ret;
	}

	if (!ctx->agent.connected) {
		ret = KP_EINPUT;
		kp_warn(ret, "not connected to any agent");
		return ret;
	}

	if ((ret = kp_safe_load(ctx, &safe, argv[optind])) != KP_SUCCESS) {
		return ret;
	}

	if ((ret = kp_safe_open(ctx, &safe)) != KP_SUCCESS) {
		return ret;
	}

	unsafe.timeout = 1000; /* TODO argv */
	if ((ret = kp_safe_get_path(ctx, &safe, unsafe.path, PATH_MAX)) != KP_SUCCESS) {
		return ret;
	}
	if (strlcpy(unsafe.password, safe.password, KP_PASSWORD_MAX_LEN) >= KP_PASSWORD_MAX_LEN) {
		errno = ENOMEM;
		return KP_ERRNO;
	}
	if (strlcpy(unsafe.metadata, safe.metadata, KP_METADATA_MAX_LEN) >= KP_METADATA_MAX_LEN) {
		errno = ENOMEM;
		return KP_ERRNO;
	}

	kp_agent_send(&ctx->agent, KP_MSG_STORE, &unsafe, sizeof(struct kp_unsafe));

	if ((ret = kp_safe_close(ctx, &safe)) != KP_SUCCESS) {
		kp_warn(ret, "cannot cleanly close safe"
			"clear text password might have leaked");
		return ret;
	}

	return KP_SUCCESS;
}
