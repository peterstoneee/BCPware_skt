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
#ifndef __VCGLIB_GLU_TESSELATOR_H
#define __VCGLIB_GLU_TESSELATOR_H
#include <vcg/space/point2.h>
#include <vector>

#ifndef GL_VERSION_1_1
#error "Please include OpenGL before including this file"
#endif


// The inclusion of glu should be always safe (if someone has already included gl stuff).
#ifndef GLU_VERSIONS
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include <GL/glu.h>
#endif
#endif

#ifndef CALLBACK
#ifdef _WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif

#include <iostream>
using namespace std;

static int vertexIndex = 0;
//static GLdouble vertices[50000][3];
static vector< vector<GLdouble> > vertices;
static vector<GLdouble> temp(3);
namespace vcg
{

	class glu_tesselator
	{


	public:
		typedef glu_tesselator this_type;


		/*
			Works with Point2 and Point3;

			sample usage:

			// tesselation input: each outline represents a polygon contour
			std::vector< std::vector<point_type> > outlines = ...;

			// tesselation output (triangles indices)
			std::vector<int> indices;

			// compute triangles indices
			glu_tesselator::tesselate(outlines, indices);

			// unroll input contours points
			std::vector<point_type> points;

			for (size_t i=0; i<outlines.size(); ++i)
			{
			for (size_t j=0; j<outlines[i].size(); ++j)
			{
			points.push_back(outlines[i][j]);
			}
			}
			// or simply call glu_tesselator::unroll(outlines, points);

			// create triangles
			for (size_t i=0; i<indices.size(); i+=3)
			{
			create_triangle(
			points[ indices[i+0] ],
			points[ indices[i+1] ],
			points[ indices[i+2] ]);
			}
			*/

		template <class point_type>
		static inline void unroll(const std::vector< std::vector<point_type> > & outlines, std::vector<point_type> & points)
		{
			for (size_t i = 0; i < outlines.size(); ++i)
			{
				for (size_t j = 0; j < outlines[i].size(); ++j)
				{
					points.push_back(outlines[i][j]);
				}
			}
		}

		template <class point_type>//建立點到面的順序與編號
		static inline void tesselate(const std::vector< std::vector<point_type> > & outlines, std::vector<int> & indices)
		{
			tess_prim_data_vec t_data;//一種vector 儲存tess_prim_data物件,tess_prim_data包含type與indices 成員

			this_type::do_tesselation(outlines, t_data);

			//int k = 0;
			for (size_t i = 0; i < t_data.size(); ++i)
			{
				const size_t st = t_data[i].indices.size();
				if (st < 3) continue;

				switch (t_data[i].type)
				{
				case GL_TRIANGLES:
					for (size_t j = 0; j < st; ++j)
					{
						indices.push_back(t_data[i].indices[j]);
					}
					break;

				case GL_TRIANGLE_STRIP:
				{
										  int i0 = t_data[i].indices[0];
										  int i1 = t_data[i].indices[1];

										  bool ccw = true;

										  for (size_t j = 2; j < st; ++j)
										  {
											  const int i2 = t_data[i].indices[j];

											  indices.push_back(i0);
											  indices.push_back(i1);
											  indices.push_back(i2);

											  if (ccw) i0 = i2;
											  else     i1 = i2;

											  ccw = !ccw;
										  }																	}
					break;

				case GL_TRIANGLE_FAN:
				{
										const int first = t_data[i].indices[0];
										int prev = t_data[i].indices[1];

										for (size_t j = 2; j < st; ++j)
										{
											const int curr = t_data[i].indices[j];

											indices.push_back(first);
											indices.push_back(prev);
											indices.push_back(curr);

											prev = curr;
										}
				}
					break;

				default:
					break;
				}
			}
		}
		class tess_prim_data
		{
		public:

			typedef tess_prim_data this_type;

			GLenum type;
			std::vector<int> indices;

