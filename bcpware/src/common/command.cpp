#include "command.h"

Translate_Command::Translate_Command(MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_matrix, bool _flag)
{
	shrill_meshes = _selected_meshes;//¤w¿ïmeshes

	old_Matrix = _old_matrix;

	md = _md;

	new_Matrix.SetIdentity();
	new_Matrix = md->mm()->cm.Tr;

	flag = _flag;
	setText(QObject::tr("translate_command"));

}
Translate_Command::~Translate_Command()
{

}
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
		Matrix44m  save_mtr = m->cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
		tri::UpdateBounding<CMeshO>::Box(m->cm);
		m->UpdateBoxAndNormals();
		m->cm.Tr = save_mtr*d;
	}
	//tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
	//tri::UpdateBounding<CMeshO>::Box(m->cm);
	////editMesh->cm.Tr.SetIdentity();
	//m->UpdateBoxAndNormals();

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
			tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			m->UpdateBoxAndNormals();
		}

	}
	flag = true;



}




Matrix_command::Matrix_command(MainWindowInterface *_curmwi, MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag) :flag(_flag), curmwi(_curmwi)
{
	old_Matrix = _old_Matrix;
	new_Matrix = sm->cm.Tr;
	//new_Matrix = sm->cm.bbox.Center();


	m = sm;
	setText(QObject::tr("Matrix_command_2"));
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
	curmwi->updateTransformMenu();
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
		curmwi->updateTransformMenu();
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
Scale_command::Scale_command(MeshDocument *_md, QSet<int> _selected_meshes, const Point3f &_dim, bool _flag)
:md(_md), shrill_meshes(_selected_meshes), old_Dim(_dim), flag(_flag)
{
	new_Dim = md->mm()->cm.bbox.Dim();
	setText(QObject::tr("scale_command"));
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

	}flag = true;
}

Scale_command::~Scale_command()
{
}

//Rotate_command::Rotate_command(MeshModel *rm, const Matrix44m &_old_matrix)
//{
//	old_matrix = _old_matrix;
//	new_matrix = rm->cm.Tr;
//	m = rm;
//	setText(QObject::tr("rotate_command"));
//}
Rotate_command::Rotate_command(MeshDocument *_md, QSet<int> _selected_meshes, const Matrix44m &_old_matrix, bool _flag)
:md(_md), shrill_meshes(_selected_meshes), old_matrix(_old_matrix), flag(_flag)
{
	new_matrix.SetIdentity();
	new_matrix = md->mm()->cm.Tr;
	int x = md->mm()->id();
	setText(QObject::tr("rotate_command"));



	Matrix44m M = Inverse(old_matrix)*new_matrix;//Ax= B, x=A-1B
	double alpha, beta, gamma; // rotations around the x,y and z axis
	beta = asin(M[0][2]);
	double cosbeta = cos(beta);
	if (math::Abs(cosbeta) > 1e-5)
	{
		alpha = asin(-M[1][2] / cosbeta);
		if ((M[2][2] / cosbeta) < 0) alpha = M_PI - alpha;
		gamma = asin(-M[0][1] / cosbeta);
		if ((M[0][0] / cosbeta) < 0) gamma = M_PI - gamma;
	}
	else
	{
		alpha = asin(-M[1][0]);
		if (M[1][1] < 0) alpha = M_PI - alpha;
		gamma = 0;
	}

	RotV[0] = math::ToDeg(alpha);
	RotV[1] = math::ToDeg(beta);
	RotV[2] = math::ToDeg(gamma);



}


