#ifndef GLU_TESSELLATOR_CAP_H
#define GLU_TESSELLATOR_CAP_H
#include "glu_tesselator.h"
#include <vcg/simplex/edge/pos.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/bounding.h>

namespace vcg {
	namespace tri {


		// This function take a mesh with one or more boundary stored as edges, and fill another mesh with a triangulation of that boundaries.
		// it assumes that boundary are planar and exploits glutessellator for the triangulaiton
		template <class MeshType>
		void CapEdgeMesh(MeshType &em, MeshType &cm, bool revertFlag = false)
		{
			typedef typename MeshType::EdgeType EdgeType;
			typedef typename MeshType::CoordType CoordType;
			std::vector< std::vector<CoordType> > outlines;
			std::vector<CoordType> outline;
			UpdateFlags<MeshType>::EdgeClearV(em);
			UpdateTopology<MeshType>::EdgeEdge(em);
			int nv = 0;
			for (size_t i = 0; i < em.edge.size(); i++) if (!em.edge[i].IsD())
			{
				if (!em.edge[i].IsV())
				{
					edge::Pos<EdgeType> startE(&em.edge[i], 0);
					edge::Pos<EdgeType> curE = startE;
					do
					{
						curE.E()->SetV();
						outline.push_back(curE.V()->P());
						curE.NextE();
						nv++;
					} while (curE != startE);
					if (revertFlag) std::reverse(outline.begin(), outline.end());
					outlines.push_back(outline);
					outline.clear();
				}
			}
			if (nv < 2) return;
			//  printf("Found %i outlines for a total of %i vertices",outlines.size(),nv);

			typename MeshType::VertexIterator vi = vcg::tri::Allocator<MeshType>::AddVertices(cm, nv);
			for (size_t i = 0; i < outlines.size(); i++)
			{
				for (size_t j = 0; j < outlines[i].size(); ++j, ++vi)
					(&*vi)->P() = outlines[i][j];
			}

			std::vector<int> indices;//點表示面的index順序
			glu_tesselator::tesselate(outlines, indices);//做面的index順序
			std::vector<CoordType> points;//存所有點
			glu_tesselator::unroll(outlines, points);//全部展開，從2維陣列展開成一維
			//typename MeshType::FaceIterator fi=tri::Allocator<MeshType>::AddFaces(cm,nv-2);
			typename MeshType::FaceIterator fi = tri::Allocator<MeshType>::AddFaces(cm, indices.size() / 3);
			for (size_t i = 0; i < indices.size(); i += 3, ++fi)
			{
				(*&fi)->V(0) = &cm.vert[indices[i + 0]];
				(*&fi)->V(1) = &cm.vert[indices[i + 1]];
				(*&fi)->V(2) = &cm.vert[indices[i + 2]];
			}
			Clean<MeshType>::RemoveDuplicateVertex(cm);
			UpdateBounding<MeshType>::Box(cm);
		}

		template <class MeshType, class point_type>
		//void My_CapEdgeMesh_edge_version(MeshType &em, MeshType &cm, std::vector< std::vector<point_type> > & outlines, bool revertFlag = false)
		void My_CapEdgeMesh(MeshType &em, MeshType &cm, std::vector< std::vector<point_type> > & outlines, bool revertFlag = false)
		{
			typedef typename MeshType::EdgeType EdgeType;
			typedef typename MeshType::CoordType CoordType;


			std::vector<CoordType> outline;//一條outline			

			//std::vector< std::vector<CoordType> > outlines;//儲存有幾條out_line			
			outlines.clear();

			UpdateFlags<MeshType>::EdgeClearV(em);
			UpdateTopology<MeshType>::EdgeEdge(em);
			int nv = 0;
			for (size_t i = 0; i < em.edge.size(); i++)//由edge找edge直到完成一條outline
			if (!em.edge[i].IsD())
			{
				if (!em.edge[i].IsV())//checks if the edge Has been visited
				{

					edge::Pos<EdgeType> startE(&em.edge[i], 0);
					edge::Pos<EdgeType> curE = startE;
					//edge::Pos<EdgeType> curQ;
					do
					{
						curE.E()->SetV();//visited
						outline.push_back(curE.V()->P());
						//curQ = curE;
						curE.NextE();//NEXT EDGE
						nv++;
						if (curE == startE)cout << "curE==startE==" << nv << endl;
					} while (curE != startE);
					if (revertFlag){
						cout << "revert" << endl; std::reverse(outline.begin(), outline.end());//revertFlag，前後顛倒
					}
					outlines.push_back(outline);//將一條outline儲存到outlines vector					
					outline.clear();
				}

			}

			if (nv < 2) return;//判斷是否小於兩條邊，兩邊不構成面
			if (outlines.size()>0)
				qDebug("outlines size %i", outlines.size());
		}

