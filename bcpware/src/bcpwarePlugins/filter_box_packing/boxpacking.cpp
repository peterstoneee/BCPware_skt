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
#include "boxpacking.h"
#include <bcpware/glarea.h>
using namespace vcg;



BoxPacking::BoxPacking()
{
	typeList << FP_LOEST_HEIGHT << FP_AUTO_PACKING << FP_GROUND << FP_AUTO_LANDING << FP_CURRENT_SELECT_MESH_LANDING << FP_AUTO_ALL_LANDING << FP_AUTO_CENTER << FP_COUNT_VOLUMN << FP_TEST_AREA << FP_TEST_ALIGN_OTHER_AXIS
		<< FP_TEST_COPY_PACKING << FP_CHANGE_COLOR << FP_MIRROR << FP_MIRROR_X << FP_MIRROR_Y << FP_MIRROR_Z
		<< FP_JUSTIFY_FRONT << FP_JUSTIFY_BACK << FP_JUSTIFY_LEFT << FP_JUSTIFY_RIGHT << FP_JUSTIFY_BOTTOM << FP_JUSTIFY_TOP << FP_Test_Quaternion << FP_COUNT_HOLES
		<< FP_SEPERATE_TEST;
	foreach(FilterIDType tt, types())
	{
		actionList << new QAction(filterName(tt), this);
		//if (tt == FP_AUTO_PACKING)  actionList.last()->setIcon(QIcon(":/myImage/images/btn_auto_packing@2x.png"));
		//if (tt == FP_GROUND)actionList.last()->setIcon(QIcon(":/myImage/images/Landing.jpg"));
		//if (tt == FP_AUTO_LANDING)  actionList.last()->setIcon(QIcon(":/myImage/images/btn_menu_land_2x.png"));
		//if (tt == FP_COUNT_HOLES)  actionList.last()->setIcon(QIcon(":/myImage/images/btn_menu_land_2x.png"));
		//if (tt == FP_AUTO_ALL_LANDING)  actionList.last()->setIcon(QIcon(":/myImage/images/Landing.jpg"));
		//if (tt == FP_AUTO_CENTER)  actionList.last()->setIcon(QIcon(":/myImage/images/center.jpg"));
		//if (tt == FP_COUNT_VOLUMN) actionList.last()->setIcon(QIcon(":/myImage/images/count_volumn.jpg"));
		//if (tt == FP_TEST_AREA) actionList.last()->setIcon(QIcon(":/myImage/color_or_no_color.jpg"));
		//if (tt == FP_TEST_ALIGN_OTHER_AXIS)actionList.last()->setIcon(QIcon(":/myImage/color_or_no_color.jpg"));
		//if (tt == FP_MIRROR)actionList.last()->setIcon(QIcon(":/myImage/color_or_no_color.jpg"));
	}
}

BoxPacking::FilterClass BoxPacking::getClass(QAction *a)
{
	switch (ID(a))
	{
	case FP_LOEST_HEIGHT:  return MeshFilterInterface::MovePos;
	case FP_AUTO_PACKING:  return MeshFilterInterface::MovePos;
	case FP_GROUND: return MeshFilterInterface::MovePos;
	case FP_AUTO_LANDING: return MeshFilterInterface::MovePos;
	case FP_CURRENT_SELECT_MESH_LANDING: return MeshFilterInterface::MovePos;
	case FP_AUTO_ALL_LANDING: return MeshFilterInterface::MovePos;
	case FP_AUTO_CENTER:return MeshFilterInterface::MovePos;
	case FP_COUNT_VOLUMN:return MeshFilterInterface::MovePos;
	case FP_TEST_AREA:return MeshFilterInterface::MovePos;
	case FP_TEST_ALIGN_OTHER_AXIS:return MeshFilterInterface::MovePos;
	case FP_TEST_COPY_PACKING:return MeshFilterInterface::MovePos;
	case FP_CHANGE_COLOR: return MeshFilterInterface::MovePos;
	case FP_MIRROR: return  MeshFilterInterface::MovePos;
	case FP_MIRROR_X: return MeshFilterInterface::MovePos;
	case FP_MIRROR_Y: return MeshFilterInterface::MovePos;
	case FP_MIRROR_Z: return MeshFilterInterface::MovePos;
	case FP_JUSTIFY_FRONT: return  MeshFilterInterface::MovePos;
	case FP_JUSTIFY_BACK: return  MeshFilterInterface::MovePos;
	case FP_JUSTIFY_LEFT: return  MeshFilterInterface::MovePos;
	case FP_JUSTIFY_RIGHT: return  MeshFilterInterface::MovePos;
	case FP_JUSTIFY_BOTTOM: return  MeshFilterInterface::MovePos;
	case FP_JUSTIFY_TOP:	return  MeshFilterInterface::MovePos;
	case FP_Test_Quaternion: return MeshFilterInterface::MovePos;
	case FP_COUNT_HOLES: return MeshFilterInterface::MovePos;
	case FP_SEPERATE_TEST: return MeshFilterInterface::MeshCreation;
	default: assert(0);
	}
	return MeshFilterInterface::Generic;
}


