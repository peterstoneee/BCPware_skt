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
/**
	20150309
	Add color and texture attribute intersection between  a grid and a plane.
**/
#include<vcg/space/plane3.h>
#include<vcg/space/segment3.h>
#include<vcg/space/intersection3.h>
#include<vcg/complex/complex.h>
#include<vcg/complex/algorithms/closest.h>
#include<vcg/complex/algorithms/update/quality.h>
#include<vcg/complex/algorithms/update/selection.h>
#include <QDebug>


#ifndef __VCGLIB_INTERSECTION_TRI_MESH
#define __VCGLIB_INTERSECTION_TRI_MESH

namespace vcg{

// NAMING CONVENTION
// INTERSECTION<SIMPLEOBJECT,COMPLEXSTUFF>
// and it returns the portion of Complexstuff intersected by the simpleobject.

/** \addtogroup complex */
/*@{*/
/** 
    Function computing the intersection between  a grid and a plane. It returns all the cells intersected
*/
template < typename  GridType,typename ScalarType>
bool IntersectionPlaneGrid( GridType & grid, Plane3<ScalarType> plane, std::vector<typename GridType::Cell *> &cells)
{
  cells.clear();
	Point3d p,_d;
	Plane3d pl;
	_d.Import(plane.Direction());
	pl.SetDirection(_d);
	pl.SetOffset(plane.Offset());
	for( int ax = 0; ax <3; ++ax)
			{ int axis = ax;
				int axis0 = (axis+1)%3;
				int axis1 = (axis+2)%3;
				int i,j;
				Point3i pi;

				Segment3<double> seg;
				seg.P0().Import(grid.bbox.min);
				seg.P1().Import(grid.bbox.min);
				seg.P1()[axis] = grid.bbox.max[axis];

				for(i = 0 ; i <= grid.siz[axis0]; ++i){
					for(j = 0 ; j <= grid.siz[axis1]; ++j)
						{
							seg.P0()[axis0] = grid.bbox.min[axis0]+ (i+0.01) * grid.voxel[axis0] ;
							seg.P1()[axis0] = grid.bbox.min[axis0]+ (i+0.01) * grid.voxel[axis0];
							seg.P0()[axis1] = grid.bbox.min[axis1]+ (j+0.01) * grid.voxel[axis1];
							seg.P1()[axis1] = grid.bbox.min[axis1]+ (j+0.01) * grid.voxel[axis1];
              if ( IntersectionPlaneSegmentEpsilon(pl,seg,p))
								{
									pi[axis] =	std::min(std::max(0,(int)floor((p[axis ]-grid.bbox.min[axis])/grid.voxel[axis])),grid.siz[axis]);
									pi[axis0] = i;
									pi[axis1] = j;
									grid.Grid(pi,axis,cells);
								}
						}
					}
			}
		sort(cells.begin(),cells.end());
		cells.erase(unique(cells.begin(),cells.end()),cells.end());
		
		return false;
	}

/*@}*/



/** \addtogroup complex */
/*@{*/
/** \brief Compute the intersection between a trimesh and a plane building an edge mesh.
 *
    Basic Function Computing the intersection between a trimesh and a plane. It returns an EdgeMesh without needing anything else.
		Note: This version always returns a segment for each triangle of the mesh which intersects with the plane. In other
		words there are 2*n vertices where n is the number of segments fo the mesh. You can run vcg::edge:Unify to unify
		the vertices closer that a given value epsilon. Note that, due to subtraction error during triangle plane intersection,
		it is not safe to put epsilon to 0. 
*/
template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
bool IntersectionPlaneMeshOld(TriMeshType & m,
									Plane3<ScalarType>  pl,
									EdgeMeshType & em)
{
  typename EdgeMeshType::VertexIterator vi;
  typename TriMeshType::FaceIterator fi;
  em.Clear();
  Segment3<ScalarType> seg;
  for(fi=m.face.begin();fi!=m.face.end();++fi)
    if(!(*fi).IsD())
    {
      if(vcg::IntersectionPlaneTriangle(pl,*fi,seg))// intersezione piano triangolo
      {
        vcg::tri::Allocator<EdgeMeshType>::AddEdges(em,1);
        vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em,2);
        (*vi).P() = seg.P0();
        em.edge.back().V(0) = &(*vi);
        vi++;
        (*vi).P() = seg.P1();
        em.edge.back().V(1) = &(*vi);
      }
    }//end for

  return true;
}

//************20150309**inline IntersectionPlaneSegment********************
template < class ScalarType >
inline bool My_Tex_IntersectioinPlaneSegment(const Plane3<ScalarType> & plane, const Segment3<ScalarType> & s,
	Point3<ScalarType> &p0, const Segment2<ScalarType> &stex, vcg::TexCoord2<ScalarType> &p0_tex)
{
	ScalarType p1_proj = s.P1()*plane.Direction() - plane.Offset();
	ScalarType p0_proj = s.P0()*plane.Direction() - plane.Offset();
	if ((p1_proj > 0) - (p0_proj < 0)) return false;

	if (p0_proj == p1_proj) return false;

	// check that we perform the computation in a way that is independent with v0 v1 swaps
	//算出線與面的焦點
	if (p0_proj < p1_proj)
	{
		p0 = s.P0() + (s.P1() - s.P0()) * fabs(p0_proj / (p1_proj - p0_proj));
		p0_tex.P() = stex.P0() + (stex.P1() - stex.P0()) * fabs(p0_proj / (p1_proj - p0_proj));
	}
	if (p0_proj > p1_proj)
	{
		p0 = s.P1() + (s.P0() - s.P1()) * fabs(p1_proj / (p0_proj - p1_proj));
		p0_tex.P() = stex.P1() + (stex.P0() - stex.P1()) * fabs(p1_proj / (p0_proj - p1_proj));
	}
	return true;
}

