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
#include "threemf_manip.h"
#include <wrap/io_trimesh/import_obj.h>

using namespace std;

//LPCWSTR* ThreeMF_Manip::genName(int i)
//{
//	std::wstring someName(to_wstring(i));
//	return someName.c_str();
//}

QString NameDisambiguator(QList<QString> &elemList, QString texturename)
{
	QString newName = texturename;
	QList<QString>::iterator mmi;

	for (mmi = elemList.begin(); mmi != elemList.end(); ++mmi)
	{
		if (*mmi == newName) // if duplicated name found
		{
			QFileInfo fi(*mmi);
			QString baseName = fi.baseName(); //  all characters in the file up to the first '.' Eg "/tmp/archive.tar.gz" -> "archive"
			QString suffix = fi.suffix();
			bool ok;

			int numDisamb;
			int startDisamb;
			int endDisamb;

			startDisamb = baseName.lastIndexOf("(");
			endDisamb = baseName.lastIndexOf(")");
			if ((startDisamb != -1) && (endDisamb != -1))
				numDisamb = (baseName.mid((startDisamb + 1), (endDisamb - startDisamb - 1))).toInt(&ok);
			else
				numDisamb = 0;

			if (startDisamb != -1)
				newName = baseName.left(startDisamb) + "(" + QString::number(numDisamb + 1) + ")";
			else
				newName = baseName + "(" + QString::number(numDisamb + 1) + ")";

			if (suffix != QString(""))
				newName = newName + "." + suffix;

			// now recurse to see if the new name is free
			newName = NameDisambiguator(elemList, newName);
		}
	}
	return newName;
}