void BoxPacking::initParameterSet(QAction * action, MeshDocument &md, RichParameterSet & parlst)
{
	//(void)m;

	switch (ID(action))
	{
	case FP_AUTO_ALL_LANDING:
	{
		parlst.addParam(new RichBool("all_or_not", false, "ALL_Mesh", ""));
		parlst.addParam(new RichBool("collision_detected", false, "bounding box collision_detected", ""));

	}break;
	case FP_TEST_AREA:
	{
		QStringList cm = QStringList() << "perVert" << "perNone";
		parlst.addParam(new RichEnum("CM_MODE", 0, cm, tr("colormode"), tr("colormode")));


	}break;
	case FP_TEST_ALIGN_OTHER_AXIS:
	{
		parlst.addParam(new RichBool("all_or_not", true, "ALL_Mesh", ""));
		parlst.addParam(new RichBool("collision_detected", true, "bounding box collision_detected", ""));
		//parlst.addParam(new RichBool(""));

		QStringList align_axis;
		align_axis.push_back("X axis");
		align_axis.push_back("Y axis");
		align_axis.push_back("Z axis");
		align_axis.push_back("Max X axis");
		align_axis.push_back("Max Y axis");
		align_axis.push_back("Max Z axis");
		align_axis.push_back("custom axis");
		parlst.addParam(new RichEnum("Align_Axis", 0, align_axis, tr("Align_Axis"), tr("")));
	}break;
	case FP_TEST_COPY_PACKING:
	{

	}break;
	case FP_CHANGE_COLOR:
	{

	}break;
	case FP_MIRROR:
	{
		QStringList align_axis;
		align_axis.push_back("X axis");
		align_axis.push_back("Y axis");
		align_axis.push_back("Z axis");
		parlst.addParam(new RichEnum("Mirror_Axis", 0, align_axis, tr("Mirror_Axis"), tr("")));
	}break;

	default:
		break;

	}
}
QString BoxPacking::filterName(FilterIDType filterID)const
{
	switch (filterID)
	{
	case FP_LOEST_HEIGHT:return tr("Transform_lowest_height");
	case FP_AUTO_PACKING:return tr("Auto Packing");
	case FP_CURRENT_SELECT_MESH_LANDING:return tr("Selected_Mesh_Landing");
	case FP_AUTO_LANDING:return tr("Land");
	case FP_AUTO_ALL_LANDING:return tr("Landing_Tool");
	case FP_AUTO_CENTER:return tr("MoveToCenter");
	case FP_COUNT_VOLUMN:return tr("FP_COUNT_VOLUMN");
	case FP_TEST_AREA: return tr("FP_TEST_AREA");
	case FP_TEST_ALIGN_OTHER_AXIS: return tr("FP_ALIGN_OTHER_AXIS");
	case FP_TEST_COPY_PACKING:return tr("FP_COPY_PACKING");
	case FP_CHANGE_COLOR:return tr("FP_CHANGE_COLOR");
	case FP_MIRROR:return tr("MIRROR");
	case FP_MIRROR_X:return tr("FLIP_BY_Y_Z_PLANE");
	case FP_MIRROR_Y:return tr("FLIP_BY_X_Z_PLANE");
	case FP_MIRROR_Z:return tr("FLIP_BY_X_Y_PLANE");
	case FP_JUSTIFY_FRONT:return tr("JUSTIFY_FRONT");
	case FP_JUSTIFY_BACK:return tr("JUSTIFY_BACK");
	case FP_JUSTIFY_LEFT:return tr("JUSTIFY_LEFT");
	case FP_JUSTIFY_RIGHT:return tr("JUSTIFY_RIGHT");
	case FP_JUSTIFY_BOTTOM:return tr("JUSTIFY_BOTTOM");
	case FP_JUSTIFY_TOP:return tr("JUSTIFY_TOP");
	case FP_Test_Quaternion:return tr("FP_Test_Quaternion");
	case FP_COUNT_HOLES:return tr("count_hole");
	case FP_SEPERATE_TEST:return tr("FP_SEPERATE_TEST");
	}
	return tr("error!");
}
QString BoxPacking::filterInfo(FilterIDType filterID) const
{
	switch (filterID)
	{
	case FP_LOEST_HEIGHT:return tr("Transform_lowest_height");
	case FP_AUTO_PACKING:return tr("Auto Packing");
	case FP_CURRENT_SELECT_MESH_LANDING:return tr("Selected_Mesh_Landing");
	case FP_AUTO_LANDING:return tr("Land");
	case FP_AUTO_ALL_LANDING:return tr("All_AutoLanding");
	case FP_AUTO_CENTER:return tr("MoveToCenter");
	case FP_COUNT_VOLUMN:return tr("FP_COUNT_VOLUMN");
	case FP_TEST_AREA:return tr("set_color_or_no_color");
	case FP_TEST_ALIGN_OTHER_AXIS: return tr("FP_ALIGN_OTHER_AXIS");
	case FP_TEST_COPY_PACKING:return tr("FP_COPY_PACKING");
	case FP_CHANGE_COLOR:return tr("FP_CHANGE_COLOR");
	case FP_MIRROR:return tr("MIRROR");
	case FP_MIRROR_X:return tr("MIRROR_X");
	case FP_MIRROR_Y:return tr("MIRROR_Y");
	case FP_MIRROR_Z:return tr("MIRROR_Z");
	case FP_JUSTIFY_FRONT:return tr("JUSTIFY_FRONT");
	case FP_JUSTIFY_BACK:return tr("JUSTIFY_BACK");
	case FP_JUSTIFY_LEFT:return tr("JUSTIFY_LEFT");
	case FP_JUSTIFY_RIGHT:return tr("JUSTIFY_RIGHT");
	case FP_JUSTIFY_BOTTOM:return tr("JUSTIFY_BOTTOM");
	case FP_JUSTIFY_TOP:return tr("JUSTIFY_TOP");
	case FP_Test_Quaternion:return tr("test_quaternion");
	case FP_COUNT_HOLES:return tr("count_hole");
	case FP_SEPERATE_TEST:return tr("FP_SEPERATE_TEST");
	}
	return tr("error!");
}