template < class ScalarType >
inline bool My_Color_IntersectionPlaneSegment(const Plane3<ScalarType> & plane, const Segment3<ScalarType> & s,
	Point3<ScalarType> & p0, const Color4b &p0_color, const Color4b &p1_color, Color4b &pp_color)
{
	float p1_proj = s.P1()*plane.Direction() - plane.Offset();
	float p0_proj = s.P0()*plane.Direction() - plane.Offset();
	if ((p1_proj>0) - (p0_proj<0)) return false;

	if (p0_proj == p1_proj) return false;

	// check that we perform the computation in a way that is independent with v0 v1 swaps
	//算出線與面的焦點	

	//***20151231*****************
	if (p0_proj < p1_proj)
	{
		p0 = s.P0() + (s.P1() - s.P0()) * fabs(p0_proj / (p1_proj - p0_proj));		
		//錯誤顏色判斷，不可用porj的正負來判斷顏色的正負
		//pp_color = p0_color + (p1_color - p0_color) * fabs(p0_proj / (p1_proj - p0_proj));

	}
	if (p0_proj > p1_proj)
	{
		p0 = s.P1() + (s.P0() - s.P1()) * fabs(p1_proj / (p0_proj - p1_proj));
		//ERROR,pp_color = p1_color + (p0_color - p1_color) * fabs(p1_proj / (p0_proj - p1_proj));
	}
	Color4b p1color = p1_color;//copy出來才可操作
	Color4b p0color = p0_color;
	
	if (p1color.X() - p0color.X()>0)
		pp_color.X() = p0color.X() + (p1color.X() - p0color.X()) * fabs(p0_proj / (p1_proj - p0_proj));
	else
		pp_color.X() = p1color.X() + (p0color.X() - p1color.X()) * fabs(p1_proj / (p0_proj - p1_proj));

	if (p1color.Y()>p0color.Y())
		pp_color.Y() = p0color.Y() + (p1color.Y() - p0color.Y()) * fabs(p0_proj / (p1_proj - p0_proj));
	else
		pp_color.Y() = p1color.Y() + (p0color.Y() - p1color.Y()) * fabs(p1_proj / (p0_proj - p1_proj));

	if (p1color.Z()>p0color.Z())
		pp_color.Z() = p0color.Z() + (p1color.Z() - p0color.Z()) * fabs(p0_proj / (p1_proj - p0_proj));
	else
		pp_color.Z() = p1color.Z() + (p0color.Z() - p1color.Z()) * fabs(p1_proj / (p0_proj - p1_proj));
	//*****************************

	return true;

}
//**************20150311********整合顏色與材質切層************************************************
template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
//bool IntersectionPlane_TexMesh(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
bool IntersectionPlane_save_edge_version(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
{
	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;
	std::vector<TexCoord2<ScalarType>> tex_coord;//2切點 材質座標
	std::vector<TexCoord2<ScalarType>> one_slice_tex_coord;//1層材質座標
	std::vector<Color4b> ptColor;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");


	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{

		ptVec.clear();
		nmVec.clear();
		tex_coord.clear();
		ptColor.clear();
		for (int j = 0; j<3; ++j)
		{

			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{
				//qDebug() << "intersect" << endl;
				const Point3<ScalarType> &p0 = m.face[i].V0(j)->cP();
				const Point3<ScalarType> &p1 = m.face[i].V1(j)->cP();
				const Point3<ScalarType> &n0 = m.face[i].V0(j)->cN();
				const Point3<ScalarType> &n1 = m.face[i].V1(j)->cN();

				ScalarType q0 = qH[m.face[i].V0(j)];
				ScalarType q1 = qH[m.face[i].V1(j)];
				//         printf("Intersection ( %3.2f %3.2f %3.2f )-( %3.2f %3.2f %3.2f )\n",p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]);
				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg(p0, p1);//p0，p1兩個點集合
				//IntersectionPlaneSegment(pl, seg, pp);

				//******材質
				if (tri::HasPerWedgeTexCoord(m))
				{
					//材質座標
					const Point2<ScalarType>&p0_tex = m.face[i].WT(j).P();
					const Point2<ScalarType>&p1_tex = m.face[i].WT((j + 1) % 3).P();
					Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
					vcg::TexCoord2<ScalarType> pp_tex;
					//qDebug("TEX");

					My_Tex_IntersectioinPlaneSegment(pl, seg, pp, seg_tex, pp_tex);

					pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
					tex_coord.push_back(pp_tex);
				}


				//****顏色
				//if (vcg::tri::HasPerFaceColor(m))
				//{		
				//	const vcg::Color4b &p0_color = m.face[i].C();
				//	const vcg::Color4b &p1_color = m.face[i].C();
				//	vcg::Color4b pp_color;
				//	My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
				//	//qDebug("face color ");
				//	pp_color = m.face[i].C();
				//	ptColor.push_back(pp_color);					
				//}
				//else 
				if (vcg::tri::HasPerVertexColor(m))
				{
					//qDebug("vertex color ");
					const vcg::Color4b &p0_color = m.face[i].V0(j)->C();
					const vcg::Color4b &p1_color = m.face[i].V1(j)->C();
					vcg::Color4b pp_color;
					My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					ptColor.push_back(pp_color);
					//qDebug("SLICE_COLOR,%i, %i,%i", pp_color.X(), pp_color.Y(), pp_color.Z());
					
				}
				
				ptVec.push_back(pp);
				
				Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
				nmVec.push_back(nn);
			}
			if (qH[m.face[i].V(j)] == 0)
			{
				ptVec.push_back(m.face[i].V(j)->cP());
				nmVec.push_back(m.face[i].V(j)->cN());
				if (tri::HasPerWedgeTexCoord(m))tex_coord.push_back(m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m))ptColor.push_back(m.face[i].V(j)->C());
			}

		}

		//*/*****2015/3/4*********修改版
		if (ptVec.size() == 2)
		{
			//qDebug("ptVec.size() == 2");

			typename EdgeMeshType::VertexIterator vi;
			vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
			(*vi).P() = ptVec[0];
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			if (vcg::tri::HasPerVertexColor(m)){
				(*vi).C() = ptColor[0];
				//qDebug("SLICE_COLOR,%i, %i,%i", ptColor[0].X(), ptColor[0].Y(), ptColor[0].Z());
			}
			em.edge.back().V(0) = &(*vi);
			vi++;

			(*vi).P() = ptVec[1];
			(*vi).N() = nmVec[1];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			em.edge.back().V(1) = &(*vi);

		}
		else if (ptVec.size() == 3)
		{
			qDebug("********ptVec.size() == 3");
			typename EdgeMeshType::VertexIterator vi;
			ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			(*vi).P() = ptVec[0];//第一點
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[0];
			(*ei).V(0) = &(*vi);//edge0的v0
			vi++;

			(*vi).P() = ptVec[1];//第二點
			(*vi).N() = nmVec[1];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			(*ei).V(1) = &(*vi);//edge0的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge1的v0
			vi++;

			(*vi).P() = ptVec[2];//第三點
			(*vi).N() = nmVec[2];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[2];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[2];
			(*ei).V(1) = &(*vi);//edge1的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge2的v0
			vi--; vi--;
			(*ei).V(1) = &(*vi);//edge2的v1

			
		}
		//*/

		//qDebug("LOG:Testtesttest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;
}
//******20150806**測試面與切平面共線時增加面**********************************************************************
template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
//bool IntersectionPlane_TexMesh_test_add_face_version(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
bool IntersectionPlane_TexMesh_use_version(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em,bool vorF=false)//傳入mesh，plane，儲存的空mesh
{
	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;
	std::vector<TexCoord2<ScalarType>> tex_coord;//2切點 材質座標
	std::vector<TexCoord2<ScalarType>> one_slice_tex_coord;//1層材質座標
	std::vector<Color4b> ptColor;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");

	
	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	//if (pl.Direction().dot(Point3<ScalarType>(1, 0, 0) )>=0)qDebug() << "pl.Direction()_true";
	////if (true)qDebug() << "pl.Direction()";
	//else qDebug() << "pl.Direction()_false";

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{

		ptVec.clear();
		nmVec.clear();
		tex_coord.clear();
		ptColor.clear();
		for (int j = 0; j<3; ++j)
		{

			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{
				if (qH[m.face[i].V0(j)]>0)qDebug() << "begin" << endl;
				if (qH[m.face[i].V1(j)]>0)qDebug() << "end" << endl;
				//qDebug() << "intersect" << endl;
				const Point3<ScalarType> &p0 = m.face[i].V0(j)->cP();
				const Point3<ScalarType> &p1 = m.face[i].V1(j)->cP();
				const Point3<ScalarType> &n0 = m.face[i].V0(j)->cN();
				const Point3<ScalarType> &n1 = m.face[i].V1(j)->cN();

				ScalarType q0 = qH[m.face[i].V0(j)];
				ScalarType q1 = qH[m.face[i].V1(j)];
				//         printf("Intersection ( %3.2f %3.2f %3.2f )-( %3.2f %3.2f %3.2f )\n",p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]);
				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg(p0, p1);//p0，p1兩個點集合
				//IntersectionPlaneSegment(pl, seg, pp);

				//******材質
				if (tri::HasPerWedgeTexCoord(m))
				{
					//材質座標
					const Point2<ScalarType>&p0_tex = m.face[i].WT(j).P();
					const Point2<ScalarType>&p1_tex = m.face[i].WT((j + 1) % 3).P();
					Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
					vcg::TexCoord2<ScalarType> pp_tex;
					//qDebug("TEX");

					My_Tex_IntersectioinPlaneSegment(pl, seg, pp, seg_tex, pp_tex);

					pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
					tex_coord.push_back(pp_tex);
				}

				//**20151005**顏色
				if (vcg::tri::HasPerFaceColor(m) && vorF)
				{		
					//const vcg::Color4b &p0_color = m.face[i].C();
					//const vcg::Color4b &p1_color = m.face[i].C();
					vcg::Color4b pp_color;
					//My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					IntersectionPlaneSegment(pl, seg, pp);
					//qDebug()<<"face color " << m.face[i].C().X() << m.face[i].C().Y() << m.face[i].C().Z();
					//pp_color = m.face[i].C();
					ptColor.push_back(m.face[i].C());
				}
				else
				if (vcg::tri::HasPerVertexColor(m))
				{
					//qDebug("vertex color ");
					const vcg::Color4b &p0_color = m.face[i].V0(j)->C();
					const vcg::Color4b &p1_color = m.face[i].V1(j)->C();
					vcg::Color4b pp_color;
					My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					ptColor.push_back(pp_color);
					//qDebug("SLICE_COLOR,%i, %i,%i", pp_color.X(), pp_color.Y(), pp_color.Z());

				}

				ptVec.push_back(pp);

				Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
				nmVec.push_back(nn);
			}
			if (qH[m.face[i].V(j)] == 0)
			{
				ptVec.push_back(m.face[i].V(j)->cP());
				nmVec.push_back(m.face[i].V(j)->cN());
				if (tri::HasPerWedgeTexCoord(m))tex_coord.push_back(m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m))ptColor.push_back(m.face[i].V(j)->C());
			}

		}

		//*/*****2015/3/4*********修改版
		if (ptVec.size() == 2)
		{
			qDebug("ptVec.size() == 2");

			typename EdgeMeshType::VertexIterator vi;
			vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
			(*vi).P() = ptVec[0];
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			if (vcg::tri::HasPerVertexColor(m)){
				(*vi).C() = ptColor[0];
				//qDebug("SLICE_COLOR,%i, %i,%i", ptColor[0].X(), ptColor[0].Y(), ptColor[0].Z());
			}
			em.edge.back().V(0) = &(*vi);
			vi++;

			(*vi).P() = ptVec[1];
			(*vi).N() = nmVec[1];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			em.edge.back().V(1) = &(*vi);

		}
		else if (ptVec.size() == 3)
		{
			qDebug("********ptVec.size() == 3");
			/*typename EdgeMeshType::VertexIterator vi;
			ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			(*vi).P() = ptVec[0];//第一點
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[0];
			(*ei).V(0) = &(*vi);//edge0的v0
			vi++;

			(*vi).P() = ptVec[1];//第二點
			(*vi).N() = nmVec[1];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			(*ei).V(1) = &(*vi);//edge0的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge1的v0
			vi++;

			(*vi).P() = ptVec[2];//第三點
			(*vi).N() = nmVec[2];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[2];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[2];
			(*ei).V(1) = &(*vi);//edge1的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge2的v0
			vi--; vi--;
			(*ei).V(1) = &(*vi);//edge2的v1*/

			//***20150804*************
			//qDebug() << "ptVec.size() == 3";
			//typename EdgeMeshType::VertexIterator vii;
			//typename EdgeMeshType::FaceIterator efii;		
			//
			//vii = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			//efii = vcg::tri::Allocator<EdgeMeshType>::AddFaces(em, 1);
			//	
			//(*vii).P() = ptVec[0];//第一點	
			//if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[0];
			//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[0];
			//(*efii).V(0) = (&*vii);//***			
			//vii++;
			//(*vii).P() = ptVec[1];//第二點
			//if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[1];
			//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[1];
			//(*efii).V(1) = (&*vii);//***			
			//vii++;
			//(*vii).P() = ptVec[2];//第3點
			//if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[2];
			//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[2];
			//(*efii).V(2) = (&*vii);
			//***
			


			//***************************
		}
		//*/

		//qDebug("LOG:Testtesttest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;


}

template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
//bool IntersectionPlane_TexMesh_20151026_work_version
bool IntersectionPlane_TexMesh(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em, bool vorF = false, int sliceColorMode = 0)//傳入mesh，plane，儲存的空mesh
{
	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;
    std::vector<TexCoord2<ScalarType>> tex_coord;//2切點 材質座標
	std::vector<TexCoord2<ScalarType>> one_slice_tex_coord;//1層材質座標
	std::vector<Color4b> ptColor;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");
	/*typename TriMeshType::template PerEdgeAttributeHandle <ScalarType> ep =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute <ScalarType>(m, "edgePosition");*/

	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	//if (pl.Direction().dot(Point3<ScalarType>(1, 0, 0) )>=0)qDebug() << "pl.Direction()_true";
	////if (true)qDebug() << "pl.Direction()";
	//else qDebug() << "pl.Direction()_false";

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{
		ptVec.clear();
		nmVec.clear();
		tex_coord.clear();
		ptColor.clear();

		Point3<ScalarType> p_first(0, 0, 0);
		Point3<ScalarType> p_second(0, 0, 0);
		ptVec.resize(3,p_first);
		nmVec.resize(3, Point3<ScalarType>(0, 0, 0));
		tex_coord.resize(3, TexCoord2<ScalarType>(0, 0));
		ptColor.resize(3, Color4b(255, 255, 255, 255));
		int count = 0;
		
		for (int j = 0; j<3; ++j)
		{
			
			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{
				Point3<ScalarType> p0;
				Point3<ScalarType> p1;
				Point3<ScalarType> n0;
				Point3<ScalarType> n1;
				ScalarType q0;
				ScalarType q1;
				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg;// (p0, p1);//p0，p1兩個點集合
				if (qH[m.face[i].V0(j)]>0)//
				{
					p0 = m.face[i].V1(j)->cP();
					n0 = m.face[i].V1(j)->cN();

					p1 = m.face[i].V0(j)->cP();					
					n1 = m.face[i].V0(j)->cN();
					//qDebug() << "begin" << endl;

					q1 = qH[m.face[i].V0(j)];
					q0 = qH[m.face[i].V1(j)];
				}else if (qH[m.face[i].V1(j)]>0)
				{
					p0 = m.face[i].V0(j)->cP();
					n0 = m.face[i].V0(j)->cN();

					p1 = m.face[i].V1(j)->cP();					
					n1 = m.face[i].V1(j)->cN();
					//qDebug() << "end" << endl;

					q0 = qH[m.face[i].V0(j)];
					q1 = qH[m.face[i].V1(j)];
				}				
				seg.Set(p0, p1);				

				if (qH[m.face[i].V0(j)] > 0)
				{
					//******材質
					if (tri::HasPerWedgeTexCoord(m))
					{
						//材質座標
                        const Point2<ScalarType>&p1_tex = m.face[i].WT(j).P();
                        const Point2<ScalarType>&p0_tex = m.face[i].WT((j + 1) % 3).P();
						Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
						vcg::TexCoord2<ScalarType> pp_tex;
						//qDebug("TEX");

						My_Tex_IntersectioinPlaneSegment(pl, seg, pp, seg_tex, pp_tex);

						pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
						tex_coord[1] = (pp_tex);
					}

					//**20151005**顏色
					if (vcg::tri::HasPerFaceColor(m) && vorF && sliceColorMode ==2 )
					{
						//const vcg::Color4b &p0_color = m.face[i].C();
						//const vcg::Color4b &p1_color = m.face[i].C();
						vcg::Color4b pp_color;
						//My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
						IntersectionPlaneSegment(pl, seg, pp);
						//qDebug()<<"face color " << m.face[i].C().X() << m.face[i].C().Y() << m.face[i].C().Z();
						//pp_color = m.face[i].C();
						ptColor[1] = (m.face[i].C());
					}
					else if (vcg::tri::HasPerVertexColor(m) && sliceColorMode ==3)
					{
						//qDebug("vertex color ");
						const vcg::Color4b &p1_color = m.face[i].V0(j)->C();
						const vcg::Color4b &p0_color = m.face[i].V1(j)->C();
						vcg::Color4b pp_color;
						My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
						ptColor[1] = (pp_color);
						//qDebug("SLICE_COLOR,%i, %i,%i", pp_color.X(), pp_color.Y(), pp_color.Z());
					}
					else if ( sliceColorMode == 0)
					{
						IntersectionPlaneSegment(pl, seg, pp);					
					}
					//p_second = pp;
					Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
					nmVec[1] = (nn);
					ptVec[1] = pp;//不可用count
				}
				else
				{
					if (tri::HasPerWedgeTexCoord(m))
					{
						//材質座標
                        const Point2<ScalarType>&p0_tex = m.face[i].WT(j).P();
                        const Point2<ScalarType>&p1_tex = m.face[i].WT((j + 1) % 3).P();
						Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
						vcg::TexCoord2<ScalarType> pp_tex;
						//qDebug("TEX");

						My_Tex_IntersectioinPlaneSegment(pl, seg, pp, seg_tex, pp_tex);

						pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
						tex_coord[0] = (pp_tex);
					}

					//**20151005**顏色
					if (vcg::tri::HasPerFaceColor(m) && vorF && sliceColorMode == 2)
					{
						//const vcg::Color4b &p0_color = m.face[i].C();
						//const vcg::Color4b &p1_color = m.face[i].C();
						vcg::Color4b pp_color;
						//My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
						IntersectionPlaneSegment(pl, seg, pp);
						//qDebug()<<"face color " << m.face[i].C().X() << m.face[i].C().Y() << m.face[i].C().Z();
						//pp_color = m.face[i].C();
						ptColor[0] = (m.face[i].C());
					}
					else if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3)
					{
						//qDebug("vertex color ");
						const vcg::Color4b &p0_color = m.face[i].V0(j)->C();
						const vcg::Color4b &p1_color = m.face[i].V1(j)->C();
						vcg::Color4b pp_color;
						My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
						ptColor[0] = (pp_color);
						//qDebug("SLICE_COLOR,%i, %i,%i", pp_color.X(), pp_color.Y(), pp_color.Z());
					}
					else if (sliceColorMode == 0)
					{
						IntersectionPlaneSegment(pl, seg, pp);
					}
					//p_first = pp;
					Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
					nmVec[0] = (nn);
					ptVec[0] = pp;
				}

				
				//qDebug() << "intersection.h_ptVec_intersect" << count;
				count++;				
			}
			//if (qH[m.face[i].V(j)] == 0)//線段與切面焦於線段頂點


			//一點與切平面相交
			if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)]<0) && qH[m.face[i].V1(j)]<0 && qH[m.face[i].V2(j)]>0)
			{
				//qDebug() << "intersection.h_ptVec_case1***count" << count;
				ptVec[1] = m.face[i].V0(j)->cP();
				nmVec[1] = (m.face[i].V0(j)->cN());
                if (tri::HasPerWedgeTexCoord(m))tex_coord[1] = (m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3)ptColor[1] = (m.face[i].V0(j)->C());
				if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)ptColor[1] = ((m.face[i].C()));
				count++;

			}//一點與切平面相交
			else if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)]<0) && qH[m.face[i].V1(j)]>0 && qH[m.face[i].V2(j)]<0)
			{
				//qDebug() << "intersection.h_ptVec_case1***count" << count;
				ptVec[0] = m.face[i].V0(j)->cP();
                nmVec[0] = (m.face[i].V0(j)->cN());
                if (tri::HasPerWedgeTexCoord(m))tex_coord[0] = (m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3)ptColor[0] = (m.face[i].V0(j)->C());
				if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)ptColor[0] = ((m.face[i].C()));
				count++;
			}//線段與切平面相交
			else if (qH[m.face[i].V0(j)] == 0 && qH[m.face[i].V1(j)] == 0)
			{
				
				//qDebug() << "intersection.h_count== 0***" << count;												
				ptVec[0] = m.face[i].V0(j)->cP();				
				nmVec[0] = (m.face[i].V0(j)->cN());
                if (tri::HasPerWedgeTexCoord(m))tex_coord[0] = (m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3)
					ptColor[0] = (m.face[i].V0(j)->C());
				if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)
					ptColor[0] = ((m.face[i].C()));
				count++;

				ptVec[1] = m.face[i].V1(j)->cP();
				nmVec[1] = (m.face[i].V1(j)->cN());
                if (tri::HasPerWedgeTexCoord(m))tex_coord[1] = (m.face[i].WT((j + 1) % 3));
				if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3)
					ptColor[1] = (m.face[i].V1(j)->C());
				if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)
					ptColor[1] = ((m.face[i].C()));
                count++;
			}
			

		}

		//*/*****2015/3/4*********修改版
		//if (ptVec.size() == 2)

		if (count==2)
		{
			//qDebug("ptVec.size() == 2");
			typename EdgeMeshType::VertexIterator vi;
            vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
			(*vi).P() = ptVec[0];
			//qDebug("ptVec[0],%f, %f,%f", ptVec[0].X(), ptVec[0].Y(), ptVec[0].Z());
			//(*vi).P() = p_first;
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))
			{
				//qDebug() << "intersection.h" << tex_coord[0].U();
                (*vi).T() = tex_coord[0];
			}
			
			if (vcg::tri::HasPerVertexColor(m) && sliceColorMode == 3){
				(*vi).C() = ptColor[0];
				//qDebug("SLICE_COLOR,%i, %i,%i", ptColor[0].X(), ptColor[0].Y(), ptColor[0].Z());
			}
			//vcg::Color4b pp_color = ;
			if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)
				(*vi).C() = ptColor[0];
			
			em.edge.back().V(0) = &(*vi);
			vi++;
			//qDebug("ptVec[1],%f, %f,%f", ptVec[1].X(), ptVec[1].Y(), ptVec[1].Z());
			(*vi).P() = ptVec[1];
			//(*vi).P() = p_second;
			(*vi).N() = nmVec[1];
            if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			//if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)(*vi).C() = ptColor[1];
			if (vcg::tri::HasPerFaceColor(m) && sliceColorMode == 2)
				(*vi).C() = ptColor[1];//vcg::Color4b(255, 255, 0, 0);//
			em.edge.back().V(1) = &(*vi);

		}
		//else if (ptVec.size() == 3)
		else if (count ==3 )
		{
			qDebug("intersection.h_ptVec.size() == 3");
			//typename EdgeMeshType::VertexIterator vi;
			//ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
			//vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			//(*vi).P() = ptVec[0];//第一點
			//(*vi).N() = nmVec[0];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[0];
			//(*ei).V(0) = &(*vi);//edge0的v0
			//vi++;

			//(*vi).P() = ptVec[1];//第二點
			//(*vi).N() = nmVec[1];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			//(*ei).V(1) = &(*vi);//edge0的v1
			//ei++;
			//(*ei).V(0) = &(*vi);//edge1的v0
			//vi++;

			//(*vi).P() = ptVec[2];//第三點
			//(*vi).N() = nmVec[2];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[2];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[2];
			//(*ei).V(1) = &(*vi);//edge1的v1
			//ei++;
			//(*ei).V(0) = &(*vi);//edge2的v0
			//vi--; vi--;
			//(*ei).V(1) = &(*vi);//edge2的v1
		}
		//*/

		//qDebug("LOG:Testtesttest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;


}

