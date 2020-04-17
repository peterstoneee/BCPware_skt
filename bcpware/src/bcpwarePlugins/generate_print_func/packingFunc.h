#include<vcg/complex/complex.h>
#include <common/interfaces.h>
#include <QVector>
#include <algorithm>
#include <map>
#include <vector>
#include <iomanip>
#include <iostream>

#define GPF_X_coordinate 0
#define GPF_Y_coordinate 1
#define GPF_Z_coordinate 2



using namespace vcg;
//***20150423_pair 比較函式	
typedef pair<int, float> PAIR;
struct CmpByValue {
	bool operator()(const PAIR& lhs, const PAIR& rhs) {
		return lhs.second < rhs.second;//bak

	}
};

namespace SKT
{
	template<typename TriMesh >
	/////////////////
	//15
	//26
	//37
	//48
	bool sortMesh(QVector<TriMesh *> &meshVector)
	{
		bool flag = true;
		for (int i = 0; i < meshVector.size() - 1 && flag; i++)
		{
			flag = false;
			for (int j = 0; j < meshVector.size() - i - 1; j++)
			{
				if (meshVector[j + 1]->cm.bbox.min.Z() < meshVector[j]->cm.bbox.min.Z() && abs(meshVector[j + 1]->cm.bbox.min.Z() - meshVector[j]->cm.bbox.min.Z()) > 0.1)
				{
					MeshModel *mmtemp = meshVector[j + 1];
					meshVector[j + 1] = meshVector[j];
					meshVector[j] = mmtemp;
					flag = true;
				}

			}

		}
		flag = true;
		for (int i = 0; i < meshVector.size() - 1 && flag; i++)
		{
			flag = false;
			for (int j = 0; j < meshVector.size() - i - 1; j++)
			{
				if (abs(meshVector[j + 1]->cm.bbox.min.Z() - meshVector[j]->cm.bbox.min.Z()) < 0.1 && (meshVector[j + 1]->cm.bbox.min.X() < meshVector[j]->cm.bbox.min.X()))
				{
					MeshModel *mmtemp = meshVector[j + 1];
					meshVector[j + 1] = meshVector[j];
					meshVector[j] = mmtemp;
					flag = true;
				}
			}

		}
		flag = true;
		for (int i = 0; i < meshVector.size() - 1 && flag; i++)
		{
			flag = false;
			for (int j = 0; j < meshVector.size() - i - 1; j++)
			{
				if (abs(meshVector[j + 1]->cm.bbox.min.Z() - meshVector[j]->cm.bbox.min.Z()) < 0.1 &&  abs(meshVector[j + 1]->cm.bbox.min.X() - meshVector[j]->cm.bbox.min.X()) < 0.1 && (meshVector[j + 1]->cm.bbox.min.Y() > meshVector[j]->cm.bbox.min.Y()))
				{
					MeshModel *mmtemp = meshVector[j + 1];
					meshVector[j + 1] = meshVector[j];
					meshVector[j] = mmtemp;
					flag = true;
				}
			}

		}
		return true;
	}

	//template<typename TriMesh, typename scalarType >
	//bool mesh_translate(TriMesh &rm, const Point3<scalarType> path)
	//{
	//	Matrix44m tranlate_to;
	//	tranlate_to.SetTranslate(path);

	//	//***20150827**************
	//	//Matrix44m tempt;
	//	//tempt = rm.Tr;		

	//	rm.Tr = tranlate_to *rm.Tr;
	//	//*********end*************
	//	tri::UpdatePosition<TriMesh>::Matrix(rm, tranlate_to, true);
	//	tri::UpdateBounding<TriMesh>::Box(rm);
	//	return true;
	//}

	//template<typename TriMesh, typename scalarType >
	//bool mesh_rotate(TriMesh &rm, scalarType degree, const Point3<scalarType> axis)
	//{
	//	Matrix44m translate_to_origin, translate_to_box_center;//***先回原點,再轉回原來位置
	//	Matrix44m rotate_m;//***20150420旋轉矩陣	
	//	//***20150827
	//	Matrix44m wholeTransformMatrix;


