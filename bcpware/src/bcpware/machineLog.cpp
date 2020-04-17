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
#include "machineLog.h"


MachineLog::MachineLog()
{
}

void MachineLog::generateMachineLog()
{
	
	//id	�y����	numeric	
	//printer_sn	�L����Ǹ�	varchar(255)
	//create_time	Server����LOG�ɶ�	datatime
	//firmware_version	Sirius Version	varchar(20)
	//firmware_version	FPGA Version	varchar(20)
	//host	OS�N�X(W: Windows, M : Mac)	varchar(20)
	//host_model	OS����	varchar(255)
	//host_version	OS����	varchar(20)
	//printer_lifetime	�C�L���}���ɶ�	int(hour)
	//printer_model	�C�L������	varchar(255)
	//printer_sn	�C�L���Ǹ�(��7~8�X���X�f�a��a)	varchar(255)
	//record_time	�n�����LOG�ɶ�	datatime
	//bcpware_version	�n�骩��	varchar(20)
	//Printhead_ lifetime	�����C�L�ɶ�	int(hour)
	//sw_type	�n�����O(BCPware)	varchar(30)
	//user_account	�C�L�̱b��	varchar(255)
	//determine	�P�_user or NKG user('Y' = NKG, 'N' = user)	varchar(10)






	
}

MachineLog::~MachineLog()
{
}
