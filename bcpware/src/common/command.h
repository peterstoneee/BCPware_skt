#pragma once
#include <QUndoCommand>
#include "interfaces.h"
#include<vcg/complex/complex.h>

class Matrix_command:public QUndoCommand//single object translate
{
public:
	
	Matrix_command(MainWindowInterface *_curmwi,MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag);//flag to trigger redo
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
	MainWindowInterface *curmwi;
	bool flag;

};
class Translate_Command :public QUndoCommand//multiple object translate
{
public:
	Translate_Command(MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_Matrix, bool _flag);
	~Translate_Command();

	void undo();
	void redo();
private:
	QSet<int> shrill_meshes;//已選meshes，
	Matrix44m old_Matrix;
	Matrix44m new_Matrix;
	MeshDocument *md;
	bool flag;

};

class Scale_command :public QUndoCommand//single object
{
public:
	//Scale_command(MeshModel *sm, const Point3f &_dim, bool _flag = true);
	Scale_command(MeshDocument *_md, QSet<int> _selected_meshes, const Point3f &_dim, bool _flag = true);
	~Scale_command();

	void undo();
	void redo();
private:
	QSet<int> shrill_meshes;//已選meshes，
	Point3f old_Dim;
	Point3f new_Dim;
	//MeshModel *m;
	MeshDocument *md;
	bool flag;
};

class Rotate_command : public QUndoCommand
{
public:
	Rotate_command(MeshModel *sm, const Matrix44m &_old_matrix);
	Rotate_command(MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_matrix, bool _flag = true);
	~Rotate_command();
	void undo();
	void redo();
private:
	QSet<int> shrill_meshes;
	MeshDocument *md;
	Matrix44m old_matrix;
	Matrix44m new_matrix;
	//MeshModel *m;
	Point3f RotV;
	Point3f Axis;
	Matrix44m back_r_matrix;
	bool flag;

};

class paste_command :public QUndoCommand
{
public:
	paste_command(MeshDocument *_md,bool _flag = true);
	~paste_command();
	void undo();
	void redo();
private:
	//QSet<int> shrill_meshes;
	MeshDocument *md;
	//MeshModel
	int selectedMeshId;
	int pasteMeshId;
	//Matrix44m back_r_matrix;
	bool flag;

};

class packing_command : public QUndoCommand
{
public:
	packing_command(MeshDocument *_md, QMap<int, Point3f> _savePos, QMap<int, Matrix44m> _all_old_Matrix, bool _flag = false);
	~packing_command();
	void undo();
	void redo();
private:
	MeshDocument *md;
	QMap<int, Point3m> savePos;
	QMap<int, Matrix44m> all_old_matrix;
	QMap<int, Matrix44m> all_new_matrix;
	bool flag;
};

class ToCenter_command :public QUndoCommand//single object translate
{
public:

	ToCenter_command(MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag);//flag to trigger redo
	~ToCenter_command();

	void undo();
	void redo();
	//bool mergeWith(const QUndoCommand *command);
private:

	Point3f old_position;
	Point3f new_position;

	Matrix44m old_Matrix;
	Matrix44m new_Matrix;

	MeshModel *m;
	bool flag;

};
//class 