void Rotate_command::undo()
{

	Matrix44m editTranslateBackToCenterMatrix;
	Matrix44m editTranslateGoToPositionMatrix, wholeTransformMatrix;


	back_r_matrix.SetIdentity();


	if (abs(RotV[0]) > 1e-5)
	{
		back_r_matrix.SetRotateDeg(-RotV[0], Point3m(1, 0, 0));
		Axis = Point3m(1, 0, 0);
	}
	else if (abs(RotV[1]) > 1e-5)
	{
		back_r_matrix.SetRotateDeg(-RotV[1], Point3m(0, 1, 0));
		Axis = Point3m(0, 1, 0);
	}
	else if (abs(RotV[2]) > 1e-5)
	{
		back_r_matrix.SetRotateDeg(-RotV[2], Point3m(0, 0, 1));
		Axis = Point3m(0, 0, 1);
	}
	back_r_matrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));


	foreach(int i, shrill_meshes)
	{
		MeshModel *m = md->getMesh(i);
		//if (m != NULL){}
		Point3m meshCenter = m->cm.bbox.Center();
		Matrix44m save_mtr = m->cm.Tr;

		editTranslateBackToCenterMatrix.SetIdentity();
		editTranslateGoToPositionMatrix.SetIdentity();
		wholeTransformMatrix.SetIdentity();

		editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
		editTranslateGoToPositionMatrix.SetTranslate(meshCenter);//***original to position

		wholeTransformMatrix = editTranslateGoToPositionMatrix*back_r_matrix*editTranslateBackToCenterMatrix;

		tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
		tri::UpdateBounding<CMeshO>::Box(m->cm);
		//editMesh->cm.Tr.SetIdentity();
		m->UpdateBoxAndNormals();

		m->cm.Tr = save_mtr*wholeTransformMatrix;
	}
}
void Rotate_command::redo()
{
	if (flag){

		Matrix44m editTranslateBackToCenterMatrix;
		Matrix44m editTranslateGoToPositionMatrix, wholeTransformMatrix;

		back_r_matrix.SetIdentity();


		if (abs(RotV[0]) > 1e-5)
		{
			back_r_matrix.SetRotateDeg(RotV[0], Point3m(1, 0, 0));
			Axis = Point3m(1, 0, 0);
		}
		else if (abs(RotV[1]) > 1e-5)
		{
			back_r_matrix.SetRotateDeg(RotV[1], Point3m(0, 1, 0));
			Axis = Point3m(0, 1, 0);
		}
		else if (abs(RotV[2]) > 1e-5)
		{
			back_r_matrix.SetRotateDeg(RotV[2], Point3m(0, 0, 1));
			Axis = Point3m(0, 0, 1);
		}
		back_r_matrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));






		foreach(int i, shrill_meshes)
		{
			MeshModel *m = md->getMesh(i);
			Point3m meshCenter = m->cm.bbox.Center();
			Matrix44m save_mtr = m->cm.Tr;

			editTranslateBackToCenterMatrix.SetIdentity();
			editTranslateGoToPositionMatrix.SetIdentity();
			wholeTransformMatrix.SetIdentity();

			editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
			editTranslateGoToPositionMatrix.SetTranslate(meshCenter);//***original to position

			wholeTransformMatrix = editTranslateGoToPositionMatrix*back_r_matrix*editTranslateBackToCenterMatrix;

			tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(m->cm);
			//editMesh->cm.Tr.SetIdentity();
			m->UpdateBoxAndNormals();
			m->cm.Tr = save_mtr*wholeTransformMatrix;
		}
	}flag = true;
}
Rotate_command::~Rotate_command()
{
}


paste_command::paste_command(MeshDocument *_md, bool _flag)
:md(_md), flag(_flag)
{
	selectedMeshId = md->getSelectedCopyMesh()->id();
	pasteMeshId = md->mm()->id();
	setText(QObject::tr("paste_command"));

}

paste_command::~paste_command()
{
}

