#ifndef __VCGLIB_THREEDPACKING_TRI_MESH
#define __VCGLIB_THREEDPACKING_TRI_MESH

#include<vcg/complex/complex.h>
#include <common/interfaces.h>
#include <algorithm>
#include <map>
#include <vector>
#include <iomanip>
#include <iostream>
using namespace vcg;
//***20150423_pair 比較函式	

typedef pair<int, float> PAIR;

//struct CmpByValue {
//	bool operator()(const PAIR& lhs, const PAIR& rhs) {
//		return lhs.second < rhs.second;//bak
//
//	}
//};
struct CmpByValueLess {
	bool operator()(const PAIR& lhs, const PAIR& rhs) {
		return lhs.second < rhs.second;//bak		
	}
};
struct CmpByValueLarger {
	bool operator()(const PAIR& lhs, const PAIR& rhs) {
		return lhs.second > rhs.second;
	}
};
//********
namespace xyz{

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

#if 0
	template<typename scalarType>
	void box_packing_algrithm_right_version(Box3<scalarType>& sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md)
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
		case 0://***mindim為x軸
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_yaxis);
			break;
		case 1://***mindim為y軸
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_xaxis);
			break;
		case 2://***mindim為z軸

			break;
		}
		//*****************
		//Box3m sinkBBox;//***建構槽
		//sinkBBox.min = vcg::Point3f(-15 / 2., -20 / 2., -30 / 2.);
		//sinkBBox.max = vcg::Point3f(15 / 2., 20 / 2., 30 / 2.);

		int maxdim = pack.cm.bbox.MaxDim();
		switch (maxdim)//***最長邊轉至與sinkbox底部面積最長邊平行
		{
		case 0:
			mesh_rotate<CMeshO, Scalarm>(pack.cm, 90.0, rotate_zaxis);
			break;
		case 1:
			break;
		case 2:
			break;
		}
		//***20150615***四個角為0123
		mesh_translate(pack.cm, sinkBox.P(3) - pack.cm.bbox.P(3));//***移至建構槽
		it = surfaceArea.find(pack.id());//***刪除物件，更新surface map
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
		//qDebug() << "pack.box.dimy"<<pack.cm.bbox.DimY();
		box_packing_layer(sinkBox, pack.cm.bbox.DimZ(), pack, sinkBox, surfaceArea, volumn_map, md);
	}