template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
//bool IntersectionPlane_TexMesh_test_20151113測試處理平行狀態
bool IntersectionPlane_TexMesh_test_20151113(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em, bool vorF = false)//傳入mesh，plane，儲存的空mesh
{
	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;
	std::vector<TexCoord2<ScalarType>> tex_coord;//2切點 材質座標
	std::vector<TexCoord2<ScalarType>> one_slice_tex_coord;//1層材質座標
	std::vector<Color4b> ptColor;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");
	/*typename TriMeshType::template PerEdgeAttributeHandle <ScalarType> ep =
	tri::Allocator<TriMeshType> :: template AddPerVertexAttribute <ScalarType>(m, "edgePosition");*/

	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	//if (pl.Direction().dot(Point3<ScalarType>(1, 0, 0) )>=0)qDebug() << "pl.Direction()_true";
	////if (true)qDebug() << "pl.Direction()";
	//else qDebug() << "pl.Direction()_false";

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{
		ptVec.clear();
		nmVec.clear();
		tex_coord.clear();
		ptColor.clear();
			
		ptVec.resize(3, Point3<ScalarType>(0, 0, 0));
		nmVec.resize(3, Point3<ScalarType>(0, 0, 0));
		tex_coord.resize(3, TexCoord2<ScalarType>(0, 0));
		ptColor.resize(3, Color4b(1, 1, 1, 1));
		int count = 0;

		Point3<ScalarType> fn = m.face[i].N();
		ScalarType facePlanedegree = pl.Direction().dot(fn);
		qDebug() << "facePlanedegree" << facePlanedegree;		

		for (int j = 0; j<3; ++j)
		{
			
			
			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{				
				//qDebug() << "intersect" << endl;
				const Point3<ScalarType> &p0 = m.face[i].V0(j)->cP();
				const Point3<ScalarType> &p1 = m.face[i].V1(j)->cP();
				const Point3<ScalarType> &n0 = m.face[i].V0(j)->cN();
				const Point3<ScalarType> &n1 = m.face[i].V1(j)->cN();

				ScalarType q0 = qH[m.face[i].V0(j)];
				ScalarType q1 = qH[m.face[i].V1(j)];
				//         printf("Intersection ( %3.2f %3.2f %3.2f )-( %3.2f %3.2f %3.2f )\n",p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]);
				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg(p0, p1);//p0，p1兩個點集合
				//IntersectionPlaneSegment(pl, seg, pp);

				//******材質
				if (tri::HasPerWedgeTexCoord(m))
				{
					//材質座標
					const Point2<ScalarType>&p0_tex = m.face[i].WT(j).P();
					const Point2<ScalarType>&p1_tex = m.face[i].WT((j + 1) % 3).P();
					Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
					vcg::TexCoord2<ScalarType> pp_tex;
					//qDebug("TEX");

					My_Tex_IntersectioinPlaneSegment(pl, seg, pp, seg_tex, pp_tex);

					pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
					tex_coord.push_back(pp_tex);
				}

				//**20151005**顏色
				if (vcg::tri::HasPerFaceColor(m) && vorF)
				{
					//const vcg::Color4b &p0_color = m.face[i].C();
					//const vcg::Color4b &p1_color = m.face[i].C();
					vcg::Color4b pp_color;
					//My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					IntersectionPlaneSegment(pl, seg, pp);
					//qDebug()<<"face color " << m.face[i].C().X() << m.face[i].C().Y() << m.face[i].C().Z();
					//pp_color = m.face[i].C();
					ptColor.push_back(m.face[i].C());
				}
				else
				if (vcg::tri::HasPerVertexColor(m))
				{
					//qDebug("vertex color ");
					const vcg::Color4b &p0_color = m.face[i].V0(j)->C();
					const vcg::Color4b &p1_color = m.face[i].V1(j)->C();
					vcg::Color4b pp_color;
					My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					ptColor.push_back(pp_color);
					//qDebug("SLICE_COLOR,%i, %i,%i", pp_color.X(), pp_color.Y(), pp_color.Z());

				}

				ptVec.push_back(pp);
				Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
				nmVec.push_back(nn);
				count++;
			}

			if (abs(facePlanedegree) - 1 < 1e-2 && (qH[m.face[i].V0(j)]) == 0)
			{
				qDebug() << "abs(qH[m.face[i].V0(j)]) == 0";
				ptVec[j] = m.face[i].V(j)->cP();
				nmVec[j] = (m.face[i].V(j)->cN());
				if (tri::HasPerWedgeTexCoord(m))tex_coord[j] = (m.face[i].WT(j));
				if (vcg::tri::HasPerVertexColor(m))ptColor[j] = (m.face[i].V(j)->C());
				count++;
				if ((qH[m.face[i].V1(j)]) > 0)
				{
					ptVec[j] = m.face[i].V1(j)->cP();
					nmVec[j] = (m.face[i].V1(j)->cN());
					if (tri::HasPerWedgeTexCoord(m))tex_coord[j] = (m.face[i].WT(j));
					if (vcg::tri::HasPerVertexColor(m))ptColor[j] = (m.face[i].V(j)->C());
					count++;
				}
				else if ((qH[m.face[i].V2(j)]) > 0)
				{
					ptVec[j] = m.face[i].V2(j)->cP();
					nmVec[j] = (m.face[i].V2(j)->cN());
					if (tri::HasPerWedgeTexCoord(m))tex_coord[j] = (m.face[i].WT(j));
					if (vcg::tri::HasPerVertexColor(m))ptColor[j] = (m.face[i].V(j)->C());
					count++;
				}

			}


			//一點與切平面相交
			//if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)]<0) && qH[m.face[i].V1(j)]<0 && qH[m.face[i].V2(j)]>0)
			//{
			//	//qDebug() << "intersection.h_ptVec_case1***count" << count;
			//	ptVec[1] = m.face[i].V0(j)->cP();
			//	nmVec[1] = (m.face[i].V0(j)->cN());
			//	if (tri::HasPerWedgeTexCoord(m))tex_coord[1] = (m.face[i].WT(j));
			//	if (vcg::tri::HasPerVertexColor(m))ptColor[1] = (m.face[i].V0(j)->C());
			//	count++;

			//}//一點與切平面相交
			//else if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)]<0) && qH[m.face[i].V1(j)]>0 && qH[m.face[i].V2(j)]<0)
			//{
			//	//qDebug() << "intersection.h_ptVec_case1***count" << count;
			//	ptVec[0] = m.face[i].V0(j)->cP();
			//	nmVec[0] = (m.face[i].V0(j)->cN());
			//	if (tri::HasPerWedgeTexCoord(m))tex_coord[0] = (m.face[i].WT(j));
			//	if (vcg::tri::HasPerVertexColor(m))ptColor[0] = (m.face[i].V0(j)->C());
			//	count++;
			//}//線段與切平面相交
			//else if (qH[m.face[i].V0(j)] == 0 && qH[m.face[i].V1(j)] == 0)
			//{
			//	//qDebug() << "intersection.h_count== 0***" << count;												
			//	ptVec[0] = m.face[i].V0(j)->cP();
			//	nmVec[0] = (m.face[i].V0(j)->cN());
			//	if (tri::HasPerWedgeTexCoord(m))tex_coord[0] = (m.face[i].WT(j));
			//	if (vcg::tri::HasPerVertexColor(m))ptColor[0] = (m.face[i].V0(j)->C());
			//	count++;

			//	ptVec[1] = m.face[i].V1(j)->cP();
			//	nmVec[1] = (m.face[i].V1(j)->cN());
			//	if (tri::HasPerWedgeTexCoord(m))tex_coord[1] = (m.face[i].WT((j + 1) % 3));
			//	if (vcg::tri::HasPerVertexColor(m))ptColor[1] = (m.face[i].V1(j)->C());
			//	count++;				
			//}

			//else if (qH[m.face[i].V(j)] == 0 && qH[m.face[i].V((j + 1) % 3)] == 0 && qH[m.face[i].V((j + 2) % 3)] == 0)
			//if (abs(facePlanedegree - 1)  < 1e-2 && abs(qH[m.face[i].V0(j)] )<1)
			/*if (abs(facePlanedegree - 1)  < 1e-2 && (qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0  )
			{
				if (qH[m.face[i].V0(j)]>0)
				{
					ptVec[j] = m.face[i].V(j)->cP();
					nmVec[j] = (m.face[i].V(j)->cN());
					if (tri::HasPerWedgeTexCoord(m))tex_coord[j] = (m.face[i].WT(j));
					if (vcg::tri::HasPerVertexColor(m))ptColor[j] = (m.face[i].V(j)->C());
					count++;
				}
				else if (qH[m.face[i].V1(j)]>0)
				{
					ptVec[j] = m.face[i].V1(j)->cP();
					nmVec[j] = (m.face[i].V1(j)->cN());
					if (tri::HasPerWedgeTexCoord(m))tex_coord[j] = (m.face[i].WT((j+1)%3));
					if (vcg::tri::HasPerVertexColor(m))ptColor[j] = (m.face[i].V((j+1)%3)->C());
					count++;
				}
				
			}*/
			//qDebug() << "abs(qH[m.face[i].V0(j)]) == 0" << abs(qH[m.face[i].V0(j)]);
			


		}

		qDebug() << "intersection.h count++" << count;
		//*/*****2015/3/4*********修改版
		//if (ptVec.size() == 2)

		if (count == 2)
		{
			qDebug("ptVec.size() == 2");
			typename EdgeMeshType::VertexIterator vi;
			vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
			(*vi).P() = ptVec[0];
			//qDebug("ptVec[0],%f, %f,%f", ptVec[0].X(), ptVec[0].Y(), ptVec[0].Z());
			//(*vi).P() = p_first;
			(*vi).N() = nmVec[0];
			if (tri::HasPerWedgeTexCoord(m))
			{
				//qDebug() << "intersection.h" << tex_coord[0].U();
				(*vi).T() = tex_coord[0];
			}

			if (vcg::tri::HasPerVertexColor(m)){
				(*vi).C() = ptColor[0];
				//qDebug("SLICE_COLOR,%i, %i,%i", ptColor[0].X(), ptColor[0].Y(), ptColor[0].Z());
			}
			em.edge.back().V(0) = &(*vi);
			vi++;
			//qDebug("ptVec[1],%f, %f,%f", ptVec[1].X(), ptVec[1].Y(), ptVec[1].Z());
			(*vi).P() = ptVec[1];
			//(*vi).P() = p_second;
			(*vi).N() = nmVec[1];
			if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			em.edge.back().V(1) = &(*vi);

		}
		//else if (ptVec.size() == 3)
		else if (count == 3)
		{
			qDebug("intersection.h_ptVec.size() == 3");
			//typename EdgeMeshType::VertexIterator vi;
			//ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
			//vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			//(*vi).P() = ptVec[0];//第一點
			//(*vi).N() = nmVec[0];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[0];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[0];
			//(*ei).V(0) = &(*vi);//edge0的v0
			//vi++;

			//(*vi).P() = ptVec[1];//第二點
			//(*vi).N() = nmVec[1];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[1];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[1];
			//(*ei).V(1) = &(*vi);//edge0的v1
			//ei++;
			//(*ei).V(0) = &(*vi);//edge1的v0
			//vi++;

			//(*vi).P() = ptVec[2];//第三點
			//(*vi).N() = nmVec[2];
			//if (tri::HasPerWedgeTexCoord(m))(*vi).T() = tex_coord[2];
			//if (vcg::tri::HasPerVertexColor(m))(*vi).C() = ptColor[2];
			//(*ei).V(1) = &(*vi);//edge1的v1
			//ei++;
			//(*ei).V(0) = &(*vi);//edge2的v0
			//vi--; vi--;
			//(*ei).V(1) = &(*vi);//edge2的v1

			//***20151113*************
			qDebug() << "ptVec.size() == 3";
			typename EdgeMeshType::VertexIterator vii;
			typename EdgeMeshType::FaceIterator efii;		
			
			vii = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
			efii = vcg::tri::Allocator<EdgeMeshType>::AddFaces(em, 1);
				
			(*vii).P() = ptVec[0];//第一點	
			if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[0];
			if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[0];
			(*efii).V(0) = (&*vii);//***			
			vii++;
			(*vii).P() = ptVec[1];//第二點
			if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[1];
			if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[1];
			(*efii).V(1) = (&*vii);//***			
			vii++;
			(*vii).P() = ptVec[2];//第3點
			if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[2];
			if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[2];
			(*efii).V(2) = (&*vii);
			//***
		}
		//*/

		//qDebug("LOG:Testtesttest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;


}

//**********************************************************************************
//***20150810要sort_edge*變成輪廓***************************************************************
template <typename MeshType,  class ScalarType, typename OutlinesPoint>
bool sortEdge(MeshType & em, std::vector< std::vector<OutlinesPoint> > &outlines_2)
{
	typedef typename MeshType::EdgeType EdgeType;
	typedef typename MeshType::CoordType CoordType;

	std::vector<OutlinesPoint> outline_struct;//一條outline			
	
	//std::vector< std::vector<OutlinePoint> > outlines;//儲存有幾條out_line			
	outlines_2.clear();

	UpdateFlags<MeshType>::EdgeClearV(em);
	UpdateTopology<MeshType>::EdgeEdge(em);	
	int nv = 0;
	for (size_t i = 0; i < em.edge.size(); i++)//由edge找edge直到完成一條outline
	{
		if (!em.edge[i].IsD())
		{
			if (!em.edge[i].IsV())//checks if the edge Has been visited
			{
				edge::Pos<EdgeType> startE(&em.edge[i], 0);
				edge::Pos<EdgeType> curE = startE;
				do
				{
					curE.E()->SetV();//visited
					//開始輸入點
					OutlinePointAttrib tempOutlinePoint;
					tempOutlinePoint.p = curE.V()->P();//點
					if (tri::HasPerVertexTexCoord(em))
					{
						tempOutlinePoint.texC = curE.V()->T();//材質
						tempOutlinePoint.texIndex = curE.V()->T().N();
						qDebug() <<"intersection.h"<< curE.V()->T().N();
					}
					if (vcg::tri::HasPerVertexColor(em))
					{
						tempOutlinePoint.pointColor = curE.V()->C();
					}
					outline_struct.push_back(tempOutlinePoint);
					curE.NextE();//NEXT EDGE
					nv++;
					if (curE == startE)cout << "curE==startE==" << nv << endl;
				} while (curE != startE);
				outlines_2.push_back(outline_struct);//將一條outline儲存到outlines vector					
				outline_struct.clear();
			}
			
		}
	}
	

	

	if (nv < 2) return 0;//判斷是否小於兩條邊，兩邊不構成面
	if (outlines_2.size()>0)
		qDebug("outlines_2 size %i", outlines_2.size());
	return 0;
}
//*********************************************************************************************

//**************20150309*******加材質版切層************************************
template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
bool IntersectionPlane_TexMesh_version0(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
{
	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;
	std::vector<TexCoord2<ScalarType>> tex_coord;//2切點 材質座標
	std::vector<TexCoord2<ScalarType>> one_slice_tex_coord;//1層材質座標
	std::vector<Color4b> ptColor;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");


	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{

		ptVec.clear();
		nmVec.clear();
		tex_coord.clear();
		for (int j = 0; j<3; ++j)
		{
			
			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{
				//qDebug() << "intersect" << endl;
				const Point3<ScalarType> &p0 = m.face[i].V0(j)->cP();
				const Point3<ScalarType> &p1 = m.face[i].V1(j)->cP();
				const Point3<ScalarType> &n0 = m.face[i].V0(j)->cN();
				const Point3<ScalarType> &n1 = m.face[i].V1(j)->cN();

				//材質座標
				const Point2<ScalarType>&p0_tex = m.face[i].WT(j).P();
				const Point2<ScalarType>&p1_tex = m.face[i].WT((j + 1) % 3).P();
				Segment2<ScalarType> seg_tex(p0_tex, p1_tex);
				vcg::TexCoord2<ScalarType> pp_tex;
				qDebug("here1");
				
				ScalarType q0 = qH[m.face[i].V0(j)];
				ScalarType q1 = qH[m.face[i].V1(j)];
				//         printf("Intersection ( %3.2f %3.2f %3.2f )-( %3.2f %3.2f %3.2f )\n",p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]);
				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg(p0, p1);//p0，p1兩個點集合
				//IntersectionPlaneSegment(pl, seg, pp);

				//顏色
				if (vcg::tri::HasPerVertexColor(m))
				{	
					const vcg::Color4b &p0_color = m.face[i].V0(j)->C();
					const vcg::Color4b &p1_color = m.face[i].V1(j)->C();
					vcg::Color4b pp_color;
					My_Color_IntersectionPlaneSegment(pl, seg, pp, p0_color, p1_color, pp_color);//求seg_tex中pp_tex點的材質座標
					ptColor.push_back(pp_color);
				}

				My_Tex_IntersectioinPlaneSegment(pl,seg,pp,seg_tex,pp_tex);

				if (tri::HasPerWedgeTexCoord(m))
				{					
					pp_tex.n() = m.face[i].WT(j).n();//紀錄點的材質編號
					qDebug("wt.n=%i", pp_tex.n());
				}
				ptVec.push_back(pp);
				tex_coord.push_back(pp_tex);
				Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
				nmVec.push_back(nn);
			}
			if (qH[m.face[i].V(j)] == 0 )
			{
				
				ptVec.push_back(m.face[i].V(j)->cP());
				nmVec.push_back(m.face[i].V(j)->cN());
				tex_coord.push_back(m.face[i].WT(j) );
				ptColor.push_back(m.face[i].V(j)->C());
			}

		}
		
		//*/*****2015/3/4*********修改版
		if (ptVec.size() == 2)
		{
			//qDebug("ptVec.size() == 2");
			
			typename EdgeMeshType::VertexIterator vi;
			vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);					
			(*vi).P() = ptVec[0];
			(*vi).N() = nmVec[0];
			(*vi).T() = tex_coord[0];
			(*vi).C() = ptColor[0];
			em.edge.back().V(0) = &(*vi);
			vi++;
			
			(*vi).P() = ptVec[1];
			(*vi).N() = nmVec[1];
			(*vi).T() = tex_coord[1];
			(*vi).C() = ptColor[0];
			em.edge.back().V(1) = &(*vi);
		}
		else if (ptVec.size() == 3)
		{
			//qDebug("********ptVec.size() == 3");
			typename EdgeMeshType::VertexIterator vi;
			ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);					
			(*vi).P() = ptVec[0];
			(*vi).N() = nmVec[0];
			(*vi).T() = tex_coord[0];
			(*vi).C() = ptColor[0];
			(*ei).V(0) = &(*vi);//edge0的v0
			vi++;
				
			(*vi).P() = ptVec[1];
			(*vi).N() = nmVec[1];
			(*vi).T() = tex_coord[1];
			(*vi).C() = ptColor[1];
			(*ei).V(1) = &(*vi);//edge0的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge1的v0
			vi++;
								
			(*vi).P() = ptVec[2];
			(*vi).N() = nmVec[2];
			(*vi).T() = tex_coord[2];
			(*vi).C() = ptColor[2];
			(*ei).V(1) = &(*vi);//edge1的v1
			ei++;
			(*ei).V(0) = &(*vi);//edge2的v0
			vi--; vi--;
			(*ei).V(1) = &(*vi);//edge2的v1
			

		}
		//*/

		//qDebug("LOG:Testtesttest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;


}

template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
bool IntersectionPlaneMesh(TriMeshType & m, Plane3<ScalarType>  pl, EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
{

	std::vector<Point3<ScalarType> > ptVec;
	std::vector<Point3<ScalarType> > nmVec;

	typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
		tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m, "TemporaryPlaneDistance");

	typename TriMeshType::VertexIterator vi;
	typename EdgeMeshType::EdgeIterator ei;//新增edge iterator
	for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) if (!(*vi).IsD())
		qH[vi] = SignedDistancePlanePoint(pl, (*vi).cP());//點到面的距離

	for (size_t i = 0; i<m.face.size(); i++)
	if (!m.face[i].IsD())
	{

		ptVec.clear();
		nmVec.clear();

		Point3<ScalarType> p_first(0, 0, 0);		
		ptVec.resize(3, p_first);
		nmVec.resize(3, Point3<ScalarType>(0, 0, 0));

		int count = 0;

		for (int j = 0; j<3; ++j)
		{
			//qDebug() <<"j="<< j << endl;
			//qDebug() << "sigedDistance" << qH[m.face[i].V(j)] << endl;
			if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
			{
				Point3<ScalarType> p0;
				Point3<ScalarType> p1;
				Point3<ScalarType> n0;
				Point3<ScalarType> n1;

				ScalarType q0;
				ScalarType q1;

				Point3<ScalarType> pp;//線與面的切點
				Segment3<ScalarType> seg;// (p0, p1);//p0，p1兩個點集合
				if (qH[m.face[i].V0(j)]>0)
				{
					p0 = m.face[i].V1(j)->cP();
					n0 = m.face[i].V1(j)->cN();

					p1 = m.face[i].V0(j)->cP();
					n1 = m.face[i].V0(j)->cN();
					qDebug() << "begin" << endl;

					q1 = qH[m.face[i].V0(j)];
					q0 = qH[m.face[i].V1(j)];
				}
				else if (qH[m.face[i].V1(j)]>0)
				{
					p0 = m.face[i].V0(j)->cP();
					n0 = m.face[i].V0(j)->cN();

					p1 = m.face[i].V1(j)->cP();
					n1 = m.face[i].V1(j)->cN();
					qDebug() << "end" << endl;

					q0 = qH[m.face[i].V0(j)];
					q1 = qH[m.face[i].V1(j)];
				}
				seg.Set(p0, p1);

				if (qH[m.face[i].V0(j)] > 0)
				{
					IntersectionPlaneSegment(pl, seg, pp);
					ptVec[1] = pp;//不可用count
				}
				else
				{
					ptVec[0] = pp;
				}

				Point3<ScalarType> nn = (n0*fabs(q1) + n1*fabs(q0)) / fabs(q0 - q1);
				nmVec.push_back(nn);
				count++;
			}

			//一點與切平面相交
			if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)] < 0) && qH[m.face[i].V1(j)]<0 && qH[m.face[i].V2(j)]>0)
			{
				ptVec[1] = m.face[i].V0(j)->cP();
				nmVec[1] = (m.face[i].V0(j)->cN());
				count++;
			}
			else if (qH[m.face[i].V0(j)] == 0 && (qH[m.face[i].V1(j)] * qH[m.face[i].V2(j)]<0) && qH[m.face[i].V1(j)]>0 && qH[m.face[i].V2(j)] < 0)
			{
				qDebug() << "intersection.h_ptVec_case1***count" << count;
				ptVec[0] = m.face[i].V0(j)->cP();
				nmVec[0] = (m.face[i].V0(j)->cN());
				count++;
			}
			else if (qH[m.face[i].V0(j)] == 0 && qH[m.face[i].V1(j)] == 0)
			{
				qDebug() << "intersection.h_count== 0***" << count;
				ptVec[0] = m.face[i].V0(j)->cP();
				nmVec[0] = (m.face[i].V0(j)->cN());
				count++;
				ptVec[1] = m.face[i].V1(j)->cP();
				nmVec[1] = (m.face[i].V1(j)->cN());
				count++;
			}
		}
		
		//*/*****2015/3/4*********修改版
		if (count == 2)
		{

			//qDebug("ptVec.size() == 2");
			typename EdgeMeshType::VertexIterator vi;
			vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
			vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
			(*vi).P() = ptVec[0];
			(*vi).N() = nmVec[0];
			em.edge.back().V(0) = &(*vi);
			vi++;
			(*vi).P() = ptVec[1];
			(*vi).N() = nmVec[1];
			em.edge.back().V(1) = &(*vi);
		}
		
		//*/

		qDebug("LOG:interSectionTest");
	}
	//qDebug("LOG:Testtesttest");
	tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m, qH);

	return true;
}