//template <class MeshType>
bool BoxPacking::applyFilter(QAction * filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb, GLArea *gla, vcg::SendBackData *sb)
{

	/*typedef typename MeshType::EdgeIterator EdgeIterator;
	typedef typename MeshType::FaceIterator FaceIterator;*/

	MeshModel &m = *md.mm();
	MeshModel *bm = md.mm();
	float t_gap = md.p_setting.getTopGap();
	float b_gap = md.p_setting.getBottom_Gap();
	float r_gap = md.p_setting.getRight_Gap();
	float l_gap = md.p_setting.getLeft_Gap();
	float bt_gap = md.p_setting.getBetweenGap();

	switch (ID(filter))
	{
#pragma region FP_AUTO_PACKING
	case FP_AUTO_PACKING:
	{
#if 1
		//QString s1 = "_temp_outlines";
		//QString s2 = "_temp_ol_mesh";
		bool auto_rotate = par.getBool("auto_rotate_or_not");
		QMap<int, float> mesh_area_map, mesh_volumn_map;//***把meshmodel編號與最大面積傳入qmap
		//foreach(MeshModel *bm, md.meshList)
		//{
		//	//if (bm->label().indexOf(s1)==-1 || bm->label().indexOf(s2)==-1)
		//	if (!(bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
		//	mesh_area_map.insert(bm->id(),bm->cm.bbox.Total_SurfaceArea());
		//}
		//foreach(MeshModel *bm, md.meshList)
		//{
		//	if (!(bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
		//	mesh_volumn_map.insert(bm->id(), bm->cm.bbox.Volume());
		//}
		//***20150908合為一個
		foreach(MeshModel *bm, md.meshList)
		{
			/*if (!(bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
			{*/
			if (bm->getMeshSort() == MeshModel::meshsort::print_item)
			{
				mesh_area_map.insert(bm->id(), bm->cm.bbox.Total_SurfaceArea());
				mesh_volumn_map.insert(bm->id(), bm->cm.bbox.Volume());
			}
			//}								

		}
		//*************************

		Box3m sinkBBox, sinkBackup;//***建構槽
		sinkBBox.min = md.groove.min;// vcg::Point3f(-15 / 2., -20 / 2., -17 / 2.);
		sinkBBox.max = md.groove.max;// vcg::Point3f(15 / 2., 20 / 2., 17 / 2.);

		sinkBackup.min = md.groove.min;
		sinkBackup.max = md.groove.max;

		//***增加gap

		float gap = 1;
		sinkBBox.min[0] = sinkBBox.min[0] + l_gap;
		sinkBBox.min[1] = sinkBBox.min[1] + b_gap;
		sinkBBox.max[0] = sinkBBox.max[0] - r_gap;
		sinkBBox.max[1] = sinkBBox.max[1] - t_gap;
		sinkBackup.min[0] = sinkBackup.min[0] + l_gap;
		sinkBackup.min[1] = sinkBackup.min[1] + b_gap;
		sinkBackup.max[0] = sinkBackup.max[0] - r_gap;
		sinkBackup.max[1] = sinkBackup.max[1] - t_gap;

		if (mesh_area_map.size() > 0)
			//xyz::box_packing_algrithm(sinkBBox, mesh_area_map, mesh_volumn_map, md, sinkBackup, bt_gap, auto_rotate);
			xyz::LAFF_First_Placement(sinkBBox, sinkBackup, mesh_area_map, mesh_volumn_map, md, bt_gap, auto_rotate);
		else return false;

		/*QMap<int, float>::Iterator iterator;
		iterator = max_element(mesh_area_map.begin(), mesh_area_map.end());//找出meshdoc中最大表面積
		qDebug() << "testtest" << iterator.key()<<iterator.value();

		MeshModel &pack_1 = *md.getMesh(iterator.key());//***得到mesh

		//***20150420***轉至最小距離
		//Box3m *m_box(&pack_1.cm.bbox);
		int mindim = pack_1.cm.bbox.MinDim();
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

		switch (mindim)//轉至高度最低
		{
		case 0:
		xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_zaxis);
		break;
		case 1:
		break;
		case 2:
		xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_xaxis);
		break;
		}
		//*****************
		Box3m sinkBBox;//***建構槽
		sinkBBox.min = vcg::Point3f(-25 / 2., -20 / 2., -25 / 2.);
		sinkBBox.max = vcg::Point3f(25 / 2., 20 / 2., 25 / 2.);

		int maxdim = pack_1.cm.bbox.MaxDim();
		switch (maxdim)//***最長軸轉至與x平行
		{
		case 0:
		xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_yaxis);
		break;
		case 1:
		break;
		case 2:
		break;
		}
		xyz::mesh_translate(pack_1.cm, sinkBBox.min - pack_1.cm.bbox.min);//***移至建構槽.min
		QMap<int, float>::iterator it;
		it = mesh_area_map.find(pack_1.id());
		mesh_area_map.erase(it);*/

#else
		QMap<int, float> mesh_area_map;//***把meshmodel編號與最大面積傳入qmap
		foreach(MeshModel *bm, md.meshList)
		{
			mesh_area_map.insert(bm->id(), bm->cm.bbox.Total_SurfaceArea());

		}
		QMap<int, float>::Iterator iterator;
		iterator = max_element(mesh_area_map.begin(), mesh_area_map.end());//找出meshdoc中最大表面積
		qDebug() << "testtest" << iterator.key() << iterator.value();

		MeshModel &pack_1 = *md.getMesh(iterator.key());//***得到mesh

		//***20150420***轉至最小距離
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸


		//*****************
		Box3m sinkBBox;//***建構槽
		sinkBBox.min = vcg::Point3f(-15 / 2., -20 / 2., -30 / 2.);
		sinkBBox.max = vcg::Point3f(15 / 2., 20 / 2., 30 / 2.);
		int sinkbox_maxdim = sinkBBox.MaxDim();
		int maxdim = pack_1.cm.bbox.MaxDim();
		switch (sinkbox_maxdim)//***最長軸轉至與x平行
		{
		case 0:
			if (maxdim == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_zaxis);
			if (maxdim == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_yaxis);
			break;
		case 1:
			if (maxdim == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_zaxis);
			if (maxdim == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_xaxis);
			break;
		case 2:
			if (maxdim == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_yaxis);
			if (maxdim == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_xaxis);
			break;
		}
		int sinkbox_mindim = sinkBBox.MinDim();
		int mindim = pack_1.cm.bbox.MinDim();
		switch (sinkbox_mindim)
		{
		case 0:
			if (mindim == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_zaxis);
			if (mindim == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_yaxis);
			break;
		case 1:
			if (mindim == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_zaxis);
			if (mindim == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_xaxis);
			break;
		case 2:
			if (mindim == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_yaxis);
			if (mindim == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_1.cm, 90.0, rotate_xaxis);
			break;
		}
		xyz::mesh_translate(pack_1.cm, sinkBBox.min - pack_1.cm.bbox.min);//***移至建構槽.min
		QMap<int, float>::iterator it;
		it = mesh_area_map.find(pack_1.id());
		mesh_area_map.erase(it);
		qDebug() << "SB max = " << sinkbox_maxdim << "   SB min = " << sinkbox_mindim;
		qDebug() << "maxdim = " << maxdim << "   mindim = " << mindim;
#endif
		QMap<int, float> id_surfaceArea;
		QMap<int, Box3m> id_box3m;


	}break;
#pragma endregion FP_AUTO_PACKING
#pragma region FP_CURRENT_SELECTED_MESH_LANDING
	case FP_CURRENT_SELECT_MESH_LANDING:
	{
		foreach(int selID, md.multiSelectID)
		{
			MeshModel *multiSm = md.getMesh(selID);
			if (multiSm != NULL)
			{
				Point3m translatePath;
				translatePath.X() = 0;
				translatePath.Y() = 0;
				translatePath.Z() = md.groove.min.Z() - multiSm->cm.bbox.min.Z();
				xyz::recurcisve_one_mesh_box_test<float>(*multiSm, md, bt_gap, true, translatePath);
			}
			/*else
			{
			Matrix44m landToGround;
			landToGround.SetIdentity();
			Point3m translatePath;
			translatePath.X() = 0;
			translatePath.Y() = 0;
			translatePath.Z() = md.groove.min.Z() - multiSm->cm.bbox.min.Z();
			landToGround.SetTranslate(translatePath);
			multiSm->cm.Tr = landToGround * multiSm->cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(multiSm->cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(multiSm->cm);

			}*/

		}

	}break;
#pragma endregion FP_CURRENT_SELECTED_MESH_LANDING
#pragma region FP_LOEST_HEIGHT
	case FP_LOEST_HEIGHT:
	{
		//Matrix44m translate_to_origin, translate_to_box_center;//***先回原點,再轉回原來位置
		//Matrix44m rotate_to_min_height;//***20150420旋轉矩陣	


		//Box3m *m_box(&m.cm.bbox);
		//int mindim = m_box->MinDim();
		//Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		//Point3m rotate_yaxis;
		//Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸
		//
		//switch (mindim)
		//{
		//case 0:
		//	
		//	//translate_to_origin.SetTranslate(-m.cm.bbox.Center());//***先回原點
		//	//rotate_to_min_height.SetRotateDeg(90.0, rotate_zaxis);//***旋轉完
		//	//translate_to_box_center.SetTranslate(m.cm.bbox.Center());//***再回原位置								
		//	//m.cm.Tr = translate_to_box_center*rotate_to_min_height*translate_to_origin;
		//	xyz::mesh_rotate<CMeshO, Scalarm>(m.cm, 90.0, rotate_zaxis);
		//	break;
		//case 1:
		//	break;
		//case 2:
		//	/*translate_to_origin.SetTranslate(-m.cm.bbox.Center());
		//	rotate_to_min_height.SetRotateDeg(90.0, rotate_xaxis);
		//	translate_to_box_center.SetTranslate(m.cm.bbox.Center());
		//	m.cm.Tr = translate_to_box_center*rotate_to_min_height*translate_to_origin;*/
		//	xyz::mesh_rotate<CMeshO, Scalarm>(m.cm, 90.0, rotate_xaxis);
		//	break;
		//}

		//tri::UpdatePosition<CMeshO>::Matrix(m.cm, m.cm.Tr, true);
		//tri::UpdateBounding<CMeshO>::Box(m.cm);
		//m.cm.Tr.SetIdentity();
		//m.UpdateBoxAndNormals();

	}break;
#pragma endregion FP_LOEST_HEIGHT
#pragma region FP_AUTO_LANDING
	case FP_AUTO_LANDING:
	{



		/*Point3m translatePath;
		translatePath.X() = 0;
		translatePath.Y() = 0;
		translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
		xyz::recurcisve_one_mesh_box_test<float>(m, md, bt_gap, true, translatePath);*/



		do
		{
			xyz::recursive_all_mesh_2<Scalarm>(md, bt_gap);
		} while (!xyz::collide_all_test<Scalarm>(md));



	}break;
#pragma endregion FP_AUTO_LANDING
#pragma region FP_AUTO_ALL_LANDING
	case FP_AUTO_ALL_LANDING:
	{
		/*do
		{
		xyz::recursive_all_mesh<Scalarm>(md, 0.2);
		} while (!xyz::collide_all_test<Scalarm>(md));
		xyz::recursive_all_mesh<Scalarm>(md, 0.2);*/

		bool all_or_not = par.getBool("all_or_not");
		bool collision_detected = par.getBool("collision_detected");
		int a;
		//
		if (all_or_not)a = 1;
		else a = 0;
		switch (a)
		{
#pragma region all
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_all_mesh_2<Scalarm>(md, bt_gap);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;
				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
#pragma endregion all
		case 0://select mesh
		{
			if (collision_detected)
			{
				vector<PAIR> mesh_z_coordinate;

				foreach(int i, md.multiSelectID)
				{
					MeshModel *mm = md.getMesh(i);
					mesh_z_coordinate.push_back(std::pair<int, float>(mm->id(), mm->cm.bbox.min.Z()));
				}

				qSort(mesh_z_coordinate.begin(), mesh_z_coordinate.end(), CmpByValue());

				for (int i = 0; i < mesh_z_coordinate.size(); i++)
				{
					MeshModel *bm = md.getMesh(mesh_z_coordinate[i].first);
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					xyz::recurcisve_one_mesh_box_test<float>(*bm, md, bt_gap, true, translatePath);
				}
			}
			else
			{
				foreach(int i, md.multiSelectID)
				{
					MeshModel *bm = md.getMesh(i);
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);
				}
			}

		}break;
		case 2:
		{
			//QMap<int, float> mesh_z_coordinate;
			vector<PAIR> mesh_z_coordinate;
			foreach(int i, md.multiSelectID)
			{
				//mesh_z_coordinate.insert(i, md.getMesh(i)->cm.bbox.min.Z());
				mesh_z_coordinate.push_back(std::pair<int, float>(i, md.getMesh(i)->cm.bbox.min.Z()));
			}

			//qSort(mesh_z_coordinate.values().begin(), mesh_z_coordinate.values().end(), qLess<float>());
			qSort(mesh_z_coordinate.begin(), mesh_z_coordinate.end(), CmpByValue());

			//foreach(int selID, mesh_z_coordinate.keys())
			for (int i = 0; i < mesh_z_coordinate.size(); i++)
			{
				MeshModel *multiSm = md.getMesh(mesh_z_coordinate[i].first);
				if (collision_detected && multiSm != NULL)
				{
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - multiSm->cm.bbox.min.Z();
					//xyz::recurcisve_one_mesh_box_test<float>(*multiSm, md, bt_gap, true, translatePath);
					xyz::recurcisve_mesh_box_height(mesh_z_coordinate, *multiSm, md, bt_gap, true, translatePath);
				}
				else
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - multiSm->cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					multiSm->cm.Tr = landToGround * multiSm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(multiSm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(multiSm->cm);

				}

			}


		}break;
		}


		////***依物體高度排序後做landing功能**********
		//do
		//{
		//	xyz::recursive_all_mesh_2<Scalarm>(md, bt_gap);
		//} while (!xyz::collide_all_test<Scalarm>(md));




		//**************



	}break;
#pragma endregion FP_AUTO_ALL_LANDING
#pragma region FP_AUTO_CENTER
	case FP_AUTO_CENTER:
	{

		//FILE *dbgff;
		//dbgff = fopen("D:\\debug4.txt", "w");//@@@
		Matrix44m move_to_center;
		move_to_center.SetIdentity();
		Point3m translatePath;
		translatePath.X() = md.groove.Center().X() - m.cm.bbox.Center().X();
		translatePath.Y() = md.groove.Center().Y() - m.cm.bbox.Center().Y();
		translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();


		/*QString myString = m.fullName();
		const char* myChar = myString.toStdString().c_str();
		fprintf(dbgff, "name %s\n", myChar);
		fflush(dbgff);
		fprintf(dbgff, "name %i\n", m.getMeshSort());
		fflush(dbgff);*/

		//if (&m != NULL){
		move_to_center.SetTranslate(translatePath);
		m.cm.Tr = move_to_center * m.cm.Tr;

		tri::UpdatePosition<CMeshO>::Matrix(m.cm, move_to_center, true);
		tri::UpdateBounding<CMeshO>::Box(m.cm);

		xyz::recurcisve_one_mesh_box_test<Scalarm>(m, md, bt_gap, true);
		//}



	}break;
#pragma endregion FP_AUTO_CENTER
#pragma region VOLUMN
	case FP_COUNT_VOLUMN:
	{
		float total_volumn = 0;// xyz::volumeOfMesh<Scalarm>(m);
		foreach(MeshModel *bm, md.meshList)
		{

			total_volumn += xyz::volumeOfMesh<Scalarm>(*bm);

		}
		//this->RealTimeLog("Manipulator", "",QString::number(total_volumn).toUtf8().data());
		//qDebug() << "mesh_volumn" << total_volumn;
		float current_volumn = xyz::volumeOfMesh<Scalarm>(*md.mm());
		QString infostr;
		infostr = QString("Total_Mesh_Volumn : %1 \nCurrent_Mesh_Volumn %2").arg(QString::number(total_volumn, 'g', 3).toUtf8().data())
			.arg(QString::number(current_volumn, 'g', 3).toUtf8().data());


		//***
		CMeshO::FaceIterator fi;

		MeshModel &m = *md.mm();
		vcg::Point3f va;
		float area = 0;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
		{
			vcg::Point3f v0 = fi->V(1)->P() - fi->V(0)->P();
			vcg::Point3f v1 = fi->V(2)->P() - fi->V(0)->P();

			SKT::CrossProduct<float>(v0, v1, va);
			area += SKT::Length<float>(va);

		}
		//***
		QMessageBox *a = new QMessageBox();
		a->setWindowTitle("Volumn");
		a->setText(infostr);
		//a->setText(QString::number(area/2.0));



		a->show();

	}break;
#pragma endregion VOLUMN
#pragma region area
	case FP_TEST_AREA:
	{
		switch (par.getEnum("CM_MODE"))
		{
		case 0:
			m.rmm.setColorMode(GLW::CMPerVert);
			break;
		case 1:
			m.rmm.setColorMode(GLW::CMNone);
			break;
		default:
			break;
		}


	}break;
#pragma endregion area
#pragma region FP_TEST_ALIGN_OTHER_AXIS
	case FP_TEST_ALIGN_OTHER_AXIS:
	{

		bool all_or_not = par.getBool("all_or_not");
		bool collision_detected = par.getBool("collision_detected");
		int align_axis = par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 1;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();;
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 0, translatePath);
				}
				break;
				case 1:
				{
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 1, translatePath);
				}
				break;
				case 2:
				{
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 2, translatePath);
				}
				break;
				case 3:
				{
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 3, translatePath);
				}
				break;
				case 4:
				{
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();
					translatePath.Z() = 0;
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 4, translatePath);
				}
				break;
				case 5:
				{
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					xyz::recurcisve_one_mesh_box_axis<float>(m, md, bt_gap, true, 5, translatePath);
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}


	}break;