		template <class MeshType, class point_type>
		void My_CapEdgeMesh_face_version(MeshType &em, MeshType &cm, std::vector< std::vector<point_type> > & outlines, bool revertFlag = false)
		//void My_CapEdgeMesh(MeshType &em, MeshType &cm, std::vector< std::vector<point_type> > & outlines, bool revertFlag = false)
		{

			typedef typename MeshType::EdgeType EdgeType;
			typedef typename MeshType::CoordType CoordType;

			//MeshType temp2;
			//vcg::tri::Append<MeshType, MeshType>::MeshCopy(temp2, em);

			std::vector<CoordType> outline;//一條outline						
			outlines.clear();

			tri::Clean<MeshType>::RemoveDuplicateVertex(em);
			em.face.EnableFFAdjacency();
			UpdateTopology<MeshType>::FaceFace(em);//***bug
			em.vert.EnableVFAdjacency();
			em.face.EnableVFAdjacency();
			tri::UpdateTopology<CMeshO>::VertexFace(em);


			int nv = 0;
			qDebug() << "My_CapEdgeMesh_place4";
			//***20150805************
			qDebug() << "20150806";
			MeshType edgeMesh;
			typename MeshType::VertexIterator vi;
			typename MeshType::EdgeIterator ei;
			//std::vector<CoordType> outline2;//一條outline		
			tri::UpdateFlags<MeshType>::FaceBorderFromNone(em);

			for (MeshType::FaceIterator fi = em.face.begin(); fi != em.face.end(); ++fi) if (!(*fi).IsD())
			{
				bool isB = false;
				for (int i = 0; i < 3; ++i)
				if ((*fi).IsB(i))
				{
					isB = true;
					ei = vcg::tri::Allocator<MeshType>::AddEdges(edgeMesh, 1);
					vi = vcg::tri::Allocator<MeshType>::AddVertices(edgeMesh, 2);
					//(*efii).V(0) = &(*vi);
					(*vi).P() = (*fi).V(i)->P();
					edgeMesh.edge.back().V(0) = &(*vi);
					vi++;
					(*vi).P() = (*fi).V((i + 1) % 3)->P();
					edgeMesh.edge.back().V(1) = &(*vi);
					//qDebug() << "My_CapEdgeMesh_place4";
				}

			}

			UpdateFlags<MeshType>::EdgeClearV(edgeMesh);
			UpdateTopology<MeshType>::EdgeEdge(edgeMesh);
			//---------------------------------
			qDebug() << "20150806_temp_edge.size()" << edgeMesh.edge.size();
			for (size_t i = 0; i < edgeMesh.edge.size(); i++)//找出有幾條線或outline
			if (!edgeMesh.edge[i].IsD())
			{
				if (!edgeMesh.edge[i].IsV())//checks if the edge Has been visited
				{
					edge::Pos<EdgeType> startE(&edgeMesh.edge[i], 0);
					edge::Pos<EdgeType> curE = startE;
					//edge::Pos<EdgeType> curQ;
					do
					{
						qDebug() << "20150806_temp_edge_place_1";
						curE.E()->SetV();//visited
						outline.push_back(curE.V()->P());
						//curQ = curE;
						curE.NextE();//NEXT EDGE
						nv++;
						//if (curE == startE)cout << "curE==startE=="<<nv << endl;				
					} while (curE != startE);
					outlines.push_back(outline);//將一條outline儲存到outlines vector					
					qDebug() << "20150806_temp_edge_outlines" << outlines.size();
					outline.clear();
				}

			}


			//*******end_20150805************************************************f
			if (nv < 2) return;//判斷是否小於兩條邊，兩邊不構成面
			/*if (outlines.size()>0)
			qDebug("outlines size %i", outlines.size());*/
		}


		template <class MeshType, class point_type>
		void Sort_Mesh_Edge(MeshType &em, MeshType &cm)
		{


		}

	} // end namespace tri
} // end namespace vcg

#endif // GLU_TESSELLATOR_CAP_H