//*******************************************************************************
/** \addtogroup complex */
/*@{*/
/** \brief  More stable version of the IntersectionPlaneMesh function
 *
 * This version of the make a first pass storing the distance to the plane
 * into a vertex attribute and then use this value to compute in a safe way the
 * intersection.
*/
template < typename  TriMeshType, typename EdgeMeshType, class ScalarType >
bool IntersectionPlaneMesh_back_version(TriMeshType & m,	Plane3<ScalarType>  pl,	EdgeMeshType & em)//傳入mesh，plane，儲存的空mesh
{
	
  std::vector<Point3<ScalarType> > ptVec;
  std::vector<Point3<ScalarType> > nmVec;

  typename TriMeshType::template PerVertexAttributeHandle < ScalarType > qH =
      tri::Allocator<TriMeshType> :: template AddPerVertexAttribute < ScalarType >(m,"TemporaryPlaneDistance");

  typename TriMeshType::VertexIterator vi;
  typename EdgeMeshType::EdgeIterator ei;//新增edge iterator
  for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
    qH[vi] =SignedDistancePlanePoint(pl,(*vi).cP());//點到面的距離
  
  for(size_t i=0;i<m.face.size();i++)
    if(!m.face[i].IsD())
	{
		
      ptVec.clear();
      nmVec.clear();
      for(int j=0;j<3;++j)
      {
		  //qDebug() <<"j="<< j << endl;
		  //qDebug() << "sigedDistance" << qH[m.face[i].V(j)] << endl;
		if ((qH[m.face[i].V0(j)] * qH[m.face[i].V1(j)])<0)//如果距離相乘為負，面有通過兩點之間，則處理
		{
		 //qDebug() << "intersect" << endl;
         const Point3<ScalarType> &p0 = m.face[i].V0(j)->cP();
         const Point3<ScalarType> &p1 = m.face[i].V1(j)->cP();
         const Point3<ScalarType> &n0 = m.face[i].V0(j)->cN();
         const Point3<ScalarType> &n1 = m.face[i].V1(j)->cN();
         float q0 = qH[m.face[i].V0(j)];
         float q1 = qH[m.face[i].V1(j)];
//         printf("Intersection ( %3.2f %3.2f %3.2f )-( %3.2f %3.2f %3.2f )\n",p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]);
         Point3<ScalarType> pp;
         Segment3<ScalarType> seg(p0,p1);
         IntersectionPlaneSegment(pl,seg,pp);
         ptVec.push_back(pp);
         Point3<ScalarType> nn =(n0*fabs(q1) + n1*fabs(q0))/fabs(q0-q1);
         nmVec.push_back(nn);
		}
		if (qH[m.face[i].V(j)] == 0 /*&& vcg::face::IsBorder(m.face[i], j)*/)
		{
			ptVec.push_back(m.face[i].V(j)->cP());
			nmVec.push_back(m.face[i].V(j)->cN());
			//qDebug("***point to plane***distance==0");
			qDebug()<<"ptVec.size=" <<ptVec.size();
		}
		
      }
      /*
	  if(ptVec.size()>=2)//原版
      {
		  //qDebug("LOG:Testtesttest");
        typename EdgeMeshType::VertexIterator vi;
        vcg::tri::Allocator<EdgeMeshType>::AddEdges(em,1);
        vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em,2);
        (*vi).P() = ptVec[0];
        (*vi).N() = nmVec[0];
        em.edge.back().V(0) = &(*vi);
        vi++;
        (*vi).P() = ptVec[1];
        (*vi).N() = nmVec[1];
        em.edge.back().V(1) = &(*vi);
      }//*/
	  //*/*****2015/3/4*********修改版
	  if (ptVec.size() == 2)
	  {
		  //qDebug("ptVec.size() == 2");
		  typename EdgeMeshType::VertexIterator vi;
		  vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 1);
		  vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 2);
		  (*vi).P() = ptVec[0];
		  (*vi).N() = nmVec[0];
		  em.edge.back().V(0) = &(*vi);
		  vi++;
		  (*vi).P() = ptVec[1];
		  (*vi).N() = nmVec[1];
		  em.edge.back().V(1) = &(*vi);
	  }
	  else if (ptVec.size() == 3)
	  {
		  //qDebug("********ptVec.size() == 3");
		  typename EdgeMeshType::VertexIterator vi;
		  ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
		  vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);		  
		  (*vi).P() = ptVec[0];
		  (*vi).N() = nmVec[0];		 
		  (*ei).V(0) = &(*vi);
		  vi++;

		  (*vi).P() = ptVec[1];
		  (*vi).N() = nmVec[1];		  
		  (*ei).V(1) = &(*vi);
		  ei++;
		  (*ei).V(0) = &(*vi);
		  vi++;

		  (*vi).P() = ptVec[2];
		  (*vi).N() = nmVec[2];		  
		  (*ei).V(1) = &(*vi);
		  ei++;
		  (*ei).V(0) = &(*vi);
		  vi--; vi--;
		  (*ei).V(1) = &(*vi);
		  
		  //***20150804*************
		  //typename EdgeMeshType::VertexIterator vi;
		  //typename EdgeMeshType::FaceIterator efi;
		  ////ei = vcg::tri::Allocator<EdgeMeshType>::AddEdges(em, 3);
		  ////vi = vcg::tri::Allocator<EdgeMeshType>::AddVertices(em, 3);
		  //efi = vcg::tri::Allocator<EdgeMeshType>::AddFaces(em, 1);
		  //
		  //for (int j = 0; j<3; ++j)
		  //{
			 // //(*efi).V(j) = (&*vi);// .P() = ptVec[i];
			 // (*(*efi).V(j)).P() =  ptVec[j];
			 // 
		  //}
		  //(efi)++;
		 
		  //***************************
	  }
	  //*/

	  qDebug("LOG:interSectionTest");
    }
	//qDebug("LOG:Testtesttest");
  tri::Allocator<TriMeshType> :: template DeletePerVertexAttribute < ScalarType >(m,qH);

  return true;
}


