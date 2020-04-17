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
#include "command.h"

Translate_Command::Translate_Command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_matrix, bool _flag) :curmwi(_curmwi)
{
	shrill_meshes = _selected_meshes;//¤w¿ïmeshes

	old_Matrix = _old_matrix;

	md = _md;

	new_Matrix.SetIdentity();
	new_Matrix = md->mm()->cm.Tr;

	flag = _flag;
	//setText(QObject::tr("translate_command"));
}

Translate_Command::~Translate_Command() {}

void Translate_Command::undo()
{
	Matrix44m b = old_Matrix;
	Matrix44m a = (old_Matrix - new_Matrix);
	Point3m c = a.GetColumn3(3);
	Matrix44m d;
	d.SetIdentity();
	d.SetTranslate(c);
	foreach(int i, shrill_meshes)
	{
		MeshModel *m = md->getMesh(i);
		/*qDebug() << "value of i: " << i;
		qDebug() << "mesh id: " << m->_id;*/
		Matrix44m  save_mtr = m->cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
		tri::UpdateBounding<CMeshO>::Box(m->cm);
		m->UpdateBoxAndNormals();
		m->cm.Tr = save_mtr*d;
		md->multiSelectID.insert(i);
	}
	//tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
	//tri::UpdateBounding<CMeshO>::Box(m->cm);
	////editMesh->cm.Tr.SetIdentity();
	//m->UpdateBoxAndNormals();

	curmwi->updateMenuForCommand();
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	//curmwi->GLA()->setHastoUpdateVBO();
}
void Translate_Command::redo()
{
	if (flag)
	{
		Matrix44m b = old_Matrix;
		Matrix44m a = (new_Matrix - old_Matrix);
		Point3m c = a.GetColumn3(3);
		Matrix44m d;
		d.SetIdentity();
		d.SetTranslate(c);

		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			//Debug() << "mesh id: " << m->_id;
			tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			m->UpdateBoxAndNormals();
		}
		curmwi->updateMenuForCommand();
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}
	flag = true;



}




Matrix_command::Matrix_command(MainWindow *_curmwi, MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag) :flag(_flag), curmwi(_curmwi)
{
	old_Matrix = _old_Matrix;
	new_Matrix = sm->cm.Tr;
	//new_Matrix = sm->cm.bbox.Center();


	m = sm;
	//setText(QObject::tr("Matrix_command_2"));
	//m->cm.bbox.DimValue
}

//void Matrix_command::undo()
//{	
//	Matrix44m delta_Transform;
//	
//	//delta_Transform= Inverse
//	//delta_Transform.
//
//	delta_Transform.SetIdentity();
//
//	delta_Transform.SetTranslate(*new Point3m(old_Matrix[0] - new_Matrix[0], old_Matrix[1] - new_Matrix[1], old_Matrix[2] - new_Matrix[2]));
//
//	tri::UpdatePosition<CMeshO>::Matrix(m->cm, delta_Transform, true);
//	tri::UpdateBounding<CMeshO>::Box(m->cm);
//	//editMesh->cm.Tr.SetIdentity();
//	m->UpdateBoxAndNormals();
//}

void Matrix_command::undo()
{
	Matrix44m b = old_Matrix;
	Matrix44m a = (old_Matrix - new_Matrix);
	Point3m c = a.GetColumn3(3);
	Matrix44m d;
	d.SetIdentity();
	d.SetTranslate(c);

	tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
	tri::UpdateBounding<CMeshO>::Box(m->cm);
	//editMesh->cm.Tr.SetIdentity();
	m->UpdateBoxAndNormals();
	curmwi->updateMenuForCommand();
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}



void Matrix_command::redo()
{
	if (flag)
	{
		Matrix44m b = old_Matrix;
		Matrix44m a = (new_Matrix - old_Matrix);
		Point3m c = a.GetColumn3(3);
		Matrix44m d;
		d.SetIdentity();
		d.SetTranslate(c);

		tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
		tri::UpdateBounding<CMeshO>::Box(m->cm);
		//editMesh->cm.Tr.SetIdentity();
		m->UpdateBoxAndNormals();
		curmwi->updateMenuForCommand();
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}
	flag = true;
}

Matrix_command::~Matrix_command()
{
}


//Scale_command::Scale_command(MeshModel *sm, const Point3f &_dim, bool _flag )
//{
//	m = sm;
//	old_Dim = _dim;
//	new_Dim = m->cm.bbox.Dim();
//	flag = _flag;
//	setText(QObject::tr("scale_command"));
//
//}
Scale_command::Scale_command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, const Point3f &_dim, bool _flag)
	:md(_md), shrill_meshes(_selected_meshes), old_Dim(_dim), flag(_flag), curmwi(_curmwi)
{
	new_Dim = md->mm()->cm.bbox.Dim();
	//setText(QObject::tr("scale_command"));
}
void Scale_command::undo()
{

	Point3f s_Dim(old_Dim.X() / new_Dim.X(), old_Dim.Y() / new_Dim.Y(), old_Dim.Z() / new_Dim.Z());


	Matrix44m scaleMatrix;
	scaleMatrix.SetIdentity();

	scaleMatrix.SetScale(s_Dim);

	//***position

	Matrix44m scale_Translate_BackTo_CenterMatrix;
	Matrix44m scale_Translate_GoBackTo_OriginalMatrix;
	Matrix44m wholeTransformMatrix;

	scale_Translate_BackTo_CenterMatrix.SetIdentity();
	scale_Translate_GoBackTo_OriginalMatrix.SetIdentity();
	wholeTransformMatrix.SetIdentity();

	if (shrill_meshes.size() > 1)
	{
		Box3m selBox;
		Matrix44m iden;
		iden.SetIdentity();
		foreach(int i, shrill_meshes)
		{
			MeshModel *mp = md->getMesh(i);
			if (md->isPrint_item(*mp))
				selBox.Add(iden, mp->cm.bbox);
		}
		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			Matrix44m  save_mtr = m->cm.Tr;
			Point3m meshCenter = selBox.Center();
			scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
			scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
			wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
			//
			tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			//editMesh->cm.Tr.SetIdentity();
			m->UpdateBoxAndNormals();
			m->cm.Tr = save_mtr*wholeTransformMatrix;

		}
	}
	else
	{
		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			Matrix44m  save_mtr = m->cm.Tr;
			Point3m meshCenter = m->cm.bbox.Center();
			scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
			scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
			wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
			//
			tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			//editMesh->cm.Tr.SetIdentity();
			m->UpdateBoxAndNormals();
			m->cm.Tr = save_mtr*wholeTransformMatrix;

		}
	}
	
	curmwi->updateMenuForCommand();
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	//qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;

}
void Scale_command::redo()
{
	if (flag)
	{
		Point3f s_Dim(new_Dim.X() / old_Dim.X(), new_Dim.Y() / old_Dim.Y(), new_Dim.Z() / old_Dim.Z());

		Matrix44m scaleMatrix;
		scaleMatrix.SetIdentity();

		scaleMatrix.SetScale(s_Dim);

		//***position
		Matrix44m scale_Translate_BackTo_CenterMatrix;
		Matrix44m scale_Translate_GoBackTo_OriginalMatrix;
		Matrix44m wholeTransformMatrix;

		scale_Translate_BackTo_CenterMatrix.SetIdentity();
		scale_Translate_GoBackTo_OriginalMatrix.SetIdentity();
		wholeTransformMatrix.SetIdentity();
		if (shrill_meshes.size() > 1)
		{
			Box3m selBox;
			Matrix44m iden;
			iden.SetIdentity();
			foreach(int i, shrill_meshes)
			{
				MeshModel *mp = md->getMesh(i);
				if (md->isPrint_item(*mp))
					selBox.Add(iden, mp->cm.bbox);
			}
			foreach(int i, shrill_meshes)
			{
				MeshModel *m = md->getMesh(i);
				Point3m meshCenter = selBox.Center();
				scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
				scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
				wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
				//
				tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(m->cm);

				m->UpdateBoxAndNormals();
			}
		}
		else
		{
			foreach(int i, shrill_meshes)
			{
				MeshModel *m = md->getMesh(i);
				Point3m meshCenter = m->cm.bbox.Center();
				scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
				scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
				wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
				//
				tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(m->cm);

				m->UpdateBoxAndNormals();
			}
		}
		
		curmwi->updateMenuForCommand();
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();

	}flag = true;
}