CComPtr<ILib3MFModel> ThreeMF_Manip::create_3mf(MeshDocument *md, CallBackPos *cb)
{
	DWORD nInterfaceVersion, v2, v3;
	CComPtr<ILib3MFModel> pModel;
	CComPtr<ILib3MFModelFactory> pFactory;
	ILib3MFModelBaseMaterial *pBaseMaterial;




	DWORD baseID;
	QMap<int, DWORD> baseIndex;
	QMap<int, QList<_int8>> textureMap;
	QMap<int, QMap<int, int>> maptex;
	QList<QString> textureFileNmae;
	NMRCreateModelFactory(&pFactory);												// Create Factory Object
	pFactory->GetInterfaceVersion(&nInterfaceVersion);								// Check 3MF Library Version
	pFactory->CreateModel(&pModel);													// Create Model Instance

	bool hasBaseMaterial = false;
	foreach(MeshModel *mm, md->meshList)
	{
		if (mm->hasDataMask(MeshModel::MM_COLOR) && (mm->rmm.colorMode == GLW::CMPerVert))
		{
			hasBaseMaterial = true;
			break;
		}
	}

	//if (hasBaseMaterial == true)
	//{
	//	pModel->AddBaseMaterialGroup(&pBaseMaterial);								// Add Materials / textures used
	//}

	//test Material
	pModel->AddBaseMaterialGroup(&pBaseMaterial);



	foreach(MeshModel *mm, md->meshList)
	{



		if (mm->cm.textures.size() > 0)
		{


			/*if (!(mm->rmm.colorMode == GLW::CMPerFace
			&& (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD) 	&& (mm->cm.face.data()->C() == Color4b(0, 0, 0, mm->cm.face.data()->C().W())))))*/
			if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			{
				QFileInfo file(mm->fullName());
				QDir::setCurrent(file.path());
				QList<_int8> textures;
				QImage img;
				for (int i = 0; i < mm->cm.textures.size(); i++)
				{
					DWORD nTextureID;
					QFileInfo tex(QString::fromStdString(mm->cm.textures[i]));
					QString imgPath = tex.absoluteFilePath();// file.path() + "/" + QString::fromStdString(mm->cm.textures[i]);
					QFileInfo temp(imgPath);
					if (!temp.exists())
					{
						int dummy = -1;
						textures.push_back(dummy);
						continue;
					}
					QString texfilenameIn3mf = tex.fileName();
					texfilenameIn3mf = NameDisambiguator(textureFileNmae, texfilenameIn3mf);
					QString path = "/3D/Textures/" + texfilenameIn3mf.replace(" ", "_");// QString::fromStdString(mm->cm.textures[i]);
					eModelTexture2DType eType = getModelTextureType(mm->cm.textures[i]);

					if (eType == 0)
					{
						img.load(imgPath);
						QByteArray byteArray;
						QBuffer buffer(&byteArray);
						buffer.open(QIODevice::WriteOnly);
						img.save(&buffer, "PNG");
						path = path.remove(path.lastIndexOf("."), tex.suffix().length() + 1);
						path = path + ".png";
						eType = MODELTEXTURETYPE_PNG;
						loadModelTexture(pModel, path.toStdString().c_str(), &byteArray, eType, &nTextureID);
					}
					else
					{
						//Add textures to 3mf package
						loadModelTexture(pModel, path.toStdString().c_str(), imgPath.toStdString().c_str(), eType, &nTextureID);
					}

					textures.push_back(nTextureID);
					textureFileNmae.push_back(tex.fileName());
				}

				textureMap.insert(mm->id(), textures);
			}
		}
	}

	// Add Mesh Object
	DWORD indexCounter = 0;
	MODELTRANSFORM *tempMatrix = new MODELTRANSFORM;
	tempMatrix->m_fFields[0][0] = 1; tempMatrix->m_fFields[0][1] = 0; tempMatrix->m_fFields[0][2] = 0; tempMatrix->m_fFields[0][3] = 0;
	tempMatrix->m_fFields[1][0] = 0; tempMatrix->m_fFields[1][1] = 1; tempMatrix->m_fFields[1][2] = 0; tempMatrix->m_fFields[1][3] = 0;
	tempMatrix->m_fFields[2][0] = 0; tempMatrix->m_fFields[2][1] = 0; tempMatrix->m_fFields[2][2] = 1; tempMatrix->m_fFields[2][3] = 0;

	foreach(MeshModel *mm, md->meshList)
	{


		DWORD pMaterialGroupID;
		pBaseMaterial->GetResourceID(&pMaterialGroupID);
		//
		baseID = pMaterialGroupID;
		baseIndex.insert(mm->id(), indexCounter);
		if (mm->hasDataMask(MeshModel::MM_COLOR) && (mm->rmm.colorMode == GLW::CMPerVert))
		{
			pBaseMaterial->AddMaterial(L"color", mm->cm.C().X(), mm->cm.C().Y(), mm->cm.C().Z(), &indexCounter);
			qDebug() << mm->cm.C().X() << mm->cm.C().Y() << mm->cm.C().Z();
			bool x = mm->hasDataMask(MeshModel::MM_COLOR);
			indexCounter++;
		}

		//tri::Clean<CMeshO>::RemoveDuplicateVertex(mm->cm);
		MODELMESHVERTEX *pVertices = new MODELMESHVERTEX[mm->cm.VN()];
		MODELMESHTRIANGLE *pTriangles = new MODELMESHTRIANGLE[mm->cm.FN()];
		vector<int> VertexID(mm->cm.vert.size());

		CMeshO::VertexIterator vi;
		int i = 0;
		for (vi = mm->cm.vert.begin(); vi != mm->cm.vert.end(); vi++)
		{
			if (!vi->IsD())
			{
				VertexID[vi - mm->cm.vert.begin()] = i;

				pVertices[i].m_fPosition[0] = vi->P().X();
				pVertices[i].m_fPosition[1] = vi->P().Y();
				pVertices[i].m_fPosition[2] = vi->P().Z();
				i++;
			}
		}

		CMeshO::FaceIterator fi;
		QMap<int, MODELMESHTEXTURE2D> faceTexture;
		QMap<int, int> testTexId;
 		//QMap<int, MODELMESH_TRIANGLECOLOR_SRGB> faceColor;
		QMap<int, MODELMESH_TRIANGLECOLOR_SRGB> vertexColorMap;//SetGradientColor
		//QMap<int, MODELMESHCOLOR_SRGB> singleFaceColor;
		QMap<int, int> faceMaterial;
		//int colorPerMode = meshColorPerMode(mm);


		//testBaseMaterial		
		QSet<Point4<byte>> storeMaterial;
		for (fi = mm->cm.face.begin(); fi != mm->cm.face.end(); fi++)
		{
			if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerFace /*&& mm->rmm.textureMode == GLW::TMNone*/)
			{
				storeMaterial.insert(fi->C().V());
			}

		}

		QMap<Point4<byte>, int>materialMap;
		foreach(const Point4<byte> &cc, storeMaterial)
		{
			pBaseMaterial->AddMaterial(L"color", cc.V()[0], cc.V()[1], cc.V()[2], &indexCounter);
			materialMap.insert(cc, indexCounter);
			indexCounter++;
		}


		


		int j = 0;
		for (fi = mm->cm.face.begin(); fi != mm->cm.face.end(); fi++)
		{
			if (!fi->IsD())
			{
				
				for (int k = 0; k < 3; k++)
				{
					pTriangles[j].m_nIndices[k] = VertexID[vcg::tri::Index(mm->cm, (*fi).V(k))];

					MODELMESHTEXTURE2D texCoor;


					if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD) && mm->cm.textures.size() > 0 && (mm->rmm.textureMode != GLW::TMNone))
					{
						if (fi->WT(0).n() >= 0)
						{
							QList<_int8> tex = textureMap.value(mm->id());
							if (tex[fi->WT(0).n()] > 0)
							{
								texCoor.m_fU[k] = fi->WT(k).u();
								texCoor.m_fV[k] = fi->WT(k).v();
								texCoor.m_nTextureID = tex[fi->WT(0).n()];

							}
							faceTexture.insert(j, texCoor);
						}
						else{
							if (mm->hasDataMask(MeshModel::MM_VERTCOLOR) && mm->rmm.colorMode == GLW::CMPerVert)
							{
								MODELMESH_TRIANGLECOLOR_SRGB vertexColor;
								vertexColor.m_Colors[k].m_Red = fi->V(k)->C().X();
								vertexColor.m_Colors[k].m_Green = fi->V(k)->C().Y();
								vertexColor.m_Colors[k].m_Blue = fi->V(k)->C().Z();
								vertexColor.m_Colors[k].m_Alpha = fi->V(k)->C().W();
								vertexColorMap.insert(j, vertexColor);
							}



							else if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerFace)
							{

								
								if (materialMap.contains(fi->C()))
									faceMaterial.insert(j, materialMap.value(fi->C()));


							}

						}

					}
					else
					{
						if (mm->hasDataMask(MeshModel::MM_VERTCOLOR) && mm->rmm.colorMode == GLW::CMPerVert)
						{
							MODELMESH_TRIANGLECOLOR_SRGB vertexColor;
							vertexColor.m_Colors[k].m_Red = fi->V(k)->C().X();
							vertexColor.m_Colors[k].m_Green = fi->V(k)->C().Y();
							vertexColor.m_Colors[k].m_Blue = fi->V(k)->C().Z();
							vertexColor.m_Colors[k].m_Alpha = fi->V(k)->C().W();
							vertexColorMap.insert(j, vertexColor);
						}



						else if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerFace)
						{

							/*MODELMESHCOLOR_SRGB color;
							color.m_Red = fi->C().X();
							color.m_Green = fi->C().Y();
							color.m_Blue = fi->C().Z();
							color.m_Alpha = fi->C().W();
							singleFaceColor.insert(j, color);*/
							if (materialMap.contains(fi->C()))
								faceMaterial.insert(j, materialMap.value(fi->C()));


						}
					}



					//if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD) && mm->cm.textures.size() > 0 && (mm->rmm.textureMode !=GLW::TMNone))
					//{
					//	if (fi->WT(0).n() >= 0)
					//	{
					//		QList<_int8> tex = textureMap.value(mm->id());
					//		if (tex[fi->WT(0).n()] > 0)
					//		{
					//			texCoor.m_fU[k] = fi->WT(k).u();
					//			texCoor.m_fV[k] = fi->WT(k).v();
					//			texCoor.m_nTextureID = tex[fi->WT(0).n()];

					//		}
					//		faceTexture.insert(j, texCoor);
					//	}
					//	
					//}



					//if (mm->hasDataMask(MeshModel::MM_VERTCOLOR) && mm->rmm.colorMode == GLW::CMPerVert)
					//{
					//	MODELMESH_TRIANGLECOLOR_SRGB vertexColor;
					//	vertexColor.m_Colors[k].m_Red = fi->V(k)->C().X();
					//	vertexColor.m_Colors[k].m_Green = fi->V(k)->C().Y();
					//	vertexColor.m_Colors[k].m_Blue = fi->V(k)->C().Z();
					//	vertexColor.m_Colors[k].m_Alpha = fi->V(k)->C().W();
					//	vertexColorMap.insert(j, vertexColor);
					//}



					//else if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerFace)
					//{

					//	/*MODELMESHCOLOR_SRGB color;
					//	color.m_Red = fi->C().X();
					//	color.m_Green = fi->C().Y();
					//	color.m_Blue = fi->C().Z();
					//	color.m_Alpha = fi->C().W();
					//	singleFaceColor.insert(j, color);*/
					//	if (materialMap.contains(fi->C()))
					//		faceMaterial.insert(j, materialMap.value(fi->C()));


					//}

					//switch (colorPerMode)					
					//{
					//case 2:
					//{
					//	MODELMESH_TRIANGLECOLOR_SRGB vertexColor;
					//	vertexColor.m_Colors[k].m_Red = fi->V(k)->C().X();
					//	vertexColor.m_Colors[k].m_Green = fi->V(k)->C().Y();
					//	vertexColor.m_Colors[k].m_Blue = fi->V(k)->C().Z();
					//	vertexColor.m_Colors[k].m_Alpha = fi->V(k)->C().W();
					//	vertexColorMap.insert(j, vertexColor);
					//	break;
					//}
					//case 3:
					//{
					//	if (fi->WT(0).n() >= 0)
					//	{
					//		texCoor.m_fU[k] = fi->WT(k).u();
					//		texCoor.m_fV[k] = fi->WT(k).v();
					//		QList<DWORD> tex = textureMap.value(mm->id());
					//		texCoor.m_nTextureID = tex[fi->WT(0).n()];
					//	}
					//	faceTexture.insert(j, texCoor);
					//	break;
					//}
					//case 4:
					//{
					//	MODELMESHCOLOR_SRGB color;
					//	color.m_Red = fi->C().X();
					//	color.m_Green = fi->C().Y();
					//	color.m_Blue = fi->C().Z();
					//	color.m_Alpha = fi->C().W();
					//	singleFaceColor.insert(j, color);
					//	break;
					//}
					//case 5:
					//{
					//	MODELMESHCOLOR_SRGB color;
					//	color.m_Red = 0;
					//	color.m_Green = 0;
					//	color.m_Blue = 0;
					//	color.m_Alpha = 255;
					//	singleFaceColor.insert(j, color);
					//	break;
					//}
					//case 6:
					//{
					//		 /* MODELMESHCOLOR_SRGB color;
					//		  color.m_Red = fi->C().X();
					//		  color.m_Green = fi->C().Y();
					//		  color.m_Blue = fi->C().Z();
					//		  singleFaceColor.insert(j, color);*/
					//	if (fi->WT(0).n() >= 0)
					//	{
					//		texCoor.m_fU[k] = fi->WT(k).u();
					//		texCoor.m_fV[k] = fi->WT(k).v();
					//		QList<DWORD> tex = textureMap.value(mm->id());
					//		texCoor.m_nTextureID = tex[fi->WT(0).n()];
					//	}
					//	faceTexture.insert(j, texCoor);
					//	break;
					//}
					//case 7:
					//{
					//	if (fi->WT(0).n() >= 0)
					//	{
					//		texCoor.m_fU[k] = fi->WT(k).u();
					//		texCoor.m_fV[k] = fi->WT(k).v();
					//		QList<DWORD> tex = textureMap.value(mm->id());
					//		texCoor.m_nTextureID = tex[fi->WT(0).n()];
					//	}
					//	faceTexture.insert(j, texCoor);
					//	break;
					//}
					//case 8:
					//{
					//	MODELMESHCOLOR_SRGB color;
					//	color.m_Red = 255;
					//	color.m_Green = 255;
					//	color.m_Blue = 255;
					//	singleFaceColor.insert(j, color);
					//	break;
					//}
					//default: break;
					//}
				}
			}
			j++;

		}

		CComPtr<ILib3MFModelMeshObject> pMeshObject;
		CComPtr<ILib3MFDefaultPropertyHandler> pDefaultPropertyHandler;
		CComPtr<ILib3MFPropertyHandler> pPropertyHandler;


		pModel->AddMeshObject(&pMeshObject);																// Create Mesh Object
		LPCWSTR label = (const wchar_t*)mm->label().utf16();
		pMeshObject->SetName(label);																		// Set custom name
		/*Model type*/
		if (mm->getMeshSort() == MeshModel::meshsort::pre_print_item)
			pMeshObject->SetType(MODELOBJECTTYPE_SUPPORT);
		if (mm->getMeshSort() == MeshModel::meshsort::print_item)
			pMeshObject->SetType(MODELOBJECTTYPE_MODEL);



		pMeshObject->SetGeometry(pVertices, mm->cm.VN(), pTriangles, mm->cm.FN());
		pMeshObject->CreatePropertyHandler(&pPropertyHandler);
		//--------------------------------------------------------------------------------------------------------------------------------------------------
		//Create Property Handler for single color mesh
		if (mm->hasDataMask(MeshModel::MM_COLOR))
		{
			pMeshObject->CreateDefaultPropertyHandler(&pDefaultPropertyHandler);
			pDefaultPropertyHandler->SetBaseMaterial(baseID, baseIndex.value(mm->id()));

		}

		//Create Property Handler for texture
		else if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
		{
			//Set texture parameters of triangles	
			/*for (int i = 0; i < mm->cm.FN(); i++)
			{
				pPropertyHandler->SetTexture(i, &(MODELMESHTEXTURE2D)faceTexture.value(i));
			}*/
			QMapIterator<int, MODELMESHTEXTURE2D> ift(faceTexture);
			while (ift.hasNext()) {
				ift.next();
				pPropertyHandler->SetTexture(ift.key(), &(MODELMESHTEXTURE2D)ift.value());
				
			}

		}

		//Create Property Handler for Color per vertex 
		if (mm->hasDataMask(MeshModel::MM_VERTCOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerVert /*&& mm->rmm.textureMode == GLW::TMNone*/)
		{
			//for (int j = 0; j < mm->cm.FN(); j++)
			//{
			//	//hResult = pPropertyHandler->SetSingleColor(j, &(MODELMESHCOLOR_SRGB)singleFaceColor.value(j));
			//	pPropertyHandler->SetGradientColor(j, &(MODELMESH_TRIANGLECOLOR_SRGB)vertexColorMap.value(j));
			//}

			QMapIterator<int, MODELMESH_TRIANGLECOLOR_SRGB> ivc(vertexColorMap);
			while (ivc.hasNext()) {
				ivc.next();
				pPropertyHandler->SetGradientColor(ivc.key(), &(MODELMESH_TRIANGLECOLOR_SRGB)ivc.value());

			}
		}

		//Create Property Handler for Color per face
		else if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_COLOR) && mm->rmm.colorMode == GLW::CMPerFace /*&& mm->rmm.textureMode == GLW::TMNone*/)
		{
			//pMeshObject->CreatePropertyHandler(&pPropertyHandler);

			//for (int j = 0; j < mm->cm.FN(); j++)
			//{
			//	//pPropertyHandler->SetSingleColor(j, &(MODELMESHCOLOR_SRGB)singleFaceColor.value(j));
			//	HRESULT  sx = pPropertyHandler->SetBaseMaterial(j, baseID, faceMaterial.value(j));

			//}

			QMapIterator<int, int> ifc(faceMaterial);
			while (ifc.hasNext()) {
				ifc.next();
				pPropertyHandler->SetBaseMaterial(ifc.key(), baseID, ifc.value());

			}


		}





		//test component	
		ILib3MFModelComponentsObject *pComponentsObject;
		ILib3MFModelComponent *pComponent;
		ILib3MFModelBuildItem *pBuildItem;

		int hResult = pModel->AddComponentsObject(&pComponentsObject);
		qDebug() << QString("%1").arg(hResult, 0, 16).toUpper();
		int result = pComponentsObject->AddComponent(pMeshObject, tempMatrix, &pComponent);
		pModel->AddBuildItem(pComponentsObject, tempMatrix, &pBuildItem);

		//pComponent->

		//--------------------------------------------------------------------------------------------------------------------------------------------------

		//qDebug() << "colorPerMode: " << colorPerMode;
		//if (colorPerMode == 1)
		//{
		//	pMeshObject->CreateDefaultPropertyHandler(&pDefaultPropertyHandler);
		//	pDefaultPropertyHandler->SetBaseMaterial(baseID, baseIndex.value(mm->id()));
		//}
		//else if (colorPerMode == 2)
		//{
		//	pMeshObject->CreatePropertyHandler(&pPropertyHandler);

		//	for (int j = 0; j < mm->cm.FN(); j++)
		//	{
		//		pPropertyHandler->SetGradientColor(j, &(MODELMESH_TRIANGLECOLOR_SRGB)vertexColorMap.value(j));
		//	}
		//}
		//else if (colorPerMode == 4 || colorPerMode == 5 || colorPerMode == 8)
		//{
		//	pMeshObject->CreatePropertyHandler(&pPropertyHandler);

		//	for (int j = 0; j < mm->cm.FN(); j++)
		//	{
		//		pPropertyHandler->SetSingleColor(j, &(MODELMESHCOLOR_SRGB)singleFaceColor.value(j));
		//	}
		//}
		//else if (colorPerMode == 3 || colorPerMode == 6 || colorPerMode == 7)
		//{
		//	pMeshObject->CreatePropertyHandler(&pPropertyHandler);

		//	for (int i = 0; i < mm->cm.FN(); i++)
		//	{
		//			//pPropertyHandler->SetSingleColor(j, &(MODELMESHCOLOR_SRGB)singleFaceColor.value(j));
		//		pPropertyHandler->SetTexture(i, &(MODELMESHTEXTURE2D)faceTexture.value(i));
		//	}
		//}


		if (cb != NULL)
		{
			if (!(*cb)((100 * mm->id()) / md->meshList.size(), "save model"))
			{

				return false;
			}
		}

	}

	return pModel;
}