/** \addtogroup complex */
/*@{*/
/** 
    Compute the intersection between a trimesh and a plane. 
		given a plane return the set of faces that are contained 
		into intersected cells.
*/
template < typename  TriMeshType, class ScalarType, class IndexingType >
bool Intersection(Plane3<ScalarType>  pl,
									IndexingType *grid,
									typename std::vector<typename TriMeshType::FaceType*> &v)
{
	typedef typename TriMeshType::FaceContainer FaceContainer;
	typedef IndexingType GridType;
	typename TriMeshType::FaceIterator fi;
	v.clear();
	typename std::vector< typename GridType::Cell* > cells;
	Intersect(*grid,pl,cells);
	typename std::vector<typename GridType::Cell*>::iterator ic;
	typename GridType::Cell fs,ls;

	for(ic = cells.begin(); ic != cells.end();++ic)
	{
		grid->Grid(*ic,fs,ls);
		typename GridType::Link * lk = fs;
		while(lk != ls){
			typename TriMeshType::FaceType & face = *(lk->Elem());
			v.push_back(&face);
			lk++;
		}//end while
	}//end for
	return true;
}

/** 
	 Computes the intersection between a Ray and a Mesh. Returns a 3D Pointset.  
*/
template < typename  TriMeshType, class ScalarType>
bool IntersectionRayMesh(	
	/* Input Mesh */		TriMeshType * m, 
	/* Ray */				const Line3<ScalarType> & ray,
	/* Intersect Point */	Point3<ScalarType> & hitPoint)
{
	//typedef typename TriMeshType::FaceContainer FaceContainer;
	typename TriMeshType::FaceIterator fi;
	bool hit=false;

	if(m==0) return false;

	//TriMeshType::FaceIterator fi;
	//std::vector<TriMeshType::FaceType*>::iterator fi;

	ScalarType bar1,bar2,dist;
	Point3<ScalarType> p1;
	Point3<ScalarType> p2;
	Point3<ScalarType> p3;
	for(fi = m->face.begin(); fi != m->face.end(); ++fi)
	{
		p1=vcg::Point3<ScalarType>( (*fi).P(0).X() ,(*fi).P(0).Y(),(*fi).P(0).Z() );
		p2=vcg::Point3<ScalarType>( (*fi).P(1).X() ,(*fi).P(1).Y(),(*fi).P(1).Z() );
		p3=vcg::Point3<ScalarType>( (*fi).P(2).X() ,(*fi).P(2).Y(),(*fi).P(2).Z() );
		if(IntersectionLineTriangle<ScalarType>(ray,p1,p2,p3,dist,bar1,bar2))
		{
			hitPoint= p1*(1-bar1-bar2) + p2*bar1 + p3*bar2;
			hit=true;
		}
	}

	return hit;
}

