/****************************************************************************
* BCPware
*
*  Copyright 2018 by
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/
#include "versionCheck.h"

VersionCheck::VersionCheck(const string &version)
{
	major = 0;
	minor = 0;
	revision = 0;
	build = 0;

	sscanf(version.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);
	if (major < 0) major = 0;
	if (minor < 0) minor = 0;
	if (revision < 0) revision = 0;
	if (build < 0) build = 0;
}
bool VersionCheck::operator< (const VersionCheck &other)
{
	/*if (major < other.major) return true;
	else if (major == other.major)
	{
		if (minor < other.minor) return true;
		else if (build == other.build)
		{
			if (revision < other.revision) return true;
			else if (minor == other.minor)
			{
				if (build < other.build) return true;
			}
		}
	}
	return false;*/


	if (major < other.major) return true;
	else if (major == other.major)
	{
		if (minor < other.minor) return true;
		else if (minor == other.minor)
		{
			if (revision < other.revision) return true;
			else if (revision == other.revision)
			{
				if (build < other.build) return true;				
			}
		}
	}
	return false;


}

std::ostream& operator << (std::ostream& stream, const VersionCheck& ver)
{
	stream << ver.major;
	stream << '.';
	stream << ver.minor;
	stream << '.';
	stream << ver.revision;
	stream << '.';
	stream << ver.build;
	return stream;
}
bool VersionCheck::operator== (const VersionCheck &other)
{
	return major == other.major 
		&& minor == other.minor 
		&& revision == other.revision 
		&& build == other.build;
}
VersionCheck::~VersionCheck()
{

}