void paste_command::undo()
{
	//delete paste mesh
	//md->delMeshes();
	md->delMesh(md->getMesh(pasteMeshId));
	pasteMeshId = -1;
}
void paste_command::redo()
{
	if (flag){
		//do ctrl+c ctrl+v 
		/*mw()->setCopyMesh();
		if (md()->selCm() != NULL)
		mw()->pastSelectMesh();
		*/
		md->setSelectedCopy(md->getMesh(selectedMeshId));
		if (md->selCm() != NULL)
		{
			md->copyMesh(md->selCm());
			pasteMeshId = md->mm()->id();

		}

	}flag = true;
}

packing_command::packing_command(MeshDocument *_md, QMap<int, Point3f>_savePos, QMap<int, Matrix44m> _all_old_Matrix, bool _flag)
:all_old_matrix(_all_old_Matrix), savePos(_savePos)
{
	md = _md;
	flag = _flag;
	foreach(MeshModel *tmm, md->meshList)
	{
		all_new_matrix.insert(tmm->id(), tmm->cm.Tr);
	}
	setText(QObject::tr("packing_command"));

}
void packing_command::undo()
{
	Matrix44m m_transform;
	//foreach(MeshModel *tmm, md->meshList)
	//{
	//	m_transform = Inverse(all_old_matrix.value(tmm->id()))*(all_new_matrix.value(tmm->id()));
	//	tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, m_transform, true);
	//	tri::UpdateBounding<CMeshO>::Box(tmm->cm);
	//	//editMesh->cm.Tr.SetIdentity();
	//	tmm->UpdateBoxAndNormals();
	//}

	foreach(MeshModel *tmm, md->meshList)
	{
		Matrix44m save_mtr = tmm->cm.Tr;
		Matrix44m wholeTransformMatrix;
		Point3f temp = savePos.value(tmm->id());
		m_transform.SetTranslate(temp-tmm->cm.bbox.Center());

		wholeTransformMatrix.SetIdentity();
		
		tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, m_transform, true);
		tri::UpdateBounding<CMeshO>::Box(tmm->cm);
		//editMesh->cm.Tr.SetIdentity();
		tmm->UpdateBoxAndNormals();

		tmm->cm.Tr = save_mtr*m_transform;
	}

}

void packing_command::redo()
{
	if (flag)
	{

		Matrix44m m_transform;		

		foreach(MeshModel *tmm, md->meshList)
		{
			Matrix44m mm4 = all_new_matrix.value(tmm->id()) - tmm->cm.Tr;
			Point3f offset =  mm4.GetColumn3(3);
			
			
			Matrix44m save_mtr = tmm->cm.Tr;			
			m_transform.SetIdentity();
			m_transform.SetTranslate(offset );			

			tri::UpdatePosition<CMeshO>::Matrix(tmm->cm, m_transform, true);
			tri::UpdateBounding<CMeshO>::Box(tmm->cm);
			//editMesh->cm.Tr.SetIdentity();
			tmm->UpdateBoxAndNormals();

			tmm->cm.Tr = save_mtr*m_transform;
		}

	}flag = true;

}
packing_command::~packing_command()
{

}



ToCenter_command::ToCenter_command(MeshModel *sm, const Matrix44m &_old_Matrix, bool _flag)
{
	old_Matrix = _old_Matrix;
	new_Matrix.SetIdentity();
	//new_Matrix = md->mm()->cm.Tr;
	m = sm;

}
void ToCenter_command::undo()
{
	Matrix44m b = old_Matrix;
	Matrix44m a = (old_Matrix - new_Matrix);
	Point3m c = a.GetColumn3(3);
	Matrix44m d;
	d.SetIdentity();
	d.SetTranslate(c);
	/*foreach(int i, shrill_meshes)
	{
		MeshModel *m = md->getMesh(i);
		Matrix44m  save_mtr = m->cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(m->cm, d, true);
		tri::UpdateBounding<CMeshO>::Box(m->cm);
		m->UpdateBoxAndNormals();
		m->cm.Tr = save_mtr*d;
	}*/
}
void ToCenter_command::redo()
{

}
ToCenter_command::~ToCenter_command()
{}