int ThreeMF_Manip::meshColorPerMode(MeshModel *mm)
{
	int i = 0;


	if (mm->rmm.colorMode == GLW::CMPerVert)
	{
		if (mm->hasDataMask(MeshModel::MM_COLOR))
			i = 1;
		else if (mm->hasDataMask(MeshModel::MM_VERTCOLOR) && !mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			i = 2;
		else if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			i = 3;
	}
	else if (mm->rmm.colorMode == GLW::CMPerFace)
	{
		if (mm->hasDataMask(MeshModel::MM_FACECOLOR) && !mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			i = 4;
		else if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
		{
			if (mm->cm.face.data()->C() == Color4b(0, 0, 0, mm->cm.face.data()->C().W()))
				i = 5;
			else
				i = 6;
		}
	}
	else
	{
		if (mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			i = 7;
		else
			i = 8;
	}

	return i;
}

HRESULT ThreeMF_Manip::loadModelTexture(ILib3MFModel *pModel, const char *imgPath, const char *imgFile, eModelTexture2DType eType, DWORD *pTextureID)
{
	HRESULT hResult;
	DWORD nErrorMsg;
	LPCSTR pszErrorMessage;
	ILib3MFModelTexture2D *pTexture2D;

	hResult = pModel->AddTexture2DUTF8(imgPath, &pTexture2D);
	if (hResult != S_OK) return hResult;

	hResult = pTexture2D->SetContentType(eType);
	if (hResult != S_OK) return hResult;

	hResult = pTexture2D->ReadFromFileUTF8(imgFile);																//Read texture from file
	if (hResult != S_OK) return hResult;


	hResult = pTexture2D->GetResourceID(pTextureID);																//Retrieve texture ID
	if (hResult != S_OK) return hResult;

	return hResult;
}

HRESULT ThreeMF_Manip::loadModelTexture(ILib3MFModel *pModel, const char *imgPath, QByteArray *imgFile, eModelTexture2DType eType, DWORD *pTextureID)
{
	HRESULT hResult;
	DWORD nErrorMsg;
	LPCSTR pszErrorMessage;
	ILib3MFModelTexture2D *pTexture2D;

	hResult = pModel->AddTexture2DUTF8(imgPath, &pTexture2D);
	if (hResult != S_OK) return hResult;

	hResult = pTexture2D->SetContentType(eType);
	if (hResult != S_OK) return hResult;

	uchar *buf = new uchar[imgFile->size()];
	QByteArray::iterator it = imgFile->begin();
	int i = 0;
	for (it; it != imgFile->end(); it++)
	{
		buf[i] = *it;
		i++;
	}

	hResult = pTexture2D->ReadFromBuffer(buf, imgFile->size());
	if (hResult != S_OK) return hResult;

	hResult = pTexture2D->GetResourceID(pTextureID);
	if (hResult != S_OK) return hResult;

	return hResult;
}

eModelTexture2DType ThreeMF_Manip::getModelTextureType(string imgFile)
{
	string ext = imgFile.substr(imgFile.find_last_of(".") + 1);
	transform(ext.begin(), ext.end(), ext.begin(), toupper);

	if (ext == "PNG")
		return MODELTEXTURETYPE_PNG;
	else if (ext == "JPG" || ext == "JPEG")
		return MODELTEXTURETYPE_JPEG;
	else
		return MODELTEXTURETYPE_UNKNOWN;
}

bool ThreeMF_Manip::save_3mf(MeshDocument *md, LPCWSTR project_name, CallBackPos *cb)
{
	HRESULT hResult;
	CComPtr<ILib3MFModel> pModel = create_3mf(md, cb);
	CComPtr<ILib3MFModelWriter> p3MFWriter;

	hResult = pModel->QueryWriter("3mf", &p3MFWriter);																		// Create Model Writer
	hResult = p3MFWriter->WriteToFile(project_name);																		// Export Model into File
	return (hResult != S_OK ? false : true);
}

HRESULT ThreeMF_Manip::addMesh(MeshDocument *md, ILib3MFModelMeshObject *pMeshObject, int objectCounter)
{
	HRESULT hResult;
	DWORD objectID;

	QString meshName = getName(pMeshObject, objectCounter);

	MeshModel *mm = new MeshModel(md, "", meshName);
	mm->is3mf = true;
	getVertices(mm, pMeshObject);
	getFacets(mm, pMeshObject);
	getProperties(mm, pMeshObject);
	DWORD type;
	hResult = pMeshObject->GetType(&type);
	switch (type)
	{
	case MODELOBJECTTYPE_OTHER:
		break;
	case MODELOBJECTTYPE_MODEL:
		mm->setMeshSort(MeshModel::print_item);
		break;
	case MODELOBJECTTYPE_SUPPORT:
		mm->setMeshSort(MeshModel::pre_print_item);
		break;
	default:
		mm->setMeshSort(MeshModel::print_item);
	}

	hResult = pMeshObject->GetResourceID(&objectID);
	if (hResult != S_OK) return hResult;

	meshObjectPerID.insert(objectID, mm);
	qDebug() << ">>>>" << mm->getMeshSort();
	return hResult;

}

HRESULT ThreeMF_Manip::addComponent(MeshDocument *md, ILib3MFModelComponentsObject *pComponentsObject, int objectCounter)
{
	HRESULT hResult;
	DWORD pComponentCount;
	DWORD pCompResourceID;
	ILib3MFModelComponent *pComponent;
	ILib3MFModelObjectResource *ppResource;
	ILib3MFModelMeshObject *pMeshObject;
	QList<int> idList;
	QList<MeshModel *>  compObjects;

	QString meshName = getName(pComponentsObject, objectCounter);

	hResult = pComponentsObject->GetComponentCount(&pComponentCount);
	if (hResult != S_OK) return hResult;

	hResult = pComponentsObject->GetResourceID(&pCompResourceID);
	if (hResult != S_OK) return hResult;

	compObjects.clear();
	idList.clear();
	for (int i = 0; i < pComponentCount; i++)
	{
		hResult = pComponentsObject->GetComponent(i, &pComponent);
		if (hResult != S_OK) return hResult;

		DWORD objectID;
		hResult = pComponent->GetObjectResourceID(&objectID);
		if (hResult != S_OK) return hResult;

		MeshModel *mm = meshObjectPerID.value(objectID);
		BOOL pbHasTransform;
		hResult = pComponent->HasTransform(&pbHasTransform);
		if (hResult != S_OK) return hResult;

		if (pbHasTransform)
		{
			MODELTRANSFORM pTransform;

			hResult = pComponent->GetTransform(&pTransform);
			if (hResult != S_OK) return hResult;

			Matrix44m mat;
			mat.SetIdentity();
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					mat.ElementAt(i, j) = pTransform.m_fFields[i][j];
				}
			}

			CMeshO::VertexIterator vi = mm->cm.vert.begin();
			for (vi; vi != mm->cm.vert.end(); vi++)
			{
				Point4f vert = Point4f(vi->P().X(), vi->P().Y(), vi->P().Z(), 1.0);
				vert = mat * vert;
				vi->P().X() = vert.X();
				vi->P().Y() = vert.Y();
				vi->P().Z() = vert.Z();
			}
		}

		compObjects.push_back(mm);
		idList.push_back(objectID);
	}

	for (int i = 0; i < compObjects.size(); i++)
	{
		if (compObjects[0]->id() != compObjects[i]->id())
		{
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(compObjects[0]->cm, compObjects[i]->cm);

			if (compObjects[i]->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			{
				CMeshO::FaceIterator fi = compObjects[0]->cm.face.begin();
				advance(fi, compObjects[0]->cm.fn - compObjects[i]->cm.fn);
				int ii = 0;
				int texOffset = compObjects[0]->cm.textures.size() - compObjects[i]->cm.textures.size();
				for (fi; fi != compObjects[0]->cm.face.end(); fi++)
				{
					fi->WT(0).n() = texOffset + compObjects[i]->cm.face[ii].WT(0).n();
					fi->WT(1).n() = texOffset + compObjects[i]->cm.face[ii].WT(0).n();
					fi->WT(2).n() = texOffset + compObjects[i]->cm.face[ii].WT(0).n();
				}
			}
		}
	}

	for (int i = 0; i < idList.size(); i++)
	{
		meshObjectPerID.remove(idList[i]);
	}

	meshObjectPerID.insert(pCompResourceID, compObjects[0]);
	return hResult;
}

QString ThreeMF_Manip::getName(ILib3MFModelMeshObject *pMeshObject, int objectCounter)
{
	HRESULT hResult;
	DWORD nErrorMsg;
	LPCSTR pszErrorMessage;
	DWORD nNeededChars;
	vector<wchar_t> pBuffer;
	wstring sName;

	hResult = pMeshObject->GetName(NULL, 0, &nNeededChars);
	if (hResult != S_OK) return hResult;

	if (nNeededChars > 0)
	{
		pBuffer.resize(nNeededChars + 1);
		hResult = pMeshObject->GetName(&pBuffer[0], nNeededChars + 1, NULL);
		pBuffer[nNeededChars] = 0;
		sName = std::wstring(&pBuffer[0]);
	}

	string sAnsiName(sName.begin(), sName.end());
	QString meshName = QString::fromStdString(sAnsiName);

	if (meshName == "")
	{
		meshName = projectName + " - object " + QString::number(objectCounter);
	}

	return meshName;
}

QString ThreeMF_Manip::getName(ILib3MFModelComponentsObject *pComponentsObject, int objectCounter)
{
	DWORD nNeededChars;
	vector<wchar_t> pBuffer;
	wstring sName;

	pComponentsObject->GetName(NULL, 0, &nNeededChars);

	if (nNeededChars > 0)
	{
		pBuffer.resize(nNeededChars + 1);
		pComponentsObject->GetName(&pBuffer[0], nNeededChars + 1, NULL);
		pBuffer[nNeededChars] = 0;
		sName = std::wstring(&pBuffer[0]);
	}

	string sAnsiName(sName.begin(), sName.end());
	QString meshName = QString::fromStdString(sAnsiName);

	if (meshName == "")
	{
		meshName = "object " + QString::number(objectCounter);
	}

	return meshName;
}

void ThreeMF_Manip::getVertices(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject)
{

	DWORD nVertexCount;
	MODELMESHVERTEX *pVertex = new MODELMESHVERTEX();

	pMeshObject->GetVertexCount(&nVertexCount);

	vertexID.clear();
	for (int i = 0; i < nVertexCount; i++)
	{
		pMeshObject->GetVertex(i, pVertex);

		float x = pVertex->m_fPosition[0];
		float y = pVertex->m_fPosition[1];
		float z = pVertex->m_fPosition[2];
		Point3f newVertex = Point3f(x, y, z);
		vertexID.insert(i, newVertex);
		CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertex(mm->cm, newVertex);
	}

}

void ThreeMF_Manip::getFacets(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject)
{
	DWORD nTriangleCount;
	MODELMESHTRIANGLE *pTriangle = new MODELMESHTRIANGLE();

	pMeshObject->GetTriangleCount(&nTriangleCount);

	for (int i = 0; i < nTriangleCount; i++)
	{
		assert(pTriangle);
		pMeshObject->GetTriangle(i, pTriangle);

		//Point3f v1 = vertexID.value(pTriangle->m_nIndices[0]);
		//Point3f v2 = vertexID.value(pTriangle->m_nIndices[1]);
		//Point3f v3 = vertexID.value(pTriangle->m_nIndices[2]);
		//CMeshO::FaceIterator fi = vcg::tri::Allocator<CMeshO>::AddFace(mm->cm, v1, v2, v3);

		int v1 = pTriangle->m_nIndices[0];
		int v2 = pTriangle->m_nIndices[1];
		int v3 = pTriangle->m_nIndices[2];
		CMeshO::FaceIterator fi = vcg::tri::Allocator<CMeshO>::AddFace(mm->cm, v1, v2, v3);
	}
}

void ThreeMF_Manip::getProperties(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject)
{
	CComPtr<ILib3MFDefaultPropertyHandler> pDefaultPropertyHandler;
	CComPtr<ILib3MFPropertyHandler> pPropertyHandler;
	eModelPropertyType pnDefaultPropertyType;
	eModelPropertyType pnPropertyType;

	pMeshObject->CreateDefaultPropertyHandler(&pDefaultPropertyHandler);
	pDefaultPropertyHandler->GetPropertyType(&pnDefaultPropertyType);

	if (pnDefaultPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_BASEMATERIALS)
		loadDefaultBaseMaterials(mm, pDefaultPropertyHandler);
	else if (pnDefaultPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_TEXCOORD2D)
		loadDefaultTexture(mm, pDefaultPropertyHandler);
	else
	{
		pMeshObject->CreatePropertyHandler(&pPropertyHandler);

		int i = 0;
		DWORD pMaterialIndex;
		DWORD pMaterialGroupID;
		MODELMESH_TRIANGLECOLOR_SRGB faceVertexColor;
		MODELMESHCOLOR_SRGB faceColor;
		MODELMESHTEXTURE2D faceTexture;
		std::map<string, int> textureIndex;
		textureIndex.clear();
		int texIndex = 0;

		for (CMeshO::FaceIterator fi = mm->cm.face.begin(); fi != mm->cm.face.end(); ++fi)
		{
			pPropertyHandler->GetPropertyType(i, &pnPropertyType);

			if (pnPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_BASEMATERIALS)
			{
				if (!(pPropertyHandler->GetBaseMaterial(i, &pMaterialGroupID, &pMaterialIndex)))
				{
					mm->Enable(tri::io::Mask::IOM_FACECOLOR);
					mm->updateDataMask(MeshModel::MM_FACECOLOR);
					mm->rmm.colorMode = vcg::GLW::CMPerFace;
					//mm->rmm.textureMode = vcg::GLW::TMNone;

					QMap<int, Color4b> baseColors = baseMats.value(pMaterialGroupID);
					Color4b newColor = baseColors.value(pMaterialIndex);
					fi->C() = newColor;
				}
			}
			//else if (pnPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_COLOR)
			//{
			//	if (!(pPropertyHandler->GetColor(i, &faceVertexColor)))
			//	{
			//		//if (faceVertexColor.m_Colors[0].m_Red == faceVertexColor.m_Colors[1].m_Red && faceVertexColor.m_Colors[0].m_Red == faceVertexColor.m_Colors[2].m_Red
			//		//	&& faceVertexColor.m_Colors[0].m_Green == faceVertexColor.m_Colors[1].m_Green && faceVertexColor.m_Colors[0].m_Green == faceVertexColor.m_Colors[2].m_Green
			//		//	&& faceVertexColor.m_Colors[0].m_Blue == faceVertexColor.m_Colors[1].m_Blue && faceVertexColor.m_Colors[0].m_Blue == faceVertexColor.m_Colors[2].m_Blue
			//		//	)
			//		//{
			//		//	/*mm->Enable(tri::io::Mask::IOM_FACECOLOR);
			//		//	mm->updateDataMask(MeshModel::MM_FACECOLOR);
			//		//	mm->rmm.colorMode = vcg::GLW::CMPerFace;
			//		//	mm->rmm.textureMode = vcg::GLW::TMNone;*/
			//		//	for (int k = 0; k < 3; k++)
			//		//	{
			//		//		fi->V(k)->C().X() = (char)faceVertexColor.m_Colors[k].m_Red;
			//		//		fi->V(k)->C().Y() = (char)faceVertexColor.m_Colors[k].m_Green;
			//		//		fi->V(k)->C().Z() = (char)faceVertexColor.m_Colors[k].m_Blue;
			//		//		fi->V(k)->C().W() = (char)faceVertexColor.m_Colors[k].m_Alpha;
			//		//	}
			//		//	/*fi->C().X() = faceVertexColor.m_Colors[0].m_Red;
			//		//	fi->C().Y() = faceVertexColor.m_Colors[0].m_Green;
			//		//	fi->C().Z() = faceVertexColor.m_Colors[0].m_Blue;
			//		//	fi->C().W() = faceVertexColor.m_Colors[0].m_Alpha;*/
			//		//}
			//		//else
			//		{
			//			mm->Enable(tri::io::Mask::IOM_VERTCOLOR);
			//			mm->updateDataMask(MeshModel::MM_VERTCOLOR);
			//			mm->rmm.colorMode = vcg::GLW::CMPerVert;						

			//			for (int k = 0; k < 3; k++)
			//			{
			//				fi->V(k)->C().X() = (char)faceVertexColor.m_Colors[k].m_Red;
			//				fi->V(k)->C().Y() = (char)faceVertexColor.m_Colors[k].m_Green;
			//				fi->V(k)->C().Z() = (char)faceVertexColor.m_Colors[k].m_Blue;
			//				fi->V(k)->C().W() = (char)faceVertexColor.m_Colors[k].m_Alpha;
			//			}
			//		}
			//		
			//	}
			//}
			else if (pnPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_COLOR)
			{
				if (!(pPropertyHandler->GetColor(i, &faceVertexColor)))
				{
					mm->Enable(tri::io::Mask::IOM_VERTCOLOR);
					mm->updateDataMask(MeshModel::MM_VERTCOLOR);
					mm->rmm.colorMode = vcg::GLW::CMPerVert;
					//mm->rmm.textureMode = vcg::GLW::TMNone;

					for (int k = 0; k < 3; k++)
					{
						fi->V(k)->C().X() = (char)faceVertexColor.m_Colors[k].m_Red;
						fi->V(k)->C().Y() = (char)faceVertexColor.m_Colors[k].m_Green;
						fi->V(k)->C().Z() = (char)faceVertexColor.m_Colors[k].m_Blue;
						fi->V(k)->C().W() = (char)faceVertexColor.m_Colors[k].m_Alpha;
					}
				}
			}
			if (pnPropertyType == NMR::eModelPropertyType::MODELPROPERTYTYPE_TEXCOORD2D)
			{
				if (!(pPropertyHandler->GetTexture(i, &faceTexture)))
				{
					mm->Enable(tri::io::Mask::IOM_WEDGTEXCOORD);
					mm->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
					//mm->rmm.colorMode = vcg::GLW::CMNone;
					mm->rmm.setTextureMode(GLW::TMPerWedgeMulti);

					fi->WT(0).u() = faceTexture.m_fU[0];
					fi->WT(1).u() = faceTexture.m_fU[1];
					fi->WT(2).u() = faceTexture.m_fU[2];
					fi->WT(0).v() = faceTexture.m_fV[0];
					fi->WT(1).v() = faceTexture.m_fV[1];
					fi->WT(2).v() = faceTexture.m_fV[2];

					std::map<string, int>::iterator it;
					it = textureIndex.find(meshTextures.value(faceTexture.m_nTextureID));
					if (it == textureIndex.end())
					{
						textureIndex.insert(std::pair<string, int >(meshTextures.value(faceTexture.m_nTextureID), texIndex));
						mm->cm.textures.push_back(meshTextures.value(faceTexture.m_nTextureID));
						texIndex++;
					}
					fi->WT(0).n() = textureIndex.at(meshTextures.value(faceTexture.m_nTextureID));
					fi->WT(1).n() = textureIndex.at(meshTextures.value(faceTexture.m_nTextureID));
					fi->WT(2).n() = textureIndex.at(meshTextures.value(faceTexture.m_nTextureID));
				}
			}
			i++;
		}
	}
}

void ThreeMF_Manip::loadDefaultBaseMaterials(MeshModel *mm, ILib3MFDefaultPropertyHandler *pDefaultPropertyHandler)
{
	DWORD pMaterialGroupID;
	DWORD pMaterialIndex;

	pDefaultPropertyHandler->GetBaseMaterial(&pMaterialGroupID, &pMaterialIndex);

	QMap<int, Color4b> baseColors = baseMats.value(pMaterialGroupID);
	Color4b newColor = baseColors.value(pMaterialIndex);
	newColor[3] = 255;
	mm->updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTMARK);
	mm->Enable(tri::io::Mask::IOM_VERTCOLOR);
	mm->updateDataMask(MeshModel::MM_VERTCOLOR);
	//mm->updateDataMask(MeshModel::MM_COLOR);
	tri::InitVertexIMark(mm->cm);
	tri::UpdateColor<CMeshO>::PerVertexConstant(mm->cm, newColor);
	tri::UpdateNormal<CMeshO>::PerVertex(mm->cm);
	mm->cm.C() = newColor;
	mm->rmm.colorMode = vcg::GLW::CMPerVert;
	mm->rmm.textureMode = vcg::GLW::TMNone;
}

void ThreeMF_Manip::loadDefaultTexture(MeshModel *mm, ILib3MFDefaultPropertyHandler *pDefaultPropertyHandler)
{
	ModelResourceID pTextureID;
	FLOAT pfU, pfV;

	pDefaultPropertyHandler->GetTexture(&pTextureID, &pfU, &pfV);
	mm->Enable(tri::io::Mask::IOM_WEDGTEXCOORD);
	mm->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
	mm->rmm.colorMode = vcg::GLW::CMNone;
	mm->rmm.setTextureMode(GLW::TMPerWedgeMulti);

	std::map<string, int> textureIndex;
	textureIndex.clear();
	int texIndex = 0;

	for (CMeshO::FaceIterator fi = mm->cm.face.begin(); fi != mm->cm.face.end(); fi++)
	{
		fi->WT(0).u() = pfU;
		fi->WT(1).u() = pfU;
		fi->WT(2).u() = pfU;
		fi->WT(0).v() = pfV;
		fi->WT(1).v() = pfV;
		fi->WT(2).v() = pfV;

		std::map<string, int>::iterator it;
		it = textureIndex.find(meshTextures.value(pTextureID));
		if (it == textureIndex.end())
		{
			textureIndex.insert(std::pair<string, int >(meshTextures.value(pTextureID), texIndex));
			mm->cm.textures.push_back(meshTextures.value(pTextureID));
			texIndex++;
		}

		fi->WT(0).n() = textureIndex.at(meshTextures.value(pTextureID));
		fi->WT(1).n() = textureIndex.at(meshTextures.value(pTextureID));
		fi->WT(2).n() = textureIndex.at(meshTextures.value(pTextureID));
	}
}

bool ThreeMF_Manip::open_3mf(MainWindow *mw, MeshDocument *md, LPCWSTR project_name, CallBackPos *cb, bool isImport)
{
	// General Variables
	HRESULT hResult;
	DWORD nInterfaceVersion, v2, v3;
	BOOL pbHasNext;
	DWORD nErrorMsg;
	LPCSTR pszErrorMessage;

	// Objects
	CComPtr<ILib3MFModel> pModel;
	CComPtr<ILib3MFModelFactory> pFactory;
	CComPtr<ILib3MFModelReader> p3MFReader;
	CComPtr<ILib3MFModelResourceIterator> pBaseResourceIterator;
	CComPtr<ILib3MFModelResourceIterator> pTextureResourceIterator;
	CComPtr<ILib3MFModelResourceIterator> pObjectResourceIterator;
	CComPtr<ILib3MFModelResourceIterator> pObjectResourceIterator2;



	CComPtr<ILib3MFModelBuildItemIterator> pBuildItemIterator;

	CoInitialize(NULL);
	NMRCreateModelFactory(&pFactory);
	pFactory->GetInterfaceVersion(&nInterfaceVersion);
	pFactory->CreateModel(&pModel);

	// Read 3MF file
	QFileInfo fInfo = QString::fromStdWString(project_name);
	projectName = fInfo.fileName();
	wstring sFilename(project_name);
	string sAnsiFilename(sFilename.begin(), sFilename.end());

	// Create Model Writer
	hResult = pModel->QueryReader("3mf", &p3MFReader);
	if (hResult != S_OK) return false;

	hResult = p3MFReader->ReadFromFile(sFilename.c_str());
	if (hResult != S_OK)
		return false;

#pragma region material
	// Iterate through all base materials
	assert(&pBaseResourceIterator);
	hResult = pModel->GetBaseMaterials(&pBaseResourceIterator);
	if (hResult == S_OK)
	{
		hResult = pBaseResourceIterator->MoveNext(&pbHasNext);
		if (hResult == S_OK)
		{
			//#pragma region pbHasNext1
			while (pbHasNext)
			{
				CComPtr<ILib3MFModelResource> pResource;
				ModelResourceID ResourceID;
				ILib3MFModelBaseMaterial *pBaseMaterial;

				hResult = pBaseResourceIterator->GetCurrent(&pResource);
				if (hResult != S_OK) return false;

				hResult = pResource->GetResourceID(&ResourceID);
				if (hResult != S_OK) return false;

				hResult = pModel->GetBaseMaterialByID(ResourceID, &pBaseMaterial);
				if (hResult != S_OK) return false;

				DWORD baseCount;
				pBaseMaterial->GetCount(&baseCount);

				QMap<int, Color4b> baseColor;
				for (int i = 0; i < baseCount; i++)
				{
					BYTE r, g, b, alpha;
					pBaseMaterial->GetDisplayColor((DWORD)i, &r, &g, &b, &alpha);

					Color4b color = Color4b(r, g, b, alpha);
					baseColor.insert(i, color);
				}

				baseMats.insert(ResourceID, baseColor);
				pBaseResourceIterator->MoveNext(&pbHasNext);
			}
		}
	}
#pragma endregion material
#pragma region texture
	// Iterate through all textures
	assert(&pTextureResourceIterator);
	hResult = pModel->Get2DTextures(&pTextureResourceIterator);
	if (hResult == S_OK)
	{
		hResult = pTextureResourceIterator->MoveNext(&pbHasNext);
		if (hResult == S_OK)
		{
			QString temppath = PicaApplication::getRoamingDir();
			QString foldername = "temptexture";
			QDir tempDir(temppath);

			if (!QDir(temppath + "/" + foldername).exists())
			{
				tempDir.mkpath(foldername);
			}

			tempDir.setPath(temppath + "/" + foldername);
			meshTextures.clear();

			while (pbHasNext)
			{
				CComPtr<ILib3MFModelResource> pResource;
				ModelResourceID ResourceID;
				ILib3MFModelTexture2D *pTexture2D;
				char texturePath[150];
				memset(texturePath, 0, 150);
				ULONG neededChars;
				ULONG bufferSize = 150;

				hResult = pTextureResourceIterator->GetCurrent(&pResource);
				if (hResult != S_OK) return false;

				pResource->GetResourceID(&ResourceID);
				if (hResult != S_OK) return false;

				hResult = pModel->GetTexture2DByID(ResourceID, &pTexture2D);
				if (hResult != S_OK) return false;

				hResult = pTexture2D->GetPathUTF8(texturePath, bufferSize, &neededChars);
				if (hResult != S_OK) return false;

				QFileInfo tempfileinfo((QString)texturePath);
				eModelTexture2DType eType;
				pTexture2D->GetContentType(&eType);
				QString imgPath = tempfileinfo.filePath();
				QString path = imgPath.remove(imgPath.lastIndexOf("."), tempfileinfo.suffix().length() + 1);

				switch (eType)
				{
				case 0: path = tempfileinfo.filePath(); break;
				case 1: path = path.append(".png"); break;
				case 2: path = path.append(".jpg"); break;
				default: path = tempfileinfo.filePath(); break;
				}

				QFileInfo tempTexture(path);
				QString tempTexturePath = tempDir.path() + "/" + tempTexture.fileName();

				hResult = pTexture2D->WriteToFileUTF8(tempTexturePath.toStdString().c_str());
				if (hResult != S_OK) return false;

				//meshTextures.insert(ResourceID, tempTexture.fileName().toStdString());
				meshTextures.insert(ResourceID, tempTexturePath.toStdString().c_str());
				pTextureResourceIterator->MoveNext(&pbHasNext);
			}
		}
	}
#pragma endregion texture

	int objectCounter = 1;
	int numOfObject = 1;
	meshObjectPerID.clear();
	meshComponentPerID.clear();

	assert(&pObjectResourceIterator);
	hResult = pModel->GetObjects(&pObjectResourceIterator);
	hResult = pModel->GetObjects(&pObjectResourceIterator2);
	if (hResult != S_OK) return false;

	hResult = pObjectResourceIterator2->MoveNext(&pbHasNext);
	while (pbHasNext)
	{
		hResult = pObjectResourceIterator2->MoveNext(&pbHasNext);
		numOfObject++;
	}

	hResult = pObjectResourceIterator->MoveNext(&pbHasNext);
	if (hResult != S_OK) return false;

	for (int z = 1; z < numOfObject; z++)
		//while (pbHasNext)
	{
		CComPtr<ILib3MFModelResource> pResource;
		DWORD pResourceID;
		ILib3MFModelComponentsObject *pComponentsObject;
		ILib3MFModelMeshObject *pMeshObject;

		hResult = pObjectResourceIterator->GetCurrent(&pResource);
		if (hResult != S_OK) return false;

		pResource->GetResourceID(&pResourceID);

		hResult = pResource->QueryInterface(IID_Lib3MF_ModelMeshObject, (void **)&pMeshObject);
		if (hResult == S_OK)
		{
			hResult = addMesh(md, pMeshObject, z);
			if (hResult != S_OK) return false;
		}

		hResult = pResource->QueryInterface(IID_Lib3MF_ModelComponentsObject, (void **)&pComponentsObject);
		if (hResult == S_OK)
		{
			hResult = addComponent(md, pComponentsObject, z);
			if (hResult != S_OK) return false;
		}

		//objectCounter++;
		hResult = pObjectResourceIterator->MoveNext(&pbHasNext);

		if (cb != NULL)
		{
			if (!(*cb)((100 * z) / numOfObject - 1, "open model"))
			{
				return false;
			}
		}
	}

	//pModel->GetMeshObjects();


	QList<int> printedMesh;

	hResult = pModel->GetBuildItems(&pBuildItemIterator);
	if (hResult != S_OK) return false;

	hResult = pBuildItemIterator->MoveNext(&pbHasNext);
	if (hResult != S_OK) return false;

	QList<int> imported3mfList;
	while (pbHasNext)
	{
		DWORD ResourceID;
		MODELTRANSFORM pTransform;
		CComPtr<ILib3MFModelBuildItem> pBuildItem;
		CComPtr<ILib3MFModelObjectResource> pObjectResource;
		BOOL bHasTransform;

		hResult = pBuildItemIterator->GetCurrent(&pBuildItem);
		if (hResult != S_OK) return false;

		hResult = pBuildItem->GetObjectResource(&pObjectResource);
		if (hResult != S_OK) return false;

		hResult = pObjectResource->GetResourceID(&ResourceID);
		if (hResult != S_OK) return false;

		MeshModel *mm = new MeshModel(md, "", meshObjectPerID.value(ResourceID)->label());
		mm->updateDataMask(meshObjectPerID.value(ResourceID)->dataMask());
		vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(mm->cm, meshObjectPerID.value(ResourceID)->cm);
		mm->cm.Tr = meshObjectPerID.value(ResourceID)->cm.Tr;
		mm->rmm = meshObjectPerID.value(ResourceID)->rmm;
		mm->setMeshSort(meshObjectPerID.value(ResourceID)->getMeshSort());
		mm->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
		//test vbo
		//if (mm->cm.textures.size() >= 0 /*&& mm->cm.textures.size() < 2*/ /*&& mm->rmm.colorMode != vcg::GLW::ColorMode::CMPerFace*/)
		//{
		//	mm->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
		//	//mw->GLA()->refreshVBO();
		//}

		hResult = pBuildItem->HasObjectTransform(&bHasTransform);
		if (hResult != S_OK) return false;

		if (bHasTransform)
		{
			hResult = pBuildItem->GetObjectTransform(&pTransform);
			if (hResult != S_OK) return false;

			Matrix44m mat;
			mat.SetIdentity();
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					mat.ElementAt(i, j) = pTransform.m_fFields[i][j];
				}
			}

			CMeshO::VertexIterator vi = mm->cm.vert.begin();
			//tri::UpdatePosition<CMeshO>::Matrix(mm->cm, mat, true);
			for (vi; vi != mm->cm.vert.end(); vi++)
			{
				Point4f vert = Point4f(vi->P().X(), vi->P().Y(), vi->P().Z(), 1.0);
				vert = mat * vert;
				vi->P().X() = vert.X();
				vi->P().Y() = vert.Y();
				vi->P().Z() = vert.Z();
			}
		}

		mm->is3mf = true;
		//tri::Clean<CMeshO>::RemoveDuplicateVertex(mm->cm);
		md->meshList.push_back(mm);

		MeshModel *forResetMesh = new MeshModel(md, "", mm->label());
		forResetMesh->updateDataMask(mm->dataMask());
		vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(forResetMesh->cm, mm->cm);
		forResetMesh->cm.Tr = mm->cm.Tr;
		forResetMesh->rmm = mm->rmm;
		forResetMesh->is3mf = true;
		forResetMesh->setMeshSort(mm->getMeshSort());

		forResetMesh->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
		//test vbo
		//if (/*forResetMesh->glw.TMIdd[0].size() < 2 &&*/ forResetMesh->rmm.colorMode != vcg::GLW::ColorMode::CMPerFace)
		//{
		//	forResetMesh->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
		//	//mw->GLA()->refreshVBO();
		//}



		md->resetMeshList.insert(mm->id(), forResetMesh);
		emit md->meshSetChanged();
		emit md->meshAdded(mm->id(), mm->rmm);
		md->setCurrentMesh(mm->id());

		if (isImport == true)
		{
			imported3mfList.push_back(mm->id());
		}

		if (!printedMesh.contains(ResourceID))
		{
			printedMesh.push_back(ResourceID);
		}

		hResult = pBuildItemIterator->MoveNext(&pbHasNext);
		if (hResult != S_OK) return false;
	}

	if (!printedMesh.isEmpty())
	{
		foreach(int i, printedMesh)
		{
			meshObjectPerID.remove(i);
		}
	}

	if (!meshObjectPerID.isEmpty())
	{
		//SYDNY 10/04/2017---------------------
		if (md->multiSelectID.count() != NULL)
		{
			md->multiSelectID.clear();
		}

		foreach(MeshModel *mm, meshObjectPerID)
		{
			mm->is3mf = true;
			//tri::Clean<CMeshO>::RemoveDuplicateVertex(mm->cm);
			md->meshList.push_back(mm);

			//test vbo
			mm->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
			//if (mm->cm.textures.size() >= 0 /*&& mm->cm.textures.size() < 2*/ && mm->rmm.colorMode != vcg::GLW::ColorMode::CMPerFace)
			//{
			//	
			//	//mw->GLA()->refreshVBO();
			//}

			MeshModel *forResetMesh = new MeshModel(md, "", mm->label());
			forResetMesh->updateDataMask(mm->dataMask());
			vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(forResetMesh->cm, mm->cm);
			forResetMesh->cm.Tr = mm->cm.Tr;
			forResetMesh->rmm = mm->rmm;
			forResetMesh->setMeshSort(mm->getMeshSort());
			forResetMesh->is3mf = true;
			forResetMesh->glw.SetHint(vcg::GLW::Hint::HNUseVBO);

			//if ( /*forResetMesh->glw.TMIdd[0].size() < 2 &&*/ mm->rmm.colorMode != vcg::GLW::ColorMode::CMPerFace)
			//{
			//	
			//	//mw->GLA()->refreshVBO();
			//}

			md->resetMeshList.insert(mm->id(), forResetMesh);
			emit md->meshSetChanged();
			emit md->meshAdded(mm->id(), mm->rmm);
			md->setCurrentMesh(mm->id());

			if (isImport == true)
			{
				imported3mfList.push_back(mm->id());


			}
			//SYDNY 10/04/2017---------------
			md->multiSelectID.insert(mm->id());
		}
	}

	if (isImport == true)
		mw->currentViewContainer()->undoStack->push(new importFrom_command(mw, imported3mfList, false));

	return true;
}