			tess_prim_data(void) { }
			tess_prim_data(GLenum t) : type(t) { }
		};
		typedef std::vector<tess_prim_data> tess_prim_data_vec;
		template <class point_type>
		static void do_tesselation(const std::vector< std::vector<point_type> > & outlines, tess_prim_data_vec & t_data)
		{
			GLUtesselator * tess = gluNewTess();
			//#ifdef __APPLE__
			//			gluTessCallback(tess, GLU_TESS_BEGIN_DATA,  (GLvoid (CALLBACK *)(...))(this_type::begin_cb));
			//			gluTessCallback(tess, GLU_TESS_END_DATA,    (GLvoid (CALLBACK *)(...))(this_type::end_cb));
			//			gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLvoid (CALLBACK *)(...))(this_type::vertex_cb));
			//#else
			gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (GLvoid(CALLBACK *)())(this_type::begin_cb));//
			gluTessCallback(tess, GLU_TESS_END_DATA, (GLvoid(CALLBACK *)())(this_type::end_cb));
			gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLvoid(CALLBACK *)())(this_type::vertex_cb));
			//gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (GLvoid(CALLBACK *)())(this_type::combineData));
			gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid(CALLBACK *)())tessErrorCB);
			//#endif
			//==========2015_1_20_add=================
			//gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_TRUE);
			gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
			//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
			//========================================
			void * polygon_data = (void *)(&t_data);//t_data為儲存face indices與type的vector，polygon_data為無型態指標

			GLdouble vertex[3];

			size_t k = 0;
			gluTessBeginPolygon(tess, polygon_data);//定義描述一個polygon
			for (size_t i = 0; i < outlines.size(); ++i)
			{
				gluTessBeginContour(tess);//開始一個輪廓
				for (size_t j = 0; j < outlines[i].size(); ++j)
				{
					this_type::get_position(outlines[i][j], vertex);
					gluTessVertex(tess, vertex, (void *)k);
					++k;
				}
				gluTessEndContour(tess);//結束輪廓
			}
			gluTessEndPolygon(tess);//結束polygon

			gluDeleteTess(tess);
		}


		template <class point_type>
		static void draw_tesselation(std::vector< std::vector<point_type> > & outlines, int boundary_only = 0/*, byte *colorv = 0*/)
		{
			GLUtesselator * tess = gluNewTess();

			gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (GLvoid(CALLBACK *)())(this_type::begin_cb2));//
			gluTessCallback(tess, GLU_TESS_END_DATA, (GLvoid(CALLBACK *)())(this_type::end_cb2));
			gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLvoid(CALLBACK *)())(this_type::vertex_cb2));
			gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (GLvoid(CALLBACK *)())(this_type::combineData));
			gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid(CALLBACK *)())tessErrorCB);

			//==========2015_1_20_add=================
			//gluTessProperty(tess, GLU_TESS_TOLERANCE, 0.01);
			gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, boundary_only);
			//gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
			//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD );
			if (boundary_only)
			{
				//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
				gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NEGATIVE);
				//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
			}
			else
			{
				//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
				gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);//current_version
			}
			//
			//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

			//========================================
			//void * polygon_data = (void *)(&t_data);//t_data為儲存face indices與type的vector，polygon_data為無型態指標

			//GLdouble vertex[3];
			//std::vector<std::vector<std::vector<GLdouble>>> get_outline;
			//GLdouble get_outline[4][146][3];
			//GLdouble get_outline[100][500][3];

			int max_ol_size;
			find_max(outlines, max_ol_size);
			//cout <<"max_size" <<max_ol_size << endl;
			GLdouble ***get_outline = create_array<GLdouble>(outlines.size(), max_ol_size, 3);

			getoutline(outlines, get_outline[0][0], max_ol_size);

			vertexIndex = 0;//紀錄新增點的index			

			/*for (int i = 0; i < 2000; i++)
			{
			vertices[i][0] = 0;
			vertices[i][1] = 0;
			vertices[i][2] = 0;
			}*/

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			if (boundary_only)
			{
				glColor4f(1, 1, 1, 0);
				glLineWidth(5);
			}
			else
			{
				glLineWidth(1);
				glColor4f(0, 0, 0, 1);
			}

			//glColor4ubv(&colorv[0]);
			//glColor4f(1, 1, 1, 1);
			//glColor4i(216,138, 28, 1);
			//size_t k = 0;
			gluTessBeginPolygon(tess, 0);//定義描述一個polygon
			for (size_t i = 0; i < outlines.size(); ++i)
			{
				gluTessBeginContour(tess);//開始一個輪廓				
				for (size_t j = 0; j < outlines[i].size(); ++j)
				{
					//this_type::get_position(outlines[i][j], vertex);
					//gluTessVertex(tess, vertex, (void *)k);
					gluTessVertex(tess, get_outline[i][j], get_outline[i][j]);
					//(double)outlines[i][j]
					//++k;
				}
				gluTessEndContour(tess);//結束輪廓
			}
			gluTessEndPolygon(tess);//結束polygon



			free(get_outline);
			//free(vertices)
			vertices.clear();
			//vector<vector<GLdouble>>(vertices).swap(vertices);
			gluDeleteTess(tess);
		}

		template<class T>//動態三維陣列
		static T ***create_array(int Time, int Height, int Width)//動態三維陣列
		{
			int i, j, k;
			T ***space3d, **space2d, *space1d;


			space3d = (T ***)malloc(Time * sizeof(T **)+Time * Height * sizeof(T *)+Time * Height * Width * sizeof(T));
			space2d = (T **)(space3d + Time);

			space1d = (T *)(space2d + Time*Height);

			for (i = 0; i < Time; i++, space2d += Height)
				space3d[i] = space2d;
			for (i = 0; i < Time; i++)
			for (j = 0; j < Height; j++, space1d += Width)
				space3d[i][j] = space1d;

			for (i = 0; i < Time; i++)
			for (j = 0; j < Height; j++)
			for (k = 0; k < Width; k++)
				*(*(*(space3d + i) + j) + k) = 0;


			//free(space3d);


			return space3d;
		}

	protected:

		template <class point_type>
		//static void getoutline(const std::vector< std::vector<point_type> > & outlines, std::vector<std::vector<std::vector<GLdouble>>> &ol)
		static void getoutline(const std::vector< std::vector<point_type> > & outlines, GLdouble *ol, int  m)
		{
			for (int i = 0; i < outlines.size(); i++)
			{
				for (int j = 0; j < outlines[i].size(); j++)
				{
					ol[i * m * 3 + j * 3 + 0] = (GLdouble)outlines[i][j][0];
					ol[i * m * 3 + j * 3 + 1] = (GLdouble)outlines[i][j][1];
					ol[i * m * 3 + j * 3 + 2] = (GLdouble)outlines[i][j][2];
					//ol[i * m * 3 + j * 3][0] = &(GLdouble)outlines[i][j][0]

				}
			}

		}
		template <class point_type>
		static void find_max(const std::vector< std::vector<point_type> > & outlines, int &max_size)
		{
			max_size = outlines[0].size();
			for (int i = 1; i < outlines.size(); i++)
			{
				if (max_size < outlines[i].size())max_size = outlines[i].size();
			}
		}


		static void CALLBACK begin_cb(GLenum type, void * polygon_data)
		{
			tess_prim_data_vec * t_data = (tess_prim_data_vec *)polygon_data;
			t_data->push_back(tess_prim_data(type));//儲存為何種type,triangle, triangle fan, triangle_strip
		}

		static void CALLBACK end_cb(void * polygon_data)
		{
			(void)polygon_data;
		}

		static void CALLBACK vertex_cb(void * vertex_data, void * polygon_data)
		{
			tess_prim_data_vec * t_data = (tess_prim_data_vec *)polygon_data;
			t_data->back().indices.push_back((int)((size_t)vertex_data));
		}
		static void CALLBACK begin_cb2(GLenum type)
		{
			glBegin(type);
		}

		static void CALLBACK end_cb2(void * polygon_data)
		{
			glEnd();
		}
		static void CALLBACK vertex_cb2(const void * vertex_data)
		{
			const GLdouble *ptr = (const GLdouble*)vertex_data;

			glVertex3dv(ptr);
		}
		static void CALLBACK combineData(const GLdouble newVertex[3], const void *vertex_data[4],
			const GLfloat weight[4], void **outData)
		{	
			//***
			//vertices.push_back(temp);

			//vertices[vertexIndex][0] = newVertex[0];
			//vertices[vertexIndex][1] = newVertex[1];
			//vertices[vertexIndex][2] = newVertex[2];

			////*outData = vertices[vertexIndex];   // assign the address of new intersect vertex
			//*outData = &vertices[vertexIndex];
			//++vertexIndex;
			//*******20150907****			

			temp.assign(newVertex, newVertex + 3);
			vertices.push_back(temp);
			*outData = &vertices[vertexIndex][0];//把vector傳給array，outData沒初始化，不知道為什麼可以成功
			++vertexIndex;

			/*GLdouble **temp2 = setupHMM(vertices, vertices.size(), 3);//***應該正確的作法
			*outData = temp2[vertexIndex];*/
			

		}

		static void CALLBACK tessErrorCB(GLenum errorCode)
		{
			const GLubyte *errorStr;

			errorStr = gluErrorString(errorCode);
			cout << "[ERROR]: " << errorStr << endl;
			//qDebug() << "[ERROR]: " << errorStr ;
		}



		template <class scalar_type>
		static inline void get_position(const vcg::Point2<scalar_type> & p, GLdouble * d)
		{
			d[0] = (GLdouble)(p[0]);
			d[1] = (GLdouble)(p[1]);
			d[2] = (GLdouble)(0);
		}

		template <class scalar_type>
		static inline void get_position(const vcg::Point3<scalar_type> & p, GLdouble * d)
		{
			d[0] = (GLdouble)(p[0]);
			d[1] = (GLdouble)(p[1]);
			d[2] = (GLdouble)(p[2]);
		}

		template <class T>
		static T** setupHMM(vector<vector<T> > &vals, int N, int M)
		{
			T** temp;
			temp = new T*[N];
			for (unsigned i = 0; (i < N); i++)
			{
				temp[i] = new T[M];
				for (unsigned j = 0; (j < M); j++)
				{
					temp[i][j] = vals[i][j];
				}
			}
			return temp;
		}


	};

} // end namespace vcg

#endif // __VCGLIB_GLU_TESSELATOR_H
