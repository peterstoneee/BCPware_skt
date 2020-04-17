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
#ifndef COMMAND_H
#define COMMAND_H


#include <QUndoCommand>
#include "mainwindow.h"
#include "../common/interfaces.h"
#include<vcg/complex/complex.h>

class MainWindow;

class Matrix_command:public QUndoCommand//single object translate
{
public:
	
	Matrix_command(MainWindow *_curmwi, MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag);//flag to trigger redo
	~Matrix_command();

	void undo();
	void redo();
	//bool mergeWith(const QUndoCommand *command);
private:

	Point3f old_position;
	Point3f new_position;

	Matrix44m old_Matrix;
	Matrix44m new_Matrix;

	MeshModel *m;
	MainWindow *curmwi;
	bool flag;

};
class Translate_Command :public QUndoCommand//multiple object translate
{
public:
	Translate_Command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_Matrix, bool _flag);
	~Translate_Command();
	void undo();
	void redo();

private:
	QSet<int> shrill_meshes;//已選meshes，
	Matrix44m old_Matrix;
	Matrix44m new_Matrix;
	MeshDocument *md;
	MainWindow *curmwi;
	bool flag;

};

class Scale_command :public QUndoCommand//single object
{
public:
	//Scale_command(MeshModel *sm, const Point3f &_dim, bool _flag = true);
	Scale_command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, const Point3f &_dim, bool _flag = true);
	~Scale_command();

	void undo();
	void redo();
private:
	QSet<int> shrill_meshes;//已選meshes，
	Point3f old_Dim;
	Point3f new_Dim;
	//MeshModel *m;
	MeshDocument *md;
	MainWindow *curmwi;
	bool flag;
};

class Rotate_command : public QUndoCommand
{
public:
	//Rotate_command(MeshModel *sm, const Matrix44m &_old_matrix);
	Rotate_command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, Point3d _rotation, QList<Point3f> _old_center, bool _flag = true);
	~Rotate_command();
	void undo();
	void redo();
private:
	QSet<int> shrill_meshes;
	MeshDocument *md;
	Point3f RotV;
	Point3f Axis;
	Point3d rotation;
	float degree_offset;
	QList<Point3f> old_center;
	QList<Point3f> new_center;
	MainWindow *curmwi;
	bool flag;
};

class paste_command :public QUndoCommand
{
public:
	paste_command(MainWindow *_curmwi, QList<MeshModel *> _copiedMeshList, bool _flag = true);
	~paste_command();
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	QList<MeshModel *> copyList;
	QList<int> paste_oldIdList;	
	bool flag;

};

class import_command : public QUndoCommand
{
public:
	import_command(MainWindow *_curmwi, QString _filename, Point3m _scaleN, int id, bool _flag = true);
	~import_command();
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	QString filename;
	Point3m scaleN;
	int import_oldIdList;
	bool flag;
};

class delete_command : public QUndoCommand
{
public:
	delete_command(MainWindow *_curmwi, QList<MeshModel *> _deletedMeshList, QList<int> _deletedMeshId, bool _flag = true);
	~delete_command();
	void undo();
	void redo();
private:
	bool delete_undo(MeshModel *delToUndo);

	MainWindow *curmwi;
	QList<MeshModel *> deleteList;
	QList<int> delete_oldIdList;
	bool flag;
};

class colorChange_command : public QUndoCommand
{
public:
	colorChange_command(MainWindow *, MeshDocument *, QSet<int>, QMap<int, Color4b> , QMap<int, int>,QMap<int, RenderMode>,QMap<int,QList<Color4b>>, QMap<int,  QList<Color4b>>, bool _flag = false);
	~colorChange_command() {}
	void undo();
	void redo();

private:
	MainWindow *curmwi;
	MeshDocument *md;
	QSet<int> shrill_meshes;
	//QList<Color4b> oldColor;	
	
	QMap<int, int> oldMeshDataMask;
	QMap<int, int> newMeshDataMask;

	QMap<int, RenderMode> oldMeshRenderMode;
	QMap<int, RenderMode> newMeshRenderMode;