#endif
#if 0
	//***20150424
	template<typename scalarType>
	void box_packing_layer_right_version(Box3<scalarType> current_box, scalarType currentheight, MeshModel& pack_1, Box3<scalarType>& sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md)
	{
		typename QMap<int, scalarType>::Iterator it;
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

		scalarType gap = 0.2;//***20150813設置物體與物體間的間隙

		//qDebug() << "current_box" << current_box.Volume();
		//***space1***要如何改
		Point3<scalarType> space1_min(current_box.min.X(), current_box.min.Y(), pack_1.cm.bbox.min.Z());
		Point3<scalarType> space1_max(pack_1.cm.bbox.min.X() - gap, current_box.max.Y(), current_box.min.Z() + currentheight);
		Box3<scalarType> space_1(space1_min, space1_max);
		//***space2
		Point3<scalarType> space2_min(pack_1.cm.bbox.min.X(), current_box.min.Y(), current_box.min.Z());
		Point3<scalarType> space2_max(current_box.max.X(), pack_1.cm.bbox.min.Y() - gap, current_box.min.Z() + currentheight);
		Box3<scalarType> space_2(space2_min, space2_max);

		Box3<scalarType> choose_space;
		//qDebug() << "place5" << space_1.Volume() << space_2.Volume();

		if (space_1.Volume() > space_2.Volume())choose_space = (space_1);
		else choose_space = (space_2);


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


		int temp_id = -1;
		MeshModel *pack_next;
		for (int i = 0; i != get_volumn.size(); ++i)
		{
			if (get_volumn[i].second < choose_space.Volume())
			{
				//qDebug() << "get_volumn[i]" << get_volumn[i].second;
				pack_next = md.getMesh(get_volumn[i].first);

				/*qDebug() << "place3" << pack_next->cm.bbox.DimX();
				qDebug() << "place3" << pack_next->cm.bbox.DimY();
				qDebug() << "place3" << pack_next->cm.bbox.DimZ();
				qDebug() << "place3" << choose_space.DimValue(choose_space.MaxDim());
				qDebug() << "place3" << choose_space.DimValue(choose_space.MinDim());
				qDebug() << "place3" << choose_space.DimValue(choose_space.MidDim());*/
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
			qDebug() << "temp_id=" << temp_id;
			sinkBox.min.Z() += currentheight + gap;
			box_packing_algrithm(sinkBox, surfaceArea, volumn_map, md);
			return;
		}
		else
		{
			pack_next = md.getMesh(temp_id);

			switch (choose_space.MaxDim())//***最長邊轉至與空間中最長邊平行
				//switch (1)//***最長邊轉至與空間中y軸邊平行
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
			xyz::mesh_translate(pack_next->cm, choose_space.P(3) - pack_next->cm.bbox.P(3));//***移至choose_space.min


			it = surfaceArea.find(pack_next->id());
			surfaceArea.erase(it);
			it = volumn_map.find(pack_next->id());
			volumn_map.erase(it);

			if (surfaceArea.count() != 0)box_packing_layer(choose_space, currentheight, *pack_next, sinkBox, surfaceArea, volumn_map, md);
			else return;
		}

	}
#endif

	template<typename scalarType>
	void LAFF_First_Placement(Box3<scalarType> container, Box3<scalarType> original_container, QMap<int, scalarType> &boxesSurfaceArea, QMap<int, scalarType> &boxesVolume, MeshDocument &md, float bt_gap, bool auto_rotate, float z_offset = 0, bool isFirstPlacement = true)
	{
		MeshModel *pack;
		Point3m rotate_xaxis(1.0, 0.0, 0.0);
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);

		if (auto_rotate) // Rotate the mesh so that MinDim = DimZ and MaxDim = DimY
		{
			foreach(MeshModel *rm, md.meshList)
			{
				int mindim = rm->cm.bbox.MinDim();
				float checkMeshX = rm->cm.bbox.DimX();
				float checkMeshY = rm->cm.bbox.DimY();

				switch (mindim)
				{
				case 0:
				{
						  if (checkMeshX == checkMeshY)
							  break;
						  else
						  {
							  xyz::mesh_rotate<CMeshO, Scalarm>(rm->cm, 90.0, rotate_yaxis);
							  break;
						  }
				}
				case 1:
				{
						  if (checkMeshX == checkMeshY)
							  break;
						  else
						  {
							  xyz::mesh_rotate<CMeshO, Scalarm>(rm->cm, 90.0, rotate_xaxis);
							  break;
						  }
				}
				default: break;
				}

				int maxdim = rm->cm.bbox.MaxDim();
				float reCheckMeshX = rm->cm.bbox.DimX();
				float reCheckMeshZ = rm->cm.bbox.DimZ();

				switch (maxdim)
				{
				case 0:
				{
						  if (reCheckMeshX == reCheckMeshZ)
							  break;
						  else
						  {
							  xyz::mesh_rotate<CMeshO, Scalarm>(rm->cm, 90.0, rotate_zaxis);
							  break;
						  }
				}
				case 2:
				{
						  if (reCheckMeshX == reCheckMeshZ)
							  break;
						  else
						  {
							  xyz::mesh_rotate<CMeshO, Scalarm>(rm->cm, 90.0, rotate_xaxis);
							  break;
						  }
				}
				}
			}
		}

		// This will get the mesh that has the largest total surface area
		typename QMap<int, scalarType>::Iterator it;
		it = max_element(boxesSurfaceArea.begin(), boxesSurfaceArea.end());

		// Check if there are meshes that have the same value with the largest total surface area
		int num_of_maxSA = 0;
		QMap<int, float> mesh_with_same_SA;
		typename QMap<int, scalarType>::Iterator iter;
		for (iter = boxesSurfaceArea.begin(); iter != boxesSurfaceArea.end(); iter++)
		{
			float iter_Key = md.getMesh(iter.key())->cm.bbox.Total_SurfaceArea();
			float it_Key = md.getMesh(it.key())->cm.bbox.Total_SurfaceArea();

			float iter_RKey = round(iter_Key * 10) / 10.0;
			float it_RKey = round(it_Key * 10) / 10.0;

			//if (md.getMesh(iter.key())->cm.bbox.Total_SurfaceArea() == md.getMesh(it.key())->cm.bbox.Total_SurfaceArea())
			if (iter_RKey == it_RKey)
			{
				mesh_with_same_SA.insert(iter.key(), md.getMesh(iter.key())->cm.bbox.DimZ()); // Get the height of meshes
				num_of_maxSA++;
				qDebug() << "num_of_maxSA: " << num_of_maxSA;
			}
		}

		// For multiple meshes (with largest total surface area), choose the mesh with the minimum height
		if (num_of_maxSA > 1)
		{
			QMap<int, float>::iterator minH_it;
			minH_it = min_element(mesh_with_same_SA.begin(), mesh_with_same_SA.end());
			pack = md.getMesh(minH_it.key());
			qDebug() << "height of the mesh: " << pack->cm.bbox.DimZ();
		}
		else
		{
			pack = md.getMesh(it.key());
		}

		mesh_translate(pack->cm, container.P(2) - pack->cm.bbox.P(2));

		// Determine the height of the container and decrement the number of meshes
		z_offset = pack->cm.bbox.DimZ();
		it = boxesSurfaceArea.find(pack->id());
		boxesSurfaceArea.erase(it);
		it = boxesVolume.find(pack->id());
		boxesVolume.erase(it);

		if (boxesSurfaceArea.isEmpty())
			return;
		else
			LAFF_Second_Placement_WidthSpace(container, original_container, boxesSurfaceArea, boxesVolume, md, *pack, bt_gap, z_offset, isFirstPlacement, pack->id());
	}

	template<typename scalarType>
	void LAFF_Second_Placement_WidthSpace(Box3<scalarType> container, Box3<scalarType> original_container, QMap<int, scalarType> &boxesSurfaceArea, QMap<int, scalarType> &boxesVolume, MeshDocument &md, MeshModel &mm, float bt_gap, float z_offset, bool isFirstPlacement, int firstPlacementMeshID)
	{
		isFirstPlacement = false;

		// available space below the last placed mesh
		Point3<scalarType> min_widthSpace(container.min.X(), container.min.Y(), mm.cm.bbox.min.Z());
		Point3<scalarType> max_widthSpace(container.max.X(), mm.cm.bbox.min.Y() - bt_gap, container.min.Z() + z_offset);
		Box3<scalarType> widthSpace(min_widthSpace, max_widthSpace);

		// sort the meshes from largest volume to smallest volume to prioritize largest volume in packing
		vector<PAIR> sorted_mesh_volume;
		typename QMap<int, scalarType>::Iterator it;
		for (it = boxesVolume.begin(); it != boxesVolume.end(); it++)
		{
			sorted_mesh_volume.push_back(std::pair<int, scalarType>(it.key(), it.value()));
		}
		qSort(sorted_mesh_volume.begin(), sorted_mesh_volume.end(), CmpByValueLarger());

		MeshModel *pack;
		int temp_id = -1;
		for (int i = 0; i < sorted_mesh_volume.size(); i++)
		{
			if (sorted_mesh_volume[i].second < widthSpace.Volume())
			{
				pack = md.getMesh(sorted_mesh_volume[i].first);
				if (pack->cm.bbox.DimX() > widthSpace.DimX() || pack->cm.bbox.DimY() > widthSpace.DimY())
					continue;
				else
				{
					temp_id = pack->id();
					break;
				}
			}
		}

		if (temp_id == -1)
		{
			LAFF_Second_Placement_DepthSpace(original_container, original_container, boxesSurfaceArea, boxesVolume, md, *md.getMesh(firstPlacementMeshID), bt_gap, z_offset, isFirstPlacement, firstPlacementMeshID);
		}

		if (boxesSurfaceArea.isEmpty())
			return;

		pack = md.getMesh(temp_id);
		mesh_translate(pack->cm, widthSpace.P(2) - pack->cm.bbox.P(2));
		it = boxesSurfaceArea.find(pack->id());
		boxesSurfaceArea.erase(it);
		it = boxesVolume.find(pack->id());
		boxesVolume.erase(it);

		if (pack->cm.bbox.DimZ() > z_offset) z_offset = pack->cm.bbox.DimZ();

		if (boxesSurfaceArea.isEmpty())
			return;
		else
			LAFF_Second_Placement_WidthSpace(widthSpace, original_container, boxesSurfaceArea, boxesVolume, md, *pack, bt_gap, z_offset, isFirstPlacement, firstPlacementMeshID);
	}

	template<typename scalarType>
	void LAFF_Second_Placement_DepthSpace(Box3<scalarType> container, Box3<scalarType> original_container, QMap<int, scalarType> &boxesSurfaceArea, QMap<int, scalarType> &boxesVolume, MeshDocument &md, MeshModel &mm, float bt_gap, float z_offset, bool isFirstPlacement, int firstPlacementMeshID)
	{
		// available space at the right of the last placed mesh
		Point3<scalarType> min_depthSpace(mm.cm.bbox.max.X() + bt_gap, container.min.Y(), mm.cm.bbox.min.Z());
		Point3<scalarType> max_depthSpace(container.max.X(), container.max.Y(), container.min.Z() + z_offset);
		Box3<scalarType> depthSpace(min_depthSpace, max_depthSpace);

		// sort the meshes from largest volume to smallest volume to prioritize largest volume in packing
		vector<PAIR> sorted_mesh_volume;
		typename QMap<int, scalarType>::Iterator it;
		for (it = boxesVolume.begin(); it != boxesVolume.end(); it++)
		{
			sorted_mesh_volume.push_back(std::pair<int, scalarType>(it.key(), it.value()));
		}
		qSort(sorted_mesh_volume.begin(), sorted_mesh_volume.end(), CmpByValueLarger());

		MeshModel *pack;
		int temp_id = -1;
		for (int i = 0; i < sorted_mesh_volume.size(); i++)
		{
			if (sorted_mesh_volume[i].second < depthSpace.Volume())
			{
				pack = md.getMesh(sorted_mesh_volume[i].first);
				if (pack->cm.bbox.DimX() > depthSpace.DimX() || pack->cm.bbox.DimY() > depthSpace.DimY())
					continue;
				else
				{
					temp_id = pack->id();
					break;
				}
			}
		}

		if (temp_id == -1)
		{
			bool auto_rotate = false;
			isFirstPlacement = true;
			original_container.min.Z() += z_offset + bt_gap;
			LAFF_First_Placement(original_container, original_container, boxesSurfaceArea, boxesVolume, md, bt_gap, auto_rotate, z_offset, isFirstPlacement);
		}

		if (boxesSurfaceArea.isEmpty())
			return;

		pack = md.getMesh(temp_id);
		mesh_translate(pack->cm, depthSpace.P(2) - pack->cm.bbox.P(2));
		it = boxesSurfaceArea.find(pack->id());
		boxesSurfaceArea.erase(it);
		it = boxesVolume.find(pack->id());
		boxesVolume.erase(it);

		if (pack->cm.bbox.DimZ() > z_offset) z_offset = pack->cm.bbox.DimZ();

		if (boxesSurfaceArea.isEmpty())
			return;
		else
			LAFF_Second_Placement_WidthSpace(depthSpace, original_container, boxesSurfaceArea, boxesVolume, md, *pack, bt_gap, z_offset, isFirstPlacement, pack->id());
	}

	template<typename scalarType>
	void box_packing_algrithm(Box3<scalarType> sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md, Box3<scalarType> original_box, scalarType bt_gap, bool auto_rotate_switch, scalarType z_offset = 0, int getSuitableMesh = -1)
	{
		typename QMap<int, scalarType>::Iterator it;
		//
		MeshModel *pack;
		//***20150918**增加選到符合大小的的mesh判斷
		if (getSuitableMesh > 0)
		{
			pack = md.getMesh(getSuitableMesh);//***得到mesh
		}
		else
		{
			it = max_element(surfaceArea.begin(), surfaceArea.end());//找出meshdoc中最大表面積
			pack = md.getMesh(it.key());//***得到mesh
		}
		//MeshModel &pack = *md.getMesh(it.key());//***得到mesh


		//*******20150821**************************************************		
		//typename QMap<int, scalarType>::Iterator it2 = surfaceArea.begin();
		//scalarType maxArea;
		////it2 = surfaceArea.begin();
		//maxArea = it2.value();
		//int tempid = it2.key();
		//while (it2!=surfaceArea.end())
		//{			
		//	//if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) > choose_space.DimValue(choose_space.MaxDim()))continue;
		//	if (it2.value() > maxArea  )
		//	{
		//		MeshModel &pack = *md.getMesh(it2.key());
		//		if (pack.cm.bbox.DimValue(pack.cm.bbox.MaxDim())>sinkBox.DimValue(sinkBox.DimY()))
		//		{		
		//			++it2;
		//			continue;
		//		}
		//		else
		//		{
		//			maxArea = it2.value();
		//			tempid = it2.key();
		//		}
		//	}
		//	++it2;
		//}		
		//MeshModel &pack = *md.getMesh(tempid);

		//******************************************************
		if (auto_rotate_switch)
		{
			int mindim = pack->cm.bbox.MinDim();
			Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
			Point3m rotate_yaxis(0.0, 1.0, 0.0);
			Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

			switch (mindim)//***轉至高度最低 disable auto_rotate
			{
			case 0://***mindim為x軸
				mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_yaxis);
				break;
			case 1://***mindim為y軸
				mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_xaxis);
				break;
			case 2://***mindim為z軸
				break;
			}
			//*****************
			//Box3m sinkBBox;//***建構槽
			//sinkBBox.min = vcg::Point3f(-15 / 2., -20 / 2., -30 / 2.);
			//sinkBBox.max = vcg::Point3f(15 / 2., 20 / 2., 30 / 2.);

			//int maxdim = pack.cm.bbox.MaxDim();
			int maxdim;//
			if (sinkBox.DimX() >= sinkBox.DimY())maxdim = 0;
			else maxdim = 1;
			qDebug() << "pack.cm.bbox.MaxDim()" << maxdim;
			switch (maxdim)//***最長邊轉至與sinkbox底部面積最長邊平行
			{
			case 0:
			{
					  if (pack->cm.bbox.MaxDim() == 1)mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_zaxis);

			}
				break;
			case 1:
			{
					  if (pack->cm.bbox.MaxDim() == 0)mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_zaxis);
			}
				break;
			case 2:
				break;
			}
		}
		//***20150615***四個角為0123
		mesh_translate(pack->cm, sinkBox.P(3) - pack->cm.bbox.P(3));//***移至建構槽
		it = surfaceArea.find(pack->id());//***刪除物件，更新surface map
		surfaceArea.erase(it);
		it = volumn_map.find(pack->id());
		volumn_map.erase(it);
		if (surfaceArea.count() == 0)return;

		/*else
		{
		height_assumption += pack.cm.bbox.DimY();
		sinkBox.min.Y() += pack.cm.bbox.DimY();
		box_packing_algrithm(height_assumption, sinkBox, surfaceArea, volumn_map, md);
		}*/

		//sinkBox.min.Y() += pack.cm.bbox.DimY();
		//qDebug() << "pack.box.dimy"<<pack.cm.bbox.DimY();

		//***20150819
		/*Box3<scalarType> seperate_2;
		qDebug() << "sinkBox.cm.bbox.DimY()" << sinkBox.DimY();

		Box3<scalarType> seperate_1 = sinkBox.Seperate(sinkBox, seperate_2, 0, pack.cm.bbox.DimY(), 0);*/

		//***seperate_1為分割的box
		//***seperate_2為原box切完剩下的部分
		pack->cm.bbox.DimZ();
		if (pack->cm.bbox.DimZ() > z_offset)z_offset = pack->cm.bbox.DimZ();


		box_packing_layer(sinkBox, z_offset, *pack, sinkBox, surfaceArea, volumn_map, md, pack->cm.bbox.DimY(), original_box, bt_gap, auto_rotate_switch);

	}



	//***20150818***修改packing演算法
	template<typename scalarType>
	void box_packing_layer(Box3<scalarType> current_box, scalarType currentheight, MeshModel& pack_1, Box3<scalarType>& sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md, float cut_offset, Box3<scalarType> sinkBackup, scalarType bt_gap, bool auto_rotate_switch, scalarType floor_z_offset = 0)
	{
		typename QMap<int, scalarType>::Iterator it;
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

		scalarType gap = bt_gap;//***20150813設置物體與物體間的間隙

		//qDebug() << "current_box" << current_box.Volume();
		//***space1***要如何改
		Point3<scalarType> space1_min(current_box.min.X(), current_box.min.Y(), pack_1.cm.bbox.min.Z());
		Point3<scalarType> space1_max(pack_1.cm.bbox.min.X() - gap, current_box.max.Y(), current_box.min.Z() + currentheight);
		Box3<scalarType> space_1(space1_min, space1_max);
		//***space2***
		/*Point3<scalarType> space2_min(pack_1.cm.bbox.min.X(), current_box.min.Y(), current_box.min.Z());
		Point3<scalarType> space2_max(current_box.max.X(), pack_1.cm.bbox.min.Y() - gap, current_box.min.Z() + currentheight);
		Box3<scalarType> space_2(space2_min, space2_max);*/

		Box3<scalarType> choose_space;
		//qDebug() << "place5" << space_1.Volume() << space_2.Volume();

		//if (space_1.Volume() > space_2.Volume())choose_space = (space_1);
		//else 
		choose_space = (space_1);


		/*vector<std::pair<int, float>> get_volumn;
		copy(test.begin(), test.end(), back_inserter(get_volumn));*/

		//***排序mesh的體積,Map to Vector	
		vector<PAIR> get_volumn;// (volumn_map.begin(), volumn_map.end());

		for (it = volumn_map.begin(); it != volumn_map.end(); ++it)
			get_volumn.push_back(std::pair<int, scalarType>(it.key(), it.value()));
		qDebug() << "get_volumn.size" << get_volumn.size();
		qSort(get_volumn.begin(), get_volumn.end(), CmpByValue());
		reverse(get_volumn.begin(), get_volumn.end());

		/*for (int i = 0; i != get_volumn.size(); ++i)
		qDebug() << "get_volumn[i]" << get_volumn[i].second;*/


		int temp_id = -1;
		MeshModel *pack_next;
		for (int i = 0; i != get_volumn.size(); ++i)
		{
			qDebug() << "get_volumn[i]" << get_volumn[i].second;
			if (get_volumn[i].second < choose_space.Volume() && auto_rotate_switch == false)//與剩餘box體積相比
			{
				//qDebug() << "get_volumn[i]" << get_volumn[i].second;
				pack_next = md.getMesh(get_volumn[i].first);

				if ((pack_next->cm.bbox.DimX() > choose_space.DimX()) || (pack_next->cm.bbox.DimY() > choose_space.DimY()))continue;
				else {
					temp_id = get_volumn[i].first;
					break;
				}
				//*****
			}
			else if (get_volumn[i].second < choose_space.Volume() && auto_rotate_switch == true)
			{
				pack_next = md.getMesh(get_volumn[i].first);
				//***判斷物體長寬高是否超出choose_space 大小
				qDebug() << "pack_next" << pack_next->cm.bbox.DimX() << pack_next->cm.bbox.DimY() << pack_next->cm.bbox.DimZ();
				qDebug() << pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) << choose_space.DimValue(choose_space.MinDim());
				qDebug() << "choose_space" << choose_space.DimX() << choose_space.DimY() << choose_space.DimZ();
				//(round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim())*100)/100)
				//(round(choose_space.DimValue(choose_space.MinDim()) * 100) / 100)

				if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) * 100) / 100) >(round(choose_space.DimValue(choose_space.MaxDim()) * 100) / 100))
					continue;
				else if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) * 100) / 100) > (round(choose_space.DimValue(choose_space.MinDim()) * 100) / 100))
					continue;
				else if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) * 100) / 100) > (round(choose_space.DimValue(choose_space.MidDim()) * 100) / 100))
					continue;
				else {
					temp_id = get_volumn[i].first;
					qDebug() << "place4" << temp_id;
					break;
				}



			}
		}

		if (currentheight > floor_z_offset)floor_z_offset = currentheight;

		if (temp_id == -1)//沒有符合剩餘體積的物體
		{
			Box3<scalarType> seperate_2;
			Box3<scalarType> seperate_1 = sinkBox.Seperate(sinkBox, seperate_2, 0, cut_offset + gap, 0);
			int seperate_2_xy_max, seperate_2_xy_min;//***找出seperate dimx, dimy哪個大
			if (seperate_2.DimX() >= seperate_2.DimY()){ seperate_2_xy_max = 0, seperate_2_xy_min = 1; }
			else { seperate_2_xy_max = 1; seperate_2_xy_min = 0; }

			int getMeshid = -1;
			for (int i = 0; i != get_volumn.size(); ++i)//重新搜尋符合的最大mesh。
			{
				pack_next = md.getMesh(get_volumn[i].first);
				if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimValue(seperate_2_xy_max))
					&& (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) < seperate_2.DimValue(seperate_2_xy_min)))
				{
					getMeshid = get_volumn[i].first;
					box_packing_algrithm(seperate_2, surfaceArea, volumn_map, md, sinkBackup, bt_gap, auto_rotate_switch, floor_z_offset, getMeshid);
					break;
				}

			}
			if (getMeshid == -1)
			{
				sinkBackup.min.Z() += floor_z_offset + gap;
				box_packing_algrithm(sinkBackup, surfaceArea, volumn_map, md, sinkBackup, bt_gap, auto_rotate_switch);//如果往上一層，則不傳高度，重新

			}

			//pack_next = md.getMesh(get_volumn[0].first);
			//if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimY()))			
			//if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimValue(seperate_2_xy_max))
			//	&& (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) < seperate_2.DimValue(seperate_2_xy_min))
			//	)
			//{
			//	qDebug() << "next row";
			//	box_packing_algrithm(seperate_2, surfaceArea, volumn_map, md, sinkBackup,floor_z_offset);
			//}
			//else
			//{
			//	sinkBackup.min.Z() += floor_z_offset + gap;
			//	box_packing_algrithm(sinkBackup, surfaceArea, volumn_map, md, sinkBackup);//如果往上一層，則不傳高度，重新
			//}			
		}
		else
		{
			pack_next = md.getMesh(temp_id);
			if (auto_rotate_switch)
			{
				switch (choose_space.MaxDim())//***最長邊轉至與空間中最長邊平行
					//switch (0)//***最長邊轉至與空間中y軸邊平行
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
					//switch (2)
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
			}
			else
			{
				/*if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) > choose_space.DimValue(choose_space.MaxDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) > choose_space.DimValue(choose_space.MinDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) > choose_space.DimValue(choose_space.MidDim()))continue;
				else {
				temp_id = get_volumn[i].first;
				qDebug() << "place4" << temp_id;
				break;
				}*/

			}
			Point3<scalarType> testpoint3 = choose_space.P(3) - pack_next->cm.bbox.P(3);
			qDebug() << "testpoint3" << testpoint3[0] << testpoint3[1] << testpoint3[2];
			xyz::mesh_translate(pack_next->cm, choose_space.P(3) - pack_next->cm.bbox.P(3));//***移至choose_space.min


			if (pack_next->cm.bbox.DimY() > cut_offset)cut_offset = pack_next->cm.bbox.DimY();//將最大Y與高往下傳
			if (pack_next->cm.bbox.DimZ() > currentheight)currentheight = pack_next->cm.bbox.DimZ();

			it = surfaceArea.find(pack_next->id());
			surfaceArea.erase(it);
			it = volumn_map.find(pack_next->id());
			volumn_map.erase(it);


			if (surfaceArea.count() != 0)box_packing_layer(choose_space, currentheight, *pack_next, sinkBox, surfaceArea, volumn_map, md, cut_offset, sinkBackup, bt_gap, auto_rotate_switch, floor_z_offset);
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

	//***20151006***one_mesh	
	template<typename scalarType>
	bool recurcisve_one_mesh_box_test(MeshModel &mmm, MeshDocument &md, scalarType gap, bool x, Point3<scalarType> movePosition = { 0, 0, 0 })
	{

		//FILE *dbgff;
		//dbgff = fopen("D:\\debug5.txt", "w");//@@@
		x = true;
		Matrix44m landToGround;
		landToGround.SetIdentity();
		Point3<scalarType> pp(0, 0, 0);
		if (movePosition != pp)
		{
			landToGround.SetTranslate(movePosition);
			mmm.cm.Tr = landToGround * mmm.cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		}
		landToGround.SetIdentity();
		Point3<scalarType> translatePath;
		translatePath.SetZero();

		foreach(MeshModel *bm, md.meshList)
		{
			QString myString = bm->fullName();
			const char* myChar = myString.toStdString().c_str();


			if (bm->getMeshSort() == MeshModel::meshsort::print_item)
			if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.Z() >= md.groove.min.Z())
			{

				translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
				landToGround.SetTranslate(translatePath);
				mmm.cm.Tr = landToGround * mmm.cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
				tri::UpdateBounding<CMeshO>::Box(mmm.cm);

				x = recurcisve_one_mesh_box_test<scalarType>(mmm, md, gap, x);
				break;
			}
		}
		//fprintf(dbgff, "e-1\n"); //@@@
		//fflush(dbgff);
		return x;
	}
	//****
	//***20160802***one_mesh	
	template<typename scalarType>
	bool recurcisve_one_mesh_box_axis(MeshModel &mmm, MeshDocument &md, scalarType gap, bool x, int align_axis, Point3<scalarType> movePosition = { 0, 0, 0 })
	{
		x = true;
		Matrix44m landToGround;
		landToGround.SetIdentity();
		Point3<scalarType> pp(0, 0, 0);
		if (movePosition != pp)
		{
			landToGround.SetTranslate(movePosition);
			mmm.cm.Tr = landToGround * mmm.cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		}
		landToGround.SetIdentity();
		Point3<scalarType> translatePath;
		translatePath.SetZero();

		foreach(MeshModel *bm, md.meshList)
		{
			QString myString = bm->fullName();
			const char* myChar = myString.toStdString().c_str();

			switch (align_axis)
			{
			case 0:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.X() >= md.groove.min.X())
					  {
						  translatePath.X() = bm->cm.bbox.max.X() - mmm.cm.bbox.min.X() + gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }
			}break;
			case 1:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.Y() >= md.groove.min.Y())
					  {
						  translatePath.Y() = bm->cm.bbox.max.Y() - mmm.cm.bbox.min.Y() + gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }
			}break;
			case 2:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.Z() >= md.groove.min.Z())
					  {
						  translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }

			}break;
			case 3:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && md.groove.max.X() >= bm->cm.bbox.max.X())
					  {
						  translatePath.X() = bm->cm.bbox.min.X() - mmm.cm.bbox.max.X() - gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }
			}break;
			case 4:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && md.groove.max.Y() >= bm->cm.bbox.max.Y())
					  {
						  translatePath.Y() = bm->cm.bbox.min.Y() - mmm.cm.bbox.max.Y() - gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }
			}break;
			case 5:
			{
					  if (bm->getMeshSort() == MeshModel::meshsort::print_item)
					  if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && md.groove.max.Z() >= bm->cm.bbox.max.Z())
					  {
						  translatePath.Z() = bm->cm.bbox.min.Z() - mmm.cm.bbox.max.Z() - gap;
						  landToGround.SetTranslate(translatePath);
						  mmm.cm.Tr = landToGround * mmm.cm.Tr;
						  tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
						  tri::UpdateBounding<CMeshO>::Box(mmm.cm);

						  x = recurcisve_one_mesh_box_axis<scalarType>(mmm, md, gap, x, align_axis);
						  break;
					  }

			}break;


			}

		}
		//fprintf(dbgff, "e-1\n"); //@@@
		//fflush(dbgff);
		return x;
	}
	//****

	template<typename scalarType>//依高度排序的方法
	void recursive_all_mesh_2(MeshDocument &md, scalarType gap)
	{
		//============
		//***排序mesh的
		vector<PAIR> mesh_z_coordinate;// (volumn_map.begin(), volumn_map.end());

		foreach(MeshModel *mm, md.meshList)
		{
			mesh_z_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.min.Z()));
		}

		qSort(mesh_z_coordinate.begin(), mesh_z_coordinate.end(), CmpByValue());
		//reverse(get_volumn.begin(), get_volumn.end());


		Point3m translatePath;
		bool xx = true;
		for (int i = 0; i < mesh_z_coordinate.size(); i++)
		{
			qDebug() << "mesh_z_coordinate[i].second" << mesh_z_coordinate[i].second;
			MeshModel *dm = md.getMesh(mesh_z_coordinate[i].first);
			translatePath.SetZero();
			translatePath.Z() = md.groove.min.Z() - dm->cm.bbox.min.Z();
			xyz::recurcisve_mesh_box_height<scalarType>(mesh_z_coordinate, *dm, md, gap, true, translatePath);

		}
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

	//***20151006***依據物體高度往上疊
	template<typename scalarType>
	bool recurcisve_mesh_box_height(vector<PAIR> &m_sortz, MeshModel &mmm, MeshDocument &md, scalarType gap, bool x, Point3<scalarType> movePosition = { 0, 0, 0 })
	{
		x = true;
		Matrix44m landToGround;
		landToGround.SetIdentity();
		Point3<scalarType> pp(0, 0, 0);
		if (movePosition != pp)
		{
			landToGround.SetTranslate(movePosition);
			mmm.cm.Tr = landToGround * mmm.cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		}
		landToGround.SetIdentity();
		Point3<scalarType> translatePath;
		translatePath.SetZero();

		MeshModel *bm;
		for (int i = 0; i < m_sortz.size(); i++)
		{
			bm = md.getMesh(m_sortz[i].first);
			if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) /*&& bm->cm.bbox.min.Z() >= md.groove.min.Z()*/
				&& mmm.cm.bbox.min.Z() >= bm->cm.bbox.min.Z())
			{
				translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
				landToGround.SetTranslate(translatePath);
				mmm.cm.Tr = landToGround * mmm.cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
				tri::UpdateBounding<CMeshO>::Box(mmm.cm);
				x = recurcisve_mesh_box_height<scalarType>(m_sortz, mmm, md, gap, x);
				break;
			}
		}
		return x;
	}


	template<typename scalarType>
	bool collide_all_test(MeshDocument &md)
	{
		foreach(MeshModel *em, md.meshList)
		{
			foreach(MeshModel *dm, md.meshList)
			{
				if (em->id() != dm->id() && em->cm.bbox.Collide(dm->cm.bbox)) return false;
			}
		}
		return true;
	}

	//***20151001***box往下降
	template<typename scalarType>
	bool recurcisve_mesh_box_test_old(MeshModel &mmm, MeshDocument &md, scalarType gap, bool x, Point3<scalarType> movePosition = { 0, 0, 0 })
	{
		x = true;
		Matrix44m landToGround;
		landToGround.SetIdentity();
		Point3<scalarType> pp(0, 0, 0);
		if (movePosition != pp)
		{
			landToGround.SetTranslate(movePosition);
			mmm.cm.Tr = landToGround * mmm.cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		}
		landToGround.SetIdentity();
		Point3<scalarType> translatePath;
		translatePath.SetZero();

		foreach(MeshModel *bm, md.meshList)
		{
			/*if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && mmm.cm.bbox.min.Z() < bm->cm.bbox.min.Z())
			{
			break;

			}else*/
			if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.Z() >= md.groove.min.Z()
				&& mmm.cm.bbox.min.Z() >= bm->cm.bbox.min.Z())
			{
				translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
				landToGround.SetTranslate(translatePath);
				mmm.cm.Tr = landToGround * mmm.cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
				tri::UpdateBounding<CMeshO>::Box(mmm.cm);
				x = recurcisve_mesh_box_test<scalarType>(mmm, md, gap, x);
				break;
			}
		}
		return x;
	}
	template<typename scalarType>
	void recursive_all_mesh_old(MeshDocument &md, scalarType gap)
	{
		Point3m translatePath;
		bool xx = true;
		foreach(MeshModel *am, md.meshList)
		{
			translatePath.SetZero();
			translatePath.Z() = md.groove.min.Z() - am->cm.bbox.min.Z();
			xyz::recurcisve_mesh_box_test<scalarType>(*am, md, gap, xx, translatePath);

		}

	}

	template<typename scalarType>//align x,y,z axis
	void recursive_align_axis(MeshDocument &md, scalarType gap, int align_axis)
	{
		//============
		//***排序mesh的
		vector<PAIR> align_axis_coordinate;// (volumn_map.begin(), volumn_map.end());

		foreach(MeshModel *mm, md.meshList)
		{
			switch (align_axis)
			{
			case 0:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.min.X()));
				break;
			case 1:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.min.Y()));
				break;
			case 2:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.min.Z()));
				break;
			case 3:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.max.X()));
				break;
			case 4:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.max.Y()));
				break;
			case 5:
				align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.max.Z()));
				break;
			}
			//align_axis_coordinate.push_back(std::pair<int, scalarType>(mm->id(), mm->cm.bbox.min.Y()));
		}
		if (align_axis == 0 || align_axis == 1 || align_axis == 2)
			qSort(align_axis_coordinate.begin(), align_axis_coordinate.end(), CmpByValueLess());
		else if (align_axis == 3 || align_axis == 4 || align_axis == 5)
			qSort(align_axis_coordinate.begin(), align_axis_coordinate.end(), CmpByValueLarger());
		//reverse(get_volumn.begin(), get_volumn.end());


		Point3m translatePath;
		bool xx = true;
		for (int i = 0; i < align_axis_coordinate.size(); i++)
		{
			//qDebug() << "mesh_z_coordinate[i].second" << mesh_z_coordinate[i].second;
			MeshModel *dm = md.getMesh(align_axis_coordinate[i].first);
			translatePath.SetZero();
			switch (align_axis)
			{
			case 0:
				translatePath = Point3m(md.groove.min.X() - dm->cm.bbox.min.X(), 0, 0); break;
				break;
			case 1:
				translatePath = Point3m(0, md.groove.min.Y() - dm->cm.bbox.min.Y(), 0); break;
				break;
			case 2:
				translatePath = Point3m(0, 0, md.groove.min.Z() - dm->cm.bbox.min.Z()); break;
				break;
			case 3:
				translatePath = Point3m(md.groove.max.X() - dm->cm.bbox.max.X(), 0, 0); break;
				break;
			case 4:
				translatePath = Point3m(0, md.groove.max.Y() - dm->cm.bbox.max.Y(), 0); break;
				break;
			case 5:
				translatePath = Point3m(0, 0, md.groove.max.Z() - dm->cm.bbox.max.Z()); break;
				break;
			}
			//translatePath.Z() = md.groove.min.Y() - dm->cm.bbox.min.Y();

			xyz::recurcisve_mesh_box_axis<scalarType>(align_axis_coordinate, *dm, md, gap, true, align_axis, translatePath);

		}


	}


	template<typename scalarType>
	bool recurcisve_mesh_box_axis(vector<PAIR> &m_sort_axis, MeshModel &mmm, MeshDocument &md, scalarType gap, bool x, int axis, Point3<scalarType> movePosition = { 0, 0, 0 })
	{
		x = true;
		Matrix44m landToGround;
		landToGround.SetIdentity();
		Point3<scalarType> pp(0, 0, 0);
		if (movePosition != pp)
		{
			landToGround.SetTranslate(movePosition);
			mmm.cm.Tr = landToGround * mmm.cm.Tr;
			tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
			tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		}
		landToGround.SetIdentity();
		Point3<scalarType> translatePath;
		translatePath.SetZero();


		MeshModel *bm;
		for (int i = 0; i < m_sort_axis.size(); i++)
		{
			bm = md.getMesh(m_sort_axis[i].first);
			//***
			float compare_position = 0;
			switch (axis)
			{
			case 0:
				compare_position = mmm.cm.bbox.min.X() - bm->cm.bbox.min.X();
				break;
			case 1:
				compare_position = mmm.cm.bbox.min.Y() - bm->cm.bbox.min.Y();
				break;;
			case 2:
				compare_position = mmm.cm.bbox.min.Z() - bm->cm.bbox.min.Z();
				break;
			case 3:
				compare_position = bm->cm.bbox.max.X() - mmm.cm.bbox.max.X();
				break;
			case 4:
				compare_position = bm->cm.bbox.max.Y() - mmm.cm.bbox.max.Y();
				break;
			case 5:
				compare_position = bm->cm.bbox.max.Z() - mmm.cm.bbox.max.Z();
				break;
			}//***
			if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) /*&& bm->cm.bbox.min.Z() >= md.groove.min.Z()*/
				&& compare_position >= 0)
			{
				switch (axis)
				{
				case 0:
					translatePath.X() = bm->cm.bbox.max.X() - mmm.cm.bbox.min.X() + gap;
					break;
				case 1:
					translatePath.Y() = bm->cm.bbox.max.Y() - mmm.cm.bbox.min.Y() + gap;
					break;
				case 2:
					translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
					break;
				case 3:
					translatePath.X() = bm->cm.bbox.min.X() - mmm.cm.bbox.max.X() - gap;
					break;
				case 4:
					translatePath.Y() = bm->cm.bbox.min.Y() - mmm.cm.bbox.max.Y() - gap;
					break;
				case 5:
					translatePath.Z() = bm->cm.bbox.min.Z() - mmm.cm.bbox.max.Z() - gap;
					break;
				}

				landToGround.SetTranslate(translatePath);
				mmm.cm.Tr = landToGround * mmm.cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
				tri::UpdateBounding<CMeshO>::Box(mmm.cm);
				x = recurcisve_mesh_box_axis<scalarType>(m_sort_axis, mmm, md, gap, x, axis);
				break;
			}
		}
		return x;
	}

	template<typename scalarType>
	void box_packing_algrithm_left(Box3<scalarType> sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md, Box3<scalarType> original_box, scalarType bt_gap, bool auto_rotate_switch, scalarType z_offset = 0, int getSuitableMesh = -1)
	{
		typename QMap<int, scalarType>::Iterator it;
		//
		MeshModel *pack;
		//***20150918**增加選到符合大小的的mesh判斷
		if (getSuitableMesh > 0)
		{
			pack = md.getMesh(getSuitableMesh);//***得到mesh
		}
		else
		{
			it = max_element(surfaceArea.begin(), surfaceArea.end());//找出meshdoc中最大表面積
			pack = md.getMesh(it.key());//***得到mesh
		}
		//MeshModel &pack = *md.getMesh(it.key());//***得到mesh


		//*******20150821**************************************************		
		//typename QMap<int, scalarType>::Iterator it2 = surfaceArea.begin();
		//scalarType maxArea;
		////it2 = surfaceArea.begin();
		//maxArea = it2.value();
		//int tempid = it2.key();
		//while (it2!=surfaceArea.end())
		//{			
		//	//if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) > choose_space.DimValue(choose_space.MaxDim()))continue;
		//	if (it2.value() > maxArea  )
		//	{
		//		MeshModel &pack = *md.getMesh(it2.key());
		//		if (pack.cm.bbox.DimValue(pack.cm.bbox.MaxDim())>sinkBox.DimValue(sinkBox.DimY()))
		//		{		
		//			++it2;
		//			continue;
		//		}
		//		else
		//		{
		//			maxArea = it2.value();
		//			tempid = it2.key();
		//		}
		//	}
		//	++it2;
		//}		
		//MeshModel &pack = *md.getMesh(tempid);

		//******************************************************
		if (auto_rotate_switch)
		{
			int mindim = pack->cm.bbox.MinDim();
			Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
			Point3m rotate_yaxis(0.0, 1.0, 0.0);
			Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

			switch (mindim)//***轉至高度最低 disable auto_rotate
			{
			case 0://***mindim為x軸
				mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_yaxis);
				break;
			case 1://***mindim為y軸
				mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_xaxis);
				break;
			case 2://***mindim為z軸
				break;
			}
			//*****************
			//Box3m sinkBBox;//***建構槽
			//sinkBBox.min = vcg::Point3f(-15 / 2., -20 / 2., -30 / 2.);
			//sinkBBox.max = vcg::Point3f(15 / 2., 20 / 2., 30 / 2.);

			//int maxdim = pack.cm.bbox.MaxDim();
			int maxdim;//
			if (sinkBox.DimX() >= sinkBox.DimY())maxdim = 0;
			else maxdim = 1;
			qDebug() << "pack.cm.bbox.MaxDim()" << maxdim;
			switch (maxdim)//***最長邊轉至與sinkbox底部面積最長邊平行
			{
			case 0:
			{
					  if (pack->cm.bbox.MaxDim() == 1)mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_zaxis);

			}
				break;
			case 1:
			{
					  if (pack->cm.bbox.MaxDim() == 0)mesh_rotate<CMeshO, Scalarm>(pack->cm, 90.0, rotate_zaxis);
			}
				break;
			case 2:
				break;
			}
		}
		//***20150615***四個角為0123
		mesh_translate(pack->cm, sinkBox.P(3) - pack->cm.bbox.P(3));//***移至建構槽
		it = surfaceArea.find(pack->id());//***刪除物件，更新surface map
		surfaceArea.erase(it);
		it = volumn_map.find(pack->id());
		volumn_map.erase(it);
		if (surfaceArea.count() == 0)return;

		/*else
		{
		height_assumption += pack.cm.bbox.DimY();
		sinkBox.min.Y() += pack.cm.bbox.DimY();
		box_packing_algrithm(height_assumption, sinkBox, surfaceArea, volumn_map, md);
		}*/

		//sinkBox.min.Y() += pack.cm.bbox.DimY();
		//qDebug() << "pack.box.dimy"<<pack.cm.bbox.DimY();

		//***20150819
		/*Box3<scalarType> seperate_2;
		qDebug() << "sinkBox.cm.bbox.DimY()" << sinkBox.DimY();

		Box3<scalarType> seperate_1 = sinkBox.Seperate(sinkBox, seperate_2, 0, pack.cm.bbox.DimY(), 0);*/

		//***seperate_1為分割的box
		//***seperate_2為原box切完剩下的部分
		pack->cm.bbox.DimZ();
		if (pack->cm.bbox.DimZ() > z_offset)z_offset = pack->cm.bbox.DimZ();


		box_packing_layer_left(sinkBox, z_offset, *pack, sinkBox, surfaceArea, volumn_map, md, pack->cm.bbox.DimY(), original_box, bt_gap, auto_rotate_switch);

	}



	//***20150818***修改packing演算法
	template<typename scalarType>
	void box_packing_layer_left(Box3<scalarType> current_box, scalarType currentheight, MeshModel& pack_1, Box3<scalarType>& sinkBox, QMap<int, scalarType>& surfaceArea, QMap<int, scalarType>& volumn_map, MeshDocument &md, float cut_offset, Box3<scalarType> sinkBackup, scalarType bt_gap, bool auto_rotate_switch, scalarType floor_z_offset = 0)
	{
		typename QMap<int, scalarType>::Iterator it;
		Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
		Point3m rotate_yaxis(0.0, 1.0, 0.0);
		Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

		scalarType gap = bt_gap;//***20150813設置物體與物體間的間隙

		//qDebug() << "current_box" << current_box.Volume();
		//***space1***
		Point3<scalarType> space1_min(current_box.min.X(), current_box.min.Y(), pack_1.cm.bbox.min.Z());
		Point3<scalarType> space1_max(pack_1.cm.bbox.min.X() - gap, current_box.max.Y(), current_box.min.Z() + currentheight);
		Box3<scalarType> space_1(space1_min, space1_max);
		//***space2***
		/*Point3<scalarType> space2_min(pack_1.cm.bbox.min.X(), current_box.min.Y(), current_box.min.Z());
		Point3<scalarType> space2_max(current_box.max.X(), pack_1.cm.bbox.min.Y() - gap, current_box.min.Z() + currentheight);
		Box3<scalarType> space_2(space2_min, space2_max);*/

		Box3<scalarType> choose_space;
		//qDebug() << "place5" << space_1.Volume() << space_2.Volume();

		//if (space_1.Volume() > space_2.Volume())choose_space = (space_1);
		//else 
		choose_space = (space_1);


		/*vector<std::pair<int, float>> get_volumn;
		copy(test.begin(), test.end(), back_inserter(get_volumn));*/

		//***排序mesh的體積,Map to Vector	
		vector<PAIR> get_volumn;// (volumn_map.begin(), volumn_map.end());

		for (it = volumn_map.begin(); it != volumn_map.end(); ++it)
			get_volumn.push_back(std::pair<int, scalarType>(it.key(), it.value()));
		qDebug() << "get_volumn.size" << get_volumn.size();
		qSort(get_volumn.begin(), get_volumn.end(), CmpByValue());
		reverse(get_volumn.begin(), get_volumn.end());

		/*for (int i = 0; i != get_volumn.size(); ++i)
		qDebug() << "get_volumn[i]" << get_volumn[i].second;*/


		int temp_id = -1;
		MeshModel *pack_next;
		for (int i = 0; i != get_volumn.size(); ++i)
		{
			qDebug() << "get_volumn[i]" << get_volumn[i].second;
			if (get_volumn[i].second < choose_space.Volume() && auto_rotate_switch == false)//與剩餘box體積相比
			{
				//qDebug() << "get_volumn[i]" << get_volumn[i].second;
				pack_next = md.getMesh(get_volumn[i].first);

				if ((pack_next->cm.bbox.DimX() > choose_space.DimX()) || (pack_next->cm.bbox.DimY() > choose_space.DimY()))continue;
				else {
					temp_id = get_volumn[i].first;
					break;
				}
				//*****
			}
			else if (get_volumn[i].second < choose_space.Volume() && auto_rotate_switch == true)
			{
				pack_next = md.getMesh(get_volumn[i].first);
				//***判斷物體長寬高是否超出choose_space 大小
				qDebug() << "pack_next" << pack_next->cm.bbox.DimX() << pack_next->cm.bbox.DimY() << pack_next->cm.bbox.DimZ();
				qDebug() << pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) << choose_space.DimValue(choose_space.MinDim());
				qDebug() << "choose_space" << choose_space.DimX() << choose_space.DimY() << choose_space.DimZ();
				//(round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim())*100)/100)
				//(round(choose_space.DimValue(choose_space.MinDim()) * 100) / 100)

				if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) * 100) / 100) >(round(choose_space.DimValue(choose_space.MaxDim()) * 100) / 100))
					continue;
				else if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) * 100) / 100) > (round(choose_space.DimValue(choose_space.MinDim()) * 100) / 100))
					continue;
				else if ((round(pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) * 100) / 100) > (round(choose_space.DimValue(choose_space.MidDim()) * 100) / 100))
					continue;
				else {
					temp_id = get_volumn[i].first;
					qDebug() << "place4" << temp_id;
					break;
				}



			}
		}

		if (currentheight > floor_z_offset)floor_z_offset = currentheight;

		if (temp_id == -1)//沒有符合剩餘體積的物體
		{
			Box3<scalarType> seperate_2;
			Box3<scalarType> seperate_1 = sinkBox.Seperate(sinkBox, seperate_2, 0, cut_offset + gap, 0);
			int seperate_2_xy_max, seperate_2_xy_min;//***找出seperate dimx, dimy哪個大
			if (seperate_2.DimX() >= seperate_2.DimY()){ seperate_2_xy_max = 0, seperate_2_xy_min = 1; }
			else { seperate_2_xy_max = 1; seperate_2_xy_min = 0; }

			int getMeshid = -1;
			for (int i = 0; i != get_volumn.size(); ++i)//重新搜尋符合的最大mesh。
			{
				pack_next = md.getMesh(get_volumn[i].first);
				if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimValue(seperate_2_xy_max))
					&& (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) < seperate_2.DimValue(seperate_2_xy_min)))
				{
					getMeshid = get_volumn[i].first;
					box_packing_algrithm_left(seperate_2, surfaceArea, volumn_map, md, sinkBackup, bt_gap, auto_rotate_switch, floor_z_offset, getMeshid);
					break;
				}

			}
			if (getMeshid == -1)
			{
				sinkBackup.min.Z() += floor_z_offset + gap;
				box_packing_algrithm_left(sinkBackup, surfaceArea, volumn_map, md, sinkBackup, bt_gap, auto_rotate_switch);//如果往上一層，則不傳高度，重新

			}

			//pack_next = md.getMesh(get_volumn[0].first);
			//if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimY()))			
			//if (seperate_2.Volume() > 0 && (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) < seperate_2.DimValue(seperate_2_xy_max))
			//	&& (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) < seperate_2.DimValue(seperate_2_xy_min))
			//	)
			//{
			//	qDebug() << "next row";
			//	box_packing_algrithm(seperate_2, surfaceArea, volumn_map, md, sinkBackup,floor_z_offset);
			//}
			//else
			//{
			//	sinkBackup.min.Z() += floor_z_offset + gap;
			//	box_packing_algrithm(sinkBackup, surfaceArea, volumn_map, md, sinkBackup);//如果往上一層，則不傳高度，重新
			//}			
		}
		else
		{
			pack_next = md.getMesh(temp_id);
			if (auto_rotate_switch)
			{
				switch (choose_space.MaxDim())//***最長邊轉至與空間中最長邊平行
					//switch (0)//***最長邊轉至與空間中y軸邊平行
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
					//switch (2)
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
			}
			else
			{
				/*if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MaxDim()) > choose_space.DimValue(choose_space.MaxDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MinDim()) > choose_space.DimValue(choose_space.MinDim()))continue;
				else if (pack_next->cm.bbox.DimValue(pack_next->cm.bbox.MidDim()) > choose_space.DimValue(choose_space.MidDim()))continue;
				else {
				temp_id = get_volumn[i].first;
				qDebug() << "place4" << temp_id;
				break;
				}*/

			}
			Point3<scalarType> testpoint3 = choose_space.P(3) - pack_next->cm.bbox.P(3);
			qDebug() << "testpoint3" << testpoint3[0] << testpoint3[1] << testpoint3[2];
			xyz::mesh_translate(pack_next->cm, choose_space.P(3) - pack_next->cm.bbox.P(3));//***移至choose_space.min


			if (pack_next->cm.bbox.DimY() > cut_offset)cut_offset = pack_next->cm.bbox.DimY();//將最大Y與高往下傳
			if (pack_next->cm.bbox.DimZ() > currentheight)currentheight = pack_next->cm.bbox.DimZ();

			it = surfaceArea.find(pack_next->id());
			surfaceArea.erase(it);
			it = volumn_map.find(pack_next->id());
			volumn_map.erase(it);


			if (surfaceArea.count() != 0)box_packing_layer_left(choose_space, currentheight, *pack_next, sinkBox, surfaceArea, volumn_map, md, cut_offset, sinkBackup, bt_gap, auto_rotate_switch, floor_z_offset);
			else return;
		}

	}



}
#endif