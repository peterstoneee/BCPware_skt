/****************************************************************************
/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
* My Extension to VCGLib
*
*  Copyright 2018 by
*
* This file is part of VCGLib.
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
#ifndef __SLICE_FUNCTION
#define __SLICE_FUNCTION

#include <common/interfaces.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/hole.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/bitquad_creation.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/attribute_seam.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/update/curvature_fitting.h>
#include <vcg/complex/algorithms/pointcloud_normal.h>
#include <vcg/space/fitting3.h>
#include <wrap/gl/glu_tessellator_cap.h>

#include <functional>

using namespace vcg;
namespace xyz{
	void delOl(MeshDocument &md, QString s1, QString s2)
	{
		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
			{
				md.delMesh(bm);
			}

		}
	}


	template<typename scalarType>
	void slice_roution(MeshDocument &md, scalarType sliceheight, bool faceColor = false)
	{
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
		}*/
		QString olMName = md.p_setting.getOlMeshName();
		QString capMName = md.p_setting.getCapMeshName();
		//delOl(md, "_temp_outlines", "_temp_ol_mesh");
		delOl(md, olMName, capMName);
		//*****************************************
		Point3m planeAxis(0, 0, 1);
		planeAxis.Normalize();

		Point3m planeCenter;
		Plane3m slicingPlane;


		//***20150428會當掉，還沒查到原因
		//m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		//qDebug("5_place");		
		planeCenter = Point3m(0, 0, sliceheight);//從最底部開始		
		qDebug("planeCenter %f, %f, %f", planeCenter[0], planeCenter[1], planeCenter[2]);
		slicingPlane.Init(planeCenter, planeAxis);//init切平面
		//******切層演算法***************
		//QList<MeshModel*> tmml = md.meshList;
		//qDebug("2_place");
		QString cap3in = olMName + "_3_intersect";
		foreach(MeshModel *tmm, md.meshList)
		{
			/*bool b1 = vcg::tri::HasPerWedgeTexCoord(tmm->cm);
			bool b2 = vcg::tri::HasPerVertexColor(tmm->cm);
			bool b3 = vcg::tri::HasPerFaceColor(tmm->cm);*/
			if (sliceheight >= tmm->cm.bbox.min.Z() && sliceheight <= tmm->cm.bbox.max.Z())
			if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) || vcg::tri::HasPerVertexColor(tmm->cm) || vcg::tri::HasPerFaceColor(tmm->cm))
			{
				//MeshModel *cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMTexWire));
				//MeshModel *cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//***backup						
				MeshModel *cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//***210151109
				//MeshModel *cap3 = md.addNewMesh("", cap3in, false, RenderMode(GLW::DMTexWire));//***210151113
				cap->setMeshSort(MeshModel::meshsort::slice_item);
				//***20151109
				cap->rmm.drawMode = GLW::DMTexWire;//backup


				//MeshModel *cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMOutline_Test));
				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm))
				{
					cap->Enable(vcg::tri::io::Mask::IOM_VERTTEXCOORD);
					//cap->rmm.textureMode = GLW::TMPerVert;
					//qDebug("2_place %i, %i, ", tmm->glw.TMId[0], tmm->glw.TMId[1]);
				}
				//判斷有無材質，且存在材質
				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) && !tmm->cm.textures.empty() && tmm->rmm.textureMode != vcg::GLW::TMNone)
					cap->rmm.textureMode = GLW::TMPerVert;


				if ((vcg::tri::HasPerVertexColor(tmm->cm) || vcg::tri::HasPerFaceColor(tmm->cm)) /*&& (tmm->rmm.colorMode == GLW::ColorMode::CMPerVert || tmm->rmm.colorMode == GLW::ColorMode::CMPerFace)*/)
				{
					cap->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
					//===20151111
					cap->Enable(vcg::tri::io::Mask::IOM_FACECOLOR);
					//cap->rmm.colorMode = tmm->rmm.colorMode;//***20160301
					cap->rmm.colorMode = GLW::CMPerVert;//backup

				}
				cap->glw.TMIdd = tmm->glw.TMIdd;
				//vcg::IntersectionPlane_TexMesh<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm, faceColor);

				//GLW::CMNone 0
				//GLW::CMPerVert 3
				//GLW::CMPerFace 2
				int tempmode = (int)tmm->rmm.colorMode;
				//int tempTextureMode


				vcg::IntersectionPlane_TexMesh<CMeshO, CMeshO, Scalarm>(tmm->cm, slicingPlane, cap->cm, faceColor, tempmode);
				//tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);//不用產生cap,backup
				//***20150811******************										 
				//tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
				//vcg::sortEdge<CMeshO, CMeshO, OutlinePointAttrib>(cap->cm, cap->outlines_2);
				//******************************	

				//***20160226	
				//MeshModel* cap2 = md.addNewMesh("", "_temp_ol_mesh", false, RenderMode(GLW::DMCapEdge));
				//RenderMode tempR(GLW::DMCapEdge);										
				//MeshModel* cap2 = md.addNewMesh("", capMName, false, tempR);
				//cap2->setMeshSort(MeshModel::meshsort::slice_item);
				//tri::My_CapEdgeMesh(cap->cm, cap2->cm, cap2->outlines);//backup
				//cap2->UpdateBoxAndNormals();
				//
				//qDebug("sampler_filter_3_place");
			}
			else
			{
				//MeshModel* cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMTexWire));//產生的mesh cap設定為不是當下的mesh
				MeshModel* cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//產生的mesh cap設定為不是當下的mesh
				cap->setMeshSort(MeshModel::meshsort::slice_item);
				//MeshModel* cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMOutline_Test));//產生的mesh cap設定為不是當下的mesh
				vcg::IntersectionPlaneMesh<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);

				//MeshModel* cap2 = md.addNewMesh("", "_temp_ol_mesh", false, RenderMode(GLW::DMCapEdge));
				MeshModel* cap2 = md.addNewMesh("", capMName, false, RenderMode(GLW::DMCapEdge));
				tri::My_CapEdgeMesh(cap->cm, cap2->cm, cap2->outlines);
				cap2->UpdateBoxAndNormals();
				qDebug("slice_function_outlines %i", cap2->outlines.size());



			}
		}

	}
	template<typename scalarType>
	void slice_roution_selected(MeshDocument &md, scalarType sliceheight, bool faceColor = false)
	{

		QString olMName = md.p_setting.getOlMeshName();
		QString capMName = md.p_setting.getCapMeshName();
		delOl(md, olMName, capMName);
		//*****************************************
		Point3m planeAxis(0, 0, 1);
		planeAxis.Normalize();

		Point3m planeCenter;
		Plane3m slicingPlane;


		//***20150428會當掉，還沒查到原因
		//m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		//qDebug("5_place");		
		planeCenter = Point3m(0, 0, sliceheight);//從最底部開始		
		qDebug("planeCenter %f, %f, %f", planeCenter[0], planeCenter[1], planeCenter[2]);
		slicingPlane.Init(planeCenter, planeAxis);//init切平面
		//******切層演算法***************
		//QList<MeshModel*> tmml = md.meshList;
		//qDebug("2_place");
		QString cap3in = olMName + "_3_intersect";
		foreach(int id, md.multiSelectID)
		{
			MeshModel *tmm = md.getMesh(id);
			/*bool b1 = vcg::tri::HasPerWedgeTexCoord(tmm->cm);
			bool b2 = vcg::tri::HasPerVertexColor(tmm->cm);
			bool b3 = vcg::tri::HasPerFaceColor(tmm->cm);*/
			if (sliceheight >= tmm->cm.bbox.min.Z() && sliceheight <= tmm->cm.bbox.max.Z())
			if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) || vcg::tri::HasPerVertexColor(tmm->cm) || vcg::tri::HasPerFaceColor(tmm->cm))
			{
				//MeshModel *cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMTexWire));
				//MeshModel *cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//***backup						
				MeshModel *cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//***210151109
				//MeshModel *cap3 = md.addNewMesh("", cap3in, false, RenderMode(GLW::DMTexWire));//***210151113
				cap->setMeshSort(MeshModel::meshsort::slice_item);
				//***20151109
				cap->rmm.drawMode = GLW::DMTexWire;//backup


				//MeshModel *cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMOutline_Test));
				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm))
				{
					cap->Enable(vcg::tri::io::Mask::IOM_VERTTEXCOORD);
					//cap->rmm.textureMode = GLW::TMPerVert;
					//qDebug("2_place %i, %i, ", tmm->glw.TMId[0], tmm->glw.TMId[1]);
				}
				//判斷有無材質，且存在材質
				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) && !tmm->cm.textures.empty())
					cap->rmm.textureMode = GLW::TMPerVert;

				if ((vcg::tri::HasPerVertexColor(tmm->cm) || vcg::tri::HasPerFaceColor(tmm->cm)) /*&& (tmm->rmm.colorMode == GLW::ColorMode::CMPerVert || tmm->rmm.colorMode == GLW::ColorMode::CMPerFace)*/)
				{
					cap->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
					//===20151111
					cap->Enable(vcg::tri::io::Mask::IOM_FACECOLOR);
					//cap->rmm.colorMode = tmm->rmm.colorMode;//***20160301
					cap->rmm.colorMode = GLW::CMPerVert;//backup

				}
				cap->glw.TMIdd = tmm->glw.TMIdd;
				//vcg::IntersectionPlane_TexMesh<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm, faceColor);

				//GLW::CMNone 0
				//GLW::CMPerVert 3
				//GLW::CMPerFace 2
				int tempmode = (int)tmm->rmm.colorMode;


				vcg::IntersectionPlane_TexMesh<CMeshO, CMeshO, Scalarm>(tmm->cm, slicingPlane, cap->cm, faceColor, tempmode);
				//tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);//不用產生cap,backup
				//***20150811******************										 
				//tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
				//vcg::sortEdge<CMeshO, CMeshO, OutlinePointAttrib>(cap->cm, cap->outlines_2);
				//******************************	

				//***20160226	
				//MeshModel* cap2 = md.addNewMesh("", "_temp_ol_mesh", false, RenderMode(GLW::DMCapEdge));
				//RenderMode tempR(GLW::DMCapEdge);										
				//MeshModel* cap2 = md.addNewMesh("", capMName, false, tempR);
				//cap2->setMeshSort(MeshModel::meshsort::slice_item);
				//tri::My_CapEdgeMesh(cap->cm, cap2->cm, cap2->outlines);//backup
				//cap2->UpdateBoxAndNormals();
				//
				//qDebug("sampler_filter_3_place");
			}
			else
			{
				//MeshModel* cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMTexWire));//產生的mesh cap設定為不是當下的mesh
				MeshModel* cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//產生的mesh cap設定為不是當下的mesh
				cap->setMeshSort(MeshModel::meshsort::slice_item);
				//MeshModel* cap = md.addNewMesh("", "_temp_outlines", false, RenderMode(GLW::DMOutline_Test));//產生的mesh cap設定為不是當下的mesh
				vcg::IntersectionPlaneMesh<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);

				//MeshModel* cap2 = md.addNewMesh("", "_temp_ol_mesh", false, RenderMode(GLW::DMCapEdge));
				MeshModel* cap2 = md.addNewMesh("", capMName, false, RenderMode(GLW::DMCapEdge));
				tri::My_CapEdgeMesh(cap->cm, cap2->cm, cap2->outlines);
				cap2->UpdateBoxAndNormals();
				qDebug("slice_function_outlines %i", cap2->outlines.size());



			}
		}

	}


	void findMINMax(float x0, float x1, float x2, float minn, float maxx)
	{
		minn = maxx = x0;

		if (x1<minn) minn = x1;

		if (x1>maxx) maxx = x1;

		if (x2<minn) minn = x2;

		if (x2>maxx) maxx = x2;
	}
	int planeBoxOverlap(Point3m normal, Point3m vert, Point3m maxbox)	// -NJMP-
	{

		int q;

		float vmin[3], vmax[3], v;

		for (q = 0; q <= 2; q++)

		{

			v = vert[q];					// -NJMP-

			if (normal[q] > 0.0f)

			{

				vmin[q] = -maxbox[q] - v;	// -NJMP-

				vmax[q] = maxbox[q] - v;	// -NJMP-

			}

			else

			{

				vmin[q] = maxbox[q] - v;	// -NJMP-

				vmax[q] = -maxbox[q] - v;	// -NJMP-

			}

		}

		if ((normal*vmin) > 0.0f) return 0;	// -NJMP-

		if ((normal* vmax) >= 0.0f) return 1;	// -NJMP-



		return 0;

	}