/** 
	 Computes the intersection between a Ray and a Mesh. Returns a 3D Pointset, baricentric's coordinates 
	 and a pointer of intersected face.
*/
template < typename  TriMeshType, class ScalarType>
bool IntersectionRayMesh(	
	/* Input Mesh */		TriMeshType * m, 
	/* Ray */				const Line3<ScalarType> & ray,
	/* Intersect Point */	Point3<ScalarType> & hitPoint,
	/* Baricentric coord 1*/ ScalarType &bar1,
	/* Baricentric coord 2*/ ScalarType &bar2,
	/* Baricentric coord 3*/ ScalarType &bar3,
	/* FacePointer */ typename TriMeshType::FacePointer fp
	)
{
	//typedef typename TriMeshType::FaceContainer FaceContainer;
	typename TriMeshType::FaceIterator fi;
	bool hit=false;

	if(m==0) return false;

	//TriMeshType::FaceIterator fi;
	//std::vector<TriMeshType::FaceType*>::iterator fi;

	ScalarType dist;
	Point3<ScalarType> p1;
	Point3<ScalarType> p2;
	Point3<ScalarType> p3;
	for(fi = m->face.begin(); fi != m->face.end(); ++fi)
	{
		p1=vcg::Point3<ScalarType>( (*fi).P(0).X() ,(*fi).P(0).Y(),(*fi).P(0).Z() );
		p2=vcg::Point3<ScalarType>( (*fi).P(1).X() ,(*fi).P(1).Y(),(*fi).P(1).Z() );
		p3=vcg::Point3<ScalarType>( (*fi).P(2).X() ,(*fi).P(2).Y(),(*fi).P(2).Z() );
		if(IntersectionLineTriangle<ScalarType>(ray,p1,p2,p3,dist,bar1,bar2))
		{
			bar3 = (1-bar1-bar2);
			hitPoint= p1*bar3 + p2*bar1 + p3*bar2;
			fp = &(*fi);
			hit=true;
		}
	}

	return hit;
}

