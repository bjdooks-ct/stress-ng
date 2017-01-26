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
 *  stress_zero
 *	stress reading of /dev/zero
 */
int stress_zero(args_t *args)
{
	int fd;
	const size_t page_size = stress_get_pagesize();

	if ((fd = open("/dev/zero", O_RDONLY)) < 0) {
		pr_fail_err(args->name, "open");
		return EXIT_FAILURE;
	}

	do {
		char buffer[page_size];
		ssize_t ret;
#if defined(__linux__)
		int32_t *ptr;
#endif

		ret = read(fd, buffer, sizeof(buffer));
		if (ret < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				continue;
			pr_fail_err(args->name, "read");
			(void)close(fd);
			return EXIT_FAILURE;
		}

#if defined(__linux__)
		/*
		 *  check if we can mmap /dev/zero
		 */
		ptr = mmap(NULL, page_size, PROT_READ, MAP_SHARED | MAP_ANONYMOUS,
			fd, page_size * mwc16());
		if (ptr == MAP_FAILED) {
			if (errno == ENOMEM)
				continue;
			pr_fail_err(args->name, "mmap /dev/zero");
			(void)close(fd);
			return EXIT_FAILURE;
		}
		/* Quick sanity check if first 32 bits are zero */
		if (*ptr != 0) {
			pr_fail_err(args->name, "mmap'd /dev/zero not null");
			(void)munmap(ptr, page_size);
			(void)close(fd);
			return EXIT_FAILURE;
		}
		(void)munmap(ptr, page_size);
#endif
		inc_counter(args);
	} while (opt_do_run && (!args->max_ops || *args->counter < args->max_ops));
	(void)close(fd);

	return EXIT_SUCCESS;
}
