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
	
	//id	流水號	numeric	
	//printer_sn	印表機序號	varchar(255)
	//create_time	Server收到LOG時間	datatime
	//firmware_version	Sirius Version	varchar(20)
	//firmware_version	FPGA Version	varchar(20)
	//host	OS代碼(W: Windows, M : Mac)	varchar(20)
	//host_model	OS版本	varchar(255)
	//host_version	OS版號	varchar(20)
	//printer_lifetime	列印機開機時間	int(hour)
	//printer_model	列印機種類	varchar(255)
	//printer_sn	列印機序號(第7~8碼為出貨地國家)	varchar(255)
	//record_time	軟體紀錄LOG時間	datatime
	//bcpware_version	軟體版號	varchar(20)
	//Printhead_ lifetime	機器列印時間	int(hour)
	//sw_type	軟體類別(BCPware)	varchar(30)
	//user_account	列印者帳號	varchar(255)
	//determine	判斷user or NKG user('Y' = NKG, 'N' = user)	varchar(10)






	
}

MachineLog::~MachineLog()
{
}
