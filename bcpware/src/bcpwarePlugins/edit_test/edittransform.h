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
#ifndef EDITTRANSFORM_H
#define EDITTRANSFORM_H

#include <QObject>
#include <common/interfaces.h>
#include <bcpware/glarea.h>
class EditTransform :public QObject, public MeshEditInterFace_v2
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_EDIT_INTERFACE_V2_IID)
	Q_INTERFACES(MeshEditInterFace_v2)

public:
	enum {
		EDP_EDIT_TRANSFORM,
		EDP_EDIT_ROTATE,
		EDP_EDIT_SCALE
	};
	EditTransform();
	void initParameterSet(QAction *editAction, MeshDocument &md, RichParameterSet & /*parent*/);
	QString EditTransform::filterName(FilterIDType filter) const;
	bool applyEdit_v2(QAction *, MeshDocument &md, RichParameterSet &par, CallBackPos *cb/*,GLArea *gla*/);


private:
	QFont qFont;

	Matrix44m original_Transform;
	Matrix44m delta_Transform;
	Matrix44m wholeTransformMatrix;

	Scalarm degree_offset;
	Scalarm scale_x_offset;
	Scalarm scale_y_offset;
	Scalarm scale_z_offset;
	Scalarm scale_all_offset;
	bool lockAxis;

	Scalarm x_offset;
	Scalarm y_offset;
	Scalarm z_offset;

	bool aroundOrigin;

	bool isSnapping;
	Scalarm  snapto;//float snapto;

	QString inputnumberstring;
	Scalarm   inputnumber;

	bool isMoving;
	vcg::Point2i startdrag;
	vcg::Point2i enddrag;

	Scalarm currScreenOffset_X;   // horizontal offset (screen space)
	Scalarm currScreenOffset_Y;   // vertical offset (screen space)

	// when the user is dragging, the mouse offset is stored here,
	// two sets of variables are used, since the offset will be accumulated in
	// the currOffset* variables when finished dragging
	Scalarm displayOffset;        // mouse offset value (single axis)
	Scalarm displayOffset_X;      // mouse X offset value
	Scalarm displayOffset_Y;      // mouse Y offset value
	Scalarm displayOffset_Z;      // mouse Z offset value

	// offset is accumulated here... user can change the offset by dragging mouse until
	// satisfied, accumulating changes
	// if the user confirms, this offset is applied to the matrix
	Scalarm currOffset;     // combined offset value (single axis)
	Scalarm currOffset_X;     // X offset value
	Scalarm currOffset_Y;     // Y offset value
	Scalarm currOffset_Z;     // Z offset value

	Point3m screen_xaxis;
	Point3m screen_yaxis;
	Point3m screen_zaxis;

private:
	void applyMatrix(MeshModel *editMesh, GLArea *);
	void applyMatrix2(Box3m, GLArea *, MeshDocument *);
	//MeshModel *editMesh;

private:
	//MeshModel *editMesh;
	//bool MyPick(const int &x, const int &y, Point3m &pp, float mydepth);

public:
	virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea *, MeshDocument *);
	virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea *, MeshDocument *);
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &, GLArea *, MeshDocument *);
	bool MyPick(const int &x, const int &y, Point3m &pp, float mydepth);
	bool updateModelProjectionMatrix(GLArea *);
	void resetOffsets();
	void allMeshApplyMatrix(MeshDocument &md, GLArea *);
signals:
	//void matrix_changed(MeshModel *sm, const Matrix44m execute_matrix);

};

#endif // EDITTRANSFORM_H
