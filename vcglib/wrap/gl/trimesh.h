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

#ifndef __VCG_GLTRIMESH
#define __VCG_GLTRIMESH

#include <queue>
#include <vector>

//#include <GL/glew.h>
#include <wrap/gl/space.h>
#include <wrap/gl/math.h>
#include <vcg/space/color4.h>
#include <wrap/gl/glu_tessellator_cap.h>

namespace vcg {

	template <typename T>
	class GL_TYPE_NM
	{
	public:
		static GLenum SCALAR() { assert(0); return 0; }
	};
	template <> class GL_TYPE_NM<float>
	{
	public:
		typedef GLfloat ScalarType;
		static GLenum SCALAR() { return GL_FLOAT; }
	};
	template <> class GL_TYPE_NM<double>
	{
	public:
		typedef GLdouble ScalarType;
		static GLenum SCALAR() { return GL_DOUBLE; }
	};

	//template <> GL_TYPE::SCALAR<double>() { return GL_DOUBLE; }

	// classe base di glwrap usata solo per poter usare i vari drawmode, normalmode senza dover
	// specificare tutto il tipo (a volte lunghissimo)
	// della particolare classe glwrap usata.
	class GLW
	{
	public:
		enum DrawMode	{ DMNone, DMBox, DMPoints, DMWire, DMHidden, DMFlat, DMSmooth, DMFlatWire, DMRadar, DMLast, DMTexWire, DMTexWireOne, DMTexWireQuad, DMCapEdge, DMOutline, DMOutline_Test, DMOutline_test_triangle, DMSELECTION };
		enum NormalMode	{ NMNone, NMPerVert, NMPerFace, NMPerWedge, NMLast };
		enum ColorMode	{ CMNone, CMPerMesh, CMPerFace, CMPerVert, CMLast, CMTestSig, CMBlack, CMWhite };
		enum TextureMode{ TMNone, TMPerVert, TMPerWedge, TMPerWedgeMulti };
		/*enum updateMode
		{
		VBOUMVert = 0x0001,
		VBOUMColor = 0x0002
		};*/
		enum Hint {
			HNUseTriStrip = 0x0001,				// ha bisogno che ci sia la fftopology gia calcolata!
			//		HNUseEdgeStrip		  = 0x0002,			//
			HNUseDisplayList = 0x0004,
			HNCacheDisplayList = 0x0008,		// Each mode has its dl;
			HNLazyDisplayList = 0x0010,			// Display list are generated only when requested
			HNIsTwoManifold = 0x0020,			// There is no need to make DetachComplex before .
			HNUsePerWedgeNormal = 0x0040,		//
			HNHasFFTopology = 0x0080,		// E' l'utente che si preoccupa di tenere aggiornata la topologia ff
			HNHasVFTopology = 0x0100,		// E' l'utente che si preoccupa di tenere aggiornata la topologia vf
			HNHasVertNormal = 0x0200,		// E' l'utente che si preoccupa di tenere aggiornata le normali per faccia
			HNHasFaceNormal = 0x0400,		// E' l'utente che si preoccupa di tenere aggiornata le normali per vertice
			HNUseVArray = 0x0800,
			HNUseLazyEdgeStrip = 0x1000,		// Edge Strip are generated only when requested
			HNUseVBO = 0x2000,		// Use Vertex Buffer Object
			HNIsPolygonal = 0x4000    // In wireframe modes, hide faux edges
		};

		enum Change {
			CHVertex = 0x01,
			CHNormal = 0x02,
			CHColor = 0x04,
			CHFace = 0x08,
			CHFaceNormal = 0x10,
			CHRender = 0x20,
			CHAll = 0xff
		};
		enum HintParami {
			HNPDisplayListSize = 0,
			HNPPointDistanceAttenuation = 1,
			HNPPointSmooth = 2
		};
		enum HintParamf {
			HNPCreaseAngle = 0,	// crease angle in radians
			HNPZTwist = 1,				// Z offset used in Flatwire and hiddenline modality
			HNPPointSize = 2		// the point size used in point rendering
		};

		template<class MeshType>
		class VertToSplit
		{
		public:
			typename MeshType::face_base_pointer f;
			char z;
			char edge;
			bool newp;
			typename MeshType::vertex_pointer v;
		};

		// GL Array Elemet
		class GLAElem {
		public:
			int glmode;
			int len;
			int start;
		};
	};



	template <class MeshType, class outlinePoint, bool partial = false, class FACE_POINTER_CONTAINER = std::vector<typename MeshType::FacePointer> >
	class GlTrimesh : public GLW
	{
	public:

		typedef typename MeshType::VertexType VertexType;
		typedef typename MeshType::FaceType FaceType;
		typedef typename MeshType::FaceType::WedgeTexCoordOcf::TexCoordType TexCoordTypee;

		typedef typename MeshType::VertexType::CoordType CoordType;
		typedef typename MeshType::VertexType::ScalarType ScalarType;
		typedef typename MeshType::VertexIterator VertexIterator;
		typedef typename MeshType::EdgeIterator EdgeIterator;
		typedef typename MeshType::FaceIterator FaceIterator;


		FACE_POINTER_CONTAINER face_pointers;

		MeshType *m;

		std::vector< std::vector<CoordType> > *outlines;//儲存有幾條out_line
		//***20150810
		std::vector<std::vector<outlinePoint>> *outlines_struct;
		float *outline_thick;
		bool *draw_circle_in_quad_line;

		unsigned int array_buffers[4];
		struct vboBuffer
		{
			unsigned int array_buffers[4];
			vboBuffer()
			{
				array_buffers[0] = 0;
				array_buffers[1] = 0;
				array_buffers[2] = 0;
				array_buffers[3] = 0;
			}
		};
		std::vector<vboBuffer> multi_array_buffers;

		std::vector<unsigned int> TMId;
		//Circle
		std::vector<Point2f> circleVertex;
		void createCircleVertex()
		{
			const float DEG2RAD = 3.14159 / 180;
			for (int i = 0; i < 360; i += 8)
			{
				float degInRad = i*DEG2RAD;
				circleVertex.push_back(Point2f(cos(degInRad), sin(degInRad)));

			}


		}

		int curr_hints;      // the current hints

		// The parameters of hints
		int   HNParami[8];
		float HNParamf[8];

		DrawMode cdm; // Current DrawMode
		NormalMode cnm; // Current NormalMode
		ColorMode ccm; // Current ColorMode

		//***20150512 增加viewer與tex配對
		std::vector<std::vector<unsigned int>> TMIdd;
		//std::list<std::vector<unsigned int>> ttttt;
		//***增加gla的指標
		int viewerNum;

		//********************
		static NormalMode convertDrawModeToNormalMode(DrawMode dm)
		{
			switch (dm)
			{
			case(DMFlat) :
			case(DMFlatWire) :
			case(DMRadar) :
						  return NMPerFace;
			case(DMPoints) :
			case(DMWire) :
			case(DMSmooth) :
						   return NMPerVert;
			default:
				return NMNone;
			}
			return NMNone;
		}

		GlTrimesh()
		{
			//glv = 0;
			TMIdd.resize(3);
			viewerNum = 0;
			m = 0;
			outlines = 0;
			outlines_struct = 0;
			dl = 0xffffffff;
			curr_hints = HNUseLazyEdgeStrip;
			cdm = DMNone;
			ccm = CMNone;
			cnm = NMNone;

			SetHintParamf(HNPCreaseAngle, float(M_PI / 5));
			SetHintParamf(HNPZTwist, 0.00005f);
			//***20151001
			SetHintParamf(HNPPointSize, 10.0f);
			SetHintParami(HNPPointDistanceAttenuation, 1);
			SetHintParami(HNPPointSmooth, 0);
			array_buffers[0] = 0;
			array_buffers[1] = 0;
			array_buffers[2] = 0;
			array_buffers[3] = 0;

			createCircleVertex();
		}

		~GlTrimesh()
		{
			//Delete the VBOs
			/*if (curr_hints&HNUseVBO)
			{
			for (int i = 0; i < 4; ++i)
			if (glIsBuffer(GLuint(array_buffers[i])))
			glDeleteBuffers(1, (GLuint *)(array_buffers + i));
			}*/
			//indices = std::vector<unsigned int>();
			//vertTex = std::vector<TexCoordTypee>();

			for (int i = 0; i < multi_array_buffers.size(); i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (glIsBuffer(GLuint(multi_array_buffers[i].array_buffers[j])))
						glDeleteBuffers(1, (GLuint *)(array_buffers + i));
				}
			}


			std::vector<unsigned int>().swap(indices);
			std::vector<TexCoordTypee>().swap(vertTex);
			std::vector<GLfloat>().swap(vertexArray);
			std::vector<GLfloat>().swap(normalArray);

			std::vector<unsigned char>().swap(colorArray);

			std::vector<std::vector<TexCoordTypee>>().swap(vertTex_multi);
			std::vector<std::vector<unsigned int>>().swap(indices_muilti);
			std::vector<std::vector<GLfloat>>().swap(vertexArray_multi);
			std::vector<std::vector<GLfloat>>().swap(normalArray_multi);
			std::vector<std::vector<unsigned char>>().swap(colorArray_multi);
			std::vector<int>().swap(vertexArray_multi_size);



			TMIdd = std::vector<std::vector<unsigned int>>();
			TMId = std::vector<unsigned int>();
			//delete[] array_buffers;
		}


		void releaseVector()
		{
			//std::vector<unsigned int>().swap(indices);

			std::vector<TexCoordTypee>().swap(verTex2);
			std::vector<GLfloat>().swap(vertexArray);
			std::vector<GLfloat>().swap(normalArray);

			std::vector<unsigned char>().swap(colorArray);
		}

		void releaseVector_2()
		{
			//std::vector<unsigned int>().swap(indices);

			std::vector<std::vector<TexCoordTypee>>().swap(vertTex_multi);
			std::vector<std::vector<GLfloat>>().swap(vertexArray_multi);
			std::vector<std::vector<GLfloat>>().swap(normalArray_multi);
			std::vector<std::vector<unsigned char>>().swap(colorArray_multi);
		}

		//***circle ***//
		//std::vector<Point3f> circleVectex;


		unsigned int dl;
		std::vector<unsigned int> indices;
		std::vector<TexCoordTypee> vertTex;


		std::vector<TexCoordTypee> verTex2;
		std::vector<GLfloat> vertexArray, normalArray;
		std::vector<unsigned char> colorArray;
		//std::vector<vcg::Color4b> vertFaceColor;

		/*
			test multitexture VBO

			*/
		std::vector<std::vector<unsigned int>> indices_muilti;
		std::vector<std::vector<TexCoordTypee>> vertTex_multi;

		std::vector<std::vector<TexCoordTypee>> verTex2_multi;
		std::vector<std::vector<GLfloat>> vertexArray_multi, normalArray_multi;
		std::vector<std::vector<unsigned char>> colorArray_multi;
		std::vector<int> vertexArray_multi_size;
		/*
		*********************************
		*/



		void SetHintParami(const HintParami hip, const int value)
		{
			HNParami[hip] = value;
		}
		int GetHintParami(const HintParami hip) const
		{
			return HNParami[hip];
		}
		void SetHintParamf(const HintParamf hip, const float value)
		{
			HNParamf[hip] = value;
		}
		float GetHintParamf(const HintParamf hip) const
		{
			return HNParamf[hip];
		}
		void SetHint(Hint hn)
		{
			curr_hints |= hn;
		}
		void ClearHint(Hint hn)
		{
			curr_hints &= (~hn);
		}


		GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
		{
			GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

			glGenBuffersARB(1, &id);                        // create a vbo
			glBindBufferARB(target, id);                    // activate vbo id to use
			glBufferDataARB(target, dataSize, data, usage); // upload data to video card

			// check data size in VBO is same as input array, if not return 0 and delete VBO
			int bufferSize = 0;
			glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
			if (dataSize != bufferSize)
			{
				glDeleteBuffersARB(1, &id);
				id = 0;

			}

			return id;
		}



		//void Update(/*Change c=CHAll*/)
		//{
		//	if (m == 0) return;

		//	if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
		//	{
		//		indices.clear();
		//		vertTex.clear();
		//		for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
		//		{
		//			indices.push_back((unsigned int)((*fi).V(0) - &(*m->vert.begin())));
		//			indices.push_back((unsigned int)((*fi).V(1) - &(*m->vert.begin())));
		//			indices.push_back((unsigned int)((*fi).V(2) - &(*m->vert.begin())));
		//		}
		//		for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
		//		{
		//			//CFaceO * one_face = vi->VFp();
		//			vertTex.push_back(vi->VFp()->WT(0));

		//		}

		//		if (curr_hints&HNUseVBO)
		//		{
		//			if (!glIsBuffer(array_buffers[3]))
		//				glGenBuffers(4, (GLuint*)array_buffers);
		//			glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
		//			glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);

		//			glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
		//			glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].N()), GL_STATIC_DRAW_ARB);

		//			glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
		//			glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType::Color4b), (char *)&(m->vert[0].C()), GL_STATIC_DRAW_ARB);
		//			glBindBuffer(GL_ARRAY_BUFFER, array_buffers[3]);
		//			glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(TexCoordTypee), &(vertTex[0]), GL_STATIC_DRAW_ARB);
		//		}


		//		glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);
		//		glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);
		//		glColorPointer(4, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType::Color4b), 0);
		//		glTexCoordPointer(2, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(TexCoordTypee), 0);
		//	}

		//	//int C=c;
		//	//if((C&CHVertex) || (C&CHFace)) {
		//	//	ComputeBBox(*m);
		//	//	if(!(curr_hints&HNHasFaceNormal)) m->ComputeFaceNormal();
		//	//	if(!(curr_hints&HNHasVertNormal)) m->ComputeVertexNormal();
		//	//	C= (C | CHFaceNormal);
		//	//}
		//	//if((C&CHFace) && (curr_hints&HNUseEdgeStrip)) 		 ComputeEdges();
		//	//if((C&CHFace) && (curr_hints&HNUseLazyEdgeStrip)) ClearEdges();
		//	//if(MeshType::HasFFTopology())
		//	//	if((C&CHFace) && (curr_hints&HNUseTriStrip)) 		{
		//	//			if(!(curr_hints&HNHasFFTopology)) m->FFTopology();
		//	//			ComputeTriStrip();
		//	//		}
		//	//if((C&CHFaceNormal) && (curr_hints&HNUsePerWedgeNormal))		{
		//	//	  if(!(curr_hints&HNHasVFTopology)) m->VFTopology();
		//	//		CreaseWN(*m,MeshType::scalar_type(GetHintParamf(HNPCreaseAngle)));
		//	//}
		//	//if(C!=0) { // force the recomputation of display list
		//	//	cdm=DMNone;
		//	//	ccm=CMNone;
		//	//	cnm=NMNone;
		//	//}
		//	//if((curr_hints&HNUseVArray) && (curr_hints&HNUseTriStrip))
		//	//	{
		//	//	 ConvertTriStrip<MeshType>(*m,TStrip,TStripF,TStripVED,TStripVEI);
		//	//	}
		//}
		//template< DrawMode dm, ColorMode cm, TextureMode tm>