/** 
    Compute the intersection between a mesh and a ball. 
		given a mesh return a new mesh made by a copy of all the faces entirely includeded in the ball plus
		new faces created by refining the ones intersected by the ball border.
		It works by recursively splitting the triangles that cross the border, as long as their area is greater than
		a given value tol. If no value is provided, 1/10^5*2*pi*radius is used 
		NOTE: the returned mesh is a triangle soup 
*/
template < typename  TriMeshType, class ScalarType>
void IntersectionBallMesh(	 TriMeshType & m, const vcg::Sphere3<ScalarType> &ball, TriMeshType & res,
													float tol = 0){

	typename TriMeshType::VertexIterator v0,v1,v2;
	typename TriMeshType::FaceIterator fi;
	std::vector<typename TriMeshType:: FaceType*> closests;
	vcg::Point3<ScalarType>	witness;
	std::pair<ScalarType, ScalarType> info;

	if(tol == 0) tol = M_PI * ball.Radius() * ball.Radius() / 100000;
	tri::UpdateSelection<TriMeshType>::FaceClear(m);
	for(fi = m.face.begin(); fi != m.face.end(); ++fi)
	if(!(*fi).IsD() && IntersectionSphereTriangle<ScalarType>(ball  ,(*fi), witness , &info))
	  (*fi).SetS();

	res.Clear();
	tri::Append<TriMeshType,TriMeshType>::Selected(res,m);
	int i =0;
	while(i<res.fn){
		 bool allIn = ( ball.IsIn(res.face[i].P(0)) && ball.IsIn(res.face[i].P(1))&&ball.IsIn(res.face[i].P(2)));
		if( IntersectionSphereTriangle<ScalarType>(ball  ,res.face[i], witness , &info) && !allIn){
				if(vcg::DoubleArea(res.face[i]) > tol)
				{
				// split the face res.face[i] in four, add the four new faces to the mesh and delete the face res.face[i]
				v0 = vcg::tri::Allocator<TriMeshType>::AddVertices(res,3);	
				fi = vcg::tri::Allocator<TriMeshType>::AddFaces(res,4);	
				
				v1 = v0; ++v1;
				v2 = v1; ++v2;
				(*v0).P() = (res.face[i].P(0) + res.face[i].P(1))*0.5;
				(*v1).P() = (res.face[i].P(1) + res.face[i].P(2))*0.5;
				(*v2).P() = (res.face[i].P(2) + res.face[i].P(0))*0.5;

				(*fi).V(0) = res.face[i].V(0);
				(*fi).V(1) = &(*v0);
				(*fi).V(2) = &(*v2);	
				++fi;

				(*fi).V(0) = res.face[i].V(1);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = &(*v0);	
				++fi;

				(*fi).V(0) = &(*v0);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = &(*v2);	
				++fi;

				(*fi).V(0) = &(*v2);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = res.face[i].V(2) ;	

				vcg::tri::Allocator<TriMeshType>::DeleteFace(res,res.face[i]);
			}
		}// there was no intersection with the boundary

	if(info.first > 0.0) // closest point - radius. If >0 is outside
		vcg::tri::Allocator<TriMeshType>::DeleteFace(res,res.face[i]);
	++i;
	}
}