#if 1
	bool triangleIntersectCube_v2(Point3m v0, Point3m v1, Point3m v2, Box3m bb)
	{
		//moving triangle to center
		Point3m center = bb.Center();
		v0 -= center;
		v1 -= center;
		v2 -= center;
		bb.Translate(-center);

		Point3m boxhalfsize = bb.max - bb.Center();

		//triangle edge vector
		Point3m e0 = v1 - v0;
		Point3m e1 = v2 - v1;
		Point3m e2 = v0 - v2;




		float p0, p1, p2, rad, fex, fey, fez, minn, maxx;
		

		std::function<bool (float, float, float, float )> funcTestX01 = 
		[&](float a, float b, float fa, float fb)->bool
		{
			p0 = a*v0.Y() - b*v0.Z();
			p2 = a*v2.Y() - b*v2.Z();			       	   
			if (p0<p2) { minn = p0; maxx = p2; }
			else { minn = p2; maxx = p0; }
			rad = fa * boxhalfsize.Y() + fb * boxhalfsize.Z();
			if (minn>rad || maxx<-rad) return 0;
		};

		std::function<bool(float, float, float, float)> funcTestX2 =
			[&](float a, float b, float fa, float fb)->bool
		{
			p0 = a*v0.Y() - b*v0.Z();
			p1 = a*v1.Y() - b*v1.Z();
			if (p0<p1) { minn = p0; maxx = p1; }
			else { minn = p1; maxx = p0; }
			rad = fa * boxhalfsize.Y() + fb * boxhalfsize.Z();
			if (minn>rad || maxx<-rad) return 0;
		};


		std::function<bool(float, float, float, float)> funcTestY02 =
			[&](float a, float b, float fa, float fb)->bool
		{
			p0 = -a*v0.X() + b*v0.Z();
			p2 = -a*v2.X() + b*v2.Z();
			if (p0<p2) { minn = p0; maxx = p2; }
			else { minn = p2; maxx = p0; }
			rad = fa * boxhalfsize.X() + fb * boxhalfsize.Z();
			if (minn>rad || maxx<-rad) return 0;
		};

		std::function<bool(float, float, float, float)> funcTestY1 =
			[&](float a, float b, float fa, float fb)->bool
		{
			p0 = -a*v0.X() + b*v0.Z();
			p1 = -a*v1.X() + b*v1.Z();
			if (p0<p1) { minn = p0; maxx = p1; }
			else { minn = p1; maxx = p0; }
			rad = fa * boxhalfsize.X() + fb * boxhalfsize.Z();
			if (minn>rad || maxx<-rad) return 0;
		};

		std::function<bool(float, float, float, float)> funcTestZ12 =
			[&](float a, float b, float fa, float fb)->bool
		{
			p1 = a*v1.X() - b*v1.Y();
			p2 = a*v2.X() - b*v2.Y();
			if (p2<p1) { minn = p2; maxx = p1; }
			else { minn = p1; maxx = p2; }
			rad = fa * boxhalfsize.X() + fb * boxhalfsize.Y();
			if (minn>rad || maxx<-rad) return 0;
		};

		std::function<bool(float, float, float, float)> funcTestZ0 =
			[&](float a, float b, float fa, float fb)->bool
		{
			p0 = a*v0.X() - b*v0.Y();
			p1 = a*v1.X() - b*v1.Y();
			if (p0<p1) { minn = p0; maxx = p1; }
			else { minn = p1; maxx = p0; }
			rad = fa * boxhalfsize.X() + fb * boxhalfsize.Y();
			if (minn>rad || maxx<-rad) return 0;
		};

		std::function<void(float, float, float)> findMINMAX = [&](float x0, float x1, float x2)
		{
			minn = maxx = x0;
			if (x1 < minn)minn = x1;
			if (x1>maxx) maxx = x1;
			if (x2<minn) minn = x2;
			if (x2>maxx) maxx = x2;

		};



		fex = fabsf(e0.X());
		fey = fabsf(e0.Y());
		fez = fabsf(e0.Z());
		bool test = funcTestX01(e0[2], e0[1], fez, fey);
		if (!test)
			return 0;
		test = funcTestY02(e0[2], e0[0], fez, fex);
		if (!test)
			return 0;
		test = funcTestZ12(e0[1], e0[0], fey, fex);
		if (!test)
			return 0;


		fex = fabsf(e1.X());
		fey = fabsf(e1.Y());
		fez = fabsf(e1.Z());
		test = funcTestX01(e1[2], e1[1], fez, fey);
		if (!test)
			return 0;
		test = funcTestY02(e1[2], e1[0], fez, fex);
		if (!test)
			return 0;
		test = funcTestZ0(e1[1], e1[0], fey, fex);
		if (!test)
			return 0;
		fex = fabsf(e2.X());
		fey = fabsf(e2.Y());
		fez = fabsf(e2.Z());
		test = funcTestX2(e2[2], e2[1], fez, fey);
		if (!test)
			return 0;
		test = funcTestY1(e2[2], e2[0], fez, fex);
		if (!test)
			return 0;
		test = funcTestZ12(e2[1], e2[0], fey, fex);
		if (!test)
			return 0;

		findMINMAX(v0[0], v1[0], v2[0]);
		if (minn>boxhalfsize[0] || maxx<-boxhalfsize[0])
			return 0;

		findMINMAX(v0[1], v1[1], v2[1]);
		if (minn>boxhalfsize[1] || maxx<-boxhalfsize[1]) 
			return 0;

		/* test in Z-direction */

		findMINMAX(v0[2], v1[2], v2[2]);
		if (minn>boxhalfsize[2] || maxx<-boxhalfsize[2])
			return 0;


		//triangle normal
		Point3m faceNormal = e0^e1;

		//box normal
		Point3m bn0(1, 0, 0);
		Point3m bn1(0, 1, 0);
		Point3m bn2(0, 0, 1);

		


		/*face normal*/
		Point3m center_n = bb.Center();
		Point3m extent = bb.max - center_n;
		

		//return abs(s) <= rr;

		if (!planeBoxOverlap(faceNormal, v0, extent))
			return 0;	// -NJMP-






		return true;


	}