	QMap<int,QList<Color4b>> recordPerMeshVertexColor;
	QMap<int, QList<Color4b>> recordPerMeshFaceColor;


	QMap<int, Color4b> oldMeshColor;
	QMap<int, Color4b> newMeshColor;
	bool flag;
};

class packing_command : public QUndoCommand
{
public:
	packing_command(MainWindow *, MeshDocument *_md, QSet<int> _selected_meshes, QMap<int, Point3f> _savePos, bool _flag = true);
	~packing_command();
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QSet<int> shrill_meshes;
	QMap<int, Point3m> savePos;
	QMap<int, Point3m> new_savePos;
	bool flag;
};

class packing_rotate_command : public QUndoCommand
{
public:
	packing_rotate_command(MainWindow *, MeshDocument *_md,QSet<int> _selected_meshes, QMap<int, Point3f> _savePos, QMap<int, Point3d> _firstRotation, QMap<int, Point3d> _secondRotation, bool _flag = false);
	~packing_rotate_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	void rotate_mesh(CMeshO &rm, float deg, Point3f axis);
	MeshDocument *md;
	QSet<int> shrill_meshes;
	QMap<int, Point3m> savePos;
	QMap<int, Point3m> new_savePos;
	QMap<int, Point3d> firstRotation;
	QMap<int, Point3d> secondRotation;
	bool flag;
};

class ToCenter_command :public QUndoCommand//single object translate
{
public:

	ToCenter_command(MainWindow *,MeshDocument *_md, QList<int> _selectedID, QList<Point3f> _savePos, bool _flag = true);
	~ToCenter_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QList<int> selectedID;
	QList<Point3m> savePos;
	QList<Point3m> new_savePos;
	bool flag;
};

class flip_command : public QUndoCommand
{
public:
	flip_command(MainWindow *,MeshDocument *_md, QSet<int> _selected_meshes, QString _flip_selected, bool _flag = true);
	~flip_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QSet<int> selected_meshes;
	QString flip_selected;
	bool flag;
};

class justify_command : public QUndoCommand
{
public:
	justify_command(MainWindow *,MeshDocument *_md, QSet<int> _selected_meshes, QList<Point3f> _savePos, bool _flag = true);
	justify_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QSet<int> selected_meshes;
	QList<Point3m> savePos;
	QList<Point3m> new_savePos;
	bool flag;
};

class land_command : public QUndoCommand
{
public:
	land_command(MainWindow *,MeshDocument *_md, QSet<int> _selected_meshes, QList<Point3f> _savePos, bool _flag = true);
	~land_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QSet<int> selected_meshes;
	QList<Point3m> savePos;
	QList<Point3m> new_savePos;
	bool flag;
};


class landAll_command : public QUndoCommand
{
public:
	landAll_command(MainWindow *,MeshDocument *_md, QMap<int, Point3f> _savePos, bool _flag = true);
	~landAll_command() {}
	void undo();
	void redo();
private:
	MainWindow *curmwi;
	MeshDocument *md;
	QMap<int, Point3m> savePos;
	QMap<int, Point3m> new_savePos;
	bool flag;
};

class reset_command : public QUndoCommand
{
public:
	reset_command(MainWindow *_mw, QList<MeshModel *> _deletedMeshList, QList<int> _deletedMeshId, QMap<int, Point3f> _resetScaleN, bool _flag = true);
	reset_command() {}
	void undo();
	void redo();
private:
	MainWindow *mw;
	QList<MeshModel *> deletedMeshList;
	QList<int> deletedMeshId;
	QMap<int, QString> resetNameList;
	QMap<int, Point3f> resetScaleN;
	QMap<int, bool> newIdList;
	bool flag;
	delete_command *dc;
	QMap<int, import_command*> icList;
};

class importFrom_command : public QUndoCommand
{
public:
	importFrom_command(MainWindow *_mw, QList<int> _imported3mfIdList, bool _flag);
	void undo();
	void redo();

private:
	MainWindow *mw;
	QList<int> imported3mfIdList;
	bool flag;
};

#endif