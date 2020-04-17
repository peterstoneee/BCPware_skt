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
#ifndef RENDER_MODE_ACTIONS_H
#define RENDER_MODE_ACTIONS_H

#include "../common/meshmodel.h"
#include <QAction>

class RenderModeAction : public QAction
{
	Q_OBJECT
public:
	RenderModeAction(const QString& title,QObject* parent);
	RenderModeAction(const unsigned int meshid,const QString& title,QObject* parent);
	RenderModeAction(const QIcon& icn,const QString& title,QObject* parent);
	RenderModeAction(const unsigned int meshid,const QIcon& icn,const QString& title,QObject* parent);

	void updateRenderMode(QList<RenderMode>& rmlist);
	virtual void updateRenderMode(RenderMode& rm) = 0;
	virtual bool isRenderModeEnabled(const RenderMode& rm) const = 0;

private:
	void commonInit(const unsigned int meshid);
	
};

class RenderModeBBoxAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeBBoxAction(QObject* parent);
	RenderModeBBoxAction(const unsigned int meshid,QObject* parent);

protected:
	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModePointsAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModePointsAction(QObject* parent);
	RenderModePointsAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeWireAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeWireAction(QObject* parent);
	RenderModeWireAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

//******************20150306 add TexWireAction
class RenderModeTexWireAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeTexWireAction(QObject* parent);
	RenderModeTexWireAction(const unsigned int meshid, QObject* parent);

	void updateRenderMode(RenderMode &rm);
	bool isRenderModeEnabled(const RenderMode &rm)const;
};
///////******************////////////////////////
//******************20150319 增加render膠水圖action*********************
class RenderModeOutlineTesselation : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeOutlineTesselation(QObject* parent);
	RenderModeOutlineTesselation(const unsigned int meshid, QObject* parent);

	void updateRenderMode(RenderMode &rm);
	bool isRenderModeEnabled(const RenderMode &rm)const;
};
//******************************************************
class RenderModeHiddenLinesAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeHiddenLinesAction(QObject* parent);
	RenderModeHiddenLinesAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFlatLinesAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeFlatLinesAction(QObject* parent);
	RenderModeFlatLinesAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};


class RenderModeFlatAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeFlatAction(QObject* parent);
	RenderModeFlatAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeSmoothAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeSmoothAction(QObject* parent);
	RenderModeSmoothAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeTexturePerVertAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeTexturePerVertAction(QObject* parent);
	RenderModeTexturePerVertAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeTexturePerWedgeAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeTexturePerWedgeAction(QObject* parent);
	RenderModeTexturePerWedgeAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeDoubleLightingAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeDoubleLightingAction(QObject* parent);
	RenderModeDoubleLightingAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFancyLightingAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeFancyLightingAction(QObject* parent);
	RenderModeFancyLightingAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeLightOnOffAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeLightOnOffAction(QObject* parent);
	RenderModeLightOnOffAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFaceCullAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeFaceCullAction(QObject* parent);
	RenderModeFaceCullAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeSelectedFaceRenderingAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeSelectedFaceRenderingAction(QObject* parent);
	RenderModeSelectedFaceRenderingAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeSelectedVertRenderingAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeSelectedVertRenderingAction(QObject* parent);
	RenderModeSelectedVertRenderingAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeColorModeNoneAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeColorModeNoneAction(QObject* parent);
	RenderModeColorModeNoneAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeColorModePerMeshAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeColorModePerMeshAction(QObject* parent);
	RenderModeColorModePerMeshAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeColorModePerVertexAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeColorModePerVertexAction(QObject* parent);
	RenderModeColorModePerVertexAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeColorModePerFaceAction : public RenderModeAction
{
	Q_OBJECT
public:
	RenderModeColorModePerFaceAction(QObject* parent);
	RenderModeColorModePerFaceAction(const unsigned int meshid,QObject* parent);

	void updateRenderMode(RenderMode& rm);
	bool isRenderModeEnabled(const RenderMode& rm) const;
};


#endif	