/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterTaskType
 * @short Description type of roster task.
 *
 * Roster task can be either adding an entry, deleting one or updating one.
 */
enum class RosterTaskType
{
	/**
	 * Do nothing.
	 */
	None,
	/**
	 * Add roster entry.
	 */
	Add,
	/**
	 * Delete roster entry.
	 */
	Delete,
	/**
	 * Update roster entry.
	 */
	Update
};

/**
 * @}
 */