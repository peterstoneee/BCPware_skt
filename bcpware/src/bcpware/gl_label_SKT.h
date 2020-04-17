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
#ifndef GL_LABEL_SKT_H
#define GL_LABEL_SKT_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/freeglut.h>
#include <QString>
#include <qbytearray.h>

namespace SKT{

	class GL_label_SKT
	{
	public:
		/*class Mode
		{
		public:
		Mode()
		{
		init();
		}
		Mode(Color4b _color)
		{
		init();
		color = _color;
		}

		void init()
		{
		color = vcg::Color4b(vcg::Color4b::White);
		angle = 0;
		rightAlign = false;
		qFont.setStyleStrategy(QFont::NoAntialias);
		qFont.setFamily("Helvetica");
		qFont.setPixelSize(12);
		}

		Mode(QFont &_qFont, vcg::Color4b _color, float _angle, bool _rightAlign)
		{
		qFont = _qFont;
		color = _color;
		angle = _angle;
		rightAlign = _rightAlign;
		}

		float angle;
		bool rightAlign;
		vcg::Color4b color;
		QFont qFont;
		};*/

	private:
		static void enter2D()
		{

			//glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT);
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_DEPTH_TEST);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
		}

		static void exit2D()
		{
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopAttrib();
		}

	public:
		enum LabelPosition { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

		static void  qstringToChar(QString &s,char *p1)
		{
			QByteArray byteArray = s.toLocal8Bit();
			p1 = new char[byteArray.size() + 1];
			strcpy(p1, byteArray.data());			
			
		}
		static void print_bitmap_string(void* font,  char* s)
		{
			if (s && strlen(s)) {
				while (*s) {
					glutBitmapCharacter(font, *s);
					s++;
				}
			}
		}

		static void draw_label(char *s, float x, float y )
		{


		}
		static void draw_label(std::vector<QString> s, float x, float y)
		{
			enter2D();
			
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);

			for(int i = 0; i < s.size(); i++)
			{
				glRasterPos2f(x, y-i); // center of screen. (-1,0) is center left.
				char *p1;
				QByteArray byteArray = s[i].toLocal8Bit();
				p1 = byteArray.data();
				glDisable(GL_DEPTH_TEST); // also disable the depth test so renders on top
				glColor3f(0, 0, 0);
				SKT::GL_label_SKT::print_bitmap_string(GLUT_BITMAP_HELVETICA_18, p1);

			}
			
			glEnable(GL_DEPTH_TEST); // Turn depth testing back on
			exit2D();

		}
		static void drawquad(int x, int y, int r, int g, int b)
		{
			if (r < 0)r = 0;
			if (g < 0)g = 0;
			glColor3ub(r, g, b);
			glTranslatef(x, y, 0);
			glBegin(GL_QUADS);			
			glVertex3f(0, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 1, 0);
			glVertex3f(0, 1, 0);
			glEnd();
			glTranslatef(-x, -y, 0);
		}

		


	};

}




#endif