#pragma endregion FP_TEST_ALIGN_OTHER_AXIS
#pragma region FP_TEST_COPY_PACKING
	case FP_TEST_COPY_PACKING:
	{
		//pending	 
		//typedef typename CMeshO::EdgeIterator EdgeIterator;
		//typedef typename CMeshO::FaceIterator FaceIterator;
		//for (EdgeIterator ei = m.cm.edge.begin(); ei != m.cm.edge.end(); ++ei)
		//{
		// Point2f x(-(ei->V(0)->P().X() - ei->V(1)->P().X()), ei->V(0)->P().Y() - ei->V(1)->P().Y());

		// Point2f tempNormal(x.Normalize());

		// Point3f v00(ei->V(0)->P().X() + tempNormal.X(), ei->V(0)->P().Y() + tempNormal.Y(), ei->V(0)->P().Z()); // v0+normal
		// Point3f v01(ei->V(0)->P().X() - tempNormal.X(), ei->V(0)->P().Y() - tempNormal.Y(), ei->V(0)->P().Z()); // v0-normal

		// Point3f v00(ei->V(1)->P().X() + tempNormal.X(), ei->V(1)->P().Y() + tempNormal.Y(), ei->V(1)->P().Z()); // v1+normal
		// Point3f v01(ei->V(1)->P().X() - tempNormal.X(), ei->V(1)->P().Y() - tempNormal.Y(), ei->V(1)->P().Z()); // v1-norma

		// //(*ei).V(0)->T().
		//}



	}break;