Scale_command::~Scale_command()
{
}

//*** 20170217 --- Modified rotate_command undo/redo functions to get the correct position of the mesh and angle of rotation every undo/redo
// Modified by KPPH R&D-I-SW, Mark
Rotate_command::Rotate_command(MainWindow *_curmwi, MeshDocument *_md, QSet<int> _selected_meshes, Point3d _rotation, QList<Point3f> _old_center, bool _flag)
{
	curmwi = _curmwi;
	md = _md;
	shrill_meshes = _selected_meshes;
	rotation = _rotation;
	old_center = _old_center;
	flag = _flag;
	//setText(QObject::tr("rotate_command"));

	RotV[0] = math::ToRad(rotation.X());
	RotV[1] = math::ToRad(rotation.Y());
	RotV[2] = math::ToRad(rotation.Z());

	if (rotation == Point3d(0.0, 0.0, 0.0))
	{
		Axis = Point3m(0, 0, 0);
		degree_offset = math::ToRad(0.0);
	}

	if (rotation.X() != 0)
	{
		Axis = Point3m(1, 0, 0);
		degree_offset = RotV[0];
	}
	if (rotation.Y() != 0)
	{
		Axis = Point3m(0, 1, 0);
		degree_offset = RotV[1];
	}
	if (rotation.Z() != 0)
	{
		Axis = Point3m(0, 0, 1);
		degree_offset = RotV[2];
	}

	new_center.clear();

	foreach(int i, shrill_meshes)
	{
		MeshModel *m = md->getMesh(i);
		new_center.push_back(m->cm.bbox.Center());
	}
}

void Rotate_command::undo()
{
	int k = 0;

	
	if (shrill_meshes.size() > 1)
	{
		Box3m selBox;
		Matrix44m iden;
		iden.SetIdentity();
		foreach(int i, shrill_meshes)
		{
			MeshModel *mp = md->getMesh(i);
			if (md->isPrint_item(*mp))
				selBox.Add(iden, mp->cm.bbox);
		}
		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			Matrix44m save_mtr = m->cm.Tr;
			Matrix44m editRotateMatrix;
			Matrix44m editTranslateBackToCenterMatrix;
			Matrix44m editTranslateGoToPositionMatrix;
			Matrix44m wholeTransformMatrix;

			Point3m meshCenter = selBox.Center();
			k++;
			editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
			editRotateMatrix.SetRotateRad(-(degree_offset), Axis);

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));
			editTranslateGoToPositionMatrix.SetTranslate(meshCenter);

			wholeTransformMatrix = editTranslateGoToPositionMatrix * editRotateMatrix * editTranslateBackToCenterMatrix;

			tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			m->UpdateBoxAndNormals();

			m->cm.Tr = wholeTransformMatrix * save_mtr;
		}
	}
	else
	{
		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			Matrix44m save_mtr = m->cm.Tr;
			Matrix44m editRotateMatrix;
			Matrix44m editTranslateBackToCenterMatrix;
			Matrix44m editTranslateGoToPositionMatrix;
			Matrix44m wholeTransformMatrix;

			Point3m meshCenter = old_center[k];
			k++;
			editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
			editRotateMatrix.SetRotateRad(-(degree_offset), Axis);

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));
			editTranslateGoToPositionMatrix.SetTranslate(meshCenter);

			wholeTransformMatrix = editTranslateGoToPositionMatrix * editRotateMatrix * editTranslateBackToCenterMatrix;

			tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			m->UpdateBoxAndNormals();

			m->cm.Tr = wholeTransformMatrix * save_mtr;
		}
	}



	curmwi->updateMenuForCommand();
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void Rotate_command::redo()
{
	if (flag)
	{
		int k = 0;
		if (shrill_meshes.size() > 1)
		{
			Box3m selBox;
			Matrix44m iden;
			iden.SetIdentity();
			foreach(int i, shrill_meshes)
			{
				MeshModel *mp = md->getMesh(i);
				if (md->isPrint_item(*mp))
					selBox.Add(iden, mp->cm.bbox);
			}
			foreach(int i, shrill_meshes)
			{
				MeshModel *m = md->getMesh(i);
				Matrix44m save_mtr = m->cm.Tr;
				Matrix44m editRotateMatrix;
				Matrix44m editTranslateBackToCenterMatrix;
				Matrix44m editTranslateGoToPositionMatrix;
				Matrix44m wholeTransformMatrix;

				Point3m meshCenter = selBox.Center();
				k++;
				editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
				editRotateMatrix.SetRotateRad(degree_offset, Axis);

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));
				editTranslateGoToPositionMatrix.SetTranslate(meshCenter);

				wholeTransformMatrix = editTranslateGoToPositionMatrix * editRotateMatrix * editTranslateBackToCenterMatrix;

				tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(m->cm);
				m->UpdateBoxAndNormals();

				m->cm.Tr = wholeTransformMatrix * save_mtr;
			}

		}
		else
		{
			foreach(int i, shrill_meshes)
			{
				MeshModel *m = md->getMesh(i);
				Matrix44m save_mtr = m->cm.Tr;
				Matrix44m editRotateMatrix;
				Matrix44m editTranslateBackToCenterMatrix;
				Matrix44m editTranslateGoToPositionMatrix;
				Matrix44m wholeTransformMatrix;

				Point3m meshCenter = new_center[k];
				k++;
				editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
				editRotateMatrix.SetRotateRad(degree_offset, Axis);

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));
				editTranslateGoToPositionMatrix.SetTranslate(meshCenter);

				wholeTransformMatrix = editTranslateGoToPositionMatrix * editRotateMatrix * editTranslateBackToCenterMatrix;

				tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(m->cm);
				m->UpdateBoxAndNormals();

				m->cm.Tr = wholeTransformMatrix * save_mtr;
			}
		}
		
		

		curmwi->updateMenuForCommand();
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}
Rotate_command::~Rotate_command()
{
}


