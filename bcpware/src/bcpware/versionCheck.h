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
#pragma once
#include <string>
#include <iostream>
#include <cstdio>
using namespace std;
class VersionCheck
{
public:
	VersionCheck(const string &version);
	~VersionCheck();
	bool operator< (const VersionCheck& other);
	bool operator==(const VersionCheck& other);
	friend ostream& operator <<(std::ostream& stream, const VersionCheck& ver);
public:
	int major, minor, revision, build;
	
};