#pragma endregion FP_TEST_COPY_PACKING
#pragma region FP_CHANGE_COLOR
	case FP_CHANGE_COLOR:
	{
		//===20151105測顏色
		/* if (!vcg::tri::HasPerVertexColor(m.cm))
		{
		m.cm.vert.EnableColor();
		}
		if (!m.hasDataMask(MeshModel::MM_VERTCOLOR))
		{
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, Color4b(150, 0, 0, 255));

		}*/

		//===complete_work-----------------------------------------------------------------------------
		const QColor color = QColorDialog::getColor(Qt::green, 0, "Select Color");

		m.updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTMARK);
		m.Enable(tri::io::Mask::IOM_VERTCOLOR);
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::InitVertexIMark(m.cm);
		tri::InitFaceIMark(m.cm);
		//tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, Color4b(255, 0, 0, 255));
		tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, Color4b(color.red(), color.green(), color.blue(), 255));
		tri::UpdateNormal<CMeshO>(m);

		//QMap<int, RenderMode>::iterator it = gla->rendermodemap.find(md.mm()->id());
		//if (it == gla->rendermodemap.end())
		//	return false;
		//it.value().colorMode = vcg::GLW::CMPerVert;
		md.mm()->rmm.colorMode = vcg::GLW::CMPerVert;
		md.mm()->rmm.textureMode = vcg::GLW::TMNone;
		//gla->updaterendermodemap();//bad
		gla->updateRendermodemapSiganl = true;
		//===--------------------------------------====


		//if(ccc) ccc->updateAllViewer();
		//->updateAllViewerRenderModeMap();
		//cVC->updateAllViewerRenderModeMap();
		//cVC->updateAllViewer();

	}break;