		void initVBOUpdate_back(DrawMode dm, ColorMode cm, TextureMode tm)
		{
			if (m == 0) return;

			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{

				//indices.clear();
				//vertTex.clear();
				//vertFaceColor.clear();
				/*indices = std::vector<unsigned int>();
				vertTex = std::vector<TexCoordTypee>();*/

				std::vector<unsigned int>().swap(indices);
				std::vector<TexCoordTypee>().swap(vertTex);
				//std::vector<TexCoordTypee>().swap(vertTex2);
				//vertFaceColor = std::vector<vcg::Color4b>();



				for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
				{
					indices.push_back((unsigned int)((*fi).V(0) - &(*m->vert.begin())));
					indices.push_back((unsigned int)((*fi).V(1) - &(*m->vert.begin())));
					indices.push_back((unsigned int)((*fi).V(2) - &(*m->vert.begin())));

					/*for (int i = 0; i < 3; i++)
					{
					verTex2.push_back(fi->WT(i));
					}*/

				}

				//if (cm == CMPerFace)
				//{
				//	for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
				//	{
				//		//CFaceO * one_face = vi->VFp();
				//		//vertTex.push_back(vi->VFp()->WT(0));
				//		vertFaceColor.push_back(vi->VFp()->C());
				//	}
				//}
				//if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
				//{
				//	for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
				//	{
				//		//CFaceO * one_face = vi->VFp();
				//		vertTex.push_back(vi->VFp()->WT(0));							
				//	}
				//}



				if (curr_hints&HNUseVBO)
				{
					/*if (!glIsBuffer(array_buffers[0]))
						array_buffers[0] = createVBO((char *)&(m->vert[0].P()), m->vn * sizeof(VertexType), GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

						if (!glIsBuffer(array_buffers[1]))
						array_buffers[1] = createVBO((char *)&(m->vert[0].N()), m->vn * sizeof(VertexType), GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

						if (!glIsBuffer(array_buffers[2]))
						{
						if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
						{
						array_buffers[2] =  createVBO(&(vertTex[0]), m->vn * sizeof(TexCoordTypee), GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);
						}
						else
						{
						array_buffers[2] =  createVBO((char *)&(m->vert[0].C()), m->vn * sizeof(VertexType::Color4b), GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);
						}
						}*/

					if (!glIsBuffer(array_buffers[0]) || !glIsBuffer(array_buffers[1]) || !glIsBuffer(array_buffers[2]))
						glGenBuffers(3, (GLuint*)array_buffers);



					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
					glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);
					//glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), NULL, GL_STATIC_DRAW);

					glBindBuffer(GL_ARRAY_BUFFER, 0);

					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
					glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].N()), GL_STATIC_DRAW_ARB);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
					{
						int count = 0;
						for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
						{
							CFaceO * one_face = vi->VFp();
							//qDebug() << vi->IsNull();
							if (one_face != NULL)
							{
								face::Pos<FaceType> he((*vi).VFp(), &*vi);

								//if ((one_face->HasWedgeTexCoordOcf()) && one_face->HasWedgeTexCoord())

								vertTex.push_back(vi->VFp()->cWT(he.VInd()));


							}
							else
								vertTex.push_back(TexCoordTypee(-1, -1));


							/*else if (cm == CMPerFace)
							vertFaceColor.push_back(vi->VFp()->C());*/
							//qDebug() << count++;
						}
						glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
						glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(TexCoordTypee), &(vertTex[0]), GL_STATIC_DRAW_ARB);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						//glTexCoordPointer(2, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(TexCoordTypee), 0);
					}
					else if (cm == CMPerVert)
					{
						glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
						glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType::Color4b), (char *)&(m->vert[0].C()), GL_STATIC_DRAW_ARB);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexType::Color4b), 0);
					}


				}


				/*glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);
				glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);*/
				//glVertexAttribPointer((GLuint)1, 4, GL_BYTE, GL_FALSE, 0, 0);
				//glColorPointer(4, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType::Color4b), 0);





			}

			//int C=c;
			//if((C&CHVertex) || (C&CHFace)) {
			//	ComputeBBox(*m);
			//	if(!(curr_hints&HNHasFaceNormal)) m->ComputeFaceNormal();
			//	if(!(curr_hints&HNHasVertNormal)) m->ComputeVertexNormal();
			//	C= (C | CHFaceNormal);
			//}
			//if((C&CHFace) && (curr_hints&HNUseEdgeStrip)) 		 ComputeEdges();
			//if((C&CHFace) && (curr_hints&HNUseLazyEdgeStrip)) ClearEdges();
			//if(MeshType::HasFFTopology())
			//	if((C&CHFace) && (curr_hints&HNUseTriStrip)) 		{
			//			if(!(curr_hints&HNHasFFTopology)) m->FFTopology();
			//			ComputeTriStrip();
			//		}
			//if((C&CHFaceNormal) && (curr_hints&HNUsePerWedgeNormal))		{
			//	  if(!(curr_hints&HNHasVFTopology)) m->VFTopology();
			//		CreaseWN(*m,MeshType::scalar_type(GetHintParamf(HNPCreaseAngle)));
			//}
			//if(C!=0) { // force the recomputation of display list
			//	cdm=DMNone;
			//	ccm=CMNone;
			//	cnm=NMNone;
			//}
			//if((curr_hints&HNUseVArray) && (curr_hints&HNUseTriStrip))
			//	{
			//	 ConvertTriStrip<MeshType>(*m,TStrip,TStripF,TStripVED,TStripVEI);
			//	}
		}
		void initVBOUpdate(DrawMode dm, ColorMode cm, TextureMode tm)
		{
			if (m == 0) return;

			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{

				std::vector<unsigned int>().swap(indices);
				std::vector<TexCoordTypee>().swap(vertTex);

				std::vector<TexCoordTypee>().swap(verTex2);
				std::vector<GLfloat>().swap(vertexArray);
				std::vector<GLfloat>().swap(normalArray);
				std::vector<unsigned char>().swap(colorArray);
				//vertFaceColor = std::vector<vcg::Color4b>();



				for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
				{

					for (int i = 0; i < 3; i++)
					{
						vertexArray.push_back(fi->V(i)->P().X());
						vertexArray.push_back(fi->V(i)->P().Y());
						vertexArray.push_back(fi->V(i)->P().Z());

						normalArray.push_back(fi->V(i)->cN().X());
						normalArray.push_back(fi->V(i)->cN().Y());
						normalArray.push_back(fi->V(i)->cN().Z());
						/*normalArray.push_back(fi->cN().X());
						normalArray.push_back(fi->cN().Y());
						normalArray.push_back(fi->cN().Z());*/
						if (cm == CMPerVert)
						{
							colorArray.push_back(fi->V(i)->C().X());
							colorArray.push_back(fi->V(i)->C().Y());
							colorArray.push_back(fi->V(i)->C().Z());
							colorArray.push_back(fi->V(i)->C().W());

						}
						if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
							verTex2.push_back(fi->WT(i));

					}

				}
				if (curr_hints&HNUseVBO)
				{

					if (!glIsBuffer(array_buffers[0]) || !glIsBuffer(array_buffers[1]) || !glIsBuffer(array_buffers[2]))
						glGenBuffers(3, (GLuint*)array_buffers);



					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
					//glBufferData(GL_ARRAY_BUFFER_ARB, m->fn * 3 * sizeof(VertexType), vertexArray, GL_STATIC_DRAW_ARB);
					glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(GLfloat), &(vertexArray[0]), GL_STATIC_DRAW);//GL_STATIC_DRAW,GL_DYNAMIC_DRAW
					//glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);

					//glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), NULL, GL_STATIC_DRAW);

					glBindBuffer(GL_ARRAY_BUFFER, 0);

					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
					//glBufferData(GL_ARRAY_BUFFER_ARB, m->fn * 3 * sizeof(VertexType), &(normalArray[0]), GL_STATIC_DRAW_ARB);
					glBufferData(GL_ARRAY_BUFFER, normalArray.size()*sizeof(GLfloat), &(normalArray[0]), GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
					{
						glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
						glBufferData(GL_ARRAY_BUFFER, m->fn * 3 * sizeof(TexCoordTypee), &(verTex2[0]), GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						//glTexCoordPointer(2, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(TexCoordTypee), 0);
					}
					else if (cm == CMPerVert)
					{
						glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
						glBufferData(GL_ARRAY_BUFFER, m->fn * 3 * 4 * sizeof(unsigned char), &(colorArray[0]), GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexType::Color4b), 0);
					}


				}


				/*glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);
				glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), 0);*/
				//glVertexAttribPointer((GLuint)1, 4, GL_BYTE, GL_FALSE, 0, 0);
				//glColorPointer(4, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType::Color4b), 0);





			}
			//releaseVector();

		}

		void initVBOUpdate_multi_texture(DrawMode dm, ColorMode cm, TextureMode tm)
		{
			if (m == 0) return;

			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{
				std::vector<std::vector<TexCoordTypee>>().swap(vertTex_multi);
				std::vector<std::vector<GLfloat>>().swap(vertexArray_multi);
				std::vector<std::vector<GLfloat>>().swap(normalArray_multi);
				std::vector<std::vector<unsigned char>>().swap(colorArray_multi);
				std::vector<int>().swap(vertexArray_multi_size);
				/*separate array according different texutre*/
				bool hastex = false;
				int textureSize = m->textures.size();

				if (m->textures.size() <= 0)
					hastex = false;
				else
					hastex = true;

				for (int i = 0; i < m->textures.size() + 1; i++)
				{
					vertexArray_multi.push_back(std::vector<GLfloat>());
					normalArray_multi.push_back(std::vector<GLfloat>());
					vertTex_multi.push_back(std::vector<TexCoordTypee>());
					colorArray_multi.push_back(std::vector<unsigned char>());
				}
				//}

				short textureNumber = -1;

				short arrayNumber = -1;

				//short arrayNumber = 0;

				for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
				{
					/*if (hastex)
					{
					textureNumber = (*fi).WT(0).n();
					textureNumber2 = (*fi).WT(0).n();
					if (textureNumber < 0)
					textureNumber = textureSize;
					}
					else
					{
					textureNumber = 0;
					}*/


					if (hastex)
					{
						textureNumber = (*fi).WT(0).n();
						if (textureNumber < 0){
							arrayNumber = textureSize;
						}
						else
						{
							arrayNumber = textureNumber;

						}
					}
					else
					{
						arrayNumber = 0;
					}

					for (int i = 0; i < 3; i++)
					{
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().X());
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().Y());
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().Z());

						if (dm == DMSmooth)
						{
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().X());
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Y());
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Z());
						}
						else if (dm == DMFlat)
						{
							normalArray_multi[arrayNumber].push_back(fi->cN().X());
							normalArray_multi[arrayNumber].push_back(fi->cN().Y());
							normalArray_multi[arrayNumber].push_back(fi->cN().Z());
						}


						if (cm == CMPerVert)
						{
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().X());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().Y());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().Z());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().W());
						}
						else if (cm == CMPerFace)
						{


							colorArray_multi[arrayNumber].push_back(fi->C().X());
							colorArray_multi[arrayNumber].push_back(fi->C().Y());
							colorArray_multi[arrayNumber].push_back(fi->C().Z());
							colorArray_multi[arrayNumber].push_back(255);
						}
						/*else
						{
						normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().X());
						normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Y());
						normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Z());
						}*/


						if (((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) && hastex)
							vertTex_multi[arrayNumber].push_back(fi->WT(i));


					}



				}
				for (int i = 0; i < vertexArray_multi.size(); i++)
				{
					vertexArray_multi_size.push_back(vertexArray_multi[i].size() / 3);
				}

				if (curr_hints&HNUseVBO)
				{
					int arraybuffersSize = 4;
					for (int i = 0; i < textureSize + 1; i++)
					{

						if (multi_array_buffers.size() <= i)
						{
							multi_array_buffers.push_back(vboBuffer());

							glGenBuffers(arraybuffersSize, (GLuint*)(multi_array_buffers[i].array_buffers));
						}
						else
						{
							if (!glIsBuffer(multi_array_buffers[i].array_buffers[0]) || !glIsBuffer(multi_array_buffers[i].array_buffers[1]) || !glIsBuffer(multi_array_buffers[i].array_buffers[2]) || !glIsBuffer(multi_array_buffers[i].array_buffers[3]))
								glGenBuffers(arraybuffersSize, (GLuint*)multi_array_buffers[i].array_buffers);
						}
						if (vertexArray_multi[i].size() == 0)continue;

						glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[0]);
						//glBufferData(GL_ARRAY_BUFFER_ARB, m->fn * 3 * sizeof(VertexType), vertexArray, GL_STATIC_DRAW_ARB);
						glBufferData(GL_ARRAY_BUFFER, vertexArray_multi[i].size()*sizeof(GLfloat), &(vertexArray_multi[i][0]), GL_STATIC_DRAW);//GL_STATIC_DRAW,GL_DYNAMIC_DRAW
						//glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);

						//glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), NULL, GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, 0);

						glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[1]);
						//glBufferData(GL_ARRAY_BUFFER_ARB, m->fn * 3 * sizeof(VertexType), &(normalArray[0]), GL_STATIC_DRAW_ARB);
						glBufferData(GL_ARRAY_BUFFER, normalArray_multi[i].size()*sizeof(GLfloat), &(normalArray_multi[i][0]), GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						if ((cm == CMPerVert || cm == CMPerFace) && colorArray_multi[i].size() > 0)
						{
							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[2]);
							glBufferData(GL_ARRAY_BUFFER, colorArray_multi[i].size() * sizeof(unsigned char), &(colorArray_multi[i][0]), GL_STATIC_DRAW);
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexType::Color4b), 0);
						}
						if (i != textureSize){

							if (((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) && vertTex_multi[i].size() > 0)
							{
								glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[3]);
								glBufferData(GL_ARRAY_BUFFER, vertTex_multi[i].size() * sizeof(TexCoordTypee), &(vertTex_multi[i][0]), GL_STATIC_DRAW);
								glBindBuffer(GL_ARRAY_BUFFER, 0);
								//glTexCoordPointer(2, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(TexCoordTypee), 0);
							}
						}



					}


					//}


				}

			}
			releaseVector_2();

		}

		void UpdateVBOData(bool updateVert, bool updateColor)
		{
			if (updateVert)
				std::vector<GLfloat>().swap(vertexArray);
			if (updateColor)
				std::vector<unsigned char>().swap(colorArray);
			for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
			{

				for (int i = 0; i < 3; i++)
				{
					vertexArray.push_back(fi->V(i)->P().X());
					vertexArray.push_back(fi->V(i)->P().Y());
					vertexArray.push_back(fi->V(i)->P().Z());

					/*normalArray.push_back(fi->V(i)->cN().X());
					normalArray.push_back(fi->V(i)->cN().Y());
					normalArray.push_back(fi->V(i)->cN().Z());*/

					if (updateColor)
					{
						colorArray.push_back(fi->V(i)->C().X());
						colorArray.push_back(fi->V(i)->C().Y());
						colorArray.push_back(fi->V(i)->C().Z());
						colorArray.push_back(fi->V(i)->C().W());

					}

					//normalArray.push_back(fi->V(i)->cN());
					/*if (cm == CMPerVert)
						colorArray.push_back(fi->V(i)->C());
						if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
						verTex2.push_back(fi->WT(i));*/

				}

			}
			//std::vector<unsigned int>().swap(indices);
			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{
				if (curr_hints&HNUseVBO)
				{
					if (updateVert)
					{
						if (glIsBuffer(array_buffers[0]))
						{
							glBindBufferARB(GL_ARRAY_BUFFER, array_buffers[0]);
							/*glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), 0, GL_STATIC_DRAW);
							glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);*/
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()));
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->fn * 3 * sizeof(VertexType), &(vertexArray[0]));
							glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArray.size()*sizeof(GLfloat), &(vertexArray[0]));
							//glBufferData(GL_ARRAY_BUFFER_ARB, vertexArray.size()*sizeof(GL_FLOAT), &(vertexArray[0]), GL_STATIC_DRAW_ARB);
						}
					}
					if (updateColor)
					{
						if (glIsBuffer(array_buffers[2]))
						{
							glBindBufferARB(GL_ARRAY_BUFFER, array_buffers[2]);
							/*glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), 0, GL_STATIC_DRAW);
							glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType::Color4b), (char *)&(m->vert[0].C()), GL_STATIC_DRAW_ARB);*/
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->vn * sizeof(VertexType), (char *)&(m->vert[0].C()));
							glBufferSubData(GL_ARRAY_BUFFER, 0, m->fn * 3 * 4 * sizeof(unsigned char), &(colorArray[0]));
							//glBufferData(GL_ARRAY_BUFFER_ARB, m->fn * 3 * 4 * sizeof(unsigned char), &(colorArray[0]), GL_STATIC_DRAW_ARB);

						}
					}

					//float  *ptr = (float  *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
					//if (ptr)
					//{
					//	//for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
					//	for (int x = 0; x < m->vert.size(); x++)
					//	{
					//		/*ptr = (char *)&(m->vert[x].P());
					//		++ptr;*/
					//		*ptr = m->vert[x].P().X();
					//		++ptr;	
					//		*ptr = m->vert[x].P().Y();
					//		++ptr;
					//		*ptr = m->vert[x].P().Z();
					//		++ptr;
					//	}								
					//glUnmapBufferARB(GL_ARRAY_BUFFER_ARB); // release pointer to mapping buffer

					//}
				}
			}
			releaseVector();
			/*for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
			{
			indices.push_back((unsigned int)((*fi).V(0) - &(*m->vert.begin())));
			indices.push_back((unsigned int)((*fi).V(1) - &(*m->vert.begin())));
			indices.push_back((unsigned int)((*fi).V(2) - &(*m->vert.begin())));
			}*/
		}
		void UpdateVBOData_multiArray(bool updateVert, bool updateNormal, bool updateColor, ColorMode cm, DrawMode dm)
		{
			int textureSize = m->textures.size();
			if (updateVert)
			{
				std::vector<std::vector<GLfloat>>().swap(vertexArray_multi);
			}
			if (updateNormal)
				std::vector<std::vector<GLfloat>>().swap(normalArray_multi);

			if (updateColor)
				std::vector<std::vector<unsigned char>>().swap(colorArray_multi);
			bool hastex = false;

			if (m->textures.size() <= 0)
				hastex = false;
			else
				hastex = true;
			for (int i = 0; i < m->textures.size() + 1; i++)
			{
				if (updateVert)
					vertexArray_multi.push_back(std::vector<GLfloat>());
				if (updateNormal)
					normalArray_multi.push_back(std::vector<GLfloat>());
				if (updateColor)
					colorArray_multi.push_back(std::vector<unsigned char>());
			}

			short textureNumber = 0;

			short arrayNumber = -1;
			for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
			{
				if (hastex)
				{
					textureNumber = (*fi).WT(0).n();
					if (textureNumber < 0){
						arrayNumber = textureSize;
					}
					else
					{
						arrayNumber = textureNumber;

					}
				}
				else
				{
					arrayNumber = 0;
				}

				for (int i = 0; i < 3; i++)
				{
					if (updateVert){
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().X());
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().Y());
						vertexArray_multi[arrayNumber].push_back(fi->V(i)->P().Z());
					}
					if (updateNormal)
					{
						if (dm == DMFlat)
						{
							normalArray_multi[arrayNumber].push_back(fi->cN().X());
							normalArray_multi[arrayNumber].push_back(fi->cN().Y());
							normalArray_multi[arrayNumber].push_back(fi->cN().Z());
						}
						else if (dm == DMSmooth)
						{
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().X());
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Y());
							normalArray_multi[arrayNumber].push_back(fi->V(i)->cN().Z());
						}
					}
					if (updateColor)
					{
						if (cm == CMPerFace)
						{
							colorArray_multi[arrayNumber].push_back(fi->C().X());
							colorArray_multi[arrayNumber].push_back(fi->C().Y());
							colorArray_multi[arrayNumber].push_back(fi->C().Z());
							colorArray_multi[arrayNumber].push_back(fi->C().W());
						}
						else if (cm == CMPerVert)
						{
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().X());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().Y());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().Z());
							colorArray_multi[arrayNumber].push_back(fi->V(i)->C().W());
						}

					}
				}

			}


			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{


				if (updateVert)
				{
					for (int i = 0; i < multi_array_buffers.size(); i++)
					{
						if (vertexArray_multi[i].size() == 0)continue;
						if (glIsBuffer(multi_array_buffers[i].array_buffers[0]))
						{
							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[0]);
							//glBufferData(GL_ARRAY_BUFFER, vertexArray_multi[i].size()*sizeof(GLfloat), &(vertexArray_multi[i][0]), GL_STATIC_DRAW);
							glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArray_multi[i].size()*sizeof(GLfloat), &(vertexArray_multi[i][0]));
						}
					}
				}
				if (updateNormal)
				{
					for (int i = 0; i < multi_array_buffers.size(); i++)
					{
						if (normalArray_multi[i].size() == 0)continue;
						if (glIsBuffer(multi_array_buffers[i].array_buffers[1]))
						{
							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[1]);
							glBufferSubData(GL_ARRAY_BUFFER, 0, normalArray_multi[i].size()*sizeof(GLfloat), &(normalArray_multi[i][0]));
						}
					}

				}
				if (updateColor)
				{
					for (int i = 0; i < multi_array_buffers.size(); i++)
					{
						if (colorArray_multi[i].size() == 0)continue;
						if (glIsBuffer(multi_array_buffers[i].array_buffers[2]))
						{
							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[2]);
							//glBufferData(GL_ARRAY_BUFFER, m->fn * 3 * 4 * sizeof(unsigned char), &(colorArray_multi[i][0]), GL_STATIC_DRAW);
							glBufferSubData(GL_ARRAY_BUFFER, 0, colorArray_multi[i].size() * sizeof(unsigned char), &(colorArray_multi[i][0]));
						}
					}

				}

			}

			releaseVector_2();

		}
		void UpdateVBOData_back(bool updateVert, bool updateColor)
		{

			//std::vector<unsigned int>().swap(indices);
			if (curr_hints&HNUseVArray || curr_hints&HNUseVBO)
			{
				if (curr_hints&HNUseVBO)
				{
					if (updateVert)
					{
						if (glIsBuffer(array_buffers[0]))
						{
							glBindBufferARB(GL_ARRAY_BUFFER_ARB, array_buffers[0]);
							/*glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), 0, GL_STATIC_DRAW);
							glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()), GL_STATIC_DRAW_ARB);*/
							glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->vn * sizeof(VertexType), (char *)&(m->vert[0].P()));
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->fn * 3 * sizeof(VertexType), &(vertexArray[0]));
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, vertexArray.size()*sizeof(GLfloat), &(vertexArray[0]));
						}
					}
					if (updateColor)
					{
						if (glIsBuffer(array_buffers[2]))
						{
							glBindBufferARB(GL_ARRAY_BUFFER_ARB, array_buffers[2]);
							/*glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType), 0, GL_STATIC_DRAW);
							glBufferData(GL_ARRAY_BUFFER_ARB, m->vn * sizeof(VertexType::Color4b), (char *)&(m->vert[0].C()), GL_STATIC_DRAW_ARB);*/
							glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->vn * sizeof(VertexType), (char *)&(m->vert[0].C()));
							//glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, m->fn * 3 * sizeof(VertexType), &(colorArray[0]));
						}
					}

					//float  *ptr = (float  *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
					//if (ptr)
					//{
					//	//for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)
					//	for (int x = 0; x < m->vert.size(); x++)
					//	{
					//		/*ptr = (char *)&(m->vert[x].P());
					//		++ptr;*/
					//		*ptr = m->vert[x].P().X();
					//		++ptr;	
					//		*ptr = m->vert[x].P().Y();
					//		++ptr;
					//		*ptr = m->vert[x].P().Z();
					//		++ptr;
					//	}								
					//glUnmapBufferARB(GL_ARRAY_BUFFER_ARB); // release pointer to mapping buffer

					//}
				}
			}
			/*for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
			{
			indices.push_back((unsigned int)((*fi).V(0) - &(*m->vert.begin())));
			indices.push_back((unsigned int)((*fi).V(1) - &(*m->vert.begin())));
			indices.push_back((unsigned int)((*fi).V(2) - &(*m->vert.begin())));
			}*/
		}


		void Draw(DrawMode dm, ColorMode cm, TextureMode tm)
		{
			switch (dm)
			{
			case	DMRadar:Draw<DMRadar>(cm, tm); break;
			case	DMNone: Draw<DMNone    >(cm, tm); break;
			case	DMBox: Draw<DMBox     >(cm, tm); break;
			case	DMPoints: Draw<DMPoints  >(cm, tm); break;
			case	DMWire: Draw<DMWire    >(cm, tm); break;
			case    DMTexWire: Draw<DMTexWire  >(cm, tm); break;
			case    DMTexWireOne: Draw<DMTexWireOne  >(cm, tm); break;
			case	DMTexWireQuad:Draw<DMTexWireQuad  >(cm, tm); break;
			case	DMHidden: Draw<DMHidden  >(cm, tm); break;
			case	DMFlat: Draw<DMFlat    >(cm, tm); break;
			case	DMSmooth: Draw<DMSmooth  >(cm, tm); break;
			case	DMFlatWire: Draw<DMFlatWire>(cm, tm); break;
			case    DMCapEdge: Draw<DMCapEdge>(cm, tm); break;
			case    DMOutline: Draw<DMOutline  >(cm, tm); break;
			case    DMOutline_Test: Draw<DMOutline_Test>(cm, tm); break;
			case    DMOutline_test_triangle: Draw<DMOutline_test_triangle>(cm, tm); break;
			case	DMSELECTION:Draw<DMSELECTION>(cm, tm); break;
			default: break;
			}
		}

		template< DrawMode dm >
		void Draw(ColorMode cm, TextureMode tm)
		{
			switch (cm)
			{
			case	CMNone: Draw<dm, CMNone   >(tm); break;
			case	CMPerMesh: Draw<dm, CMPerMesh>(tm); break;
			case	CMPerFace: Draw<dm, CMPerFace>(tm); break;
			case	CMPerVert: Draw<dm, CMPerVert>(tm); break;
			case	CMTestSig: Draw<dm, CMTestSig>(tm); break;
			case    CMBlack: Draw<dm, CMBlack>(tm); break;
			case    CMWhite: Draw<dm, CMWhite>(tm); break;
			default: break;
			}
		}

		template< DrawMode dm, ColorMode cm >
		void Draw(TextureMode tm)
		{
			switch (tm)
			{
			case	TMNone: Draw<dm, cm, TMNone          >(); break;
			case	TMPerVert: Draw<dm, cm, TMPerVert       >(); break;
			case	TMPerWedge: Draw<dm, cm, TMPerWedge      >(); break;
			case	TMPerWedgeMulti: Draw<dm, cm, TMPerWedgeMulti >(); break;
			default: break;
			}
		}

		template< DrawMode dm, ColorMode cm, TextureMode tm>
		void Draw()
		{
			if (!m) return;
			if ((curr_hints & HNUseDisplayList)){
				if (cdm == dm && ccm == cm){
					glCallList(dl);
					return;
				}
				else {
					if (dl == 0xffffffff) dl = glGenLists(1);
					glNewList(dl, GL_COMPILE);
				}
			}


			glPushMatrix();
			switch (dm)
			{
			case DMNone: break;
			case DMBox: DrawBBox(cm); break;
			case DMPoints: DrawPoints<NMPerVert, cm>(); break;
			case DMHidden:	DrawHidden(); break;
			case DMFlat:	DrawFill<NMPerFace, cm, tm>(); break;
			case DMFlatWire:	DrawFlatWire<NMPerFace, cm, tm>(); break;
			case DMRadar:	DrawRadar<NMPerFace, cm>(); break;
			case DMWire:	DrawWire<NMPerVert, cm>(); break;
			case DMSmooth: DrawFill<NMPerVert, cm, tm>(); break;
			case DMTexWire: DrawTexWire<NMPerVert, cm, tm>(); break;
			case DMTexWireOne: DrawTexWireOne<NMPerVert, cm, tm>(); break;
			case DMTexWireQuad: DrawTexWire_quad<NMPerVert, cm, tm>(); break;
			case DMCapEdge: DrawCapEdge<cm>(); break;
				//case DMCapEdge: DrawCapEdge(); break;
			case DMOutline: DrawOutline(); break;
			case DMOutline_Test: DrawTexWire_test_version(); break;
			case DMOutline_test_triangle: DrawTriangle_tesst<NMPerVert, cm, tm>(); break;
			case DMSELECTION: DrawSelection<NMPerVert, cm, tm>(); break;

			default: break;
			}
			glPopMatrix();

			if ((curr_hints & HNUseDisplayList)){
				cdm = dm;
				ccm = cm;
				glEndList();
				glCallList(dl);
			}
		}


		/*********************************************************************************************/
		/*********************************************************************************************/


		//template <NormalMode nm, ColorMode cm, TextureMode tm>
		//void DrawFill()
		//{
		//	if (m->fn == 0) return;
		//	if (cm == CMPerMesh)
		//		//glColor(m->C());
		//		glColor3f(1, 1, 1);
		//	if (cm == CMBlack)
		//		glColor3f(0, 0, 0);
		//	if (tm == TMPerWedge || tm == TMPerWedgeMulti)
		//		glDisable(GL_TEXTURE_2D);
		//	if (curr_hints&HNUseVBO)
		//	{
		//		if ((cm == CMNone) || (cm == CMPerMesh))
		//		{
		//			if (nm == NMPerVert)
		//				glEnableClientState(GL_NORMAL_ARRAY);
		//			glEnableClientState(GL_VERTEX_ARRAY);
		//			if (nm == NMPerVert)
		//			{
		//				glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
		//				glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
		//			}
		//			glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
		//			glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
		//			glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
		//			glDisableClientState(GL_VERTEX_ARRAY);
		//			if (nm == NMPerVert)
		//				glDisableClientState(GL_NORMAL_ARRAY);
		//			glBindBuffer(GL_ARRAY_BUFFER, 0);
		//			return;
		//		}
		//	}
		//	if (curr_hints&HNUseVArray)
		//	{
		//		if ((cm == CMNone) || (cm == CMPerMesh))
		//		{
		//			if (nm == NMPerVert)
		//				glEnableClientState(GL_NORMAL_ARRAY);
		//			glEnableClientState(GL_VERTEX_ARRAY);
		//			if (nm == NMPerVert)
		//				glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->N()[0]));
		//			glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->P()[0]));
		//			glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
		//			glDisableClientState(GL_VERTEX_ARRAY);
		//			if (nm == NMPerVert)
		//				glDisableClientState(GL_NORMAL_ARRAY);
		//			return;
		//		}
		//	}
		//	else
		//	if (curr_hints&HNUseTriStrip)
		//	{
		//		//if( (nm==NMPerVert) && ((cm==CMNone) || (cm==CMPerMesh)))
		//		//	if(curr_hints&HNUseVArray){
		//		//		glEnableClientState (GL_NORMAL_ARRAY  );
		//		//		glNormalPointer(GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cN()));
		//		//		glEnableClientState (GL_VERTEX_ARRAY);
		//		//		glVertexPointer(3,GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cP()));
		//		//		std::vector<GLAElem>::iterator vi;
		//		//		for(vi=TStripVED.begin();vi!=TStripVED.end();++vi)
		//		//					glDrawElements(vi->glmode ,vi->len,GL_UNSIGNED_SHORT,&TStripVEI[vi->start] );
		//		//
		//		//		glDisableClientState (GL_NORMAL_ARRAY  );
		//		//		glDisableClientState (GL_VERTEX_ARRAY);
		//		//		return;
		//		//	}
		//		//std::vector< MeshType::VertexType *>::iterator vi;
		//		//glBegin(GL_TRIANGLE_STRIP);
		//		//if(nm == NMPerFace) fip=TStripF.begin();
		//		//for(vi=TStrip.begin();vi!=TStrip.end(); ++vi){
		//		//	if((*vi)){
		//		//		if(nm==NMPerVert) glNormal((*vi)->cN());
		//		//		if(nm==NMPerFace) glNormal((*fip)->cN());
		//		//		glVertex((*vi)->P());
		//		//		}
		//		//	else
		//		//		{
		//		//			glEnd();
		//		//			glBegin(GL_TRIANGLE_STRIP);
		//		//		}
		//		//	if(nm == NMPerFace) ++fip;
		//		//	}
		//		//glEnd();
		//	}
		//	else
		//	{
		//		typename FACE_POINTER_CONTAINER::iterator fp;
		//		FaceIterator fi;
		//		short curtexname = -1;
		//		if (partial)
		//			fp = face_pointers.begin();
		//		else
		//			fi = m->face.begin();
		//		if (tm == TMPerWedgeMulti)
		//		{
		//			curtexname = (*fi).WT(0).n();
		//			if ((curtexname >= 0) && (curtexname < (int)TMId.size()))
		//			{
		//				glEnable(GL_TEXTURE_2D);
		//				//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
		//				//***20150512 import viewerID
		//				glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
		//				//switch (viewerNum)
		//				//{
		//				//case 0:
		//				//	glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
		//				//	//qDebug("1_place");
		//				//	break;
		//				//case 1:
		//				//	glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
		//				//	//qDebug("2_place");
		//				//	break;
		//				//case 2:
		//				//	glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
		//				//	//qDebug("3_place");
		//				//	break;
		//				//}
		//				//*****************************
		//			}
		//			else
		//			{
		//				glDisable(GL_TEXTURE_2D);
		//			}
		//		}
		//		if (tm == TMPerWedge)
		//			glEnable(GL_TEXTURE_2D);
		//		if (tm == TMPerVert && !TMId.empty()) // in the case of per vertex tex coord we assume that we have a SINGLE texture.//這邊要修改成多材質版本
		//		{
		//			curtexname = 0;
		//			glEnable(GL_TEXTURE_2D);
		//			glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
		//		}
		//		/////////============================================/////////////////////////////////////////////
		//		//***20151005***testPoint
		//		glBegin(GL_TRIANGLES);
		//		//glBegin(GL_POINTS);
		//		//while ((partial) ? (fp != face_pointers.end()) : (fi != m->face.end()))
		//		while (fi != m->face.end())
		//		{
		//			//FaceType & f = (partial) ? (*(*fp)) : *fi;//======================================
		//			FaceType & f = *fi;//======================================
		//			if (!f.IsD())
		//			{
		//				if (tm == TMPerWedgeMulti)
		//				{
		//					//qDebug() << "8888888" << f.WT(0).n();
		//					if (f.WT(0).n() != curtexname)
		//					{
		//						curtexname = (*fi).WT(0).n();
		//						glEnd();
		//						if (curtexname >= 0)
		//						{
		//							glEnable(GL_TEXTURE_2D);
		//							if (!TMId.empty())
		//							{
		//								//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
		//								//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
		//								//qDebug("1_place");
		//								//***20150512
		//								//qDebug() << "trimesh_viewerNum" << viewerNum;
		//								glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
		//								//switch (viewerNum)
		//								//{
		//								//case 0:
		//								//	glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
		//								//	//qDebug("1_place");
		//								//	break;
		//								//case 1:
		//								//	glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
		//								//	//qDebug() << "trimesh_viewerNum" << TMIdd[1][curtexname];
		//								//	break;
		//								//case 2:
		//								//	glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
		//								//	//qDebug("3_place");
		//								//	break;
		//								//}
		//							}
		//						}
		//						else
		//						{
		//							glDisable(GL_TEXTURE_2D);
		//						}
		//						//***20151005***
		//						glBegin(GL_TRIANGLES);
		//						//glBegin(GL_POINTS);
		//						//********************
		//					}
		//				}
		//				if (nm == NMPerFace)
		//					glNormal(f.cN());
		//				//qDebug() << "trimesh f.cN()" << f.cN().X() << f.cN().Y() << f.cN().Z();
		//				if (nm == NMPerVert)glNormal(f.V(0)->cN());
		//				else if (nm == NMPerWedge) glNormal(f.WN(0));
		//				
		//				if (cm == CMPerFace)
		//				{
		//					glColor(f.C());
		//				}
		//				if (cm == CMPerVert)
		//				{
		//					glColor(f.V(0)->C());
		//					//qDebug() << "(*ei).V(0)->C()" << f.V(0)->C().X()<< f.V(0)->C().Y() << f.V(0)->C().Z();
		//				}
		//				if (tm == TMPerVert)
		//					glTexCoord(f.V(0)->T().P());
		//				else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
		//				{
		//					glTexCoord(f.WT(0).t(0));
		//					//qDebug() << "f.WT(0).t(0)" << f.WT(0).P().X() << f.WT(0).P().Y() ;
		//				}
		//				glVertex(f.V(0)->P());
		//				if (nm == NMPerVert)
		//					glNormal(f.V(1)->cN());
		//				else if (nm == NMPerWedge)glNormal(f.WN(1));
		//				if (cm == CMPerVert)	glColor(f.V(1)->C());
		//				if (tm == TMPerVert) glTexCoord(f.V(1)->T().P());
		//				else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(1).t(0));
		//				glVertex(f.V(1)->P());
		//				if (nm == NMPerVert)	glNormal(f.V(2)->cN());
		//				else if (nm == NMPerWedge)glNormal(f.WN(2));
		//				if (cm == CMPerVert) glColor(f.V(2)->C());
		//				if (tm == TMPerVert) glTexCoord(f.V(2)->T().P());
		//				else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(2).t(0));
		//				glVertex(f.V(2)->P());
		//			}
		//		/*	if (partial)
		//				++fp;
		//			else*/
		//				++fi;
		//		}
		//		glEnd();
		//	}
		//}
		template <NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawFill_back()
		{
			if (m->fn == 0) return;

			if (cm == CMPerMesh)
				//glColor(m->C());
				glColor3f(1, 1, 1);

			if (cm == CMBlack)
				glColor3f(0, 0, 0);

			if (tm == TMPerWedge || tm == TMPerWedgeMulti)
				glEnable(GL_TEXTURE_2D);

			//if (curr_hints&HNUseVBO)
			//{
			//	short curtexname = -1;
			//	/*if ((cm == CMNone) || (cm == CMPerMesh))
			//	{*/
			//	if (nm == NMPerVert)
			//		glEnableClientState(GL_NORMAL_ARRAY);
			//	glEnableClientState(GL_VERTEX_ARRAY);
			//	if (tm == TMPerWedgeMulti)
			//		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			//	else if (cm == CMPerVert || cm == CMPerFace)
			//	{
			//		glEnableClientState(GL_COLOR_ARRAY);
			//		//glEnableVertexAttribArray(1);
			//	}
			//	if (nm == NMPerVert)
			//	{
			//		glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
			//		glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
			//	}
			//	glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
			//	glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
			//	if (tm == TMPerWedgeMulti)
			//	{
			//		FaceIterator fi;
			//		fi = m->face.begin();
			//		curtexname = (*fi).WT(0).n();
			//		if (TMIdd[0].size() > 0)//curtexname = TMIdd[0][0];
			//			//if (curtexname >-1)
			//		{
			//			glEnable(GL_TEXTURE_2D);
			//			if (!TMId.empty())
			//			{
			//				glBindTexture(GL_TEXTURE_2D, TMIdd[0][0]);
			//				glColor3f(1.0, 1.0, 1.0);
			//			}
			//		}
			//		//***bindTexture
			//		glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
			//		//glTexCoordPointer(2, GL_FLOAT, sizeof(typename  MeshType::VertexType::TexCoordfOcf), 0);
			//		glTexCoordPointer(2, GL_FLOAT, sizeof(typename  TexCoordTypee), 0);
			//	}
			//	else if (cm == CMPerVert /*|| cm == CMPerFace*/)
			//	{
			//		glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
			//		//glColorPointer(4, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType::Color4b), 0);
			//		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType::Color4b), 0);
			//		//glVertexAttribPointer((GLuint)1, 4, GL_BYTE, GL_FALSE, 0, 0);
			//	}
			//	glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
			//	glDisableClientState(GL_VERTEX_ARRAY);
			//	if (nm == NMPerVert)
			//		glDisableClientState(GL_NORMAL_ARRAY);
			//	if (tm == TMPerWedgeMulti)
			//	{
			//		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			//	}
			//	else if (cm == CMPerVert /*|| CMPerFace*/)
			//	{
			//		glDisableClientState(GL_COLOR_ARRAY);
			//	}
			//	glBindBuffer(GL_ARRAY_BUFFER, 0);
			//	//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
			//	return;
			//	//}
			//}

			if (curr_hints&HNUseVBO)
			{
				short curtexname = -1;
				/*if ((cm == CMNone) || (cm == CMPerMesh))
				{*/
				if (nm == NMPerVert)
					glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);

				if (tm == TMPerWedgeMulti)
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				else if (cm == CMPerVert || cm == CMPerFace)
				{
					glEnableClientState(GL_COLOR_ARRAY);
					//glEnableVertexAttribArray(1);
				}


				if (nm == NMPerVert)
				{
					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[1]);
					//glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
					glNormalPointer(GL_FLOAT, 3 * sizeof(GLfloat), 0);
				}
				glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
				//glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				if (tm == TMPerWedgeMulti)
				{
					FaceIterator fi;
					fi = m->face.begin();
					curtexname = (*fi).WT(0).n();
					if (TMIdd[0].size() > 0)//curtexname = TMIdd[0][0];
						//if (curtexname >-1)
					{
						glEnable(GL_TEXTURE_2D);
						if (!TMId.empty())
						{
							glBindTexture(GL_TEXTURE_2D, TMIdd[0][0]);
							glColor3f(1.0, 1.0, 1.0);
						}
					}
					//***bindTexture
					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
					//glTexCoordPointer(2, GL_FLOAT, sizeof(typename  MeshType::VertexType::TexCoordfOcf), 0);
					glTexCoordPointer(2, GL_FLOAT, sizeof(typename  TexCoordTypee), 0);
				}
				else if (cm == CMPerVert /*|| cm == CMPerFace*/)
				{
					glBindBuffer(GL_ARRAY_BUFFER, array_buffers[2]);
					//glColorPointer(4, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType::Color4b), 0);
					//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType::Color4b), 0);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
					//glVertexAttribPointer((GLuint)1, 4, GL_BYTE, GL_FALSE, 0, 0);
				}

				//glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
				//glDrawArrays(GL_TRIANGLES, 0, m->fn*9 );
				glDrawArrays(GL_TRIANGLES, 0, m->fn * 3);

				glDisableClientState(GL_VERTEX_ARRAY);
				if (nm == NMPerVert)
					glDisableClientState(GL_NORMAL_ARRAY);
				if (tm == TMPerWedgeMulti)
				{

					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else if (cm == CMPerVert /*|| CMPerFace*/)
				{
					glDisableClientState(GL_COLOR_ARRAY);
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				return;

				//}
			}


			if (curr_hints&HNUseVArray)
			{
				if ((cm == CMNone) || (cm == CMPerMesh))
				{
					if (nm == NMPerVert)
						glEnableClientState(GL_NORMAL_ARRAY);
					glEnableClientState(GL_VERTEX_ARRAY);

					if (nm == NMPerVert)
						glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->N()[0]));
					glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->P()[0]));

					glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
					glDisableClientState(GL_VERTEX_ARRAY);
					if (nm == NMPerVert)
						glDisableClientState(GL_NORMAL_ARRAY);

					return;
				}
			}
			else

				if (curr_hints&HNUseTriStrip)
				{
					//if( (nm==NMPerVert) && ((cm==CMNone) || (cm==CMPerMesh)))
					//	if(curr_hints&HNUseVArray){
					//		glEnableClientState (GL_NORMAL_ARRAY  );
					//		glNormalPointer(GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cN()));
					//		glEnableClientState (GL_VERTEX_ARRAY);
					//		glVertexPointer(3,GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cP()));
					//		std::vector<GLAElem>::iterator vi;
					//		for(vi=TStripVED.begin();vi!=TStripVED.end();++vi)
					//					glDrawElements(vi->glmode ,vi->len,GL_UNSIGNED_SHORT,&TStripVEI[vi->start] );
					//
					//		glDisableClientState (GL_NORMAL_ARRAY  );
					//		glDisableClientState (GL_VERTEX_ARRAY);
					//		return;
					//	}

					//std::vector< MeshType::VertexType *>::iterator vi;
					//glBegin(GL_TRIANGLE_STRIP);
					//if(nm == NMPerFace) fip=TStripF.begin();

					//for(vi=TStrip.begin();vi!=TStrip.end(); ++vi){
					//	if((*vi)){
					//		if(nm==NMPerVert) glNormal((*vi)->cN());
					//		if(nm==NMPerFace) glNormal((*fip)->cN());
					//		glVertex((*vi)->P());
					//		}
					//	else
					//		{
					//			glEnd();
					//			glBegin(GL_TRIANGLE_STRIP);
					//		}
					//	if(nm == NMPerFace) ++fip;
					//	}
					//glEnd();
				}
				else
				{
					typename FACE_POINTER_CONTAINER::iterator fp;
					FaceIterator fi;

					short curtexname = -1;
					if (partial)
						fp = face_pointers.begin();
					else
						fi = m->face.begin();

					if (tm == TMPerWedgeMulti)
					{
						curtexname = (*fi).WT(0).n();
						if ((curtexname >= 0) && (curtexname < (int)TMId.size()))
						{
							glEnable(GL_TEXTURE_2D);
							//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
							//***20150512 import viewerID								
							switch (viewerNum)
							{
							case 0:
								glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
								//qDebug("1_place");
								break;
							case 1:
								glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
								//qDebug("2_place");
								break;
							case 2:
								glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
								//qDebug("3_place");
								break;
							}
							//*****************************
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
						}
					}

					if (tm == TMPerWedge)
						glEnable(GL_TEXTURE_2D);

					if (tm == TMPerVert && !TMId.empty()) // in the case of per vertex tex coord we assume that we have a SINGLE texture.//這邊要修改成多材質版本
					{
						curtexname = 0;
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
					}
					/////////============================================/////////////////////////////////////////////
					//***20151005***testPoint
					glBegin(GL_TRIANGLES);
					//glBegin(GL_POINTS);

					//while ((partial) ? (fp != face_pointers.end()) : (fi != m->face.end()))

					while (fi != m->face.end())
					{
						//FaceType & f = (partial) ? (*(*fp)) : *fi;//======================================
						FaceType & f = *fi;//======================================

						if (!f.IsD())
						{

							if (tm == TMPerWedgeMulti)
							{
								//qDebug() << "8888888" << f.WT(0).n();
								if (f.WT(0).n() != curtexname)
								{

									curtexname = (*fi).WT(0).n();
									glEnd();
									if (curtexname >= 0)
									{
										glEnable(GL_TEXTURE_2D);
										if (!TMId.empty())
										{
											//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
											//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
											//qDebug("1_place");
											//***20150512
											//qDebug() << "trimesh_viewerNum" << viewerNum;											

											//glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
											switch (viewerNum)
											{
											case 0:
												glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
												//qDebug("1_place");
												break;
											case 1:
												glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
												//qDebug() << "trimesh_viewerNum" << TMIdd[1][curtexname];
												break;
											case 2:
												glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
												//qDebug("3_place");
												break;
											}

										}


									}
									else
									{
										glDisable(GL_TEXTURE_2D);
									}
									//***20151005***
									glBegin(GL_TRIANGLES);
									//glBegin(GL_POINTS);
									//********************
								}
							}




							if (nm == NMPerFace)
								glNormal(f.cN());
							//qDebug() << "trimesh f.cN()" << f.cN().X() << f.cN().Y() << f.cN().Z();
							if (nm == NMPerVert)glNormal(f.V(0)->cN());
							else if (nm == NMPerWedge) glNormal(f.WN(0));

							if (cm == CMPerFace)
							{
								glColor(f.C());

							}
							if (cm == CMPerVert)
							{
								glColor(f.V(0)->C());
								//qDebug() << "(*ei).V(0)->C()" << f.V(0)->C().X()<< f.V(0)->C().Y() << f.V(0)->C().Z();
							}

							if (tm == TMPerVert)
								glTexCoord(f.V(0)->T().P());
							else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
							{
								glTexCoord(f.WT(0).t(0));
								//qDebug() << "f.WT(0).t(0)" << f.WT(0).P().X() << f.WT(0).P().Y() ;
							}

							glVertex(f.V(0)->P());

							if (nm == NMPerVert)
								glNormal(f.V(1)->cN());
							else if (nm == NMPerWedge)glNormal(f.WN(1));
							if (cm == CMPerVert)	glColor(f.V(1)->C());
							if (tm == TMPerVert) glTexCoord(f.V(1)->T().P());
							else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(1).t(0));
							glVertex(f.V(1)->P());

							if (nm == NMPerVert)	glNormal(f.V(2)->cN());
							else if (nm == NMPerWedge)glNormal(f.WN(2));
							if (cm == CMPerVert) glColor(f.V(2)->C());
							if (tm == TMPerVert) glTexCoord(f.V(2)->T().P());
							else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(2).t(0));
							glVertex(f.V(2)->P());
						}

						/*	if (partial)
						++fp;
						else*/
						++fi;
					}

					glEnd();

				}
		}
		template <NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawFill()
		{
			if (m->fn == 0) return;

			if (cm == CMPerMesh)
				//glColor(m->C());
				glColor3f(1, 1, 1);

			if (cm == CMBlack)
				glColor3f(0, 0, 0);

			if (tm == TMPerWedge || tm == TMPerWedgeMulti)
				glEnable(GL_TEXTURE_2D);


			if (curr_hints&HNUseVBO)
			{
				short curtexname = -1;
				/*if ((cm == CMNone) || (cm == CMPerMesh))
				{*/
				if (nm == NMPerVert || nm == NMPerFace)
					glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);

				//if (tm == TMPerWedgeMulti)
				//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				//else if (cm == CMPerVert || cm == CMPerFace)
				//{
				//	glEnableClientState(GL_COLOR_ARRAY);
				//	//glEnableVertexAttribArray(1);
				//}

				for (int i = 0; i < multi_array_buffers.size(); i++)
				{
					if (nm == NMPerVert || nm == NMPerFace)
					{
						glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[1]);
						//glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
						//glNormalPointer(GL_FLOAT, 3 * sizeof(GLfloat), 0);
						glNormalPointer(GL_FLOAT, 0, 0);
					}
					glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[0]);
					//glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);
					glVertexPointer(3, GL_FLOAT, 0, 0);

					if (i != (multi_array_buffers.size() - 1))
					{
						if (tm == TMPerWedgeMulti){
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							if (TMIdd[0].size() > 0)//curtexname = TMIdd[0][0];								
							{
								glEnable(GL_TEXTURE_2D);

								if (!TMId.empty())
								{
									glBindTexture(GL_TEXTURE_2D, TMIdd[0][i]);
									glColor4f(1.0, 1.0, 1.0, 1.0);
								}
							}
							//***bindTexture
							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[3]);
							glTexCoordPointer(2, GL_FLOAT, sizeof(typename  TexCoordTypee), 0);
							glDrawArrays(GL_TRIANGLES, 0, vertexArray_multi_size[i]);
							glDisableClientState(GL_TEXTURE_COORD_ARRAY);
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1.0, 1.0, 1.0, 1.0);
							glDrawArrays(GL_TRIANGLES, 0, vertexArray_multi_size[i]);
						}
					}

					if (i == (multi_array_buffers.size() - 1))
					{
						if (cm == CMPerVert || cm == CMPerFace)
						{
							glEnableClientState(GL_COLOR_ARRAY);
							glDisable(GL_TEXTURE_2D);
							glEnable(GL_COLOR_MATERIAL);
							glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
							//glColor4f(1.0, 1.0, 1.0, 1.0);

							glBindBuffer(GL_ARRAY_BUFFER, multi_array_buffers[i].array_buffers[2]);
							glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
							glDrawArrays(GL_TRIANGLES, 0, vertexArray_multi_size[i]);
							glDisableClientState(GL_COLOR_ARRAY);
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1.0, 1.0, 1.0, 1.0);
							glDrawArrays(GL_TRIANGLES, 0, vertexArray_multi_size[i]);
						}
					}


					//int test = vertexArray_multi_size[i] / 3;





				}

				glDisableClientState(GL_VERTEX_ARRAY);
				if (nm == NMPerVert || nm == NMPerFace)
					glDisableClientState(GL_NORMAL_ARRAY);
				/*if (tm == TMPerWedgeMulti)
				{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else if (cm == CMPerVert || CMPerFace)
				{
				glDisableClientState(GL_COLOR_ARRAY);
				}*/
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				return;

			}


			if (curr_hints&HNUseVArray)
			{
				if ((cm == CMNone) || (cm == CMPerMesh))
				{
					if (nm == NMPerVert)
						glEnableClientState(GL_NORMAL_ARRAY);
					glEnableClientState(GL_VERTEX_ARRAY);

					if (nm == NMPerVert)
						glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->N()[0]));
					glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), &(m->vert.begin()->P()[0]));

					glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));
					glDisableClientState(GL_VERTEX_ARRAY);
					if (nm == NMPerVert)
						glDisableClientState(GL_NORMAL_ARRAY);

					return;
				}
			}
			else

				if (curr_hints&HNUseTriStrip)
				{
					//if( (nm==NMPerVert) && ((cm==CMNone) || (cm==CMPerMesh)))
					//	if(curr_hints&HNUseVArray){
					//		glEnableClientState (GL_NORMAL_ARRAY  );
					//		glNormalPointer(GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cN()));
					//		glEnableClientState (GL_VERTEX_ARRAY);
					//		glVertexPointer(3,GL_FLOAT,sizeof(MeshType::VertexType),&(m->vert[0].cP()));
					//		std::vector<GLAElem>::iterator vi;
					//		for(vi=TStripVED.begin();vi!=TStripVED.end();++vi)
					//					glDrawElements(vi->glmode ,vi->len,GL_UNSIGNED_SHORT,&TStripVEI[vi->start] );
					//
					//		glDisableClientState (GL_NORMAL_ARRAY  );
					//		glDisableClientState (GL_VERTEX_ARRAY);
					//		return;
					//	}

					//std::vector< MeshType::VertexType *>::iterator vi;
					//glBegin(GL_TRIANGLE_STRIP);
					//if(nm == NMPerFace) fip=TStripF.begin();

					//for(vi=TStrip.begin();vi!=TStrip.end(); ++vi){
					//	if((*vi)){
					//		if(nm==NMPerVert) glNormal((*vi)->cN());
					//		if(nm==NMPerFace) glNormal((*fip)->cN());
					//		glVertex((*vi)->P());
					//		}
					//	else
					//		{
					//			glEnd();
					//			glBegin(GL_TRIANGLE_STRIP);
					//		}
					//	if(nm == NMPerFace) ++fip;
					//	}
					//glEnd();
				}
				else
				{
					typename FACE_POINTER_CONTAINER::iterator fp;
					FaceIterator fi;

					short curtexname = -1;
					if (partial)
						fp = face_pointers.begin();
					else
						fi = m->face.begin();

					if (tm == TMPerWedgeMulti)
					{
						curtexname = (*fi).WT(0).n();
						if ((curtexname >= 0) && (curtexname < (int)TMId.size()))
						{
							glEnable(GL_TEXTURE_2D);
							//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
							//***20150512 import viewerID								
							switch (viewerNum)
							{
							case 0:
								glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
								//qDebug("1_place");
								break;
							case 1:
								glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
								//qDebug("2_place");
								break;
							case 2:
								glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
								//qDebug("3_place");
								break;
							}
							//*****************************
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
						}
					}

					if (tm == TMPerWedge)
						glEnable(GL_TEXTURE_2D);

					if (tm == TMPerVert && !TMId.empty()) // in the case of per vertex tex coord we assume that we have a SINGLE texture.//這邊要修改成多材質版本
					{
						curtexname = 0;
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
					}
					/////////============================================/////////////////////////////////////////////
					//***20151005***testPoint
					glBegin(GL_TRIANGLES);
					//glBegin(GL_POINTS);

					//while ((partial) ? (fp != face_pointers.end()) : (fi != m->face.end()))

					while (fi != m->face.end())
					{
						//FaceType & f = (partial) ? (*(*fp)) : *fi;//======================================
						FaceType & f = *fi;//======================================

						if (!f.IsD())
						{

							if (tm == TMPerWedgeMulti)
							{
								//qDebug() << "8888888" << f.WT(0).n();
								if (f.WT(0).n() != curtexname)
								{

									curtexname = (*fi).WT(0).n();
									glEnd();
									if (curtexname >= 0)
									{
										glEnable(GL_TEXTURE_2D);
										if (!TMId.empty())
										{
											//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
											//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
											//qDebug("1_place");
											//***20150512
											//qDebug() << "trimesh_viewerNum" << viewerNum;											

											//glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
											switch (viewerNum)
											{
											case 0:
												glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
												//qDebug("1_place");
												break;
											case 1:
												glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
												//qDebug() << "trimesh_viewerNum" << TMIdd[1][curtexname];
												break;
											case 2:
												glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
												//qDebug("3_place");
												break;
											}

										}


									}
									else
									{
										glDisable(GL_TEXTURE_2D);
									}
									//***20151005***
									glBegin(GL_TRIANGLES);
									//glBegin(GL_POINTS);
									//********************
								}
							}




							if (nm == NMPerFace)
								glNormal(f.cN());
							//qDebug() << "trimesh f.cN()" << f.cN().X() << f.cN().Y() << f.cN().Z();
							/*if (nm == NMPerVert)
								glNormal(f.V(0)->cN());
								else if (nm == NMPerWedge)
								glNormal(f.WN(0));*/
							if (tm != TMNone)
							{
								if (curtexname < 0)
								{
									/*glDisable(GL_TEXTURE_2D);
									glEnable(GL_COLOR_MATERIAL);
									glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);*/
									if (cm == CMPerFace)
									{
										glColor(f.C());

									}
								}
								else
								{
									//glEnable(GL_TEXTURE_2D);
								}
							}
							else
							{
								/*glDisable(GL_TEXTURE_2D);
								glEnable(GL_COLOR_MATERIAL);
								glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);*/
								if (cm == CMPerFace)
								{
									glColor(f.C());
								}
							}




							/////////////////////////////////////////////////////////////////////////////////////////////////////////////

							for (int i = 0; i < 3; i++)
							{
								if (nm == NMPerVert)
									glNormal(f.V(i)->cN());
								else if (nm == NMPerWedge)
									glNormal(f.WN(i));

								if (tm != TMNone)
								{
									if (curtexname < 0)
									{										
										if (cm == CMPerVert)
										{
											glColor(f.V(i)->C());
											//qDebug() << "(*ei).V(0)->C()" << f.V(0)->C().X()<< f.V(0)->C().Y() << f.V(0)->C().Z();
										}
									}
									else
									{										
										if (tm == TMPerVert)
											glTexCoord(f.V(i)->T().P());
										else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
										{
											glTexCoord(f.WT(i).t(0));
										}
									}
								}
								else
								{
									if (cm == CMPerVert)
									{
										glColor(f.V(i)->C());
										//qDebug() << "(*ei).V(0)->C()" << f.V(0)->C().X()<< f.V(0)->C().Y() << f.V(0)->C().Z();
									}
								}

								glVertex(f.V(i)->P());
							}





							////////////////////////////////////////






							//if (curtexname < 0){
							//	if (cm == CMPerFace)
							//	{
							//		glColor(f.C());

							//	}
							//	if (cm == CMPerVert)
							//	{
							//		glColor(f.V(0)->C());
							//		//qDebug() << "(*ei).V(0)->C()" << f.V(0)->C().X()<< f.V(0)->C().Y() << f.V(0)->C().Z();
							//	}
							//}
							//if (tm == TMPerVert)
							//	glTexCoord(f.V(0)->T().P());
							//else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti))
							//{
							//	glTexCoord(f.WT(0).t(0));
							//	//qDebug() << "f.WT(0).t(0)" << f.WT(0).P().X() << f.WT(0).P().Y() ;
							//}

							//glVertex(f.V(0)->P());

							//if (nm == NMPerVert)
							//	glNormal(f.V(1)->cN());
							//else if (nm == NMPerWedge)glNormal(f.WN(1));
							//if (cm == CMPerVert)	glColor(f.V(1)->C());
							//if (tm == TMPerVert) glTexCoord(f.V(1)->T().P());
							//else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(1).t(0));

							//glVertex(f.V(1)->P());

							//if (nm == NMPerVert)	glNormal(f.V(2)->cN());
							//else if (nm == NMPerWedge)glNormal(f.WN(2));
							//if (cm == CMPerVert) glColor(f.V(2)->C());
							//if (tm == TMPerVert) glTexCoord(f.V(2)->T().P());
							//else if ((tm == TMPerWedge) || (tm == TMPerWedgeMulti)) glTexCoord(f.WT(2).t(0));

							//glVertex(f.V(2)->P());
						}

						/*	if (partial)
						++fp;
						else*/
						++fi;
					}

					glEnd();

				}
		}


		template <NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawSelection()
		{
			if (m->fn == 0) return;

			//glDisable(GL_TEXTURE_2D);

			typename FACE_POINTER_CONTAINER::iterator fp;
			FaceIterator fi;


			/*	if (partial)
					fp = face_pointers.begin();
					else*/
			fi = m->face.begin();

			/////////============================================/////////////////////////////////////////////					
			glBegin(GL_TRIANGLES);

			//while ((partial) ? (fp != (face_pointers.end())) : (fi != (m->face.end())))
			while (fi != (m->face.end()))
			{
				FaceType & f = (partial) ? (*(*fp)) : *fi;//======================================

				if (!f.IsD())
				{

					/*if (nm == NMPerFace)
						glNormal(f.cN());
						if (nm == NMPerVert)
						glNormal(f.V(0)->cN());
						if (nm == NMPerWedge)
						glNormal(f.WN(0));*/
					glVertex(f.V(0)->P());

					/*if (nm == NMPerVert)
						glNormal(f.V(1)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(1));	*/
					glVertex(f.V(1)->P());

					/*if (nm == NMPerVert)	glNormal(f.V(2)->cN());
					if (nm == NMPerWedge)glNormal(f.WN(2));		*/
					glVertex(f.V(2)->P());
				}

				//if (partial)
				//{
				//	//std::advance(fp, 2);
				//	++fp;							
				//	if (fp != face_pointers.end())++fp;
				//}
				//else
				//{
				//std::advance(fi, 2);
				++fi;
				if (fi != m->face.end())++fi;
				//if (fi != m->face.end())++fi;
				//if (fi != m->face.end())++fi;

				//}
			}

			glEnd();
			//*************************************************************
			//if (m->fn == 0) return;

			//if (curr_hints&HNUseVBO)
			//{
			//	short curtexname = -1;
			//	/*if ((cm == CMNone) || (cm == CMPerMesh))
			//	{*/
			//
			//	glEnableClientState(GL_VERTEX_ARRAY);

			//	
			//	glBindBuffer(GL_ARRAY_BUFFER, array_buffers[0]);
			//	glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(typename MeshType::VertexType), 0);

			//	

			//	glDrawElements(GL_TRIANGLES, m->fn * 3, GL_UNSIGNED_INT, &(*indices.begin()));

			//	glDisableClientState(GL_VERTEX_ARRAY);					
			//	
			//	glBindBuffer(GL_ARRAY_BUFFER, 0);

			//	return;
			//}

			//
			//else
			//{
			//	typename FACE_POINTER_CONTAINER::iterator fp;
			//	FaceIterator fi;

			//	short curtexname = -1;
			//	if (partial)
			//		fp = face_pointers.begin();
			//	else
			//		fi = m->face.begin();

			//	
			//	glDisable(GL_TEXTURE_2D);						
			//
			//	/////////============================================/////////////////////////////////////////////
			//	//***20151005***testPoint
			//	glBegin(GL_TRIANGLES);						

			//	while (fi != m->face.end())
			//	{
			//		//FaceType & f = (partial) ? (*(*fp)) : *fi;//======================================
			//		FaceType & f = *fi;//======================================

			//		if (!f.IsD())
			//		{					
			//			
			//			
			//			glVertex(f.V(0)->P());											
			//			glVertex(f.V(1)->P());												
			//			glVertex(f.V(2)->P());
			//		}

			//		/*	if (partial)
			//		++fp;
			//		else*/
			//		++fi;
			//	}

			//	glEnd();

			//}


		}

		// A draw wireframe that hides faux edges
		template <NormalMode nm, ColorMode cm>
		void DrawWirePolygonal()
		{
			if (cm == CMPerMesh)   glColor(m->C());
			FaceIterator fi;
			typename FACE_POINTER_CONTAINER::iterator fp;

			if (partial)
				fp = face_pointers.begin();
			else
				fi = m->face.begin();

			glBegin(GL_LINES);

			while ((partial) ? (fp != face_pointers.end()) : (fi != m->face.end()))
			{
				typename MeshType::FaceType & f = (partial) ? (*(*fp)) : *fi;

				if (!f.IsD())
				{

					if (nm == NMPerFace)	glNormal(f.cN());
					if (cm == CMPerFace)	glColor(f.C());

					if (!f.IsF(0)) {
						if (nm == NMPerVert)	glNormal(f.V(0)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(0));
						if (cm == CMPerVert)
							glColor(f.V(0)->C());
						glVertex(f.V(0)->P());

						if (nm == NMPerVert)	glNormal(f.V(1)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(1));
						if (cm == CMPerVert)	glColor(f.V(1)->C());
						glVertex(f.V(1)->P());
					}

					if (!f.IsF(1)) {
						if (nm == NMPerVert)	glNormal(f.V(1)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(1));
						if (cm == CMPerVert)	glColor(f.V(1)->C());
						glVertex(f.V(1)->P());

						if (nm == NMPerVert)	glNormal(f.V(2)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(2));
						if (cm == CMPerVert)	glColor(f.V(2)->C());
						glVertex(f.V(2)->P());
					}

					if (!f.IsF(2)) {
						if (nm == NMPerVert)	glNormal(f.V(2)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(2));
						if (cm == CMPerVert)	glColor(f.V(2)->C());
						glVertex(f.V(2)->P());

						if (nm == NMPerVert)	glNormal(f.V(0)->cN());
						if (nm == NMPerWedge)glNormal(f.WN(0));
						if (cm == CMPerVert)	glColor(f.V(0)->C());
						glVertex(f.V(0)->P());
					}

				}

				if (partial)
					++fp;
				else
					++fi;
			}

			glEnd();
		}

		/// Basic Point drawing fucntion
		// works also for mesh with deleted vertices
		template<NormalMode nm, ColorMode cm>
		void DrawPointsBase()
		{
			glBegin(GL_POINTS);
			if (cm == CMPerMesh) glColor(m->C());

			for (VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi)if (!(*vi).IsD())
			{
				if (nm == NMPerVert) glNormal((*vi).cN());
				if (cm == CMPerVert) glColor((*vi).C());
				glVertex((*vi).P());
			}
			glEnd();
		}

		/// Utility function that computes in eyespace the current distance between the camera and the center of the bbox of the mesh
		double CameraDistance(){
			CoordType res;
			Matrix44<ScalarType> mm;
			glGetv(GL_MODELVIEW_MATRIX, mm);
			CoordType  c = m->bbox.Center();
			res = mm*c;
			return Norm(res);
		}

		template<NormalMode nm, ColorMode cm>
		void DrawPoints()
		{
			glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
			if (GetHintParami(HNPPointSmooth) > 0) glEnable(GL_POINT_SMOOTH);
			else glDisable(GL_POINT_SMOOTH);
			glPointSize(GetHintParamf(HNPPointSize));
			if (GetHintParami(HNPPointDistanceAttenuation) > 0)
			{
				float camDist = (float)CameraDistance();
				float quadratic[] = { 0.0f, 0.0f, 1.0f / (camDist*camDist), 0.0f };
				//***20151001***mark
				/*glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
				glPointParameterf(GL_POINT_SIZE_MAX, 16.0f);
				glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);*/
			}
			else
			{
				float quadratic[] = { 1.0f, 0.0f, 0.0f };
				glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
				glPointSize(GetHintParamf(HNPPointSize));
			}

			if (m->vn != (int)m->vert.size())
			{
				DrawPointsBase<nm, cm>();
			}
			else
			{
				if (cm == CMPerMesh)
					glColor(m->C());
				if (m->vert.size() != 0)
				{
					// Perfect case, no deleted stuff,
					// draw the vertices using vertex arrays
					if (nm == NMPerVert)
					{
						glEnableClientState(GL_NORMAL_ARRAY);
						glNormalPointer(GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), &(m->vert.begin()->N()[0]));
					}
					if (cm == CMPerVert)
					{
						glEnableClientState(GL_COLOR_ARRAY);
						glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), &(m->vert.begin()->C()[0]));
					}

					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_TYPE_NM<ScalarType>::SCALAR(), sizeof(VertexType), &(m->vert.begin()->P()[0]));

					glDrawArrays(GL_POINTS, 0, m->vn);

					glDisableClientState(GL_VERTEX_ARRAY);
					if (nm == NMPerVert)  glDisableClientState(GL_NORMAL_ARRAY);
					if (cm == CMPerVert)  glDisableClientState(GL_COLOR_ARRAY);
				}
			}
			glPopAttrib();
			return;
		}

		void DrawHidden()
		{
			//const float ZTWIST=HNParamf[HNPZTwist];
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1);
			//glDepthRange(ZTWIST,1.0f);
			glDisable(GL_LIGHTING);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			DrawFill<NMNone, CMNone, TMNone>();
			glDisable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_LIGHTING);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			//glDepthRange(0.0f,1.0f-ZTWIST);
			DrawWire<NMPerVert, CMNone>();
			glPopAttrib();
			//	glDepthRange(0,1.0f);
		}

		template <NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawFlatWire()
		{
			//const float ZTWIST=HNParamf[HNPZTwist];
			//glDepthRange(ZTWIST,1.0f);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1);
			DrawFill<nm, cm, tm>();
			glDisable(GL_POLYGON_OFFSET_FILL);
			//glDepthRange(0.0f,1.0f-ZTWIST);
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			//glColorMaterial(GL_FRONT,GL_DIFFUSE);
			glColor3f(.3f, .3f, .3f);
			DrawWire<nm, CMNone>();
			glPopAttrib();
			//glDepthRange(0,1.0f);
		}

		template <NormalMode nm, ColorMode cm>
		void DrawRadar()
		{
			const float ZTWIST = HNParamf[HNPZTwist];
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(0);
			glDepthRange(ZTWIST, 1.0f);

			if (cm == CMNone)
				glColor4f(0.2f, 1.0f, 0.4f, 0.2f);
			//	DrawFill<nm,cm,TMNone>();
			Draw<DMFlat, CMNone, TMNone>();

			glDepthMask(1);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			//	DrawFill<nm,cm,TMNone>();
			Draw<DMFlat, CMNone, TMNone>();

			glDepthRange(0.0f, 1.0f - ZTWIST);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glColor4f(0.1f, 1.0f, 0.2f, 0.6f);
			Draw<DMWire, CMNone, TMNone>();
			glDisable(GL_BLEND);
			glDepthRange(0, 1.0f);

		}



