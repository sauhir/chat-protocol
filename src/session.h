/* Chat session structure and related functionality.
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

#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>

/* Chat session structure */
typedef struct _chatSession {
    char *nickname; /* User's nickname */
    char *token;    /* Session token used for simple authentication */
} chatSession;

char *create_token(char *token, size_t size);
chatSession *create_session();

#endif
