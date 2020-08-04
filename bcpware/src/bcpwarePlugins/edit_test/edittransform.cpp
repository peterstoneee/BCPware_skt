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
#include "edittransform.h"



EditTransform::EditTransform()
{
	typeList << EDP_EDIT_TRANSFORM
		<< EDP_EDIT_ROTATE
		<< EDP_EDIT_SCALE;

	screen_xaxis = { 0, 0, 0 };
	screen_yaxis = { 0, 0, 0 };
	screen_zaxis = { 0, 0, 0 };

	foreach(FilterIDType tt, types())
	{
		//qDebug() << "EditTransform" << tt;
		actionList << new QAction(filterName(tt), this);
		qDebug() << "filterName(tt)" << filterName(tt);
	}
	delta_Transform.SetIdentity();


	//qDebug() << "typeList" << types().size();
}
void EditTransform::initParameterSet(QAction *editAction, MeshDocument &md, RichParameterSet & parlst)
{
	switch (ID(editAction))
	{
	case EDP_EDIT_TRANSFORM:
	{
		parlst.addParam(new RichFloat("X_translate", 0, "X", ""));
		parlst.addParam(new RichFloat("Y_translate", 0, "Y", ""));
		parlst.addParam(new RichFloat("Z_translate", 0, "Z", ""));

	}
	break;
	case EDP_EDIT_ROTATE:
	{
		QStringList rotate_Axis;
		rotate_Axis.push_back("X_Axis");
		rotate_Axis.push_back("Y_Axis");
		rotate_Axis.push_back("Z_Axis");
		parlst.addParam(new RichEnum("Axis", 0, rotate_Axis, tr("Axis"), tr("")));
		parlst.addParam(new RichFloat("rotate_degree", 0, "Degree", ""));

	}
	break;
	case EDP_EDIT_SCALE:
	{
		parlst.addParam(new RichFloat("X_scale", 1, "X", ""));
		parlst.addParam(new RichFloat("Y_scale", 1, "Y", ""));
		parlst.addParam(new RichFloat("Z_scale", 1, "Z", ""));
		parlst.addParam(new RichBool("Lock_axis", true, "Lock", ""));
		parlst.addParam(new RichFloat("scale_all_axis", 1, "Scale Ratio", ""));
	}
	break;
	default:
		break;
	}

}
QString EditTransform::filterName(FilterIDType editEnum) const
{

	switch (editEnum)
	{
	case EDP_EDIT_TRANSFORM: return tr("edit_translate");
	case EDP_EDIT_ROTATE: return tr("edit_rotate");
	case EDP_EDIT_SCALE: return tr("edit_scale");

	default: assert(0);
	}

	return QString();

}
bool EditTransform::applyEdit_v2(QAction *editAction, MeshDocument &md, RichParameterSet &par, CallBackPos *cb/*,GLArea *gla*/)
{
	if (md.multiSelectID.size() > 1)
	{
		/*
		multiselect Transform Test
		*/
		Box3m selBox = md.selBBox();
		foreach(int i, md.multiSelectID)
		{
			MeshModel * editMesh = md.getMesh(i);
			delta_Transform.SetIdentity();
			wholeTransformMatrix.SetIdentity();
			switch (ID(editAction))
			{
			case EDP_EDIT_TRANSFORM:
			{
				x_offset = par.getFloat("X_translate");
				y_offset = par.getFloat("Y_translate");
				z_offset = par.getFloat("Z_translate");
				delta_Transform.SetTranslate(*new Point3m(x_offset, y_offset, z_offset));
				wholeTransformMatrix = delta_Transform;
				//wholeTransformMatrix = delta_Transform * original_Transform;
				tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
				//editMesh->cm.Tr.SetIdentity();
				editMesh->UpdateBoxAndNormals();

				//applyMatrix(editMesh);
				//qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
			}break;
			case EDP_EDIT_ROTATE:
			{
				if (md.isPrint_item(*editMesh))
				{
					Point3m axis, tranVec;
					Matrix44m trTran, trTranInv;

					switch (par.getEnum("Axis"))
					{
					case 0: axis = Point3m(1, 0, 0); break;
					case 1: axis = Point3m(0, 1, 0); break;
					case 2: axis = Point3m(0, 0, 1); break;
					}

					degree_offset = par.getFloat("rotate_degree");


					Matrix44m editRotateMatrix;
					Matrix44m editTranslateBackToCenterMatrix;
					Matrix44m editTranslateGoToPositionMatrix;

					Point3m meshCenter = selBox.Center();
					editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
					editRotateMatrix.SetRotateDeg(degree_offset, axis);
					editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
					editTranslateGoToPositionMatrix.SetTranslate(meshCenter);//***original to position

					wholeTransformMatrix = editTranslateGoToPositionMatrix*editRotateMatrix*editTranslateBackToCenterMatrix;
					//editMesh->mesh_rotate_list.push_back(std::pair<Point3m,Matrix44m>(axis, editRotateMatrix));

					tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
					tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
					//editMesh->cm.Tr.SetIdentity();
					editMesh->UpdateBoxAndNormals();
				}

			}break;
			case EDP_EDIT_SCALE:
			{
				if (md.isPrint_item(*editMesh))
				{
					Matrix44m scaleMatrix;
					scale_x_offset = par.getFloat("X_scale");
					scale_y_offset = par.getFloat("Y_scale");
					scale_z_offset = par.getFloat("Z_scale");
					scale_all_offset = par.getFloat("scale_all_axis");
					lockAxis = par.getBool("Lock_axis");
					if (lockAxis)
					{
						scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
						scaleMatrix.SetScale(*new Point3m(scale_all_offset, scale_all_offset, scale_all_offset));
					}
					else
					{
						scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
						scaleMatrix.SetScale(*new Point3m(scale_x_offset, scale_y_offset, scale_z_offset));
					}


					//***position

					Matrix44m scale_Translate_BackTo_CenterMatrix;
					Matrix44m scale_Translate_GoBackTo_OriginalMatrix;

					Point3m meshCenter = selBox.Center();
					scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
					scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
					wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
					//


					tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
					tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
					//editMesh->cm.Tr.SetIdentity();
					editMesh->UpdateBoxAndNormals();
					//qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
				}
			}break;
			}
			editMesh->cm.Tr = wholeTransformMatrix * original_Transform;
		}

		/*
		============================================================================
		*/
		/*
		rotate each objects
		*/
		//foreach(int i, md.multiSelectID)
		//{
		//	MeshModel * editMesh = md.getMesh(i);
		//	
		//	delta_Transform.SetIdentity();
		//	wholeTransformMatrix.SetIdentity();
		//	//***20150827
		//	original_Transform = editMesh->cm.Tr;			
		//	switch (ID(editAction))
		//	{

		//	case EDP_EDIT_TRANSFORM:
		//	{
		//							   x_offset = par.getFloat("X_translate");
		//							   y_offset = par.getFloat("Y_translate");
		//							   z_offset = par.getFloat("Z_translate");
		//							   delta_Transform.SetTranslate(*new Point3m(x_offset, y_offset, z_offset));
		//							   wholeTransformMatrix = delta_Transform;
		//							   //wholeTransformMatrix = delta_Transform * original_Transform;
		//							   tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
		//							   tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
		//							   //editMesh->cm.Tr.SetIdentity();
		//							   editMesh->UpdateBoxAndNormals();

		//							   //applyMatrix(editMesh);
		//							   //qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
		//	}break;
		//	case EDP_EDIT_ROTATE:
		//	{
		//							if (md.isPrint_item(*editMesh))
		//							{
		//								Point3m axis, tranVec;
		//								Matrix44m trTran, trTranInv;

		//								switch (par.getEnum("Axis"))
		//								{
		//								case 0: axis = Point3m(1, 0, 0); break;
		//								case 1: axis = Point3m(0, 1, 0); break;
		//								case 2: axis = Point3m(0, 0, 1); break;
		//								}

		//								degree_offset = par.getFloat("rotate_degree");
		//								

		//								Matrix44m editRotateMatrix;
		//								Matrix44m editTranslateBackToCenterMatrix;
		//								Matrix44m editTranslateGoToPositionMatrix;

		//								Point3m meshCenter = editMesh->cm.bbox.Center();
		//								editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
		//								editRotateMatrix.SetRotateDeg(degree_offset, axis);
		//								editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
		//								editTranslateGoToPositionMatrix.SetTranslate(meshCenter);//***original to position

		//								wholeTransformMatrix = editTranslateGoToPositionMatrix*editRotateMatrix*editTranslateBackToCenterMatrix;
		//								//editMesh->mesh_rotate_list.push_back(std::pair<Point3m,Matrix44m>(axis, editRotateMatrix));



		//								tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
		//								tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
		//								//editMesh->cm.Tr.SetIdentity();
		//								editMesh->UpdateBoxAndNormals();
		//							}

		//	}break;
		//	case EDP_EDIT_SCALE:
		//	{
		//						   if (md.isPrint_item(*editMesh))
		//						   {
		//							   Matrix44m scaleMatrix;
		//							   scale_x_offset = par.getFloat("X_scale");
		//							   scale_y_offset = par.getFloat("Y_scale");
		//							   scale_z_offset = par.getFloat("Z_scale");
		//							   scale_all_offset = par.getFloat("scale_all_axis");
		//							   lockAxis = par.getBool("Lock_axis");
		//							   if (lockAxis)
		//							   {
		//								   scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
		//								   scaleMatrix.SetScale(*new Point3m(scale_all_offset, scale_all_offset, scale_all_offset));
		//							   }
		//							   else
		//							   {
		//								   scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
		//								   scaleMatrix.SetScale(*new Point3m(scale_x_offset, scale_y_offset, scale_z_offset));
		//							   }


		//							   //***position

		//							   Matrix44m scale_Translate_BackTo_CenterMatrix;
		//							   Matrix44m scale_Translate_GoBackTo_OriginalMatrix;

		//							   Point3m meshCenter = editMesh->cm.bbox.Center();
		//							   scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
		//							   scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
		//							   wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
		//							   //


		//							   tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
		//							   tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
		//							   //editMesh->cm.Tr.SetIdentity();
		//							   editMesh->UpdateBoxAndNormals();
		//							   //qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
		//						   }
		//	}break;
		//	}
		//	editMesh->cm.Tr = wholeTransformMatrix * original_Transform;

		//}

	}
	else{
		MeshModel *editMesh = md.mm();
		/*qDebug() << "applyEdit_v2";
		x_offset = par.getFloat("X_rotate");
		y_offset = par.getFloat("Y_rotate");
		z_offset = par.getFloat("Z_rotate");
		delta_Transform.SetTranslate(*new Point3m(x_offset, y_offset, z_offset));
		tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, delta_Transform, true);
		tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
		editMesh->cm.Tr.SetIdentity();
		editMesh->UpdateBoxAndNormals();
		qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;*/
		delta_Transform.SetIdentity();
		wholeTransformMatrix.SetIdentity();
		//***20150827
		original_Transform = editMesh->cm.Tr;

		/*if (md.hasContainStringMesh("_temp_outlines"))
		{
		QVector<MeshModel *> vmm;
		md.getMeshByContainString("_temp_outlines", vmm);
		foreach(MeshModel *dmm, vmm)
		{
		qDebug("meshname" + (*dmm).label().toLatin1());
		md.delMesh(dmm);
		}
		MeshModel* tempG = md.getMeshByLabel("_temp_outlines");
		md.delMesh(tempG);
		if (md.hasContainStringMesh("_temp_ol_mesh"))
		{
		QVector<MeshModel *> vmm;
		md.getMeshByContainString("_temp_ol_mesh", vmm);
		foreach(MeshModel *dmm, vmm)
		{
		qDebug("meshname" + (*dmm).label().toLatin1());
		md.delMesh(dmm);
		}
		}
		qDebug("filter_1_place");
		}*/

		//if (editMesh)
		switch (ID(editAction))
		{

		case EDP_EDIT_TRANSFORM:
		{
			x_offset = par.getFloat("X_translate");
			y_offset = par.getFloat("Y_translate");
			z_offset = par.getFloat("Z_translate");
			delta_Transform.SetTranslate(*new Point3m(x_offset, y_offset, z_offset));
			wholeTransformMatrix = delta_Transform;
			//wholeTransformMatrix = delta_Transform * original_Transform;
			tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
			//editMesh->cm.Tr.SetIdentity();
			editMesh->UpdateBoxAndNormals();

			//applyMatrix(editMesh);
			//qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
		}break;
		case EDP_EDIT_ROTATE:
		{
			if (md.isPrint_item(*editMesh))
			{
				Point3m axis, tranVec;
				Matrix44m trTran, trTranInv;

				switch (par.getEnum("Axis"))
				{
				case 0: axis = Point3m(1, 0, 0); break;
				case 1: axis = Point3m(0, 1, 0); break;
				case 2: axis = Point3m(0, 0, 1); break;
				}

				degree_offset = par.getFloat("rotate_degree");

				//editMesh->cm.Tr;
				Matrix44m editRotateMatrix;
				Matrix44m editTranslateBackToCenterMatrix;
				Matrix44m editTranslateGoToPositionMatrix;

				Point3m meshCenter = editMesh->cm.bbox.Center();
				editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
				editRotateMatrix.SetRotateDeg(degree_offset, axis);
				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				editTranslateGoToPositionMatrix.SetTranslate(meshCenter);//***original to position

				editMesh->mesh_rotate_list.push_back(std::pair<Point3m, Matrix44m>(axis, editRotateMatrix));
				wholeTransformMatrix = editTranslateGoToPositionMatrix*editRotateMatrix*editTranslateBackToCenterMatrix;

				tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
				//editMesh->cm.Tr.SetIdentity();
				editMesh->UpdateBoxAndNormals();
			}

		}break;
		case EDP_EDIT_SCALE:
		{
			if (md.isPrint_item(*editMesh))
			{
				Matrix44m scaleMatrix;
				scale_x_offset = par.getFloat("X_scale");
				scale_y_offset = par.getFloat("Y_scale");
				scale_z_offset = par.getFloat("Z_scale");
				scale_all_offset = par.getFloat("scale_all_axis");
				lockAxis = par.getBool("Lock_axis");
				if (lockAxis)
				{
					scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
					scaleMatrix.SetScale(*new Point3m(scale_all_offset, scale_all_offset, scale_all_offset));
				}
				else
				{
					scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
					scaleMatrix.SetScale(*new Point3m(scale_x_offset, scale_y_offset, scale_z_offset));
				}


				//***position

				Matrix44m scale_Translate_BackTo_CenterMatrix;
				Matrix44m scale_Translate_GoBackTo_OriginalMatrix;

				Point3m meshCenter = editMesh->cm.bbox.Center();
				scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
				scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
				wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;
				//


				tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
				//editMesh->cm.Tr.SetIdentity();
				editMesh->UpdateBoxAndNormals();
				//qDebug() << "x_offset y_offset z_offset" << x_offset << y_offset << z_offset;
			}
		}break;
		}
		editMesh->cm.Tr = wholeTransformMatrix * original_Transform;
		return true;
	}
}
void EditTransform::allMeshApplyMatrix(MeshDocument &md, GLArea *glaa)
{
	foreach(int i, md.multiSelectID)
	{
		MeshModel *tempModel = md.getMesh(i);
		applyMatrix(tempModel, glaa);
	}
}
void EditTransform::applyMatrix(MeshModel *editMesh, GLArea * gla)
{
	//*********註解掉*********************
	//delta_Transform.SetIdentity();
	//wholeTransformMatrix.SetIdentity();
	////delta_Transform.SetTranslate(*new Point3m(x_offset, y_offset, z_offset));

	//Matrix44m editRotateMatrix;
	//Matrix44m editTranslateBackToCenterMatrix;
	//Matrix44m editTranslateGoToPositionMatrix;

	//Point3m axis(1., 0., 0.);
	//Point3m meshCenter = editMesh->cm.bbox.Center();
	//editRotateMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
	//editRotateMatrix.SetRotateDeg(x_offset, axis);
	//editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));
	//editTranslateGoToPositionMatrix.SetTranslate(meshCenter);

	//wholeTransformMatrix = editTranslateGoToPositionMatrix*editRotateMatrix*editTranslateBackToCenterMatrix;

	//tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, wholeTransformMatrix, true);
	//tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
	//editMesh->cm.Tr.SetIdentity();
	//editMesh->UpdateBoxAndNormals();
	//*************************************************************


	//screen_xaxis = screen_xaxis.normalized() * 2;
	//screen_yaxis = screen_yaxis.normalized() * 2;
	//screen_zaxis = screen_zaxis.normalized() * 2;


	Scalarm mouseXoff = (currScreenOffset_X / float(gla->width()));
	Scalarm mouseYoff = (currScreenOffset_Y / float(gla->height()));
	//***20160304***增加sliceBar移動。
	switch (editMesh->getMeshSort())
	{
	case MeshModel::meshsort::decorate_item:
	{
		displayOffset_X = 0;// 
		displayOffset_Y = 0;//
		displayOffset_Z = (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);
		//GLArea::equ[3] = (displayOffset_Z);
		//qDebug() << "equ[3] " << GLArea::equ[3]-10;
	}
	break;
	case MeshModel::meshsort::print_item:
	{
		displayOffset_X = (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
		displayOffset_Y = (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);
		displayOffset_Z = (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);
		//qDebug() << "displayOffset_Z " << displayOffset_Z;
	}
	break;
	case MeshModel::meshsort::slice_item:
	{
		//displayOffset_X = (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
		//displayOffset_Y = (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);
		//displayOffset_Z = (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);
		displayOffset_X = 0;
		displayOffset_Y = 0;
		displayOffset_Z = 0;
	}
	break;
	default:
	{
		displayOffset_X = (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
		displayOffset_Y = (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);
		displayOffset_Z = (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);
	}
	break;
	}

	qDebug() << "displayOffset" << screen_xaxis[0] << mouseXoff << displayOffset_X << screen_yaxis[0] << mouseYoff << displayOffset_Y << displayOffset_Z;
	qDebug() << "mouseXoff" << mouseXoff << mouseYoff;
	//***20160309
	MeshDocument *md = editMesh->parent;
	/*if (abs(editMesh->cm.bbox.Center().Z())>100)
	qDebug() << "tooBigbigbigbig" << editMesh->cm.bbox.Center().Z() << "   ***" << md->bbox().max.Z();*/

	//delta_Transform.SetIdentity();
	if (md->isDecorate_item(*editMesh) && md->count_print_item() > 0)//******sliceBar
	{
		if (editMesh->cm.bbox.Center().Z() > md->bbox().max.Z() && displayOffset_Z > 0)
		{
			displayOffset_X = 0;
			displayOffset_Y = 0;
			displayOffset_Z = md->bbox().max.Z() - editMesh->cm.bbox.Center().Z();
		}
		else
			if (editMesh->cm.bbox.Center().Z() < md->bbox().min.Z() && displayOffset_Z < 0)
			{
				displayOffset_X = 0;
				displayOffset_Y = 0;
				displayOffset_Z = md->bbox().min.Z() - editMesh->cm.bbox.Center().Z();

			}
		delta_Transform.SetTranslate(*new Point3m(displayOffset_X, displayOffset_Y, displayOffset_Z));


	}
	else	delta_Transform.SetTranslate(*new Point3m(displayOffset_X, displayOffset_Y, displayOffset_Z));

	//***20150827*************

	editMesh->cm.Tr = delta_Transform*editMesh->cm.Tr;
	//***********

	tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, delta_Transform, true);
	tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
	//***20160304


	//editMesh->cm.Tr.SetIdentity();
	editMesh->UpdateBoxAndNormals();
	//resetOffsets();

}

void EditTransform::applyMatrix2(Box3m selectedBBox, GLArea *gla, MeshDocument *md)
{
	Scalarm mouseXoff = (currScreenOffset_X / float(gla->width()));
	Scalarm mouseYoff = (currScreenOffset_Y / float(gla->height()));

	displayOffset_X = (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
	displayOffset_Y = (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);
	if (gla->getxyOrz())
	{
		displayOffset_X = (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
		displayOffset_Y = (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);
		displayOffset_Z = 0;
	}
	else
	{
		displayOffset_X = 0;
		displayOffset_Y = 0;
		displayOffset_Z = (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);
	}
	qDebug() << "x, y, z: " << displayOffset_X << ", " << displayOffset_Y << ", " << displayOffset_Z;
	qDebug() << "Center: " << selectedBBox.Center().X() << ", " << selectedBBox.Center().Y() << ", " << selectedBBox.Center().Z();

	MeshModel *tm = md->mm();
	float xMin = tm->cm.bbox.min.X();
	float yMin = tm->cm.bbox.min.Y();
	float zMin = tm->cm.bbox.min.Z();
	float xMax = tm->cm.bbox.max.X();
	float yMax = tm->cm.bbox.max.Y();
	float zMax = tm->cm.bbox.max.Z();

	foreach(int i, md->multiSelectID)
	{
		MeshModel *tmm = md->getMesh(i);

		if (xMin > tmm->cm.bbox.min.X())
			xMin = tmm->cm.bbox.min.X();
		if (xMax < tmm->cm.bbox.max.X())
			xMax = tmm->cm.bbox.max.X();
		if (yMin > tmm->cm.bbox.min.Y())
			yMin = tmm->cm.bbox.min.Y();
		if (yMax < tmm->cm.bbox.max.Y())
			yMax = tmm->cm.bbox.max.Y();
		if (zMin > tmm->cm.bbox.min.Z())
			zMin = tmm->cm.bbox.min.Z();
		if (zMax < tmm->cm.bbox.max.Z())
			zMax = tmm->cm.bbox.max.Z();
	}

	if (-(md->groove.DimX() / 2) >(xMin + displayOffset_X))
		//displayOffset_X = 0;
		displayOffset_X = displayOffset_X - ((xMin + displayOffset_X) - (-md->groove.DimX() / 2));
	if ((md->groove.DimX() / 2) < (xMax + displayOffset_X))
		//displayOffset_X = 0;
		displayOffset_X = displayOffset_X - ((xMax + displayOffset_X) - (md->groove.DimX() / 2));
	if (-(md->groove.DimY() / 2) > (yMin + displayOffset_Y))
		//displayOffset_Y = 0;
		displayOffset_Y = displayOffset_Y - ((yMin + displayOffset_Y) - (-md->groove.DimY() / 2));
	if ((md->groove.DimY() / 2) < (yMax + displayOffset_Y))
		//displayOffset_Y = 0;
		displayOffset_Y = displayOffset_Y - ((yMax + displayOffset_Y) - (md->groove.DimY() / 2));
	if ((md->groove.DimZ() / 2) < (zMax + displayOffset_Z))
		//displayOffset_Z = 0;
		displayOffset_Z = displayOffset_Z - ((zMax + displayOffset_Z) - (md->groove.DimZ() / 2));
	if (-(md->groove.DimZ() / 2) > (zMin + displayOffset_Z))
		//displayOffset_Z = 0;
		displayOffset_Z = displayOffset_Z - ((zMin + displayOffset_Z) - (-md->groove.DimZ() / 2));

	delta_Transform.SetTranslate(*new Point3m(displayOffset_X, displayOffset_Y, displayOffset_Z));

	foreach(int i, md->multiSelectID)
	{
		MeshModel *editMesh = md->getMesh(i);
		editMesh->cm.Tr = delta_Transform * editMesh->cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(editMesh->cm, delta_Transform, true);
		tri::UpdateBounding<CMeshO>::Box(editMesh->cm);
		editMesh->UpdateBoxAndNormals();
	}

	selectedBBox.Translate(Point3f(displayOffset_X, displayOffset_Y, displayOffset_Z));
}

//**************************************************************************************************
void EditTransform::mousePressEvent(QMouseEvent *event, MeshModel &model, GLArea * gla, MeshDocument *md)
{
	//***20150904
	/*MeshDocument *md = model.parent;
	if (md->hasContainStringMesh("_temp_outlines"))
	{
	QVector<MeshModel *> vmm;
	md->getMeshByContainString("_temp_outlines", vmm);
	foreach(MeshModel *dmm, vmm)
	{
	qDebug("meshname" + (*dmm).label().toLatin1());
	md->delMesh(dmm);
	}
	MeshModel* tempG = md->getMeshByLabel("_temp_outlines");
	md->delMesh(tempG);
	if (md->hasContainStringMesh("_temp_ol_mesh"))
	{
	QVector<MeshModel *> vmm;
	md->getMeshByContainString("_temp_ol_mesh", vmm);
	foreach(MeshModel *dmm, vmm)
	{
	qDebug("meshname" + (*dmm).label().toLatin1());
	md->delMesh(dmm);
	}
	}
	}*/
	//***
	//qDebug() << "editTransform transform";
	isMoving = true;
	startdrag = Point2i(event->x(), event->y());
	//if (abs(model.cm.bbox.Center().Z())>100)
	//qDebug() << "tooBigbigbigbigmousePressEvent" << model.cm.bbox.Center().Z() << "   ***" << md->bbox().max.Z();
	//qDebug() << "editTransform transform" << startdrag[0];
	//gla->update();
}

void EditTransform::mouseMoveEvent(QMouseEvent * event, MeshModel &model, GLArea * gla, MeshDocument *md)
{
	if (isMoving)
	{
		enddrag = Point2i(event->x(), event->y());
		currScreenOffset_X = enddrag[0] - startdrag[0];
		currScreenOffset_Y = enddrag[1] - startdrag[1];
		qDebug() << "editTransform transform" << enddrag[0];
		//qDebug() << "currScreenOffset_X transform" << enddrag[0] - startdrag[0];
		//qDebug() << "currScreenOffset_Y transform" << enddrag[1] - startdrag[1];
		//***20150409要同步
		startdrag = Point2i(event->x(), event->y());
		//***20160422
		//allMeshApplyMatrix(*md, gla);
		//applyMatrix(&model, gla);

		Box3m selectedBBox;
		Matrix44m iden;
		iden.SetIdentity();

		foreach(int i, md->multiSelectID)
		{
			if (md->getMesh(i)->getMeshSort() == MeshModel::meshsort::print_item)
				selectedBBox.Add(iden, md->getMesh(i)->cm.bbox);
		}

		applyMatrix2(selectedBBox, gla, md);
	}
	//gla->update();
}

void EditTransform::mouseReleaseEvent(QMouseEvent * event, MeshModel &model, GLArea * gla, MeshDocument *md)
{
	if (isMoving)
	{
		isMoving = false;
		enddrag = Point2i(event->x(), event->y());
		currScreenOffset_X = enddrag[0] - startdrag[0];//起點終點
		currScreenOffset_Y = enddrag[1] - startdrag[1];
		//UpdateMatrix(model, gla, true);
	}
	//***20160525*******
	//emit matrix_changed(&model, model.cm.Tr);
	//****************
	gla->update();
}


bool EditTransform::MyPick(const int &x, const int &y, Point3m &pp, float mydepth)
{
	double res[3];
	GLdouble mm[16], pm[16]; GLint vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, mm);
	glGetDoublev(GL_PROJECTION_MATRIX, pm);
	glGetIntegerv(GL_VIEWPORT, vp);
	/*qDebug() << "mm" << mm[0] << mm[1] << mm[2] << mm[3] << mm[4] << mm[5] << mm[6] << mm[7] << mm[8] << mm[9]
		<< mm[10] << mm[11] << mm[12] << mm[13] << mm[14] << mm[15];
		qDebug() << "pm" << pm[0] << pm[1] << pm[2] << pm[3] << pm[4] << pm[5] << pm[6] << pm[7] << pm[8] << pm[9]
		<< pm[10] << pm[11] << pm[12] << pm[13] << pm[14] << pm[15];*/
	gluUnProject(x, y, mydepth, mm, pm, vp, &res[0], &res[1], &res[2]);//將window coordinate(x, y) 對應到object space,
	pp = Point3m(Scalarm(res[0]), Scalarm(res[1]), Scalarm(res[2]));
	return true;
}
void EditTransform::resetOffsets()
{
	currScreenOffset_X = 0;   // horizontal offset (screen space)
	currScreenOffset_Y = 0;   // vertical offset (screen space)
	currOffset_X = 0;
	currOffset_Y = 0;
	currOffset_Z = 0;

}
bool EditTransform::updateModelProjectionMatrix(GLArea *gla)
{

	Point3m center, right, top, front, back;

	MyPick(gla->width()*0.5f, gla->height()*0.5f, center, 0.5f);
	MyPick(gla->width()*0.99f, gla->height()*0.5f, right, 0.5f);
	MyPick(gla->width()*0.5f, gla->height()*0.01f, top, 0.5f);
	MyPick(gla->width()*0.5f, gla->height()*0.5f, front, 0.01f);
	MyPick(gla->width()*0.5f, gla->height()*0.5f, back, 0.99f);
	//qDebug() << "editTransform"<<center[0] << center[1] << center[2];

	screen_xaxis = (right - center) * Scalarm(2.0);//***screenspace
	screen_yaxis = (top - center)   * Scalarm(2.0);
	screen_zaxis = (front - center) * Scalarm(2.0);//
	//qDebug() << "screen_xaxis" << screen_xaxis[0] << screen_xaxis[1] << screen_xaxis[2];
	return true;
}