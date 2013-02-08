/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
 * Contact: m@ioalex.net
 *
 * http://mp3splt.sourceforge.net/
 *
 *********************************************************/
/**********************************************************
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#include "mutex.h"
#include "ui_types.h"

void init_mutex(SPLT_MUTEX *mutex)
{
  g_static_mutex_init(mutex);
}

void clear_mutex(SPLT_MUTEX *mutex)
{
  g_static_mutex_free(mutex);
}

void lock_mutex(SPLT_MUTEX *mutex)
{
  g_static_mutex_lock(mutex);
}

void unlock_mutex(SPLT_MUTEX *mutex)
{
  g_static_mutex_unlock(mutex);
}