#pragma endregion FP_CHANGE_COLOR
#pragma region FP_MIRROR
	case FP_MIRROR:
	{
		//***test_section*****************
		/*CMeshO::FaceIterator fi;
		fi->NewBitFlag();*/
		//*******************
		CMeshO::VertexIterator vi;
		Point3m meshCenter;// = m.cm.bbox.Center();
		Matrix44m editTranslateBackToCenterMatrix;


		MeshModel *mirroMesh;
		foreach(int i, md.multiSelectID)
		{
			if (i != -1)
			{
				mirroMesh = md.getMesh(i);
				meshCenter = mirroMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);


				for (vi = mirroMesh->cm.vert.begin(); vi != mirroMesh->cm.vert.end(); ++vi)
				{

					switch (par.getEnum("Mirror_Axis"))
					{
					case 0:
						vi->P().Y() = -vi->P().Y();
						break;
					case 1:
						vi->P().X() = -vi->P().X();
						break;
					case 2:
						vi->P().Z() = -vi->P().Z();
						break;
					default:
						break;
					}
				}
				//vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mirroMesh->cm);
				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);
				//tri::UpdateNormal<CMeshO>(mirroMesh);

			}


		}




	}break;
#pragma endregion FP_MIRROR
#pragma region FP_MIRROR_X
	case FP_MIRROR_X:
	{
		//***test_section*****************
		/*CMeshO::FaceIterator fi;
		fi->NewBitFlag();*/
		//*******************
		CMeshO::VertexIterator vi;
		CMeshO::FaceIterator fi;
		Point3m meshCenter;// = m.cm.bbox.Center();
		Matrix44m editTranslateBackToCenterMatrix;


		MeshModel *mirroMesh;
		foreach(int i, md.multiSelectID)
		{
			if (i != -1)
			{
				mirroMesh = md.getMesh(i);
				meshCenter = mirroMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				/*vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);
				vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);*/


				for (vi = mirroMesh->cm.vert.begin(); vi != mirroMesh->cm.vert.end(); ++vi)
				{
					vi->P().X() = -vi->P().X();
				}
				tri::Clean<CMeshO>::FlipNormalOutside(mirroMesh->cm);
				m.UpdateBoxAndNormals();
				m.clearDataMask(MeshModel::MM_FACEFACETOPO);

				//vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mirroMesh->cm);
				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				/*vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);*/
				//vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);
				//tri::UpdateNormal<CMeshO>(mirroMesh);

			}


		}

	}break;
#pragma endregion FP_MIRROR_X
#pragma region FP_MIRROR_Y
	case FP_MIRROR_Y:
	{
		//***test_section*****************
		/*CMeshO::FaceIterator fi;
		fi->NewBitFlag();*/
		//*******************
		CMeshO::VertexIterator vi;
		CMeshO::FaceIterator fi;
		Point3m meshCenter;// = m.cm.bbox.Center();
		Matrix44m editTranslateBackToCenterMatrix;


		MeshModel *mirroMesh;
		foreach(int i, md.multiSelectID)
		{
			if (i != -1)
			{
				mirroMesh = md.getMesh(i);
				meshCenter = mirroMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);
				//vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);

				for (vi = mirroMesh->cm.vert.begin(); vi != mirroMesh->cm.vert.end(); ++vi)
				{
					vi->P().Y() = -vi->P().Y();
				}
				//vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);
				//for (fi = mirroMesh->cm.face.begin(); fi != mirroMesh->cm.face.end(); ++fi)
				//{
				//	/*Point3f p0 = fi->P0(1);
				//	Point3f p1 = fi->P1(1);
				//	Point3f p2 = fi->P2(1);
				//	fi->P1(1) = p0;
				//	fi->P0(1) = p1;*/
				//	qDebug() << "fi->cN().Y()" << fi->cN().X() << fi->cN().Y() << fi->cN().Z();
				///*	fi->N().X() = -fi->N().X();
				//	fi->N().Y() = -fi->N().Y();
				//	fi->N().Z() = -fi->N().Z();*/
				//	//fi->N() = -fi->N();
				//	
				//	qDebug() << "fi->cN().Y()" << fi->cN().X() << fi->cN().Y()<<fi->cN().Z();
				//}								
				tri::Clean<CMeshO>::FlipNormalOutside(mirroMesh->cm);
				m.UpdateBoxAndNormals();
				m.clearDataMask(MeshModel::MM_FACEFACETOPO);

				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				/*vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);*/
				/*tri::UpdateTopology<CMeshO>::FaceFace(mirroMesh->cm);
				vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);*/
			}
		}
	}break;
#pragma endregion FP_MIRROR_Y
#pragma region FP_MIRROR_Z
	case FP_MIRROR_Z:
	{
		//***test_section*****************
		/*CMeshO::FaceIterator fi;
		fi->NewBitFlag();*/
		//*******************
		CMeshO::VertexIterator vi;
		CMeshO::FaceIterator fi;
		Point3m meshCenter;// = m.cm.bbox.Center();
		Matrix44m editTranslateBackToCenterMatrix;


		MeshModel *mirroMesh;
		foreach(int i, md.multiSelectID)
		{
			if (i != -1)
			{
				mirroMesh = md.getMesh(i);
				meshCenter = mirroMesh->cm.bbox.Center();

				editTranslateBackToCenterMatrix.SetTranslate(-(meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);

				for (vi = mirroMesh->cm.vert.begin(); vi != mirroMesh->cm.vert.end(); ++vi)
				{
					vi->P().Z() = -vi->P().Z();
				}
				tri::Clean<CMeshO>::FlipNormalOutside(mirroMesh->cm);
				m.UpdateBoxAndNormals();
				m.clearDataMask(MeshModel::MM_FACEFACETOPO);

				//vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mirroMesh->cm);
				editTranslateBackToCenterMatrix.SetTranslate((meshCenter));//***back to center
				tri::UpdatePosition<CMeshO>::Matrix(mirroMesh->cm, editTranslateBackToCenterMatrix, true);
				tri::UpdateBounding<CMeshO>::Box(mirroMesh->cm);
				/*vcg::tri::UpdateNormal<CMeshO>::PerVertex(mirroMesh->cm);*/
				//vcg::tri::UpdateNormal<CMeshO>::PerFace(mirroMesh->cm);
				//tri::UpdateNormal<CMeshO>(mirroMesh);

			}


		}

	}break;
#pragma endregion FP_MIRROR_Z
#pragma region FP_JUSTIFY_FRONT
	case FP_JUSTIFY_FRONT:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 1;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}

	}break;
