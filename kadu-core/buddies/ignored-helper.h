/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IGNORED_HELPER_H
#define IGNORED_HELPER_H

#include "exports.h"

class BuddySet;

class KADUAPI IgnoredHelper
{

public:
	static bool isIgnored(BuddySet contacts);
	static void setIgnored(BuddySet contacts, bool ignored = true);

};

#endif // IGNORED_HELPER_H
