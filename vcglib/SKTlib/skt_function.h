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

using namespace vcg;

namespace SKT
{
	
	template<typename TriMesh, typename scalarType >
	bool mesh_translate(TriMesh &rm, const Point3<scalarType> path)
	{
		Matrix44m tranlate_to;
		tranlate_to.SetTranslate(path);

		//***20150827**************
		//Matrix44m tempt;
		//tempt = rm.Tr;		

		rm.Tr = tranlate_to *rm.Tr;
		//*********end*************
		tri::UpdatePosition<TriMesh>::Matrix(rm, tranlate_to, true);
		tri::UpdateBounding<TriMesh>::Box(rm);
		return true;
	}

	template<typename TriMesh, typename scalarType >
	bool mesh_rotate(TriMesh &rm, scalarType degree, const Point3<scalarType> axis)
	{
		Matrix44m translate_to_origin, translate_to_box_center;//***先回原點,再轉回原來位置
		Matrix44m rotate_m;//***20150420旋轉矩陣	
		//***20150827
		Matrix44m wholeTransformMatrix;


		Box3m *m_box(&rm.bbox);

		int mindim = m_box->MinDim();

		translate_to_origin.SetTranslate(-rm.bbox.Center());//***先回原點
		rotate_m.SetRotateDeg(degree, axis);//***旋轉完
		translate_to_box_center.SetTranslate(rm.bbox.Center());//***再回原位置
		wholeTransformMatrix = (translate_to_box_center * rotate_m * translate_to_origin);

		//***20150827***	紀錄旋轉的矩陣		
		/*Matrix44m original_tr, old_temp_translation, old_temp_rotation, old_meshcenter, old_meshuncenter;
		old_temp_rotation = rm.Tr;
		old_temp_rotation.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));

		old_temp_translation.SetTranslate(rm.Tr.GetColumn3(3));
		old_meshcenter.SetTranslate(old_temp_rotation * (-rm.bbox.Center()));
		old_meshuncenter.SetTranslate(old_temp_rotation * rm.bbox.Center());*/

		rm.Tr = wholeTransformMatrix * rm.Tr;
		//***********************
		tri::UpdatePosition<TriMesh>::Matrix(rm, wholeTransformMatrix, true);
		tri::UpdateBounding<TriMesh>::Box(rm);

		//***20150827_comment rm.Tr.SetIdentity();

		//rm.UpdateBoxAndNormals();
		return true;
	}
	
	
	
	
	
	
	
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
	scalarType execute3DPDashboard(std::wstring c,int flag)
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
	scalarType show3DPDashboard(WCHAR *windowName,int flag)//hide 0 show 5
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
	


	

}
#endif