#pragma endregion FP_JUSTIFY_FRONT
#pragma region FP_JUSTIFY_BACK
	case FP_JUSTIFY_BACK:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 4;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}

	}break;
#pragma endregion FP_JUSTIFY_BACK	
#pragma region FP_JUSTIFY_MIN_Y
	case FP_JUSTIFY_LEFT:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 0;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}
	}break;
#pragma endregion FP_JUSTIFY_LEFT
#pragma region FP_JUSTIFY_RIGHT
	case FP_JUSTIFY_RIGHT:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 3;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}
	}break;
#pragma endregion FP_JUSTIFY_RIGHT
#pragma region FP_JUSTIFY_BOTTOM
	case FP_JUSTIFY_BOTTOM:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 2;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}

	}break;
#pragma endregion FP_JUSTIFY_BOTTOM
#pragma region FP_JUSTIFY_TOP
	case FP_JUSTIFY_TOP:
	{
		bool all_or_not = true;// par.getBool("all_or_not");
		bool collision_detected = true;// par.getBool("collision_detected");
		int align_axis = 5;//par.getEnum("Align_Axis");

		int a;
		//
		if (all_or_not)a = 0;
		else a = 0;
		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				do
				{
					xyz::recursive_align_axis<Scalarm>(md, bt_gap, align_axis);
				} while (!xyz::collide_all_test<Scalarm>(md));
			}
			else{
				Matrix44m landToGround;
				Point3m translatePath;

				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					switch (par.getEnum("Align_Axis"))
					{
					case 0: translatePath = Point3m(md.groove.min.X() - bm->cm.bbox.min.X(), 0, 0); break;
					case 1: translatePath = Point3m(0, md.groove.min.Y() - bm->cm.bbox.min.Y(), 0); break;
					case 2: translatePath = Point3m(0, 0, md.groove.min.Z() - bm->cm.bbox.min.Z()); break;
					case 3: translatePath = Point3m(md.groove.max.X() - bm->cm.bbox.max.X(), 0, 0); break;
					case 4: translatePath = Point3m(0, md.groove.max.Y() - bm->cm.bbox.max.Y(), 0); break;
					case 5: translatePath = Point3m(0, 0, md.groove.max.Z() - bm->cm.bbox.max.Z()); break;
					}

					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}

			}
		}break;
		case 0://current mesh
		{
			if (collision_detected)
			{
				switch (align_axis)
				{
				case 0:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.min.X() - bm->cm.bbox.min.X();;
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 0, translatePath);
					}

				}
				break;
				case 1:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.min.Y() - bm->cm.bbox.min.Y();;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 1, translatePath);
					}
				}
				break;
				case 2:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 2, translatePath);
					}
				}
				break;
				case 3:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = md.groove.max.X() - bm->cm.bbox.max.X();
						translatePath.Y() = 0;
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 3, translatePath);
					}
				}
				break;
				case 4:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = md.groove.max.Y() - bm->cm.bbox.max.Y();
						translatePath.Z() = 0;
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 4, translatePath);
					}
				}
				break;
				case 5:
				{
					foreach(int k, md.multiSelectID)
					{
						MeshModel *bm = md.getMesh(k);
						Point3m translatePath;
						translatePath.X() = 0;
						translatePath.Y() = 0;
						translatePath.Z() = md.groove.max.Z() - bm->cm.bbox.max.Z();
						xyz::recurcisve_one_mesh_box_axis<float>(*bm, md, bt_gap, true, 5, translatePath);
					}
				}
				break;
				}

			}
			else
			{
				switch (align_axis)
				{
				case 0:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.min.X() - m.cm.bbox.min.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 1:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.min.Y() - m.cm.bbox.min.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 2:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - m.cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 3:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = md.groove.max.X() - m.cm.bbox.max.X();
					translatePath.Y() = 0;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 4:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = md.groove.max.Y() - m.cm.bbox.max.Y();;
					translatePath.Z() = 0;
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				case 5:
				{
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.max.Z() - m.cm.bbox.max.Z();
					landToGround.SetTranslate(translatePath);
					m.cm.Tr = landToGround * m.cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(m.cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}
				break;
				}


			}

		}break;
		}

	}break;
#pragma endregion FP_JUSTIFY_TOP
#pragma region FP_COUNT_HOLES
	case FP_COUNT_HOLES:
	{
		int hole = 0;
		//foreach(MeshModel *bm, md.meshList)
		//{

		MeshModel *copy = new MeshModel(&md, bm->fullName(), "");
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(copy->cm, bm->cm);
		//copy->cm.Tr = bm->cm.Tr;
		//copy->rmm = bm->rmm;

		tri::Clean<CMeshO>::RemoveDuplicateVertex(copy->cm);

		if (copy->getMeshSort() == MeshModel::meshsort::print_item)
		{
			copy->cm.face.EnableFFAdjacency();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(copy->cm);


			for (CMeshO::FaceIterator fi = copy->cm.face.begin(); fi != copy->cm.face.end(); ++fi)
			{

				for (int p = 0; p < 3; p++)
				{
					if (vcg::face::IsBorder(*fi, p))
					{
						hole++;
					}
				}
			}
			//tri::UpdateFlags<CMeshO>::FaceBorderFromNone(bm->cm);
		}
		//if (bm->getMeshSort() == MeshModel::meshsort::print_item)
		//hole+=vcg::tri::Clean<CMeshO>::CountHoles(bm->cm);
		md.delMesh(copy);
		copy->Clear();
		//}

		par.addParam(new RichInt("holeNum", hole, "", ""));
	}break;
