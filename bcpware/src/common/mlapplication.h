/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


/****************************************************************************
* My Extension to Meshlab
*
*  Copyright 2018 by
*
* This file is part of Meshlab.
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

#ifndef ML_APPLICATION_H
#define ML_APPLICATION_H

#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QString>

class MeshLabApplication : public QApplication
{
public:
    enum HW_ARCHITECTURE {HW_32BIT = 32,HW_64BIT = 64};
    MeshLabApplication(int &argc, char *argv[]):QApplication(argc,argv){}
    ~MeshLabApplication(){}
    bool notify(QObject * rec, QEvent * ev);
	//***20150513
    //static const QString appName(){return tr("MeshLab"); }
	static const QString appName(){ return tr("Picasso"); }
    static const QString architecturalSuffix(const HW_ARCHITECTURE hw) {return "_" + QString::number(int(hw)) + "bit";}
    static const QString appArchitecturalName(const HW_ARCHITECTURE hw) {return appName() + architecturalSuffix(hw);}
    static const QString appVer() {return tr("0.9.5"); }
    static const QString completeName(const HW_ARCHITECTURE hw){return appArchitecturalName(hw) + " v" + appVer(); }
    static const QString organization(){return tr("VCG");}


	static const QString organizationHost() { return tr(""); }
	static const QString webSite() { return tr(""); }
	static const QString downloadSite() { return tr(""); }

	//backup
    //static const QString organizationHost() {return tr("http://vcg.isti.cnr.it");}//backup
    //static const QString webSite() {return tr("http://meshlab.sourceforge.net/");}//backup
    //static const QString downloadSite() {return tr("http://downloads.sourceforge.net/project/meshlab");}//backup

    static const QString downloadUpdatesSite() {return downloadSite() + tr("/updates");}

    static const QString pluginsPathRegisterKeyName() {return tr("pluginsPath");}
    static const QString versionRegisterKeyName() {return tr("version");}
	static const QString wordSizeKeyName() { return tr("wordSize"); }
	static const QString getRoamingDir()
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, appName(), "application");
		QString location = QFileInfo(settings.fileName()).absolutePath() + "/";

		return location;
	}
	static const QString tempImageDir()
	{
		//return getRoamingDir() + "/" + tr("xyzimage");
	}
	static const QString tempPatternDir()
	{
	}
	static const QString tempZxFileDir()
	{

	}

};

class PeterApplication : public QApplication
{
public:
	enum HW_ARCHITECTURE { HW_32BIT = 32, HW_64BIT = 64 };
	PeterApplication(int &argc, char *argv[]) :QApplication(argc, argv){}
	~PeterApplication(){}
	bool notify(QObject * rec, QEvent * ev);
	//***20150918	
	static const QString appName(){ return tr("Peter"); }
	static const QString architecturalSuffix(const HW_ARCHITECTURE hw) { return "_" + QString::number(int(hw)) + "bit"; }
	static const QString appArchitecturalName(const HW_ARCHITECTURE hw) { return appName() + architecturalSuffix(hw); }
	static const QString appVer() { return tr("1.0.0"); }
	static const QString completeName(const HW_ARCHITECTURE hw){ return appArchitecturalName(hw) + " v" + appVer(); }
	static const QString organization(){ return tr("VCG"); }
	static const QString organizationHost() { return tr(""); }
	static const QString webSite() { return tr(""); }
	static const QString downloadSite() { return tr("http://downloads.sourceforge.net/project/meshlab"); }
	static const QString downloadUpdatesSite() { return downloadSite() + tr("/updates"); }
	static const QString pluginsPathRegisterKeyName() { return tr("pluginsPath"); }
	static const QString versionRegisterKeyName() { return tr("version"); }
	static const QString wordSizeKeyName() { return tr("wordSize"); }
};

#endif