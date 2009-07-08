/* Copyright (c) 2008, 2009 Nicira Networks, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, Nicira Networks gives permission
 * to link the code of its release of vswitchd with the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the same
 * license as the "OpenSSL" library), and distribute the linked
 * executables.  You must obey the GNU General Public License in all
 * respects for all of the code used other than "OpenSSL".  If you modify
 * this file, you may extend this exception to your version of the file,
 * but you are not obligated to do so.  If you do not wish to do so,
 * delete this exception statement from your version.
 *
 */

#ifndef TTY_H
#define TTY_H 1

#include <termios.h>

int tty_lock(const char *dev_name);
int tty_set_raw_mode(int fd, speed_t);
int tty_open_master_pty(void);
int tty_fork_child(int master_fd, char *argv[]);
int tty_set_window_size(int fd, int n_rows, int n_columns);

#endif /* tty.h */