#pragma endregion FP_COUNT_HOLES
#pragma region FP_Test_Quaternion
	case FP_Test_Quaternion:
	{
		/*Matrix44f moveToCenter;
		moveToCenter.SetTranslate(-m.cm.bbox.Center());

		tri::UpdatePosition<CMeshO>::Matrix(m.cm, moveToCenter, true);
		tri::UpdateBounding<CMeshO>::Box(m.cm);*/

		
		float testValue = par.getFloat("Quarternion_test_param");
		float start_radian = par.getFloat("QUATENION_START_RADIAN");
		float end_radian = par.getFloat("QUATENION_END_RADIAN");
		Point3f start_Axis(par.getFloat("QUATENION_START_X"), par.getFloat("QUATENION_START_Y"), par.getFloat("QUATENION_START_Z"));
		Point3f end_Axis(par.getFloat("QUATENION_END_X"), par.getFloat("QUATENION_END_Y"), par.getFloat("QUATENION_END_Z"));
		Matrix44f testmatrix;
		vcg::Quaternion<float> qfrom;
		vcg::Quaternionf::Construct(qfrom);
		qfrom.FromAxis(start_radian, start_Axis);
		//qfrom.ToMatrix(testmatrix);

		vcg::Quaternion<float> qfrom2;
		vcg::Quaternionf::Construct(qfrom2);
		qfrom2.FromAxis(end_radian, end_Axis);
		//qfrom2.ToMatrix(testmatrix);

		
		vcg::Quaternion<float> qfromInter;
		vcg::Quaternionf::Construct(qfromInter);
		//qfromInter.slerp(qfrom, qfrom2, 0.5);

		/*qfromInter = vcg::qqInterpolate(qfrom, qfrom2, 0.5);
		qfromInter.ToMatrix(testmatrix);*/

		float interpolate = 50.;
		//float pan_step = 2;
		Point3f pan_step(2, 0, 0);

		for (int i = 0; i < interpolate; i++)
		{
			QString name = "slerp_" + QString::number(i);
			
			
			RenderMode *newMRM = new RenderMode(GLW::DMFlat);
			if (tri::HasPerWedgeTexCoord(m.cm))
			{
				newMRM->setTextureMode(GLW::TMPerWedgeMulti);
				newMRM->setColorMode(m.rmm.colorMode);
				newMRM->setDrawMode(GLW::DMFlat);
			}
			else if (tri::HasPerVertexColor(m.cm))
			{
				newMRM->setTextureMode(GLW::TMNone);
				newMRM->setColorMode(m.rmm.colorMode);
				newMRM->setDrawMode(GLW::DMFlat);
			}

			MeshModel* currentMesh = md.addNewMesh("", name, false, *newMRM);
			currentMesh->updateDataMask(m.dataMask());//複製mask;
			currentMesh->clearDataMask(MeshModel::MM_COLOR);

			
			vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(currentMesh->cm, m.cm, false);
			tri::UpdateBounding<CMeshO>::Box(currentMesh->cm);
			currentMesh->cm.Tr = m.cm.Tr;
			currentMesh->rmm = m.rmm;
			currentMesh->glw.curr_hints = m.glw.curr_hints;

			vcg::qqInterpolate(qfrom, qfrom2, i / interpolate).ToMatrix(testmatrix);
			

			tri::UpdatePosition<CMeshO>::Matrix(currentMesh->cm, testmatrix, true);
			tri::UpdateBounding<CMeshO>::Box(currentMesh->cm);

			Matrix44m pan;
			pan.SetTranslate(pan_step*i);
			tri::UpdatePosition<CMeshO>::Matrix(currentMesh->cm, pan, true);
			tri::UpdateBounding<CMeshO>::Box(currentMesh->cm);

		}
		



		


	}
	break;
#pragma endregion FP_Test_Quaternion
#pragma region FP_SEPERATE_TEST
	case FP_SEPERATE_TEST:
	{
		std::vector<Point3<float> > ptVec;
		std::vector<Point3<float> > nmVec;

		tri::UpdateSelection<CMeshO>::FaceClear(m.cm);

		std::string name = "";

		CMeshO::FaceIterator fii = m.cm.face.begin();
		int count = 0;
		


		while (1)
		//for (int i = 0; i < 10; i++)
		{
			int count = 0;
			tri::UpdateSelection<CMeshO>::Clear(m.cm);
			for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			{
				if ((!(*fi).IsD()) && (!(*fi).IsUserBit(0x00400000)))
				{					
					(*fi).SetS();//select a face					
					break;
				}
				else
					count++;
			}
			tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);//select connected face
			//tri::UpdateSelection<CMeshO>::FaceConnectedF(m.cm);//select connected face
			int count2 = 0;
			for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			{
				if ((!(*fi).IsD()) && ((*fi).IsS()))
				{
					(*fi).SetUserBit(0x00400000);					
				}
			}
			if (count == m.cm.face.size())break;

			QString name = "separateItem_" + QString::number(count2);
			count2++;


			MeshModel* temp = md.addNewMesh("", name, false, RenderMode(GLW::DMTexWire));
			vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(temp->cm, m.cm, true);
			tri::UpdateBounding<CMeshO>::Box(temp->cm);
			temp->cm.Tr = m.cm.Tr;
			temp->rmm = m.rmm;
			temp->glw.curr_hints = m.glw.curr_hints;
			fii++;
		}
		



		


		//vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(cap->cm,m.cm,true);


		//for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi){
		//	if (!(*fi).IsD() && (*fi).IsS()) {
		//		Point3<float> p_first(0, 0, 0);
		//		Point3<float> p_second(0, 0, 0);
		//		ptVec.resize(3, p_first);


		//		//NewSelFace.push_back(&*fi);
		//		//Color4b newColor = Color4b(255, 0, 0, 255);
		//		//fi->C() = newColor;
		//		for (int j = 0; j < 3; ++j)
		//		{
		//			ptVec[j] = fi->V0(j)->cP();
		//		}
		//		{
		//			


		//			
		//		}
		//	}

		//}

		//vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
		//vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
		//(*vi).N()

		/*VertexPointer  *ivp = new VertexPointer[VN];

		VertexIterator vi = in.vert.begin();
		ivp[0] = &*vi;
		(*vi).P() = CoordType(0, -h / 2.0, 0); 
		++vi;

		ivp[1] = &*vi;
		(*vi).P() = CoordType(0, h / 2.0, 0); 
		++vi;

		FaceIterator fi=in.face.begin();

		if(r1!=0) for(i=0;i<SubDiv;++i,++fi)	{
		(*fi).V(0)=ivp[0];
		(*fi).V(1)=ivp[b1+i];
		(*fi).V(2)=ivp[b1+(i+1)%SubDiv];
		}

		if(r2!=0) for(i=0;i<SubDiv;++i,++fi) {
		(*fi).V(0)=ivp[1];
		(*fi).V(2)=ivp[b2+i];
		(*fi).V(1)=ivp[b2+(i+1)%SubDiv];
		}

		*/


		//(*fi).V(0) = ivp[0];


	}break;
#pragma endregion FP_SEPERATE_TEST
	}
	return true;

}


MESHLAB_PLUGIN_NAME_EXPORTER(ExtraMeshFilterPlugin)
