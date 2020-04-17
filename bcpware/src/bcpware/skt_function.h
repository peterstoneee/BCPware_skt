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
#ifndef __SKTLIB_SKT_FUNCTION_TRI_MESH
#define __SKTLIB_SKT_FUNCTION_TRI_MESH


#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

//#undef _WINDOWS_
#include<windows.h>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

#include<vcg/complex/complex.h>
#include <common/interfaces.h>
#include <algorithm>
#include <map>
#include <vector>
#include <cmath>
#include <Tlhelp32.h>
#include <QTime>
#include <sstream>



using namespace vcg;

namespace SKT
{

	
	class Ray
	{
	public:
		Ray(const Point3f &orig, const Point3f &dir) : orig(orig), dir(dir)
		{

			invdir = Point3f(1. / dir.X(), 1. / dir.Y(), 1. / dir.Z());
			sign[0] = (invdir.X() < 0);
			sign[1] = (invdir.Y() < 0);
			sign[2] = (invdir.Z() < 0);
		}
		Point3f orig, dir;       // ray orig and dir 
		Point3f invdir;
		int sign[3];
	};

	//class abc;
	//class titit
	//{
	//public:
	//	titit();
	//	~titit();
	//	void ddd()
	//	{
	//		SKT::abc a();
	//		
	//		abc::testttt tttt = abc::testttt::def;
	//	}

	//};
	//class abc
	//{
	//public:
	//	enum testttt{abcd, def, efg};
	//	abc();
	//	~abc();
	//	void fff()
	//	{
	//		//SKT::titit::testttt dfdfd = SKT::titit::abc;

	//	}
	//};



	template<typename scalarType>
	bool detect_hit_groove(MeshDocument &md)
	{
		bool temp = false;
		foreach(MeshModel *em, md.meshList)
		{
			if (!md.groove.isInside(em->cm.bbox))//mesh在建構槽外
			{
				//qDebug() << "skt_function_em->cm.bbox.DimZ()" << em->cm.bbox.min.Z();
				//qDebug() << "md.grooveZ" << md.groove.min.Z();


				//qDebug() << qSetRealNumberPrecision(10) << "x = " << em->cm.bbox.min.Z();
				/*qDebug() << "skt_function_em->cm.bbox.DimY()" << em->cm.bbox.min.Y();
				qDebug() << "md.grooveY" << md.groove.min.Y();

				qDebug() << "skt_function_em->cm.bbox.DimX()" << em->cm.bbox.min.X();
				qDebug() << "md.grooveX" << md.groove.min.X();
				*/
				if (em->getMeshSort() == MeshModel::meshsort::print_item)
				{
					em->sethitGroove(true);
					temp = true;
				}
			}
			else
			{
				em->sethitGroove(false);
			}
		}
		return temp;
	}

	template<typename scalarType>
	bool detect_hit_groove_bakup(MeshDocument &md)
	{
		foreach(MeshModel *em, md.meshList)
		{
			if (!md.groove.isInside(em->cm.bbox))
			{
				md.setHitGroove(em);
				return true;
			}
		}
		return false;
	}



	template<class Real>
	void CrossProduct(const Point3<Real>& p1, const Point3<Real>& p2, Point3<Real>& p){
		p.V(0) = p1.V(1) * p2.V(2) - p1.V(2) * p2.V(1);
		p.V(1) = -p1.V(0) * p2.V(2) + p1.V(2) * p2.V(0);
		p.V(2) = p1.V(0) * p2.V(1) - p1.V(1) * p2.V(0);
	}

	template<class Real>
	double SquareLength(const Point3<Real>& p){ return p.V(0) * p.V(0) + p.V(1) * p.V(1) + p.V(2) * p.V(2); }

	template<class Real>
	double Length(const Point3<Real>& p){ return sqrt(SquareLength(p)); }


	//bool test_ddd(int x)
	//{
	//	/*GLUtesselator * tess = gluNewTess();
	//	gluTessProperty(tess, GLU_TESS_WINDING_RULE, x);*/
	//	return true;
	//}