template < typename  TriMeshType, class ScalarType, class IndexingType>
void IntersectionBallMesh( IndexingType * grid,	 TriMeshType & m, const vcg::Sphere3<ScalarType> &ball, TriMeshType & res,
													float tol = 0){

	typename TriMeshType::VertexIterator v0,v1,v2;
	typename std::vector<typename TriMeshType::FacePointer >::iterator  cfi;
	typename TriMeshType::FaceIterator fi;
	std::vector<typename TriMeshType:: FaceType*> closestsF,closests;
	vcg::Point3<ScalarType>	witness;
	std::vector<vcg::Point3<ScalarType> > witnesses;
	std::vector<ScalarType>	distances;
	std::pair<ScalarType, ScalarType> info;

	if(tol == 0) tol = M_PI * ball.Radius() * ball.Radius() / 100000;

	vcg::tri::GetInSphereFaceBase(m,*grid, ball.Center(), ball.Radius(),closestsF,distances,witnesses);
	for(cfi =closestsF.begin(); cfi != closestsF.end(); ++cfi)
	if(!(**cfi).IsD() && IntersectionSphereTriangle<ScalarType>(ball  ,(**cfi), witness , &info))
		closests.push_back(&(**cfi));

	res.Clear();
	SubSet(res,closests);
	int i =0;
	while(i<res.fn){
		 bool allIn = ( ball.IsIn(res.face[i].P(0)) && ball.IsIn(res.face[i].P(1))&&ball.IsIn(res.face[i].P(2)));
		if( IntersectionSphereTriangle<ScalarType>(ball  ,res.face[i], witness , &info) && !allIn){
				if(vcg::DoubleArea(res.face[i]) > tol)
				{
				// split the face res.face[i] in four, add the four new faces to the mesh and delete the face res.face[i]
				v0 = vcg::tri::Allocator<TriMeshType>::AddVertices(res,3);	
				fi = vcg::tri::Allocator<TriMeshType>::AddFaces(res,4);	
				
				v1 = v0; ++v1;
				v2 = v1; ++v2;
				(*v0).P() = (res.face[i].P(0) + res.face[i].P(1))*0.5;
				(*v1).P() = (res.face[i].P(1) + res.face[i].P(2))*0.5;
				(*v2).P() = (res.face[i].P(2) + res.face[i].P(0))*0.5;

				(*fi).V(0) = res.face[i].V(0);
				(*fi).V(1) = &(*v0);
				(*fi).V(2) = &(*v2);	
				++fi;

				(*fi).V(0) = res.face[i].V(1);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = &(*v0);	
				++fi;

				(*fi).V(0) = &(*v0);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = &(*v2);	
				++fi;

				(*fi).V(0) = &(*v2);
				(*fi).V(1) = &(*v1);
				(*fi).V(2) = res.face[i].V(2) ;	

				vcg::tri::Allocator<TriMeshType>::DeleteFace(res,res.face[i]);
			}
		}// there was no intersection with the boundary

	if(info.first > 0.0) // closest point - radius. If >0 is outside
		vcg::tri::Allocator<TriMeshType>::DeleteFace(res,res.face[i]);
	++i;
	}
}

/*@}*/
} // end namespace vcg
#endif
