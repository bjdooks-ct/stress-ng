/*
 * Copyright (C) 2013-2017 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * This code is a complete clean re-write of the stress tool by
 * Colin Ian King <colin.king@canonical.com> and attempts to be
 * backwardly compatible with the stress tool by Amos Waterland
 * <apw@rossby.metr.ou.edu> but has more stress tests and more
 * functionality.
 *
 */
#include "stress-ng.h"

/*
 *  stress on sched_kill()
 *	stress system by rapid kills
 */
int stress_kill(args_t *args)
{
	const pid_t pid = getpid();

	if (stress_sighandler(args->name, SIGUSR1, SIG_IGN, NULL) < 0)
		return EXIT_FAILURE;

	do {
		int ret;

		ret = kill(pid, SIGUSR1);
		if ((ret < 0) && (opt_flags & OPT_FLAGS_VERIFY))
			pr_fail_err(args->name, "kill");

		/* Zero signal can be used to see if process exists */
		ret = kill(pid, 0);
		if ((ret < 0) && (opt_flags & OPT_FLAGS_VERIFY))
			pr_fail_err(args->name, "kill");

		/*
		 * Zero signal can be used to see if process exists,
		 * -1 pid means signal sent to every process caller has
		 * permission to send to
		 */
		ret = kill(-1, 0);
		if ((ret < 0) && (opt_flags & OPT_FLAGS_VERIFY))
			pr_fail_err(args->name, "kill");

		inc_counter(args);
	} while (opt_do_run && (!args->max_ops || *args->counter < args->max_ops));

	return EXIT_SUCCESS;
}
