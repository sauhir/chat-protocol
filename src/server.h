/* Chat server
 *
 * Copyright (C) 2018 Sauli Hirvi
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SERVER_H
#define SERVER_H

enum msg_type {
    MSG_ERROR = -1,
    MSG_NORMAL = 1,
    MSG_COMMAND = 2,
    MSG_HANDSHAKE = 4
};

enum cmd_type { CMD_READ = 1, CMD_WRITE = 2, CMD_CLEAR = 4 };

#endif
