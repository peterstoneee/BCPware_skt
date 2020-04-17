#ifndef __VCGLIB_THREEDPACKING_TRI_MESH
#define __VCGLIB_THREEDPACKING_TRI_MESH

#include<vcg/complex/complex.h>
#include <common/interfaces.h>
#include <algorithm>
#include <map>
#include <vector>
using namespace vcg;
//***20150423_pair 比較函式	

typedef pair<int, float> PAIR;

struct CmpByValue {
	bool operator()(const PAIR& lhs, const PAIR& rhs) {
		return lhs.second < rhs.second;
	}
};
//********
namespace xyz{
	
	template<typename TriMesh, typename scalarType >
	bool mesh_rotate(TriMesh &rm,scalarType degree, const Point3<scalarType> axis )
	{
		Matrix44m translate_to_origin, translate_to_box_center;//***先回原點,再轉回原來位置
		Matrix44m rotate_m;//***20150420旋轉矩陣	

		Box3m *m_box(&rm.bbox);

		int mindim = m_box->MinDim();	

		translate_to_origin.SetTranslate(-rm.bbox.Center());//***先回原點
		rotate_m.SetRotateDeg(degree, axis);//***旋轉完
		translate_to_box_center.SetTranslate(rm.bbox.Center());//***再回原位置
		rm.Tr = translate_to_box_center*rotate_m*translate_to_origin;
		
		tri::UpdatePosition<TriMesh>::Matrix(rm, rm.Tr, true);
		tri::UpdateBounding<TriMesh>::Box(rm);
		rm.Tr.SetIdentity();
		//rm.UpdateBoxAndNormals();
		return true;
	}
	template<typename TriMesh, typename scalarType >
	bool mesh_translate(TriMesh &rm, const Point3<scalarType> path)
	{
		Matrix44m tranlate_to;
		tranlate_to.SetTranslate(path);
		tri::UpdatePosition<TriMesh>::Matrix(rm, tranlate_to, true);
		tri::UpdateBounding<TriMesh>::Box(rm);
		return true; 
	}
	
	
	template<typename scalarType>
	void box_packing_algrithm(Box3<scalarType>& sinkBox,QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md)
	{		
		typename QMap<int, scalarType>::Iterator it;
		it = max_element(surfaceArea.begin(), surfaceArea.end());//找出meshdoc中最大表面積
		MeshModel &pack = *md.getMesh(it.key());//***得到mesh

		int mindim = pack.cm.bbox.MinDim();
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

		switch (mindim)//***轉至高度最低
		{
		case 0:
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_zaxis);
			break;
		case 1:
			break;
		case 2:
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_xaxis);
			break;
		}
		//*****************
		//Box3m sinkBBox;//***建構槽
		//sinkBBox.min = vcg::Point3f(-15 / 2., -20 / 2., -30 / 2.);
		//sinkBBox.max = vcg::Point3f(15 / 2., 20 / 2., 30 / 2.);

		int maxdim = pack.cm.bbox.MaxDim();
		switch (maxdim)//***最長邊轉至與sinkbox最長邊平行
		{
		case 0:
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_yaxis);
			break;
		case 1:
			break;
		case 2:
			break;
		}

		mesh_translate(pack.cm, sinkBox.min - pack.cm.bbox.min);//***移至建構槽.min
		it = surfaceArea.find(pack.id());//***刪除物件，更新map
		surfaceArea.erase(it);
		it = volumn_map.find(pack.id());
		volumn_map.erase(it);
		if (surfaceArea.count() == 0)return;
		/*else
		{
			height_assumption += pack.cm.bbox.DimY();
			sinkBox.min.Y() += pack.cm.bbox.DimY();
			box_packing_algrithm(height_assumption, sinkBox, surfaceArea, volumn_map, md);
		}*/
		 
		//sinkBox.min.Y() += pack.cm.bbox.DimY();
		qDebug() << "pack.box.dimy"<<pack.cm.bbox.DimY();
		box_packing_layer(sinkBox, pack.cm.bbox.DimY(), pack, sinkBox, surfaceArea, volumn_map, md);

	}

	//***20150424
	template<typename scalarType>
	void box_packing_layer(Box3<scalarType> current_box, scalarType currentheight, MeshModel& pack_1, Box3<scalarType>& sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md)
	{
		typename QMap<int, scalarType>::Iterator it;
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸
		qDebug() << "current_box" << current_box.Volume();
		//***space1***要如何改
		Point3<scalarType> space1_min(current_box.min.X() + pack_1.cm.bbox.DimX(), current_box.min.Y(), pack_1.cm.bbox.min.Z());
		Point3<scalarType> space1_max(current_box.max.X(), current_box.min.Y() + currentheight, current_box.max.Z());
		Box3<scalarType> space_1(space1_min,space1_max);
		//***space2
		Point3<scalarType> space2_min(pack_1.cm.bbox.min.X(), current_box.min.Y(), current_box.min.Z() + pack_1.cm.bbox.DimZ());
		Point3<scalarType> space2_max(current_box.max.X(), current_box.min.Y() + currentheight, current_box.max.Z());
		Box3<scalarType> space_2(space2_min, space2_max);

		Box3<scalarType> choose_space;
		qDebug() << "place5" << space_1.Volume() << space_2.Volume();

		if (space_1.Volume() > space_2.Volume())choose_space=(space_1);
		else choose_space=(space_2);
		//choose_space = (space_2);


		/*vector<std::pair<int, float>> get_volumn;
		copy(test.begin(), test.end(), back_inserter(get_volumn));*/

		//***排序mesh的體積	
		vector<PAIR> get_volumn;// (volumn_map.begin(), volumn_map.end());
		
		for (it = volumn_map.begin(); it != volumn_map.end(); ++it)
			get_volumn.push_back(std::pair<int, scalarType>(it.key(), it.value()));
		qDebug() << "get_volumn.size" << get_volumn.size();
		qSort(get_volumn.begin(), get_volumn.end(), CmpByValue());
		reverse(get_volumn.begin(), get_volumn.end());

		/*for (int i = 0; i != get_volumn.size(); ++i)
			qDebug() << "get_volumn[i]" << get_volumn[i].second;*/


		int temp_id=-1;
		MeshModel *pack_next;
		for (int i = 0; i != get_volumn.size(); ++i)
		{
			if (get_volumn[i].second < choose_space.Volume())
			{
				qDebug() << "get_volumn[i]" << get_volumn[i].second;
				pack_next = md.getMesh(get_volumn[i].first);
				
				qDebug() << "place3" << pack_next->cm.bbox.DimX();
				qDebug() << "place3" << pack_next->cm.bbox.DimY();
				qDebug() << "place3" << pack_next->cm.bbox.DimZ();
				qDebug() << "place3" << choose_space.DimValue(choose_space.MaxDim());
				qDebug() << "place3" << choose_space.DimValue(choose_space.MinDim());
				qDebug() << "place3" << choose_space.DimValue(choose_space.MidDim());
				//***判斷物體長寬高是否超出choose_space 大小
				if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) > choose_space.DimValue(choose_space.MaxDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) > choose_space.DimValue(choose_space.MinDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) > choose_space.DimValue(choose_space.MidDim()))continue;
				else {					
					temp_id = get_volumn[i].first; 
					qDebug() << "place4" << temp_id;
					break;
				}
			}
		}
		if (temp_id == -1)
		{
			qDebug() <<"temp_id="<< temp_id; 
			sinkBox.min.Y() += currentheight;
			box_packing_algrithm(sinkBox, surfaceArea, volumn_map, md);
			return;
		}
		else
		{
			pack_next = md.getMesh(temp_id);

			switch (choose_space.MaxDim())//***最長邊轉至與空間中最長邊平行
			{
			case 0:
				if (pack_next->cm.bbox.MaxDim() == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_zaxis);
				if (pack_next->cm.bbox.MaxDim() == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_yaxis);
				break;
			case 1:
				if (pack_next->cm.bbox.MaxDim() == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_zaxis);
				if (pack_next->cm.bbox.MaxDim() == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_xaxis);
				break;
			case 2:
				if (pack_next->cm.bbox.MaxDim() == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_yaxis);
				if (pack_next->cm.bbox.MaxDim() == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_xaxis);
				break;
			}

			switch (choose_space.MinDim())
			{
			case 0:
				if (pack_next->cm.bbox.MinDim() == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_zaxis);
				if (pack_next->cm.bbox.MinDim() == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_yaxis);
				break;
			case 1:
				if (pack_next->cm.bbox.MinDim() == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_zaxis);
				if (pack_next->cm.bbox.MinDim() == 2)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_xaxis);
				break;
			case 2:
				if (pack_next->cm.bbox.MinDim() == 0)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_yaxis);
				if (pack_next->cm.bbox.MinDim() == 1)xyz::mesh_rotate<CMeshO, Scalarm>(pack_next->cm, 90.0, rotate_xaxis);
				break;
			}
			xyz::mesh_translate(pack_next->cm, choose_space.min - pack_next->cm.bbox.min);//***移至choose_space.min
			
			
			it = surfaceArea.find(pack_next->id());
			surfaceArea.erase(it);
			it = volumn_map.find(pack_next->id());
			volumn_map.erase(it);
						
			if (surfaceArea.count() != 0)box_packing_layer(choose_space, currentheight, *pack_next, sinkBox, surfaceArea, volumn_map, md);
			else return;
		}

	}



	//***20150422
	template<typename scalarType>
	void box_pack_algrithm1(scalarType &xK, scalarType &zK, scalarType &yK, QMap<int, Box3<scalarType>>& remainMesh, Box3<scalarType> pack)
	{
		
		if (remainMesh.count == 0)return;
		xk -= pack.DimX();
		zk -= pack.DimZ();
		
		scalarType volumn = xk*yk*zk;
		//***
		//***1:兩個體積，用最大xk*zK的表面積判斷取哪一個, qmap要再改
		//***1.1:判斷剩餘box有無最接近適合體積，找出小於xk*zk*yk (v0)的最大box，判斷剩餘box三邊是否都小於v0的三邊
		//***1.1.1:如果沒有適合體積，找最大表面積，轉至最小高度，位移至角若，往上疊。

	}


}
#endif