#endif
	
	//bool tritriIntersection()
	//{
	//	/*test tri tri intersection*/
	//	Point3m v10(0, 0, 0), v11(3, 0, 0), v12(0, 0, 3);
	//	Point3m v20(0, 1, 0), v21(3, 1, 0), v22(0, -1, 3);
	//	//Point3m e1_10 = v11 - v10;
	//	//Point3m e1_20 = v12 - v10;
	//	//Point3m n1 = e1_10 ^ e1_20;

	//	Plane3f tri1_plane;
	//	tri1_plane.Init(v10, v11, v12);
	//	float d1 = vcg::SignedDistancePointPlane(v20, tri1_plane);
	//	float d2 = vcg::SignedDistancePointPlane(v21, tri1_plane);
	//	float d3 = vcg::SignedDistancePointPlane(v22, tri1_plane);
	//	if ((d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0))
	//		return false;


	//	//Plane3f 
	//	return true;
	//	/**/
	//}



	bool triangleIntersectCube(Point3m v0, Point3m v1, Point3m v2, Box3m bb)
	{
		//moving triangle to center
		Point3m center = bb.Center();
		v0 -= center;
		v1 -= center;
		v2 -= center;
		bb.Translate(-center);

		//triangle edge vector
		Point3m e0t = v1 - v0;
		Point3m e1t = v2 - v1;
		Point3m e2t = v0 - v2;

		//triangle normal
		Point3m faceNormal = e0t^e1t;

		//box normal
		Point3m bn0(1, 0, 0);
		Point3m bn1(0, 1, 0);
		Point3m bn2(0, 0, 1);

		//13 axis
		float halfbb_x = fabs(bb.DimX() / 2.);
		float halfbb_y = fabs(bb.DimY() / 2.);
		float halfbb_z = fabs(bb.DimZ() / 2.);

		/*9 Axis*/
		Point3m axis_bn0_e0t = bn0 ^ e0t;//1st
		Point3m axis_bn0_e1t = bn0 ^ e1t;//2nd
		Point3m axis_bn0_e2t = bn0 ^ e2t;//3rd

		Point3m axis_bn1_e0t = bn1 ^ e0t;//4th
		Point3m axis_bn1_e1t = bn1 ^ e1t;//5th
		Point3m axis_bn1_e2t = bn1 ^ e2t;//6th

		Point3m axis_bn2_e0t = bn2 ^ e0t;//7th
		Point3m axis_bn2_e1t = bn2 ^ e1t;//8th
		Point3m axis_bn2_e2t = bn2 ^ e2t;//9th

		/*test 1st axis p0=p1 yz*/
		//project triangle to axis
		float p0 = v0*axis_bn0_e0t;
		float p1 = v1*axis_bn0_e0t;
		float p2 = v2*axis_bn0_e0t;

		//project box to axis
		float r =
			halfbb_y * abs(bn1*axis_bn0_e0t) +
			halfbb_z * abs(bn2*axis_bn0_e0t);

		if (Max(-Max(p0, p2), Min(p0, p2)) > r) {
			return false;
		}

		/*test 4th axis p0 = p1, xz*/
		p0 = v0*axis_bn1_e0t;
		p1 = v1*axis_bn1_e0t;
		p2 = v2*axis_bn1_e0t;

		r = halfbb_x * abs(bn0*axis_bn1_e0t) +
			halfbb_z * abs(bn2*axis_bn1_e0t);

		if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) {
			return false;
		}
		/*test 7th axis p0 = p1, xy*/
		p0 = v0*axis_bn2_e0t;
		p1 = v1*axis_bn2_e0t;
		p2 = v2*axis_bn2_e0t;

		r = halfbb_x * abs(bn0*axis_bn2_e0t) +
			halfbb_y * abs(bn1*axis_bn2_e0t);

		if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r)
			return false;



		/*test 2st axis p1=p2 yz*/
		//project triangle to axis
		p0 = v0*axis_bn0_e1t;
		p1 = v1*axis_bn0_e1t;
		p2 = v2*axis_bn0_e1t;

		r = halfbb_y * abs(bn1*axis_bn0_e1t) +
			halfbb_z * abs(bn2*axis_bn0_e1t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r)
			return false;


		/*test 5th axis p1 = p2, xz*/
		p0 = v0*axis_bn1_e1t;
		p1 = v1*axis_bn1_e1t;
		p2 = v2*axis_bn1_e1t;

		r = halfbb_x * abs(bn0*axis_bn1_e1t) +
			halfbb_z * abs(bn2*axis_bn1_e1t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) {
			return false;
		}
		/*test 8th axis p1 = p2, xy*/
		p0 = v0*axis_bn2_e1t;
		p1 = v1*axis_bn2_e1t;
		p2 = v2*axis_bn2_e1t;

		r = halfbb_x * abs(bn1*axis_bn2_e1t) +
			halfbb_y * abs(bn2*axis_bn2_e1t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) {
			return false;
		}

		/*test 3st axis p0=p2 yz*/
		p0 = v0*axis_bn0_e2t;
		p1 = v1*axis_bn0_e2t;
		p2 = v2*axis_bn0_e2t;

		r = halfbb_y * abs(bn1*axis_bn0_e2t) +
			halfbb_z * abs(bn2*axis_bn0_e2t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r)
			return false;


		/*test 6th axis p0 = p2, xz*/
		p0 = v0*axis_bn1_e2t;
		p1 = v1*axis_bn1_e2t;
		p2 = v2*axis_bn1_e2t;

		r = halfbb_x * abs(bn0*axis_bn1_e2t) +
			halfbb_z * abs(bn2*axis_bn1_e2t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) {
			return false;
		}
		/*test 9th axis p0 = p2, xy*/
		p0 = v0*axis_bn2_e2t;
		p1 = v1*axis_bn2_e2t;
		p2 = v2*axis_bn2_e2t;

		r = halfbb_x * abs(bn1*axis_bn2_e2t) +
			halfbb_y * abs(bn2*axis_bn2_e2t);

		if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) {
			return false;
		}

		/*3 axis test*/
		float xmin = std::min(v0.X(), std::min(v1.X(), v2.X()));
		float xmax = std::max(v0.X(), std::max(v1.X(), v2.X()));
		if (xmin > halfbb_x || xmax < -halfbb_x) {
			return false;
		}

		float ymin = std::min(v0.Y(), std::min(v1.Y(), v2.Y()));
		float ymax = std::max(v0.Y(), std::max(v1.Y(), v2.Y()));
		if (ymin > halfbb_y || ymax < -halfbb_y) {
			return false;
		}

		float zmin = std::min(v0.Z(), std::min(v1.Z(), v2.Z()));
		float zmax = std::max(v0.Z(), std::max(v1.Z(), v2.Z()));
		if (zmin > halfbb_z || zmax < -halfbb_z) {
			return false;
		}


		/*face normal*/
		Point3m center_n = bb.Center();
		
		Point3m extent = bb.max - center_n;
		float rr = extent[0] * abs(faceNormal[0]) + extent[1] * abs(faceNormal[1]) + extent[2] * abs(faceNormal[2]);
		float s = center_n * faceNormal;
		float plane_distance = center_n * v0;
		//if (abs(s)-rr<  0.00001 )return false;		
		if (plane_distance > r)return false;

		//return abs(s) <= rr;

		//if (!planeBoxOverlap(faceNormal, v0, h)) return 0;	// -NJMP-




		

		return true;


	}

}


#endif