	template<class scalar>
	QTime estimatedTimee(scalar &dimz)
	{
		int layer = ceil(ceil(dimz / 0.005) / 5);
		layer * 20;
		QTime t;
		t.addSecs(layer * 20);
		return t;
	}

	template<typename scalarType>
	scalarType volumeOfMesh(MeshModel& pack_1)//***計算mesh體積公式
	{
		scalarType vols = 0;
		for (size_t i = 0; i < pack_1.cm.face.size(); i++)
		{
			vols += signedVolumnOfTraingle(pack_1.cm.face[i].V(0)->P(), pack_1.cm.face[i].V(1)->P(), pack_1.cm.face[i].V(2)->P());
		}
		abs(vols);

		return vols;
		//from t in mesh.Triangles
		//select SignedVolumeOfTriangle(t.P1, t.P2, t.P3);
	}

	template<typename scalarType>
	scalarType signedVolumnOfTraingle(vcg::Point3<scalarType> p1, vcg::Point3<scalarType> p2, vcg::Point3<scalarType> p3)
	{
		double v321 = p3.X()*p2.Y()*p1.Z();
		double v231 = p2.X()*p3.Y()*p1.Z();
		double v312 = p3.X()*p1.Y()*p2.Z();
		double v132 = p1.X()*p3.Y()*p2.Z();
		double v213 = p2.X()*p1.Y()*p3.Z();
		double v123 = p1.X()*p2.Y()*p3.Z();
		return (1.0 / 6.0)*(-v321 + v231 + v312 - v132 - v213 + v123);
	}


	/*template

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

	}*/


	template<typename scalarType>
	scalarType execute3DPDashboard(std::wstring c, int flag)
	{

		SHELLEXECUTEINFO shellinfo = { 0 };
		shellinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellinfo.hwnd = NULL;
		shellinfo.lpVerb = _T("runas");
		//shellinfo.lpFile = _T("C:\\Windows\\System32\\cmd.exe");
		//shellinfo.lpFile = _T("\\3DPDashboard.exe");
		shellinfo.lpFile = c.c_str();
		shellinfo.lpParameters = _T("");
		shellinfo.lpDirectory = _T("");
		shellinfo.nShow = flag;
		shellinfo.hInstApp = NULL;

		//bool x;
		bool x = ShellExecuteEx(&shellinfo);
		//ShellExecute(NULL, L"runas", L"3DPDashboard.exe", NULL, NULL, SW_SHOWNORMAL);
		return x;
	}

	template<typename scalarType>
	scalarType show3DPDashboard(WCHAR *windowName, int flag)//hide 0 show 5
	{
		HWND hwnd = ::FindWindowEx(NULL, NULL, NULL, windowName);
		bool x = ShowWindow(hwnd, flag);
		//CloseHandle(hwnd);//cannot close
		return x;
	}

	template<typename scalarType>
	scalarType terminateProcess(WCHAR *windowName)
	{
		HWND hwnd = ::FindWindowEx(NULL, NULL, NULL, windowName);
		//bool x = ShowWindow(hwnd, 1);
		bool x = TerminateProcess(hwnd, 1);
		return x;
	}
	//template<typename scalarType>
	//BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
	//{
	//	char buffer[128];
	//	int written = GetWindowTextA(hwnd, buffer, 128);
	//	//if (written && strstr(buffer, "Palette_64bit v0.9.6 - [Project_1]") != NULL) {
	//	if (written && strstr(buffer, "DashBoardWin") != NULL) {
	//		*(HWND*)lParam = hwnd;
	//		return FALSE;
	//	}
	//	return TRUE;
	//}

