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

#include <QString>
#include<QDateTime>
#include<QTime>
#include<QTextEdit>

#include "filterparameter.h"



struct CurrentTime
{
	QString label;
	QDateTime currenttime;

};
struct Title
{
	QString label;
};
struct EstimatedBuildTime
{
	QString label;
	QTime time;
};
struct savepath
{  
	QString label;
	//QDir savepath;
};

struct Label_Float
{
	QString label;
	float value;
	QString unit;

};
struct Label_Int
{
	QString label;
	int value;
	QString unit;
};
struct Label_string
{
	QString label;
	QString ss;
	QString unit;
};


class Print_Job_Information
{
public:
	Print_Job_Information()
	{
		estimatedBinderUsage.value = 0;
		estimated_yellow.value = 0;
		estimated_magenta.value = 0;
		estimated_cyan.value = 0;
		volumn.value = 0;
		area.value = 0;
		modelNumber.value = 0;
		totalNumOfVertices.value = 0;
		totalNumOfFacet.value = 0;
		numoflayer.value = 0;
		layerThickness.value = 0;
		wiperEstimateUsage.value = 0;

		
	};
public:
	Title title;
	CurrentTime currenttime; //現在時間

	EstimatedBuildTime estimatedBuildTime; //預估時間
	Label_Float estimatedBinderUsage; //binder用量
	Label_Float estimated_cyan; // cyan用量
	Label_Float estimated_magenta; //magenta 用量
	Label_Float estimated_yellow; //yellow用量

	
	//Label_Float estimatedMaintainBinderUsage; //binder用量
	//Label_Float estimated_Maintain_cyan; // cyan用量
	//Label_Float estimated_Maintain_magenta; //magenta 用量
	//Label_Float estimated_Maintain_yellow; //yellow用量
	//
	//Label_Float estimatedBinderUsage; //binder用量
	//Label_Float estimated_cyan; // cyan用量
	//Label_Float estimated_magenta; //magenta 用量
	//Label_Float estimated_yellow; //yellow用量

	Label_Float volumn; //體積
	Label_Float powerUsage; //體積
	Label_Float area; //面積
	Label_Int modelNumber; //mesh數量
	Label_Int totalNumOfVertices; //總點數
	Label_Int totalNumOfFacet; //總面數
	
	Label_Float numoflayer; //列印層數	
	Label_Float layerThickness; //層高

	Label_Int wiperEstimateUsage; 
	Label_Float postProcessUsage;



};

class slice_program_setting
{
public:
	slice_program_setting();
	slice_program_setting(float t,float b, float l, float r, float bt);
	~slice_program_setting();


	int getTopGap();
	float getBottom_Gap();
	float getLeft_Gap();
	float getRight_Gap();
	float getBetweenGap();
	
	void setTopGap(int);
	void setBottomGap(float);
	void setLeftGap(float);
	void setRightGap(float);
	void setBetweenGap(float);

	void setoutlineName(QString so){ outlineImageName = so; }
	void setcaplineName(QString sc){ capImageName = sc; }
	QString getoutlineName(){ return outlineImageName; }
	QString getcapImageName(){ return capImageName; }

	void setOlMeshName(QString soM){ olMeshName = soM; }
	void setCapMeshName(QString scM){ capMeshName = scM; }
	QString getOlMeshName(){ return olMeshName; }
	QString getCapMeshName(){ return capMeshName; }
	
	void setgenPicDir(QString gpd){ genPicDir = gpd; }
	QString getPicDir(){ return genPicDir; }

	void setbuildheight(Label_Float height){ buildheight = height; }
	Label_Float getbuildheight(){ return buildheight; }

	void set_print_or_not(bool temp){ print_or_not = temp; }
	bool get_print_or_not(){ return print_or_not; }

	void set_useless_print(float temp){ useless_width = temp; }
	float get_useless_print(){ return useless_width; }

	void set_add_pattern(bool temp){ add_pattern = temp; }
	float get_add_pattern(){ return add_pattern; }

	void set_plus_print_length(double temp){ plus_print_length = temp; }
	double get_plus_print_length(){ return plus_print_length; }

	void set_start_print_page(int temp){ startPrintpage = temp; }
	int get_start_print_page(){ return startPrintpage; }

	enum grid_unit{cm, inch}gd_unit;
	enum page_size{printBed_size, longPage_size}print_pagesize;

	void initValue(RichParameterSet &initset);



private:
	int vtNumberLimit;
	float top_Gap;
	float bottom_Gap;
	float left_Gap;
	float right_Gap;
	float between_Gap;
	QString printerType;

	QString genPicDir;

	QString outlineImageName;
	QString capImageName;
	QString olMeshName;
	QString capMeshName;
	double totalPrintArea;
	double printDpi;
	bool print_or_not;
	float useless_width;
	bool add_pattern;
	int startPrintpage;

	double plus_print_length;
	/*double print_empty_area;
#define total_print_area 35.6
#define groove_x 22.2
#define print_empty_area (35.6-22.2)
#define plus_pixel (35.6-22.2)*300/DSP_inchmm*/


	//***20160317***
	Label_Float buildheight; //列印高度
	Label_string printer_type; //列印機名稱

	void updateValue(RichParameterSet &set);
	


};

class jobServiceCount
{

private:

	int c_Drop;
	int k1_Drop;
	int m_Drop;
	int k2_Drop;
	int y_Drop;

	double inkDropVolumn;// = 1.5e-9;//ml
	int printHeadNozzle;

public:
	enum inktype{ c, k1, m, k2, y };
	enum jobtype{ prejob, midjob, postjob };
	jobServiceCount(int _c, int _k1, int _m, int _k2, int _y){
		c_Drop = _c;
		k1_Drop = _k1;
		m_Drop = _m;
		k2_Drop = _k2;
		y_Drop = _y;
		inkDropVolumn = 1.2e-9;//ml
		printHeadNozzle = 14080;
	}

	double maintUsage(inktype type)
	{
		double _maintUsage = 0;
		switch (type)
		{
		case inktype::c:
			return _maintUsage = inkDropVolumn * c_Drop * printHeadNozzle;
			break;
		case inktype::k1:
			return _maintUsage = inkDropVolumn * k1_Drop * printHeadNozzle;
			break;
		case inktype::m:
			return _maintUsage = inkDropVolumn * m_Drop * printHeadNozzle;
			break;
		case inktype::k2:
			return _maintUsage = inkDropVolumn * k2_Drop * printHeadNozzle;
			break;
		case inktype::y:
			return _maintUsage = inkDropVolumn * y_Drop * printHeadNozzle;
			break;

		}

	}


};