#ifdef GL_TEXTURE0_ARB
		// Multitexturing nel caso voglia usare due texture unit.
		void DrawTexture_NPV_TPW2()
		{
			unsigned int texname = (*(m->face.begin())).WT(0).n(0);
			glBindTexture(GL_TEXTURE_2D, TMId[texname]);
			typename MeshType::FaceIterator fi;
			glBegin(GL_TRIANGLES);
			for (fi = m->face.begin(); fi != m->face.end(); ++fi)if (!(*fi).IsD()){
				if (texname != (*fi).WT(0).n(0))	{
					texname = (*fi).WT(0).n(0);
					glEnd();
					glBindTexture(GL_TEXTURE_2D, TMId[texname]);
					glBegin(GL_TRIANGLES);
				}
				glMultiTexCoordARB(GL_TEXTURE0_ARB, (*fi).WT(0).t(0));
				glMultiTexCoordARB(GL_TEXTURE1_ARB, (*fi).WT(0).t(0));
				glNormal((*fi).V(0)->N());
				glVertex((*fi).V(0)->P());

				glMultiTexCoordARB(GL_TEXTURE0_ARB, (*fi).WT(1).t(0));
				glMultiTexCoordARB(GL_TEXTURE1_ARB, (*fi).WT(1).t(0));
				glNormal((*fi).V(1)->N());
				glVertex((*fi).V(1)->P());

				glMultiTexCoordARB(GL_TEXTURE0_ARB, (*fi).WT(2).t(0));
				glMultiTexCoordARB(GL_TEXTURE1_ARB, (*fi).WT(2).t(0));
				glNormal((*fi).V(2)->N());
				glVertex((*fi).V(2)->P());
			}
			glEnd();
		}