	//	Box3m *m_box(&rm.bbox);

	//	int mindim = m_box->MinDim();

	//	translate_to_origin.SetTranslate(-rm.bbox.Center());//***先回原點
	//	rotate_m.SetRotateDeg(degree, axis);//***旋轉完
	//	translate_to_box_center.SetTranslate(rm.bbox.Center());//***再回原位置
	//	wholeTransformMatrix = (translate_to_box_center * rotate_m * translate_to_origin);

	//	//***20150827***	紀錄旋轉的矩陣		
	//	/*Matrix44m original_tr, old_temp_translation, old_temp_rotation, old_meshcenter, old_meshuncenter;
	//	old_temp_rotation = rm.Tr;
	//	old_temp_rotation.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));

	//	old_temp_translation.SetTranslate(rm.Tr.GetColumn3(3));
	//	old_meshcenter.SetTranslate(old_temp_rotation * (-rm.bbox.Center()));
	//	old_meshuncenter.SetTranslate(old_temp_rotation * rm.bbox.Center());*/

	//	rm.Tr = wholeTransformMatrix * rm.Tr;
	//	//***********************
	//	tri::UpdatePosition<TriMesh>::Matrix(rm, wholeTransformMatrix, true);
	//	tri::UpdateBounding<TriMesh>::Box(rm);

	//	//***20150827_comment rm.Tr.SetIdentity();

	//	//rm.UpdateBoxAndNormals();
	//	return true;
	//}


	template<typename scalarType>//依高度排序的方法
	vector<PAIR> sortAxis(MeshDocument &md, scalarType gap,int axis)
	{
		//============
		//***排序mesh的
		vector<PAIR> mesh_axis_coordinate;// (volumn_map.begin(), volumn_map.end());
		switch (axis)
		{
		case GPF_X_coordinate:
		{
								 foreach(int mpid, md.multiSelectID)
								 {
									 MeshModel *mm = md.getMesh(mpid);
									 mesh_axis_coordinate.push_back(std::pair<int, scalarType>(mpid, mm->cm.bbox.min.X()));
								 }

								 qSort(mesh_axis_coordinate.begin(), mesh_axis_coordinate.end(), CmpByValue());
		}
			break;
		case GPF_Y_coordinate:
		{
								 foreach(int mpid, md.multiSelectID)
								 {
									 MeshModel *mm = md.getMesh(mpid);
									 mesh_axis_coordinate.push_back(std::pair<int, scalarType>(mpid, mm->cm.bbox.max.Y()));
								 }

								 qSort(mesh_axis_coordinate.begin(), mesh_axis_coordinate.end(), CmpByValue());

		}
			break;
		case GPF_Z_coordinate:
		{
								 foreach(int mpid, md.multiSelectID)
								 {
									 MeshModel *mm = md.getMesh(mpid);
									 mesh_axis_coordinate.push_back(std::pair<int, scalarType>(mpid, mm->cm.bbox.min.Z()));
								 }

								 qSort(mesh_axis_coordinate.begin(), mesh_axis_coordinate.end(), CmpByValue());
		}
			break;
		}	
		
		return mesh_axis_coordinate;

		/*Point3m translatePath;
		bool xx = true;
		for (int i = 0; i < mesh_z_coordinate.size(); i++)
		{
			qDebug() << "mesh_z_coordinate[i].second" << mesh_z_coordinate[i].second;
			MeshModel *dm = md.getMesh(mesh_z_coordinate[i].first);
			translatePath.SetZero();
			translatePath.Z() = md.groove.min.Z() - dm->cm.bbox.min.Z();
			xyz::recurcisve_mesh_box_height<scalarType>(mesh_z_coordinate, *dm, md, gap, true, translatePath);

		}*/
		//==================
		//**************************************************************************
		/*Point3m translatePath;
		bool xx = true;
		foreach(MeshModel *am, md.meshList)
		{
		translatePath.SetZero();
		translatePath.Z() = md.groove.min.Z() - am->cm.bbox.min.Z();
		xyz::recurcisve_mesh_box_test<scalarType>(*am, md, gap, xx, translatePath);

		}*/

	}





}