paste_command::paste_command(MainWindow *_curmwi, QList<MeshModel *> _copiedMeshList, bool _flag)
	: curmwi(_curmwi), copyList(_copiedMeshList), flag(_flag)
{
	paste_oldIdList.clear();

	foreach(int i, curmwi->currentViewContainer()->meshDoc.multiSelectID)
	{
		paste_oldIdList.push_back(curmwi->meshDoc()->getMesh(i)->id());
	}

	//setText(QObject::tr("paste_command"));
}

paste_command::~paste_command()
{
}

void paste_command::undo()
{
	foreach(int i, paste_oldIdList)
	{
		curmwi->meshDoc()->multiSelectID.remove(i);
		curmwi->meshDoc()->delMesh(curmwi->currentViewContainer()->meshDoc.getMesh(i));
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void paste_command::redo()
{
	if (flag)
	{
		curmwi->showLayerDlg(false);

		curmwi->currentViewContainer()->meshDoc.multiSelectID.clear();

		if (copyList.isEmpty() == false)
		{
			int i = 0;
			foreach(MeshModel *cmp, copyList)
			{
				curmwi->meshDoc()->copyMesh(cmp);
				//qDebug() << "current Mesh ID (before) : " << curmwi->currentViewContainer()->meshDoc.currentMesh->_id;
				curmwi->currentViewContainer()->meshDoc.multiSelectID.remove(curmwi->meshDoc()->mm()->id());

				curmwi->meshDoc()->mm()->setid(paste_oldIdList[i]);
				i++;
				//qDebug() << "current Mesh ID (after) : " << curmwi->currentViewContainer()->meshDoc.currentMesh->_id;
				curmwi->currentViewContainer()->meshDoc.multiSelectID.insert(curmwi->meshDoc()->mm()->id());

				foreach(MeshModel* mesh, curmwi->currentViewContainer()->meshDoc.meshList)
				{
					curmwi->GLA()->rendermodemap[mesh->id()] = RenderMode();
				}

				//curmwi->GLA()->rendermodemap[curmwi->currentViewContainer()->meshDoc.meshList.last()->_id] = RenderMode();

				MultiViewer_Container *tempmvc = curmwi->currentViewContainer();
				GLArea* tempGLArea = NULL;

				for (int i = 0; i < 1; i++)
				{
					tempGLArea = tempmvc->getViewer(i);
					tempGLArea->updateTexture();
				}

				emit curmwi->currentViewContainer()->meshDoc.meshSetChanged();
				curmwi->toCenter();
			}
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

import_command::import_command(MainWindow *_curmwi, QString _filename, Point3m _scaleN, int id, bool _flag)
	: curmwi(_curmwi), filename(_filename), scaleN(_scaleN), flag(_flag)
{
	import_oldIdList = id;
	//setText(QObject::tr("import_command"));
}

import_command::~import_command() {}

void import_command::undo()
{
	curmwi->meshDoc()->multiSelectID.remove(import_oldIdList);
	curmwi->currentViewContainer()->meshDoc.delMesh(curmwi->currentViewContainer()->meshDoc.getMesh(import_oldIdList));
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void import_command::redo()
{
	if (flag)
	{
		curmwi->meshCheckSwitch = false;
		curmwi->ScaleN = scaleN;
		curmwi->redoOccured = true;
		curmwi->importMesh(filename);
		curmwi->meshDoc()->meshList.last()->setid(import_oldIdList);
		curmwi->meshDoc()->multiSelectID.clear();
		curmwi->meshDoc()->multiSelectID.insert(import_oldIdList);
		curmwi->GLA()->rendermodemap[curmwi->meshDoc()->meshList.last()->id()] = RenderMode();
		emit curmwi->meshDoc()->meshSetChanged();
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

delete_command::delete_command(MainWindow *_curmwi, QList<MeshModel *> _deleteMeshList, QList<int> _deletedMeshId, bool _flag)
	:curmwi(_curmwi), deleteList(_deleteMeshList), delete_oldIdList(_deletedMeshId), flag(_flag)
{
	//setText(QObject::tr("delete_command"));
}

delete_command::~delete_command() {}

void delete_command::undo()
{
	curmwi->showLayerDlg(false);

	curmwi->meshDoc()->multiSelectID.clear();

	if (deleteList.isEmpty() == false)
	{
		int i = 0;

		foreach(MeshModel *cmp, deleteList)
		{
			delete_undo(cmp);
			curmwi->meshDoc()->multiSelectID.remove(curmwi->meshDoc()->mm()->id());
			curmwi->meshDoc()->mm()->setid(delete_oldIdList[i]);

			if (curmwi->meshDoc()->mm()->is3mf == false)
			{
				MeshModel *forResetMesh = new MeshModel(curmwi->meshDoc(), curmwi->meshDoc()->mm()->fullName(), "");
				forResetMesh->updateDataMask(curmwi->meshDoc()->mm()->dataMask());
				vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(forResetMesh->cm, curmwi->meshDoc()->mm()->cm);
				forResetMesh->cm.Tr = curmwi->meshDoc()->mm()->cm.Tr;
				forResetMesh->rmm = curmwi->meshDoc()->mm()->rmm;
				forResetMesh->is3mf = false;

				curmwi->meshDoc()->resetMeshList.insert(curmwi->meshDoc()->mm()->id(), forResetMesh);
			}

			i++;
			curmwi->currentViewContainer()->meshDoc.multiSelectID.insert(curmwi->meshDoc()->mm()->id());

			foreach(MeshModel* mesh, curmwi->meshDoc()->meshList)
			{
				curmwi->GLA()->rendermodemap[mesh->id()] = RenderMode();
			}

			MultiViewer_Container *tempmvc = curmwi->currentViewContainer();
			GLArea* tempGLArea = NULL;

			curmwi->renderModeTextureWedgeAct->setChecked(false);

			if (!curmwi->meshDoc()->mm()->cm.textures.empty())
			{
				curmwi->renderModeTextureWedgeAct->setChecked(true);
				if (tri::HasPerVertexTexCoord(curmwi->meshDoc()->mm()->cm))
				{
					curmwi->GLA()->setTextureMode(curmwi->meshDoc()->mm()->rmm, GLW::TMPerVert);
					curmwi->meshDoc()->mm()->rmm.setTextureMode(GLW::TMPerVert);
					curmwi->meshDoc()->mm()->rmm.setColorMode(GLW::CMNone);
				}
				if (tri::HasPerWedgeTexCoord(curmwi->meshDoc()->mm()->cm))
				{
					curmwi->GLA()->setTextureMode(curmwi->meshDoc()->mm()->rmm, GLW::TMPerWedgeMulti);
					curmwi->meshDoc()->mm()->rmm.setTextureMode(GLW::TMPerWedgeMulti);
				}

				GLArea* tempGLArea = NULL;
				for (int i = 0; i < 1; i++)
				{
					tempGLArea = curmwi->currentViewContainer()->getViewer(i);
					tempGLArea->updateTexture();
					tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);
				}
			}

			emit curmwi->currentViewContainer()->meshDoc.meshSetChanged();
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}
}

bool delete_command::delete_undo(MeshModel *delToUndo)
{
	if (delToUndo == NULL)
		return false;
	QString tmpFullName = delToUndo->fullName();

	RenderMode *newMRM = new RenderMode(GLW::DMFlat);
	if (tri::HasPerWedgeTexCoord(delToUndo->cm))
	{
		newMRM->setTextureMode(GLW::TMPerWedgeMulti);
		newMRM->setColorMode(GLW::CMPerVert);
		newMRM->setDrawMode(GLW::DMFlat);
	}
	else if (tri::HasPerVertexColor(delToUndo->cm))
	{
		newMRM->setTextureMode(GLW::TMNone);
		newMRM->setColorMode(GLW::CMPerVert);
		newMRM->setDrawMode(GLW::DMFlat);
	}

	if (tmpFullName != NULL)
	{
		QFileInfo info(tmpFullName);
		QString tempfileName = info.baseName() + "." + info.suffix();
		curmwi->currentViewContainer()->meshDoc.addNewMesh(tmpFullName, tempfileName, true, *newMRM);
	}
	else
	{
		curmwi->currentViewContainer()->meshDoc.addNewMesh("", delToUndo->label(), true, *newMRM);
	}

	curmwi->meshDoc()->multiSelectID.insert(curmwi->meshDoc()->mm()->id());
	curmwi->meshDoc()->mm()->updateDataMask(delToUndo->dataMask());
	vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(curmwi->meshDoc()->mm()->cm, delToUndo->cm);
	curmwi->meshDoc()->mm()->cm.Tr = delToUndo->cm.Tr;
	curmwi->meshDoc()->mm()->rmm = delToUndo->rmm;
	curmwi->meshDoc()->mm()->is3mf = delToUndo->is3mf;
	tri::UpdateBounding<CMeshO>::Box(curmwi->meshDoc()->mm()->cm);

	emit curmwi->meshDoc()->meshSetChanged();
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();

	return true;
}

void delete_command::redo()
{
	if (flag)
	{
		foreach(int i, delete_oldIdList)
		{
			curmwi->meshDoc()->multiSelectID.remove(i);
			curmwi->meshDoc()->delMesh(curmwi->meshDoc()->getMesh(i));
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

colorChange_command::colorChange_command(MainWindow * _curmwi, MeshDocument * _md, QSet<int> _shrill_meshes, QMap<int, Color4b> _meshOldColor, QMap<int, int> _dataMask, QMap<int, RenderMode>_meshRenderMode, QMap<int, QList<Color4b>> _recordPerMeshVertexColor, QMap<int, QList<Color4b>> _recordPerMeshFaceColor, bool _flag)
{
	curmwi = _curmwi;
	md = _md;
	shrill_meshes = _shrill_meshes;
	oldMeshColor = _meshOldColor;

	//newColor = md->mm()->cm.C();

	flag = _flag;
	oldMeshDataMask = _dataMask;
	oldMeshRenderMode = _meshRenderMode;

	recordPerMeshVertexColor = _recordPerMeshVertexColor;
	recordPerMeshFaceColor = _recordPerMeshFaceColor;

	foreach(int i, shrill_meshes)
	{
		MeshModel *m = md->getMesh(i);
		newMeshColor.insert(i, m->cm.C());
		newMeshDataMask.insert(i, m->dataMask());
		newMeshRenderMode.insert(i, m->rmm);
	}



	//setText(QObject::tr("color_changed"));
}

void colorChange_command::undo()
{
	int k = 0;
	foreach(int i, shrill_meshes)
	{
		MeshModel *chm = md->getMesh(i);
		chm->clearDataMask(MeshModel::MM_COLOR | MeshModel::MM_VERTCOLOR | MeshModel::MM_FACECOLOR);
		chm->updateDataMask(oldMeshDataMask.value(i));
		qDebug() << chm->hasDataMask(MeshModel::MM_VERTCOLOR);
		qDebug() << chm->hasDataMask(MeshModel::MM_FACECOLOR);
		qDebug() << chm->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
		chm->rmm = oldMeshRenderMode.value(i);

		if (oldMeshDataMask.value(i) & MeshModel::MM_COLOR)
		{
			tri::UpdateColor<CMeshO>::PerVertexConstant(chm->cm, oldMeshColor.value(i));
			tri::UpdateNormal<CMeshO>::PerVertex(chm->cm);
			chm->cm.C() = oldMeshColor.value(i);
		}
		else if ((oldMeshDataMask.value(i) & MeshModel::MM_VERTCOLOR)
			&& (!(oldMeshDataMask.value(i) & MeshModel::MM_WEDGTEXCOORD))
			&& (!(oldMeshDataMask.value(i) & MeshModel::MM_FACECOLOR)))
		{
			QList<Color4b> temp = recordPerMeshVertexColor.value(i); int i = 0;
			for (CMeshO::VertexIterator vi = chm->cm.vert.begin(); vi != chm->cm.vert.end(); ++vi)
			{
				if (!(*vi).IsD())
				{
					vi->C() = temp.at(i);
					++i;
				}
			}
		}
		else if ((oldMeshDataMask.value(i) & MeshModel::MM_FACECOLOR))// && (!(oldMeshDataMask.value(i) & MeshModel::MM_WEDGTEXCOORD)))
		{
			QList<Color4b> temp = recordPerMeshFaceColor.value(i); int i = 0;
			for (CMeshO::FaceIterator fi = chm->cm.face.begin(); fi != chm->cm.face.end(); ++fi)
			{
				if (!(*fi).IsD())
				{
					fi->C() = temp.at(i);
					++i;
				}
			}

			tri::UpdateColor<CMeshO>::PerVertexConstant(chm->cm, Color4b::White);
			tri::UpdateNormal<CMeshO>::PerVertex(chm->cm);
			chm->cm.C() = Color4b::White;
		}
		else
		{
			tri::UpdateColor<CMeshO>::PerVertexConstant(chm->cm, Color4b::White);
			tri::UpdateNormal<CMeshO>::PerVertex(chm->cm);
			chm->cm.C() = Color4b::White;
		}

		int e = chm->dataMask();
		curmwi->GLA()->updateRendermodemapSiganl = true;
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void colorChange_command::redo()
{
	if (flag)
	{
		foreach(int i, shrill_meshes)
		{
			MeshModel *chm = md->getMesh(i);
			chm->updateDataMask(newMeshDataMask.value(i));
			chm->rmm = newMeshRenderMode.value(i);

			if (chm->hasDataMask(MeshModel::MM_FACECOLOR) && !(chm->hasDataMask(MeshModel::MM_WEDGTEXCOORD)))
				chm->clearDataMask(MeshModel::MM_FACECOLOR);

			tri::InitVertexIMark(chm->cm);
			tri::UpdateColor<CMeshO>::PerVertexConstant(chm->cm, newMeshColor.value(i));
			tri::UpdateNormal<CMeshO>::PerVertex(chm->cm);
			chm->cm.C() = newMeshColor.value(i);
			curmwi->GLA()->updateRendermodemapSiganl = true;
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

packing_command::packing_command(MainWindow * _curmwi, MeshDocument *_md, QSet<int> _selected_meshes, QMap<int, Point3f>_savePos, bool _flag) :curmwi(_curmwi)
{
	md = _md;
	savePos = _savePos;
	flag = _flag;
	shrill_meshes = _selected_meshes;

	//setText(QObject::tr("packing_command"));

	new_savePos.clear();

	//foreach(MeshModel *tmm, md->meshList)
	//{
	foreach(int i, shrill_meshes)
	{
		MeshModel *tmm = md->getMesh(i);
		new_savePos.insert(tmm->id(), tmm->cm.bbox.Center());
	}
}
void packing_command::undo()
{
	Matrix44m m_transform;

	/*foreach(MeshModel *tmm, md->meshList)
	{*/
	foreach(int i, shrill_meshes)
	{
		MeshModel *tmm = md->getMesh(i);
		Matrix44m save_mtr = tmm->cm.Tr;
		Matrix44m wholeTransformMatrix;
		Point3f temp = savePos.value(tmm->id());
		m_transform.SetTranslate(temp - tmm->cm.bbox.Center());

		wholeTransformMatrix.SetIdentity();

		tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, m_transform, true);
		tri::UpdateBounding<CMeshO>::Box(tmm->cm);
		tmm->UpdateBoxAndNormals();

		tmm->cm.Tr = save_mtr * m_transform;
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void packing_command::redo()
{
	if (flag)
	{
		Matrix44m m_transform;

		/*foreach(MeshModel *tmm, md->meshList)
		{*/
		foreach(int i, shrill_meshes)
		{
			MeshModel *tmm = md->getMesh(i);
			Matrix44m save_mtr = tmm->cm.Tr;
			Matrix44m wholeTransformMatrix;
			Point3f temp = new_savePos.value(tmm->id());
			m_transform.SetTranslate(temp - tmm->cm.bbox.Center());

			wholeTransformMatrix.SetIdentity();

			tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, m_transform, true);
			tri::UpdateBounding<CMeshO>::Box(tmm->cm);
			tmm->UpdateBoxAndNormals();

			tmm->cm.Tr = save_mtr * m_transform;
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

packing_command::~packing_command()
{

}

packing_rotate_command::packing_rotate_command(MainWindow * _curmwi, MeshDocument *_md, QSet<int> _selected_meshes, QMap<int, Point3f> _savePos, QMap<int, Point3d> _firstRotation, QMap<int, Point3d>  _secondRotation, bool _flag)
	:curmwi(_curmwi)
{
	md = _md;
	savePos = _savePos;
	firstRotation = _firstRotation;
	secondRotation = _secondRotation;
	shrill_meshes = _selected_meshes;
	flag = _flag;
	//setText(QObject::tr("packing_with_rotate"));

	new_savePos.clear();

	/*foreach(MeshModel *tmm, md->meshList)
	{*/
	foreach(int i, shrill_meshes)
	{
		MeshModel *tmm = md->getMesh(i);
		new_savePos.insert(tmm->id(), tmm->cm.bbox.Center());
	}
}

void packing_rotate_command::rotate_mesh(CMeshO &rm, float degree_offset, Point3f Axis)
{
	Matrix44m translate_to_origin;
	Matrix44m translate_to_box_center;
	Matrix44m rotate_m;
	Matrix44m wholeTransformMatrix;

	translate_to_origin.SetTranslate(-rm.bbox.Center());
	rotate_m.SetRotateDeg(degree_offset, Axis);
	translate_to_box_center.SetTranslate(rm.bbox.Center());
	wholeTransformMatrix = (translate_to_box_center * rotate_m * translate_to_origin);

	rm.Tr = wholeTransformMatrix * rm.Tr;

	tri::UpdatePosition<CMeshO>::Matrix(rm, wholeTransformMatrix, true);
	tri::UpdateBounding<CMeshO>::Box(rm);
}

void packing_rotate_command::undo()
{
	/*foreach(MeshModel *tmm, md->meshList)
	{	*/
	foreach(int i, shrill_meshes)
	{
		MeshModel *tmm = md->getMesh(i);
		Point3d rotationMin = firstRotation.value(tmm->id());
		Point3d rotationMax = secondRotation.value(tmm->id());

		if (rotationMax.X() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(1.0, 0.0, 0.0));
		else if (rotationMax.Y() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(0.0, 1.0, 0.0));
		else if (rotationMax.Z() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(0.0, 0.0, 1.0));

		if (rotationMin.X() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(1.0, 0.0, 0.0));
		else if (rotationMin.Y() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(0.0, 1.0, 0.0));
		else if (rotationMin.Z() == 90.00)
			rotate_mesh(tmm->cm, -90.0, Point3m(0.0, 0.0, 1.0));

		Matrix44m translate_to;
		//translate_to.SetTranslate(sinkBBox.P(3));
		translate_to.SetTranslate(savePos.value(tmm->id()) - new_savePos.value(tmm->id()));

		tmm->cm.Tr = translate_to;// *tmm->cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, translate_to, true);
		tri::UpdateBounding<CMeshO>::Box(tmm->cm);
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void packing_rotate_command::redo()
{
	if (flag)
	{
		/*foreach(MeshModel *tmm, md->meshList)
		{*/
		foreach(int i, shrill_meshes)
		{
			MeshModel *tmm = md->getMesh(i);
			Point3d rotationMin = firstRotation.value(tmm->id());
			Point3d rotationMax = secondRotation.value(tmm->id());

			if (rotationMin.X() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(1.0, 0.0, 0.0));
			else if (rotationMin.Y() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(0.0, 1.0, 0.0));
			else if (rotationMin.Z() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(0.0, 0.0, 1.0));

			if (rotationMax.X() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(1.0, 0.0, 0.0));
			else if (rotationMax.Y() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(0.0, 1.0, 0.0));
			else if (rotationMax.Z() == 90.00)
				rotate_mesh(tmm->cm, 90.0, Point3m(0.0, 0.0, 1.0));

			Matrix44m translate_to;
			//translate_to.SetTranslate(sinkBBox.P(3) - tmm->cm.bbox.P(3));
			translate_to.SetTranslate(new_savePos.value(tmm->id()) - tmm->cm.bbox.Center());

			tmm->cm.Tr = translate_to * tmm->cm.Tr;

			tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, translate_to, true);
			tri::UpdateBounding<CMeshO>::Box(tmm->cm);
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

ToCenter_command::ToCenter_command(MainWindow * _curmwi, MeshDocument *_md, QList<int> _selectedID, QList<Point3f> _savePos, bool _flag)
	:curmwi(_curmwi)
{
	md = _md;
	selectedID = _selectedID;
	savePos = _savePos;

	new_savePos.clear();

	foreach(int i, selectedID)
	{
		new_savePos.push_back(md->getMesh(i)->cm.bbox.Center());
	}

	flag = _flag;
	//setText(QObject::tr("toCenter_command"));
}

void ToCenter_command::undo()
{
	int k = 0;
	foreach(int i, selectedID)
	{
		MeshModel *mm = md->getMesh(i);
		Matrix44m translate_to;
		translate_to.SetTranslate(savePos[k] - mm->cm.bbox.Center());
		k++;

		mm->cm.Tr = translate_to * mm->cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
		tri::UpdateBounding<CMeshO>::Box(mm->cm);
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void ToCenter_command::redo()
{
	if (flag)
	{
		int k = 0;
		foreach(int i, selectedID)
		{
			MeshModel *mm = md->getMesh(i);
			Matrix44m translate_to;
			translate_to.SetTranslate(new_savePos[k] - mm->cm.bbox.Center());
			k++;

			mm->cm.Tr = translate_to * mm->cm.Tr;

			tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
			tri::UpdateBounding<CMeshO>::Box(mm->cm);
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}


flip_command::flip_command(MainWindow * _curmwi, MeshDocument *_md, QSet<int> _selected_meshes, QString _flip_selected, bool _flag) :curmwi(_curmwi)
{
	md = _md;
	selected_meshes = _selected_meshes;
	flip_selected = _flip_selected;
	flag = _flag;
	//setText(QObject::tr("flip_command"));
}

void flip_command::undo()
{
	CMeshO::VertexIterator vi;
	CMeshO::FaceIterator fi;
	Point3m meshCenter;
	Matrix44m editTranslateBackToCenterMatrix;

	foreach(int i, selected_meshes)
	{
		if (flip_selected == "By X-Y plane")
		{
			MeshModel *mirrorMesh = md->getMesh(i);
			meshCenter = mirrorMesh->cm.bbox.Center();

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

			for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
			{
				vi->P().Z() = -vi->P().Z();
			}

			tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
			md->mm()->UpdateBoxAndNormals();
			md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

			editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
		}
		else if (flip_selected == "By Y-Z plane")
		{
			MeshModel *mirrorMesh = md->getMesh(i);
			meshCenter = mirrorMesh->cm.bbox.Center();

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

			for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
			{
				vi->P().X() = -vi->P().X();
			}

			tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
			md->mm()->UpdateBoxAndNormals();
			md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

			editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
		}
		else if (flip_selected == "By X-Z plane")
		{
			MeshModel *mirrorMesh = md->getMesh(i);
			meshCenter = mirrorMesh->cm.bbox.Center();

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

			for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
			{
				vi->P().Y() = -vi->P().Y();
			}

			tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
			md->mm()->UpdateBoxAndNormals();
			md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

			editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
			tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
		}
		else return;
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void flip_command::redo()
{
	if (flag)
	{
		CMeshO::VertexIterator vi;
		CMeshO::FaceIterator fi;
		Point3m meshCenter;
		Matrix44m editTranslateBackToCenterMatrix;

		foreach(int i, selected_meshes)
		{
			if (flip_selected == "By X-Y plane")
			{
				MeshModel *mirrorMesh = md->getMesh(i);
				meshCenter = mirrorMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

				for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
				{
					vi->P().Z() = -vi->P().Z();
				}

				tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
				md->mm()->UpdateBoxAndNormals();
				md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
			}
			else if (flip_selected == "By Y-Z plane")
			{
				MeshModel *mirrorMesh = md->getMesh(i);
				meshCenter = mirrorMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

				for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
				{
					vi->P().X() = -vi->P().X();
				}

				tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
				md->mm()->UpdateBoxAndNormals();
				md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
			}
			else if (flip_selected == "By X-Z plane")
			{
				MeshModel *mirrorMesh = md->getMesh(i);
				meshCenter = mirrorMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);

				for (vi = mirrorMesh->cm.vert.begin(); vi != mirrorMesh->cm.vert.end(); ++vi)
				{
					vi->P().Y() = -vi->P().Y();
				}

				tri::Clean<CMeshO>::FlipNormalOutside(mirrorMesh->cm);
				md->mm()->UpdateBoxAndNormals();
				md->mm()->clearDataMask(MeshModel::MM_FACEFACETOPO);

				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirrorMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirrorMesh->cm);
			}
			else return;
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

justify_command::justify_command(MainWindow * _curmwi, MeshDocument *_md, QSet<int> _selected_meshes, QList<Point3f> _savePos, bool _flag) :curmwi(_curmwi)
{
	md = _md;
	selected_meshes = _selected_meshes;
	savePos = _savePos;

	new_savePos.clear();

	foreach(int i, selected_meshes)
	{
		new_savePos.push_back(md->getMesh(i)->cm.bbox.Center());
	}

	flag = _flag;
	//setText(QObject::tr("justify_command"));
}

void justify_command::undo()
{
	int k = 0;
	foreach(int i, selected_meshes)
	{
		MeshModel *mm = md->getMesh(i);
		Matrix44m translate_to;
		translate_to.SetTranslate(savePos[k] - mm->cm.bbox.Center());
		k++;

		mm->cm.Tr = translate_to * mm->cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
		tri::UpdateBounding<CMeshO>::Box(mm->cm);
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void justify_command::redo()
{
	if (flag)
	{
		int k = 0;
		foreach(int i, selected_meshes)
		{
			MeshModel *mm = md->getMesh(i);
			Matrix44m translate_to;
			translate_to.SetTranslate(new_savePos[k] - mm->cm.bbox.Center());
			k++;

			mm->cm.Tr = translate_to * mm->cm.Tr;

			tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
			tri::UpdateBounding<CMeshO>::Box(mm->cm);
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

land_command::land_command(MainWindow * _curmwi, MeshDocument *_md, QSet<int> _selected_meshes, QList<Point3f> _savePos, bool _flag) :curmwi(_curmwi)
{
	md = _md;
	selected_meshes = _selected_meshes;
	savePos = _savePos;

	new_savePos.clear();

	foreach(int i, selected_meshes)
	{
		new_savePos.push_back(md->getMesh(i)->cm.bbox.Center());
	}

	flag = _flag;
	//setText(QObject::tr("land_command"));
}

void land_command::undo()
{
	int k = 0;
	foreach(int i, selected_meshes)
	{
		MeshModel *mm = md->getMesh(i);
		Matrix44m translate_to;
		translate_to.SetTranslate(savePos[k] - mm->cm.bbox.Center());
		k++;

		mm->cm.Tr = translate_to * mm->cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
		tri::UpdateBounding<CMeshO>::Box(mm->cm);
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void land_command::redo()
{
	if (flag)
	{
		int k = 0;
		foreach(int i, selected_meshes)
		{
			MeshModel *mm = md->getMesh(i);
			Matrix44m translate_to;
			translate_to.SetTranslate(new_savePos[k] - mm->cm.bbox.Center());
			k++;

			mm->cm.Tr = translate_to * mm->cm.Tr;

			tri::UpdatePosition<CMeshO>::Matrix(mm->cm, translate_to, true);
			tri::UpdateBounding<CMeshO>::Box(mm->cm);
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

landAll_command::landAll_command(MainWindow * _curmwi, MeshDocument *_md, QMap<int, Point3f> _savePos, bool _flag) :curmwi(_curmwi)
{
	md = _md;
	savePos = _savePos;

	new_savePos.clear();

	foreach(MeshModel *tmm, md->meshList)
	{
		new_savePos.insert(tmm->id(), tmm->cm.bbox.Center());
	}

	flag = _flag;
	//setText(QObject::tr("landAll_command"));
}

void landAll_command::undo()
{
	foreach(MeshModel *tmm, md->meshList)
	{
		Matrix44m translate_to;
		translate_to.SetTranslate(savePos.value(tmm->id()) - tmm->cm.bbox.Center());

		tmm->cm.Tr = translate_to * tmm->cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, translate_to, true);
		tri::UpdateBounding<CMeshO>::Box(tmm->cm);
	}
	curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
}

void landAll_command::redo()
{
	if (flag)
	{
		foreach(MeshModel *tmm, md->meshList)
		{
			Matrix44m translate_to;
			translate_to.SetTranslate(new_savePos.value(tmm->id()) - tmm->cm.bbox.Center());

			tmm->cm.Tr = translate_to * tmm->cm.Tr;

			tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, translate_to, true);
			tri::UpdateBounding<CMeshO>::Box(tmm->cm);
		}
		curmwi->GLA()->reFreshGlListlist(); curmwi->GLA()->setHastoRefreshVBO();
	}

	flag = true;
}

reset_command::reset_command(MainWindow * _curmwi, QList<MeshModel *> _deletedMeshList, QList<int> _deletedMeshId, QMap<int, Point3f> _resetScaleN, bool _flag) :mw(_curmwi)
{

	deletedMeshList = _deletedMeshList;
	deletedMeshId = _deletedMeshId;
	resetScaleN = _resetScaleN;
	flag = _flag;
	icList.clear();

	QList<int> list = mw->currentViewContainer()->meshDoc.multiSelectID.toList();
	qSort(list.begin(), list.end(), qLess<int>());
	newIdList.clear();

	foreach(int i, list)
	{
		newIdList.insert(i, mw->meshDoc()->getMesh(i)->is3mf);

		if (mw->meshDoc()->getMesh(i)->is3mf == false)
		{
			//import_command *ic = new import_command(mw, mw->meshDoc()->getMesh(i)->fullName(), resetScaleN.value(i), i, false);
			import_command *ic = new import_command(mw, mw->meshDoc()->getMesh(i)->fullName(), resetScaleN.value(i), i, true);
			icList.insert(i, ic);
		}
	}

	dc = new delete_command(mw, deletedMeshList, deletedMeshId, true);

	//setText(QObject::tr("reset_command"));
}

void reset_command::undo()
{
	foreach(int i, newIdList.keys())
	{
		if (icList.contains(i))
		{
			icList.value(i)->undo();
		}

		else
		{
			mw->meshDoc()->delMesh(mw->meshDoc()->getMesh(i));
		}
	}
	mw->GLA()->reFreshGlListlist(); mw->GLA()->setHastoRefreshVBO();

	dc->undo();
	mw->objList->updateTable();
}

void reset_command::redo()
{
	if (flag)
	{
		mw->meshDoc()->multiSelectID.clear();
		dc->redo();

		foreach(int i, newIdList.keys())
		{
			if (newIdList.value(i) == false)
				icList.value(i)->redo();
			else
			{
				MeshModel *rm = new MeshModel(mw->meshDoc(), "", mw->meshDoc()->resetMeshList.value(i)->label());
				rm->updateDataMask(mw->meshDoc()->resetMeshList.value(i)->dataMask());
				vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(rm->cm, mw->meshDoc()->resetMeshList.value(i)->cm);
				rm->cm.Tr = mw->meshDoc()->resetMeshList.value(i)->cm.Tr;
				rm->rmm = mw->meshDoc()->resetMeshList.value(i)->rmm;
				rm->is3mf = mw->meshDoc()->resetMeshList.value(i)->is3mf;

				rm->setid(i);
				rm->is3mf = true;
				mw->meshDoc()->meshList.push_back(rm);
				mw->meshDoc()->multiSelectID.insert(rm->id());
				tri::UpdateBounding<CMeshO>::Box(rm->cm);
				tri::UpdateNormal<CMeshO>::PerFaceNormalized(rm->cm);
				emit mw->meshDoc()->meshSetChanged();
				emit mw->meshDoc()->meshAdded(rm->id(), rm->rmm);

				mw->renderModeTextureWedgeAct->setChecked(false);

				if (!rm->cm.textures.empty())
				{
					mw->renderModeTextureWedgeAct->setChecked(true);
					if (tri::HasPerVertexTexCoord(rm->cm))
					{
						mw->GLA()->setTextureMode(rm->rmm, GLW::TMPerVert);
						rm->rmm.setTextureMode(GLW::TMPerVert);
						rm->rmm.setColorMode(GLW::CMNone);
					}
					if (tri::HasPerWedgeTexCoord(rm->cm))
					{
						mw->GLA()->setTextureMode(rm->rmm, GLW::TMPerWedgeMulti);
						rm->rmm.setTextureMode(GLW::TMPerWedgeMulti);
					}

					GLArea* tempGLArea = NULL;
					for (int i = 0; i < 1; i++)
					{
						tempGLArea = mw->currentViewContainer()->getViewer(i);
						tempGLArea->updateTexture();
						tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);
					}
				}
			}
		}
		mw->GLA()->reFreshGlListlist(); mw->GLA()->setHastoRefreshVBO();

		mw->objList->updateTable();
	}

	flag = true;
}

importFrom_command::importFrom_command(MainWindow *_mw, QList<int> _imported3mfIdList, bool _flag)
{
	mw = _mw;
	imported3mfIdList = _imported3mfIdList;
	flag = _flag;
}

void importFrom_command::undo()
{
	foreach(int i, imported3mfIdList)
	{
		mw->meshDoc()->multiSelectID.remove(i);
		MeshModel *dm = mw->meshDoc()->getMesh(i);
		mw->meshDoc()->delMesh(dm);
	}
	mw->GLA()->reFreshGlListlist(); mw->GLA()->setHastoRefreshVBO();
}

void importFrom_command::redo()
{
	if (flag)
	{
		foreach(int i, imported3mfIdList)
		{
			MeshModel *rm = new MeshModel(mw->meshDoc(), "", mw->meshDoc()->resetMeshList.value(i)->label());
			rm->updateDataMask(mw->meshDoc()->resetMeshList.value(i)->dataMask());
			vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(rm->cm, mw->meshDoc()->resetMeshList.value(i)->cm);
			rm->cm.Tr = mw->meshDoc()->resetMeshList.value(i)->cm.Tr;
			rm->rmm = mw->meshDoc()->resetMeshList.value(i)->rmm;
			rm->is3mf = mw->meshDoc()->resetMeshList.value(i)->is3mf;

			rm->setid(i);
			rm->is3mf = true;
			mw->meshDoc()->meshList.push_back(rm);
			tri::UpdateBounding<CMeshO>::Box(rm->cm);
			tri::UpdateNormal<CMeshO>::PerFaceNormalized(rm->cm);
			emit mw->meshDoc()->meshSetChanged();
			emit mw->meshDoc()->meshAdded(rm->id(), rm->rmm);

			mw->renderModeTextureWedgeAct->setChecked(false);

			if (!rm->cm.textures.empty())
			{
				mw->renderModeTextureWedgeAct->setChecked(true);
				if (tri::HasPerVertexTexCoord(rm->cm))
				{
					mw->GLA()->setTextureMode(rm->rmm, GLW::TMPerVert);
					rm->rmm.setTextureMode(GLW::TMPerVert);
					rm->rmm.setColorMode(GLW::CMNone);
				}
				if (tri::HasPerWedgeTexCoord(rm->cm))
				{
					mw->GLA()->setTextureMode(rm->rmm, GLW::TMPerWedgeMulti);
					rm->rmm.setTextureMode(GLW::TMPerWedgeMulti);
				}

				GLArea* tempGLArea = NULL;
				for (int i = 0; i < 1; i++)
				{
					tempGLArea = mw->currentViewContainer()->getViewer(i);
					tempGLArea->updateTexture();
					tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);
				}
			}
		}
		mw->GLA()->reFreshGlListlist(); mw->GLA()->setHastoRefreshVBO();

		mw->objList->updateTable();
	}

	flag = true;
}