#endif


		/*int MemUsed()
		{
		int tot=sizeof(GlTrimesh);
		tot+=sizeof(mesh_type::edge_type)*edge.size();
		tot+=sizeof(MeshType::VertexType *) * EStrip.size();
		tot+=sizeof(MeshType::VertexType *) * TStrip.size();
		tot+=sizeof(MeshType::FaceType *)   * TStripF.size();
		return tot;
		}*/

	private:
		//template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawOutline()
		{
			///***20150807********************
			/*glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glColor3f(1, 0, 0);
			glLineWidth(5);
			glPointSize(10);
			//qDebug() << "outlines.size()" << (*outlines).size();

			for (int i = 0; i < (*outlines).size(); i++)
			{
			glBegin(GL_LINE_LOOP);
			for (int j = 0; j < (*outlines)[i].size(); j++)
			{
			//	//第一點
			//	glColor3f(0.88, 0.67, 0.24);
			//qDebug() << "(*outlines)[i][j]" << (*outlines)[i][j][0] << (*outlines)[i][j][1] << (*outlines)[i][j][2];
			glVertex((*outlines)[i][j]);
			//
			}
			glEnd();

			//glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);

			}

			glPopAttrib();*/
			//*****end***20150807*********************
			//*********20150811********畫排序過的 tex_Outline**********************
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glColor3f(0.2, 0.2, 0.2);
			glLineWidth(10);
			glPointSize(8);
			//qDebug() << "outlines_struct.size()" << (*outlines_struct).size();
			//qDebug() << "outlines.size()" << (*outlines).size();

			for (int i = 0; i < (*outlines_struct).size(); i++)
			{
				//qDebug() << "outlines_struct[i].size()" << (*outlines_struct)[i].size();
				glBegin(GL_LINE_LOOP);
				for (int j = 0; j < (*outlines_struct)[i].size(); j++)
				{
					//	//第一點
					//	glColor3f(0.88, 0.67, 0.24);
					//qDebug() << "*outlines_struct)[i][j].p" << (*outlines_struct)[i][j].p)[0] << (*outlines_struct)[i][j].p)[1] << (*outlines_struct)[i][j].p)[2];
					glVertex((*outlines_struct)[i][j].p);

				}
				glEnd();

			}
			for (int i = 0; i < (*outlines_struct).size(); i++)
			{
				//qDebug() << "outlines_struct[i].size()" << (*outlines_struct)[i].size();
				glBegin(GL_POINTS);
				for (int j = 0; j < (*outlines_struct)[i].size(); j++)
				{
					glVertex((*outlines_struct)[i][j].p);

				}
				glEnd();

			}
			glPopAttrib();
		}

		template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawTexWire() // 20160719 // 2015_3_6加入render edge的函式 
		{

			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);

			/*glLineWidth(5);
			glPointSize(11);*/
			glLineWidth(20);
			glPointSize(10);
			//qDebug("DrawTexWirePos");
			for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			{
				//if (vcg::tri::HasPerVertexTexCoord(*m))
				if (tm == TMPerVert)
				{
					glEnable(GL_TEXTURE_2D);
					if ((*ei).V(0)->T().n() != curtexname)
					{
						//qDebug("TMPerVert= %i", (*ei).V(0)->T().n());
						curtexname = (*ei).V(0)->T().n();
					}
					if (curtexname >= 0)
					{
						//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
						//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
						switch (viewerNum)
						{
						case 0:
							glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
							//qDebug("1_place");
							break;
						case 1:
							glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
							//qDebug("2_place");
							break;
						case 2:
							glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
							//qDebug("3_place");
							break;
						}
					}
				}
				else glDisable(GL_TEXTURE_2D);
				//******Points*************************************
				//glBegin(GL_POINTS);
				////glBegin(GL_LINES);
				////第一點
				//if (cm == GLW::CMPerVert)
				//{							
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());							
				//}
				//else if ( cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if ( cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//
				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm==TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				////qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				//glVertex((*ei).V(0)->P());

				////第二點
				//if ( cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex((*ei).V(1)->P());
				//glEnd();
				//***Points*****************//
				//glBegin(GL_POINTS);						
				////第一點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);

				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm == TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				////qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				//glVertex((*ei).V(0)->P());

				////第二點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex((*ei).V(1)->P());
				//glEnd();
				//***Lines****************						
				glBegin(GL_LINES);
				//第一點
				if (cm == GLW::CMPerVert)
				{
					//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
					glColor((*ei).V(0)->C());
				}
				else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)
					glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)
					glColor3f(1, 1, 1);

				//if (vcg::tri::HasPerVertexTexCoord(*m))
				if (tm == TMPerVert)
				{
					//qDebug("tex_HasPerVertexTexCoord");
					//glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(0)->T().P());
				}
				//qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				glVertex((*ei).V(0)->P());

				//第二點
				if (cm == GLW::CMPerVert)
				{
					//qDebug("COLOR");
					glColor((*ei).V(1)->C());
					//glColor3f(0.88, 0.67, 0.24);
				}
				else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				if (tm == TMPerVert)
				{
					//glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(1)->T().P());
				}
				glVertex((*ei).V(1)->P());
				glEnd();
				//*****************

			}
			glPopAttrib();



		}

		/*void lineToQuad(Point3m p0, Point3m p1, Point3m &p1_o, Point3m &p2_o, Point3m &p3_o, Point3m &p4_o)
		{
		Point3m line_normal(p1.y-p0.y,p1.x-p0.x, 0);
		line_normalline.normalized();
		float slize = 2;

		p1_o = p0 + line_normal*slize;
		p2_o = p0 - line_normal*slize;
		p3_o = p1 - line_normal*slize;
		p4_o = p1 + line_normal*slize;


		}*/


		template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawTexWireOne() // 20160719 // 2015_3_6加入render edge的函式 
		{
			/*GLdouble x[2];
			glGetDoublev(GL_ALIASED_LINE_WIDTH_RANGE,x);*/

			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);

			/*glLineWidth(5);
			glPointSize(11);*/
			glLineWidth(2);
			glPointSize(10);
			//qDebug("DrawTexWirePos");
			for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			{
				//if (vcg::tri::HasPerVertexTexCoord(*m))
				if (tm == TMPerVert)
				{
					glEnable(GL_TEXTURE_2D);
					if ((*ei).V(0)->T().n() != curtexname)
					{
						//qDebug("TMPerVert= %i", (*ei).V(0)->T().n());
						curtexname = (*ei).V(0)->T().n();
					}
					if (curtexname >= 0)
					{
						//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
						//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
						switch (viewerNum)
						{
						case 0:
							glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
							//qDebug("1_place");
							break;
						case 1:
							glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
							//qDebug("2_place");
							break;
						case 2:
							glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
							//qDebug("3_place");
							break;
						}
					}
				}
				else glDisable(GL_TEXTURE_2D);
				//******Points*************************************
				//glBegin(GL_POINTS);
				////glBegin(GL_LINES);
				////第一點
				//if (cm == GLW::CMPerVert)
				//{							
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());							
				//}
				//else if ( cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if ( cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//
				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm==TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				////qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				//glVertex((*ei).V(0)->P());

				////第二點
				//if ( cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex((*ei).V(1)->P());
				//glEnd();
				//***Points*****************//
				//glBegin(GL_POINTS);						
				////第一點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);

				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm == TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				////qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				//glVertex((*ei).V(0)->P());

				////第二點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex((*ei).V(1)->P());
				//glEnd();
				//***Lines****************						
				glBegin(GL_LINES);
				//第一點
				if (cm == GLW::CMPerVert)
				{
					//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
					glColor((*ei).V(0)->C());
				}
				else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)
					glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)
					glColor3f(1, 1, 1);

				//if (vcg::tri::HasPerVertexTexCoord(*m))
				if (tm == TMPerVert)
				{
					//qDebug("tex_HasPerVertexTexCoord");
					//glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(0)->T().P());
				}
				//qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				glVertex((*ei).V(0)->P());

				//第二點
				if (cm == GLW::CMPerVert)
				{
					//qDebug("COLOR");
					glColor((*ei).V(1)->C());
					//glColor3f(0.88, 0.67, 0.24);
				}
				else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				if (tm == TMPerVert)
				{
					//glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(1)->T().P());
				}
				glVertex((*ei).V(1)->P());
				glEnd();
				//*****************

			}
			glPopAttrib();



		}






		template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawTexWire_quad()//2015_3_6加入render edge的函式
		{
			//qDebug() << "DrawTexWire_quad"<<tm;
			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			glDisable(GL_TEXTURE_2D);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);

			/*glLineWidth(5);
			glPointSize(11);*/
			glLineWidth(1);
			glPointSize(10);
			//qDebug("DrawTexWirePos");
			float width = *outline_thick;
			//int width2 = *outline_thick;
			const float DEG2RAD = 3.14159 / 180;

			for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			{
				//if (vcg::tri::HasPerVertexTexCoord(*m))

				if (tm == TMPerVert)
				{
					//glEnable(GL_TEXTURE_2D);
					if ((*ei).V(0)->T().n() != curtexname)
					{
						//qDebug("TMPerVert= %i", (*ei).V(0)->T().n());
						curtexname = (*ei).V(0)->T().n();
					}
					if (curtexname >= 0)
					{
						//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
						//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
						switch (viewerNum)
						{
						case 0:
							glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
							//qDebug("1_place");
							break;
						case 1:
							glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
							//qDebug("2_place");
							break;
						case 2:
							glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
							//qDebug("3_place");
							break;
						}
					}
				}
				else glDisable(GL_TEXTURE_2D);
				//******Quad*************************************
				Point2f x(-(ei->V(0)->P().Y() - ei->V(1)->P().Y()), (ei->V(0)->P().X() - ei->V(1)->P().X()));

				Point2f tempNormal(x.Normalize());

				Point3f v00(ei->V(0)->P().X() + width*tempNormal.X(), ei->V(0)->P().Y() + width*tempNormal.Y(), ei->V(0)->P().Z()); // v0+normal
				Point3f v01(ei->V(0)->P().X() - width*tempNormal.X(), ei->V(0)->P().Y() - width*tempNormal.Y(), ei->V(0)->P().Z()); // v0-normal

				Point3f v10(ei->V(1)->P().X() + width*tempNormal.X(), ei->V(1)->P().Y() + width*tempNormal.Y(), ei->V(1)->P().Z()); // v1+normal
				Point3f v11(ei->V(1)->P().X() - width*tempNormal.X(), ei->V(1)->P().Y() - width*tempNormal.Y(), ei->V(1)->P().Z()); // v1-norma

				if (*draw_circle_in_quad_line)
				{
					glBegin(GL_POLYGON);

					if (cm == GLW::CMPerVert)
					{
						//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
						glColor((*ei).V(0)->C());
					}
					else if (cm == GLW::CMWhite)glColor3f(1, 1, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
					else if (cm == GLW::CMBlack)
						glColor3f(0, 0, 0);
					else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
					/*for (int i = 0; i < 360; i+=3)
					{
					float degInRad = i*DEG2RAD;
					glVertex3f(cos(degInRad)*width + (*ei).V(0)->P().X(), sin(degInRad)*width + (*ei).V(0)->P().Y(), (*ei).V(0)->P().Z());
					}*/
					for (int i = 0; i < circleVertex.size(); i++)
					{
						glVertex3f(circleVertex.at(i).X()*width + (*ei).V(0)->P().X(), circleVertex.at(i).Y()*width + (*ei).V(0)->P().Y(), (*ei).V(0)->P().Z());
					}

					glEnd();
				}
				//qDebug() << "DrawTexWire_quad2" << tm;
				if (tm != TMNone)
				{
					if (curtexname < 0)
					{

						if (cm == CMPerVert)
						{
							glDisable(GL_TEXTURE_2D);
							glEnable(GL_COLOR_MATERIAL);
							glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
							glColor4f(1, 1, 1, 1);
						}
						else
						{
							glEnable(GL_TEXTURE_2D);
							glColor4f(1, 1, 1, 1);
						}
					}
					else
					{
						/*glDisable(GL_TEXTURE_2D);
						glEnable(GL_COLOR_MATERIAL);
						glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);*/
						glEnable(GL_TEXTURE_2D);
						glColor4f(1, 1, 1, 1);
					}
				}else
				{
					glDisable(GL_TEXTURE_2D);
					glEnable(GL_COLOR_MATERIAL);
					glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
					glColor4f(1, 1, 1, 1);
				}



				glBegin(GL_QUADS);

				if (cm == GLW::CMWhite)
					glColor3f(1, 1, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)
					glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)
					glColor3f(1, 1, 1);
				///////////////////////////////////////////////////////////////////
				if (tm != TMNone)
				{
					if (curtexname < 0)
					{
						
						if (cm == CMPerVert)
						{
							
							glColor((*ei).V(0)->C());

						}
					}
					else
					{						
						
						glTexCoord((*ei).V(0)->T().P());
					}
				}
				else
				{
					
					if (cm == CMPerVert)
					{
						
						//glColor4f(1, 1, 0, 1);
						glColor((*ei).V(0)->C());
					}
				}
				glVertex(v00);
				glVertex(v01);

				if (cm == GLW::CMWhite)
					glColor3f(1, 1, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)
					glColor3f(0, 0, 0);
				else if (cm == GLW::CMNone)
					glColor3f(1, 1, 1);
				///////////////////////////////////////////////////////////////////
				if (tm != TMNone)
				{
					if (curtexname < 0)
					{
						
						if (cm == CMPerVert)
						{							
							glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
							glColor((*ei).V(1)->C());

						}
					}
					else
					{
						
						//glColor4f(1, 1, 0, 1);
						glTexCoord((*ei).V(1)->T().P());
					}
				}
				else
				{
					
					if (cm == CMPerVert)
					{
						
						//glColor4f(1, 1, 0, 1);						
						glColor((*ei).V(1)->C());
					}
				}
				glVertex(v11);
				glVertex(v10);

				///////////////////////////////////////////////////////////////////////////







				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());
				//}
				//else if (cm == GLW::CMWhite)glColor3f(1, 1, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);

				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm == TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				//glVertex(v00);
				//glVertex(v01);
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(1, 1, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex(v11);
				//glVertex(v10);
				glEnd();

				//***Lines****************						
				//glBegin(GL_LINES);
				////第一點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
				//	glColor((*ei).V(0)->C());
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)
				//	glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);

				////if (vcg::tri::HasPerVertexTexCoord(*m))
				//if (tm == TMPerVert)
				//{
				//	//qDebug("tex_HasPerVertexTexCoord");
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(0)->T().P());
				//}
				////qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				//glVertex((*ei).V(0)->P());

				////第二點
				//if (cm == GLW::CMPerVert)
				//{
				//	//qDebug("COLOR");
				//	glColor((*ei).V(1)->C());
				//	//glColor3f(0.88, 0.67, 0.24);
				//}
				//else if (cm == GLW::CMWhite)glColor3f(0, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				//else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else if (cm == GLW::CMNone)glColor3f(1, 1, 1);
				//if (tm == TMPerVert)
				//{
				//	glColor4f(1, 1, 1, 1);
				//	glTexCoord((*ei).V(1)->T().P());
				//}
				//glVertex((*ei).V(1)->P());
				//glEnd();
				//*****************

			}
			glPopAttrib();



		}


		template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawTexWire_backup()//2015_3_6加入render edge的函式
		{
			//qDebug("_tex_COLOR");
			//qDebug("Wire");
			//if(!(curr_hints & (HNUseEdgeStrip | HNUseLazyEdgeStrip) ) )
			/*if ((curr_hints & HNIsPolygonal))
			{
			DrawWirePolygonal<nm, cm>();
			qDebug("here1");
			}
			else
			{
			qDebug("here2");
			glPushAttrib(GL_POLYGON_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			DrawFill<nm, cm, TMNone>();
			glPopAttrib();
			}*/
			/*if (tm == TMPerVert && (m->fn == 0 && m->en > 0))
			{*/
			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);
			glLineWidth(3);
			glPointSize(5);
			//qDebug("DrawTexWirePos");
			for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			{
				if (vcg::tri::HasPerVertexTexCoord(*m))
				{

					glEnable(GL_TEXTURE_2D);
					if ((*ei).V(0)->T().n() != curtexname)
					{
						//qDebug("TMPerVert= %i", (*ei).V(0)->T().n());
						curtexname = (*ei).V(0)->T().n();
					}
					if (curtexname >= 0)
					{
						//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
						//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
						switch (viewerNum)
						{
						case 0:
							glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
							//qDebug("1_place");
							break;
						case 1:
							glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
							//qDebug("2_place");
							break;
						case 2:
							glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
							//qDebug("3_place");
							break;
						}
					}
				}
				else glDisable(GL_TEXTURE_2D);

				/*glBegin(GL_POINTS);
				//第一點
				if (vcg::tri::HasPerVertexColor(*m))
				{
				//qDebug("COLOR");
				glColor((*ei).V(0)->C());
				//glColor3f(0.88,0.67, 0.24);
				}
				else glColor3f(0, 0, 0);
				if (vcg::tri::HasPerVertexTexCoord(*m))
				{
				glColor4f(1, 1, 1, 1);
				glTexCoord((*ei).V(0)->T().P());
				}
				glVertex((*ei).V(0)->P());

				//第二點
				if (vcg::tri::HasPerVertexColor(*m))
				{
				//qDebug("COLOR");
				glColor((*ei).V(1)->C());
				//glColor3f(0.88, 0.67, 0.24);
				}
				else glColor3f(0, 0, 0);
				if (vcg::tri::HasPerVertexTexCoord(*m))
				{
				glColor4f(1, 1, 1, 1);
				glTexCoord((*ei).V(1)->T().P());
				}
				glVertex((*ei).V(1)->P());
				glEnd();*/
				glBegin(GL_LINES);
				//第一點
				if (vcg::tri::HasPerVertexColor(*m) && cm == GLW::CMPerVert /*&& ((*ei).V(0)->C() != vcg::Color4b(vcg::Color4b::White))*/)
				{
					//qDebug("trimesh_COLOR");
					//qDebug() << "(*ei).V(0)->C().X()" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();
					glColor((*ei).V(0)->C());
					//glColor3f(0.88,0.67, 0.24);
				}
				else if (vcg::tri::HasPerVertexColor(*m) && cm == GLW::CMNone)glColor3f(1, 0, 1);//***如果沒有點顏色或CM設定為CMNone的話，則顏色設為白色
				else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				//else glColor3f(0, 0, 0);
				if (vcg::tri::HasPerVertexTexCoord(*m))
				{
					//qDebug("tex_HasPerVertexTexCoord");
					glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(0)->T().P());
				}
				//qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

				glVertex((*ei).V(0)->P());

				//第二點
				if (vcg::tri::HasPerVertexColor(*m) && cm == GLW::CMPerVert  /*&& ((*ei).V(1)->C() != vcg::Color4b(vcg::Color4b::White))*/)
				{
					//qDebug("COLOR");
					glColor((*ei).V(1)->C());
					//glColor3f(0.88, 0.67, 0.24);
				}
				else if (vcg::tri::HasPerVertexColor(*m) && cm == GLW::CMNone)glColor3f(1, 0, 1);
				else if (cm == GLW::CMBlack)glColor3f(0, 0, 0);
				if (vcg::tri::HasPerVertexTexCoord(*m))
				{
					glColor4f(1, 1, 1, 1);
					glTexCoord((*ei).V(1)->T().P());
				}
				glVertex((*ei).V(1)->P());
				glEnd();




			}
			glPopAttrib();

			//}else
			/*{
				glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
				{
				glVertex((*ei).V(0)->P());
				glVertex((*ei).V(1)->P());
				}
				glEnd();
				glPopAttrib();
				}*/


		}

		template <NormalMode nm, ColorMode cm>
		void DrawWire()
		{
			//qDebug("Wire");
			//if(!(curr_hints & (HNUseEdgeStrip | HNUseLazyEdgeStrip) ) )
			if ((curr_hints & HNIsPolygonal))
			{
				DrawWirePolygonal<nm, cm>();
			}
			else
			{
				glPushAttrib(GL_POLYGON_BIT);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				DrawFill<nm, cm, TMNone>();
				glPopAttrib();
			}
			if (m->fn == 0 && m->en > 0)
			{
				glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
				{
					glVertex((*ei).V(0)->P());
					glVertex((*ei).V(1)->P());
				}
				glEnd();
				glPopAttrib();
			}
			//	{
			//			if(!HasEdges()) ComputeEdges();

			//if(cm==CMPerMesh)	glColor(m->C());
			//std::vector< MeshType::VertexType *>::iterator vi;
			//glBegin(GL_LINE_STRIP);
			//for(vi=EStrip.begin();vi!=EStrip.end(); ++vi){
			//	if((*vi)){
			//			glNormal((*vi)->N());
			//			glVertex((*vi)->P());
			//		}
			//	else
			//		{
			//			glEnd();
			//			glBegin(GL_LINE_STRIP);
			//		}
			//}
			//glEnd();
			//	}
		}
		template <ColorMode cm>
		void DrawCapEdge()//2015_3_6加入render edge的函式
		{
			glPushAttrib(GL_ENABLE_BIT);
			//qDebug("size %i", (*outlines).size());
			BYTE a[4] = { 0, 1, 1, 0 };
			BYTE b[4] = { 0, 0, 0, 0 };

			//if (!outlines->empty())vcg::glu_tesselator::draw_tesselation(*outlines);
			if (cm == CMTestSig)//true為畫白色交界線，false 為畫黑色cap_edge
			{
				if (!outlines->empty())
					//vcg::glu_tesselator::draw_tesselation(*outlines, 1, a);//畫白色線，蓋掉不需要的線
					//***20160805
					vcg::glu_tesselator::draw_tesselation(*outlines, 1);//畫白色線，蓋掉不需要的線
			}
			else
			{
				//if (!outlines->empty())vcg::glu_tesselator::draw_tesselation(*outlines, 0, b);//畫黑色面
				//***20160805
				if (!outlines->empty())vcg::glu_tesselator::draw_tesselation(*outlines, 0);//畫黑色面
			}


			///***20150807********************
			/*glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);`
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glColor3f(1, 0, 0);
			glLineWidth(14);
			glPointSize(10);
			qDebug() << "outlines).size()" << (*outlines).size();

			for (int i = 0; i < (*outlines).size(); i++)
			{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < (*outlines)[i].size(); j++)
			{
			//	//第一點
			//	glColor3f(0.88, 0.67, 0.24);
			//qDebug() << "(*outlines)[i][j]" << (*outlines)[i][j][0] << (*outlines)[i][j][1] << (*outlines)[i][j][2];
			glVertex((*outlines)[i][j]);
			//
			}
			glEnd();

			//glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);

			}

			glPopAttrib();*/
			//*****end***20150807*********************
			glPopAttrib();
		}

		void DrawBBox(ColorMode cm)
		{
			if (cm == CMPerMesh) glColor(m->C());
			glBoxWire(m->bbox);
		}

		template<NormalMode nm, ColorMode cm, TextureMode tm>
		void DrawTriangle_tesst()
		{
			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			glDisable(GL_LIGHTING);
			glLineWidth(9);
			glPointSize(5);
			//qDebug("DrawTexWirePos");
			//for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			for (FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi)
			{
				if (vcg::tri::HasPerVertexTexCoord(*m))
					if (tm == TMPerVert)
					{
						glEnable(GL_TEXTURE_2D);
						if ((*fi).V(0)->T().n() != curtexname)
						{
							//qDebug("TMPerVert= %i", (*ei).V(0)->T().n());
							curtexname = (*fi).V(0)->T().n();
						}
						if (curtexname >= 0)
						{
							//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
							//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
							switch (viewerNum)
							{
							case 0:
								glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
								//qDebug("1_place");
								break;
							case 1:
								glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
								//qDebug("2_place");
								break;
							case 2:
								glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
								//qDebug("3_place");
								break;
							}
						}
					}
					else glDisable(GL_TEXTURE_2D);

					glBegin(GL_TRIANGLES);
					//第一點

					glColor3f(1, 0, 0);
					if (tm == TMPerVert)
					{
						//qDebug("tex_HasPerVertexTexCoord");
						glColor4f(1, 1, 1, 1);
						glTexCoord((*fi).V(0)->T().P());
					}
					glVertex((*fi).V(0)->P());

					//第二點	
					if (tm == TMPerVert)
					{
						glColor4f(1, 1, 1, 1);
						glTexCoord((*fi).V(0)->T().P());
					}
					glVertex((*fi).V(1)->P());

					//第三點	
					if (tm == TMPerVert)
					{
						glColor4f(1, 1, 1, 1);
						glTexCoord((*fi).V(2)->T().P());
					}
					glVertex((*fi).V(2)->P());

					glEnd();

			}
			glPopAttrib();
		}

		void DrawTexWire_test_version()//2015_0812
		{
			qDebug("DrawTexWire_test_version_Pos 5");
			//這邊qDebug curtexname有可能會產生qvector assertion。
			short curtexname = -1;
			//qDebug() << "drawTexWire";
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_COLOR_MATERIAL);
			/*glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			/*glEnable(GL_POINT_SMOOTH);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);*/
			glDisable(GL_LIGHTING);
			glLineWidth(5);
			glDisable(GL_TEXTURE_2D);
			//glPointSize(7);
			/*for (EdgeIterator ei = m->edge.begin(); ei != m->edge.end(); ++ei)
			{
			if (vcg::tri::HasPerVertexTexCoord(*m))
			{

			glEnable(GL_TEXTURE_2D);
			if ((*ei).V(0)->T().n() != curtexname)
			{
			qDebug("DrawTexWire_test_version_TMPerVert= %i", (*ei).V(0)->T().n());
			curtexname = (*ei).V(0)->T().n();
			}
			if (curtexname >= 0)
			{
			//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
			//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
			switch (viewerNum)
			{
			case 0:
			glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
			//qDebug("1_place");
			break;
			case 1:
			glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
			//qDebug("2_place");
			break;
			case 2:
			glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
			//qDebug("3_place");
			break;
			}
			}
			}
			else glDisable(GL_TEXTURE_2D);
			}*/

			for (int i = 0; i < (*outlines_struct).size(); i++)
			{
				glBegin(GL_LINE_LOOP);
				for (int j = 0; j < (*outlines_struct)[i].size(); j++)
				{
					//	//第一點
					//qDebug() << "*outlines_struct)[i][j].p" << (*outlines_struct)[i][j].p)[0] << (*outlines_struct)[i][j].p)[1] << (*outlines_struct)[i][j].p)[2];
					if (vcg::tri::HasPerVertexColor(*m) && ((*outlines_struct)[i][j].pointColor.X() != 255 && (*outlines_struct)[i][j].pointColor.Y() != 255 && (*outlines_struct)[i][j].pointColor.Z() != 255))
					{
						//qDebug("outlines_struct_COLOR");
						glColor((*outlines_struct)[i][j].pointColor);
						//glColor3f(0.88, 0.67, 0.24);
					}
					else glColor3f(0, 0, 0);

					//if (vcg::tri::HasPerVertexTexCoord(*m))
					//{
					//	glColor4f(1, 1, 1, 1);
					//	glEnable(GL_TEXTURE_2D);
					//	if ((*outlines_struct)[i][j].texIndex != curtexname)
					//	{
					//		qDebug("TMPerVert= %i", (*outlines_struct)[i][j].texIndex);
					//		curtexname = (*outlines_struct)[i][j].texIndex;

					//	}
					//	if (curtexname >= 0)
					//	{
					//		//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
					//		//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
					//		switch (viewerNum)
					//		{
					//		case 0:
					//			glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
					//			//qDebug("1_place");
					//			break;
					//		case 1:
					//			glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
					//			//qDebug("2_place");
					//			break;
					//		case 2:
					//			glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
					//			//qDebug("3_place");
					//			break;
					//		}
					//	}
					//	glTexCoord((*outlines_struct)[i][j].texC.P());

					//}
					//else glDisable(GL_TEXTURE_2D);
					glVertex((*outlines_struct)[i][j].p);
				}
				glEnd();

			}

			//-****20150925********
			for (int i = 0; i < (*outlines_struct).size(); i++)
			{

				for (int j = 0; j < (*outlines_struct)[i].size(); j++)
				{
					if (vcg::tri::HasPerVertexTexCoord(*m))
					{

						glEnable(GL_TEXTURE_2D);
						if ((*outlines_struct)[i][j].texIndex != curtexname)
						{

							curtexname = (*outlines_struct)[i][j].texIndex;
						}
						if (curtexname >= 0)
						{
							//qDebug("TMId[curtexname]= %i", TMId[curtexname]);
							//glBindTexture(GL_TEXTURE_2D, TMId[curtexname]);
							switch (viewerNum)
							{
							case 0:
								glBindTexture(GL_TEXTURE_2D, TMIdd[0][curtexname]);
								//qDebug("1_place");
								break;
							case 1:
								glBindTexture(GL_TEXTURE_2D, TMIdd[1][curtexname]);
								//qDebug("2_place");
								break;
							case 2:
								glBindTexture(GL_TEXTURE_2D, TMIdd[2][curtexname]);
								//qDebug("3_place");
								break;
							}
						}
					}
					else glDisable(GL_TEXTURE_2D);


					glBegin(GL_LINES);
					//第一點

					if (vcg::tri::HasPerVertexTexCoord(*m))
					{
						qDebug("outline_struct_texcoord");
						//glColor4f(1, 0, 1, 1);
						glTexCoord((*outlines_struct)[i][j].texC.P());
					}
					//qDebug() << "(*ei).V(0)->C().X()1" << (*ei).V(0)->C().X() << (*ei).V(0)->C().Y() << (*ei).V(0)->C().Z();

					glVertex((*outlines_struct)[i][j].p);

					//第二點							
					if (vcg::tri::HasPerVertexTexCoord(*m))
					{
						//glColor4f(1, 0, 1, 1);
						glTexCoord((*outlines_struct)[i][(j + 1) % (*outlines_struct)[i].size()].texC.P());
					}
					glVertex((*outlines_struct)[i][(j + 1) % (*outlines_struct)[i].size()].p);
					glEnd();

				}
			}

			//***************
			//glColor4f(1, 1, 1, 1);
			glPopAttrib();



		}


	};// end class

} // end namespace

#endif