	template<typename scalarType>
	bool terminateCMDProcess(WCHAR *processName)
	{
		//ShellExecuteW(NULL, L"runas", processName, L"", NULL, SW_SHOWNORMAL);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, processName) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

					TerminateProcess(hProcess, 1);
					// Do stuff..

					CloseHandle(hProcess);
				}
			}
		}
		return 0;

	}

	template<typename scalarType>
	DWORD findCMDProcess(WCHAR *processName)
	{
		//ShellExecuteW(NULL, L"runas", processName, L"", NULL, SW_SHOWNORMAL);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, processName) == 0)
				{
					//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);					

					//CloseHandle(hProcess);
					return entry.th32ProcessID;
				}
			}
		}
		return 0;

	}

	template<typename scalarType>
	void WriteToAFile(stringstream &ss, string filename)
	{
		ofstream file;
		file.open(filename.c_str());
		file << ss.rdbuf();
		file.close();
	}

	template<typename scalarType>
	bool readfromfile(string filename, QMap<int, int> &map)
	{
		string line;
		ifstream myfile(filename.c_str());
		if (myfile.is_open())
		{
			/*getline(myfile, line)
			string delimiter = "=";
			size_t pos = 0;
			string token = line.substr(0, line.find(delimiter));
			line.erase(0, line.find(delimiter) + delimiter.length());
			string token2 = line.substr();
			int x = atoi(token.c_str());	*/

			/*if (filename == token2)
			{*/
			while (getline(myfile, line))
			{
				string delimiter = "=";
				size_t pos = 0;

				string token = line.substr(0, line.find(delimiter));
				line.erase(0, line.find(delimiter) + delimiter.length());
				string token2 = line.substr();
				int x = atoi(token.c_str());
				int x2 = atoi(token2.c_str());
				map.insert(x, x2);
				cout << x << x2 << endl;
			}
			myfile.close();
			return true;
			/*}
			else return false;*/

		}
		else
		{
			cout << "unable to open file";
			return false;
		}
	}

	static bool testIntersection_one(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir, float &tt)
	{
		// Compute the plane supporting the triangle (p1, p2, p3)
		//     normal: n
		//     offset: d
		//
		// A point P lies on the supporting plane iff n.dot(P) + d = 0
		//
#define EPSILON 0.000001f
		Point3m e21, e31, e32, triN, q, r, s;
		e21 = v2 - v1;
		e31 = v3 - v1;
		triN = e21^e31;

		triN = triN.normalized();
		float d = -triN.dot(v1);

		// A point P belongs to the line from P1 to P2 iff
		//     P = P1 + t * (P2 - P1)
		//
		// Find the intersection point P(t) between the line and
		// the plane supporting the triangle:
		//     n.dot(P) + d = 0
		//                  = n.dot(P1 + t (P2 - P1)) + d
		//                  = n.dot(P1) + t n.dot(P2 - P1) + d
		//
		//     t = -(n.dot(P1) + d) / n.dot(P2 - P1)
		//
		Point3m P21 = dir;
		float nDotP21 = triN.dot(P21);

		if (fabs(nDotP21) < EPSILON)
			return false;
		float t = -(triN.dot(orig) + d) / nDotP21;
		Point3m P = orig + dir*t;

		/*if (t<0 || t > 1)
			return false;*/

		// Plane bounding the inside half-space of edge (p1, p2): 
		//     normal: n21 = n x (p2 - p1)
		//     offset: d21 = -n21.dot(p1)
		//
		// A point P is in the inside half-space iff n21.dot(P) + d21 > 0
		//

		Point3m n21, n32, n13;
		n21 = triN^e21;
		float d21 = -n21.dot(v1);

		if (n21.dot(P) + d21 <= 0)
			return false;

		// Edge (p2, p3)
		e32 = v3 - v2;
		n32 = triN ^ e32;
		float d32 = -n32.dot(v2);

		if (n32.dot(P) + d32 <= 0)
			return false;

		// Edge (p3, p1)
		n13 = triN ^ -e31;
		float d13 = -n13.dot(v3);
		if (n13.dot(P) + d13 <= 0)
			return false;
		tt = t;

	}
	static bool testIntersection(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir, float &tt)
	{
		Point3m e1, e2, q, r, s;
		e1 = v2 - v1;
		e2 = v3 - v1;

		q = dir^e2;

		//dir.normalized();

		#define EPSILON 0.000001f
		float a = e1.dot(q);
#if 0
		if (a <EPSILON)
		{

			return false;
		}
		s = orig - v1;
		float u = s.dot(q);
		if (u < 0.0 || u > a)
		{

			return false;
		}
		r = s^ e1;	
		float v =  (dir.dot(r));
		if (v < 0.0f || v + u > a)
		{
			return false;
		}
#else
		if (a < EPSILON && a > -EPSILON)
		{

			return false;
		}

		float f = 1.0f / a;
		s = orig - v1;
		// NORMALIZE(tvec);
		float u = f * (s.dot(q));

		if (u <0.0f || u > 1.0f)
		{
			return false;
		}
		r = s^ e1;
		// NORMALIZE(qvec);
		float v = f* (dir.dot(r));
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

#endif
		tt = f * (e2.dot(r));
		/*if (t<0 || t > 1)
			return false;*/
			return true;

	}
	static bool testIntersection_cube(Box3f box, Ray r)
	{
		float tmin, tmax, tymin, tymax, tzmin, tzmax;
		Point3f bounds[2];
		bounds[0] = box.min;
		bounds[1] = box.max;
		tmin = (bounds[r.sign[0]].X() - r.orig.X()) * r.invdir.X();
		tmax = (bounds[1 - r.sign[0]].X() - r.orig.X()) * r.invdir.X();
		tymin = (bounds[r.sign[1]].Y() - r.orig.Y()) * r.invdir.Y();
		tymax = (bounds[1 - r.sign[1]].Y() - r.orig.Y()) * r.invdir.Y();

		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[r.sign[2]].Z() - r.orig.Z()) * r.invdir.Z();
		tzmax = (bounds[1 - r.sign[2]].Z() - r.orig.Z()) * r.invdir.Z();

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}
	static QString createJsonStringCommand(QString board, QString command, QStringList value)
	{


		QVariantMap firstFloor;
		QVariantMap secondFloor;
		QVariantMap thirdFloor;

		QString jsonString;
		QJsonDocument jsonDoc;

		secondFloor.insert(command, value);

		firstFloor.insert(board, secondFloor);


		//firstFloor.insert(QString("total_pages"), QString::number(test_count));

		jsonDoc = QJsonDocument::fromVariant(firstFloor);

		QString test = jsonDoc.toJson();

		return QString(jsonDoc.toJson());


	}


	static QString convertToVrml2FileName(const QString &fileName)
	{
		QFile outfile(fileName);

		if (outfile.exists())
		{
			if (!outfile.open(QIODevice::ReadOnly))
			{
				qDebug() << "file open Error";
				return fileName;
			}
			QString firstLine = outfile.readLine();
			outfile.close();
			QString vrmlString = "#VRML V1.0 ascii";
			if (firstLine.contains("#VRML V1.0 ascii"))
			{
				QString temppath = PicaApplication::getRoamingDir();
				QDir dir(temppath);//backup
				if (dir.exists("xyzVrml2"))
				{
					dir.setPath(temppath + "/xyzVrml2");					
				}
				else
				{
					dir.setPath(temppath);
					dir.mkpath("xyzVrml2");
				}
				dir.setPath(temppath + "/xyzVrml2");
				QFileInfo inputName(fileName);
				QString destinationPath = QString("%1/%2_vrml2.wrl").arg(dir.absolutePath()).arg(inputName.baseName());

				QStringList arguments;
				arguments.append(fileName);//input
				arguments.append(destinationPath);//ouput

				QProcess *convertToVrml2 = new QProcess;
				convertToVrml2->start("vrml1tovrml2", arguments);
				qDebug() << "vrmlString1";
				return destinationPath;
			}

		}
		QString temppath = PicaApplication::getRoamingDir();
		return fileName;
	}

}
#endif
