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

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sodium.h>

#include "kickpass.h"

#include "command.h"
#include "edit.h"
#include "editor.h"
#include "prompt.h"
#include "safe.h"
#include "storage.h"

static kp_error_t edit(struct kp_ctx *ctx, int argc, char **argv);
static kp_error_t parse_opt(struct kp_ctx *, int, char **);
static kp_error_t edit_password(struct kp_safe *);
static kp_error_t confirm_empty_password(bool *);
static void usage(void);

struct kp_cmd kp_cmd_edit = {
	.main  = edit,
	.usage = usage,
	.opts  = "edit [-pm] <safe>",
	.desc  = "Edit a password safe with $EDIT",
};

static bool password = false;
static bool metadata = false;

kp_error_t
edit(struct kp_ctx *ctx, int argc, char **argv)
{
	kp_error_t ret = KP_SUCCESS;
	struct kp_safe safe;

	if ((ret = parse_opt(ctx, argc, argv)) != KP_SUCCESS) {
		return ret;
	}

	if (argc - optind != 1) {
		warnx("missing safe name");
		return KP_EINPUT;
	}

	if ((ret = kp_safe_load(ctx, &safe, argv[optind])) != KP_SUCCESS) {
		return ret;
	}

	if ((ret = kp_storage_open(ctx, &safe)) != KP_SUCCESS) {
		return ret;
	}

	if (password) {
		if ((ret = edit_password(&safe)) != KP_SUCCESS) {
			return ret;
		}
	}

	if (metadata) {
		if ((ret = kp_edit(ctx, &safe)) != KP_SUCCESS) {
			return ret;
		}
	}

	if ((ret = kp_storage_save(ctx, &safe)) != KP_SUCCESS) {
		return ret;
	}

	if ((ret = kp_safe_close(ctx, &safe)) != KP_SUCCESS) {
		return ret;
	}

	return KP_SUCCESS;
}

static kp_error_t
edit_password(struct kp_safe *safe)
{
	kp_error_t ret = KP_SUCCESS;
	char *password;
	size_t password_len;
	bool confirm = true;

	password = sodium_malloc(KP_PASSWORD_MAX_LEN+1);
	if ((ret = kp_prompt_password("safe", true, password)) != KP_SUCCESS) {
		goto out;
	}

	password_len = strlen(password);
	if (password_len == 0) {
		if ((ret = confirm_empty_password(&confirm)) != KP_SUCCESS) {
			sodium_free(password);
			return ret;
		}
	}

	if (confirm) {
		memcpy(safe->password, password, password_len);
		safe->password[password_len] = '\0';
		safe->password_len = password_len;
	}

out:
	sodium_free(password);
	return ret;
}

static kp_error_t
confirm_empty_password(bool *confirm)
{
	char *prompt = "Empty password. Do you really want to update password ? (y/n) [n] ";
	char *response;
	size_t response_len;
	FILE *tty;

	*confirm = false;

	tty = fopen("/dev/tty", "r+");
	if (!tty) {
		warn("cannot access /dev/tty");
		return KP_EINPUT;
	}

	fprintf(tty, "%s", prompt);
	fflush(tty);
	response = fgetln(stdin, &response_len);

	if (response[0] == 'y') *confirm = true;

	fclose(tty);
	return KP_SUCCESS;
}

static kp_error_t
parse_opt(struct kp_ctx *ctx, int argc, char **argv)
{
	int opt;
	static struct option longopts[] = {
		{ "password", no_argument, NULL, 'p' },
		{ "metadata", no_argument, NULL, 'm' },
		{ NULL,       0,           NULL, 0   },
	};

	while ((opt = getopt_long(argc, argv, "pm", longopts, NULL)) != -1) {
		switch (opt) {
		case 'p':
			password = true;
			break;
		case 'm':
			metadata = true;
			break;
		default:
			warnx("unknown option %c", opt);
			return KP_EINPUT;
		}
	}

	if (password && metadata) {
		warnx("Editing both password and metadata is default behavior. You can ommit options.");
	}

	/* Default edit all */
	if (!password && !metadata) {
		password = true;
		metadata = true;
	}

	return KP_SUCCESS;
}

void
usage(void)
{
	printf("options:\n");
	printf("    -p, --password     Edit only password\n");
	printf("    -m, --metadata     Edit only metadata\n");
}
