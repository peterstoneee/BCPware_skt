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
#ifndef THREEMF_MANIP_H
#define THREEMF_MANIP_H

#define _WIN32_DCOM

#include <atlbase.h>
#include "mainwindow.h"
#include "NMR_COMFactory.h"
#include "NMR_DLLInterfaces.h"
//#include "./model/com/NMR_COMFactory.h"
//#include "./model/com/NMR_DLLInterfaces.h"

using namespace NMR;

class ThreeMF_Manip
{
public:
	ThreeMF_Manip() {}
	~ThreeMF_Manip() {}
	bool save_3mf(MeshDocument *md, LPCWSTR project_name, CallBackPos *cb = 0);
	bool open_3mf(MainWindow *mw, MeshDocument *md, LPCWSTR project_name, CallBackPos *cb = 0, bool isImport = false);

	//template <class OpenMeshType>
	//bool Open3mf(OpenMeshType &m, const char * filename)
	//{
	//	typedef typename OpenMeshType::VertexPointer VertexPointer;
	//	typedef typename OpenMeshType::ScalarType ScalarType;
	//	typedef typename OpenMeshType::VertexType VertexType;
	//	typedef typename OpenMeshType::EdgeType   EdgeType;
	//	typedef typename OpenMeshType::FaceType FaceType;
	//	typedef typename OpenMeshType::VertexIterator VertexIterator;
	//	typedef typename OpenMeshType::FaceIterator FaceIterator;
	//	typedef typename OpenMeshType::CoordType CoordType;

	//	// General Variables
	//	HRESULT hResult;
	//	DWORD nInterfaceVersion;
	//	BOOL pbHasNext;

	//	// Objects
	//	CComPtr<ILib3MFModel> pModel;
	//	CComPtr<ILib3MFModelFactory> pFactory;
	//	CComPtr<ILib3MFModelReader> p3MFReader;
	//	CComPtr<ILib3MFModelBuildItemIterator> pBuildItemIterator;
	//	CComPtr<ILib3MFModelResourceIterator> pResourceIterator;


	//	// Initialize COM
	//	CoUninitialize();
	//	hResult = CoInitializeEx(nullptr, 0);
	//	if (hResult != S_OK) {
	//		qDebug() << "could not initialize COM: " << std::hex << hResult ;
	//		return -1;
	//	}

	//	// Create Factory Object
	//	hResult = NMRCreateModelFactory(&pFactory);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get Model Factory: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}


	//	//// Check 3MF Library Version
	//	/*hResult = pFactory->GetInterfaceVersion(&nInterfaceVersion);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get 3MF Library version: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	if ((nInterfaceVersion != NMR_APIVERSION_INTERFACE)) {
	//		std::cout << "invalid 3MF Library version: " << NMR_APIVERSION_INTERFACE << std::endl;
	//		return -1;
	//	}*/

	//	// Create Model Instance
	//	hResult = pFactory->CreateModel(&pModel);
	//	if (hResult != S_OK) {
	//		std::cout << "could not create model: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}
	//	
	//	std::string str = filename;
	//	std::wstring sFilename(str.length(), L' '); // Make room for characters
	//	// Copy string to wstring.
	//	std::copy(str.begin(), str.end(), sFilename.begin());


	//	// Read 3MF file
	//	//std::wstring sFilename(filename);
	//	std::string sAnsiFilename(filename);


	//	// Create Model Writer
	//	hResult = pModel->QueryReader("3mf", &p3MFReader);
	//	if (hResult != S_OK) {
	//		std::cout << "could not create model reader: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	// Import Model from 3MF File
	//	std::cout << "reading " << sAnsiFilename << "..." << std::endl;
	//	hResult = p3MFReader->ReadFromFile(sFilename.c_str());
	//	if (hResult != S_OK) {
	//		std::cout << "could not read file: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	// Iterate through all the Objects
	//	hResult = pModel->GetObjects(&pResourceIterator);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get object: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	hResult = pResourceIterator->MoveNext(&pbHasNext);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get next object: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	
	//	while (pbHasNext)
	//	{
	//		CComPtr<ILib3MFModelResource> pResource;
	//		ILib3MFModelMeshObject * pMeshObject;
	//		ILib3MFModelComponentsObject * pComponentsObject;
	//		ModelResourceID ResourceID;

	//		// get current resource
	//		hResult = pResourceIterator->GetCurrent(&pResource);
	//		if (hResult != S_OK) {
	//			std::cout << "could not get resource: " << std::hex << hResult << std::endl;
	//			return -1;
	//		}

	//		// get resource ID
	//		hResult = pResource->GetResourceID(&ResourceID);
	//		if (hResult != S_OK) {
	//			std::cout << "could not get resource id: " << std::hex << hResult << std::endl;
	//			return -1;
	//		}

	//		// Query mesh interface
	//		hResult = pResource->QueryInterface(IID_Lib3MF_ModelMeshObject, (void **)&pMeshObject);
	//		if (hResult == S_OK)
	//		{
	//			DWORD nVertexCount, nTriangleCount;

	//			// Retrieve Mesh Vertex Count
	//			hResult = pMeshObject->GetVertexCount(&nVertexCount);
	//			if (hResult != S_OK)return -1;

	//			// Retrieve Mesh face Count
	//			pMeshObject->GetTriangleCount(&nTriangleCount);

	//			VertexIterator vi = vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertexCount);
	//			FaceIterator fi = vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nTriangleCount);
	//			
	//			DWORD  vnum = 0;
	//			DWORD  num = 0;

	//			//Retrieve Mesh Vertex
	//			//MODELMESHVERTEX *pVertices = (MODELMESHVERTEX *)malloc(nVertexCount * 12);
	//			//MODELMESHVERTEX pVertices[nVertexCount];				
	//			/*for (int f = 0; f < nVertexCount; f++)
	//				pVertices[f] = *new MODELMESHVERTEX;*/
	//			
	//			
	//			//pMeshObject->GetVertices(pVertices, nVertexCount, &vnum);
	//			VertexPointer  *ivp = new VertexPointer[nVertexCount];

	//			for (int v = 0; v < nVertexCount; v++)
	//			{
	//				MODELMESHVERTEX pVertex;
	//				pMeshObject->GetVertex(v, &pVertex);

	//				(*vi).P().X() = pVertex.m_fPosition[0];
	//				(*vi).P().Y() = pVertex.m_fPosition[1];
	//				(*vi).P().Z() = pVertex.m_fPosition[2];
	//				ivp[v] = &*vi;					
	//				++vi;
	//			}
	//			
	//			for (int t = 0; t < nTriangleCount; t++)
	//			{
	//				MODELMESHTRIANGLE pTriangle;
	//				pMeshObject->GetTriangle(t, &pTriangle);

	//				for (int n = 0; n < 3; n++)
	//				{
	//					(*fi).V(n) = ivp[pTriangle.m_nIndices[n]];								
	//				}
	//				//(*fi).N() = vcg::Point3f(0, 1, 0);
	//				++fi;
	//			}

	//			///*****************************************************


	//		}



	//		// Query component interface
	//		//hResult = pResource->QueryInterface(IID_Lib3MF_ModelComponentsObject, (void **)&pComponentsObject);
	//		//if (hResult == S_OK)
	//		//{
	//		//	std::cout << "------------------------------------------------------" << std::endl;
	//		//	std::cout << "component object #" << ResourceID << ": " << std::endl;

	//		//	// Show Component Object Information
	//		//	hResult = ShowComponentsObjectInformation(pComponentsObject);
	//		//	if (hResult != S_OK)
	//		//		return -1;
	//		//}

	//		hResult = pResourceIterator->MoveNext(&pbHasNext);
	//		if (hResult != S_OK)
	//		{
	//			std::cout << "could not get next object: " << std::hex << hResult << std::endl;
	//			return -1;
	//		}
	//	}


	//	// Iterate through all the Build items
	//	hResult = pModel->GetBuildItems(&pBuildItemIterator);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get build items: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	hResult = pBuildItemIterator->MoveNext(&pbHasNext);
	//	if (hResult != S_OK) {
	//		std::cout << "could not get next build item: " << std::hex << hResult << std::endl;
	//		return -1;
	//	}

	//	//while (pbHasNext)
	//	//{

	//	//	DWORD ResourceID;
	//	//	MODELTRANSFORM Transform;
	//	//	CComPtr<ILib3MFModelBuildItem> pBuildItem;
	//	//	// Retrieve Build Item
	//	//	hResult = pBuildItemIterator->GetCurrent(&pBuildItem);
	//	//	if (hResult != S_OK) {
	//	//		std::cout << "could not get build item: " << std::hex << hResult << std::endl;
	//	//		return -1;
	//	//	}

	//	//	// Retrieve Resource
	//	//	CComPtr<ILib3MFModelObjectResource> pObjectResource;
	//	//	hResult = pBuildItem->GetObjectResource(&pObjectResource);
	//	//	if (hResult != S_OK) {
	//	//		std::cout << "could not get build item resource: " << std::hex << hResult << std::endl;
	//	//		return -1;
	//	//	}

	//	//	// Retrieve Resource ID
	//	//	hResult = pObjectResource->GetResourceID(&ResourceID);
	//	//	if (hResult != S_OK) {
	//	//		std::cout << "could not get resource id: " << std::hex << hResult << std::endl;
	//	//		return -1;
	//	//	}

	//	//	// Output
	//	//	std::cout << "------------------------------------------------------" << std::endl;
	//	//	std::cout << "Build item (Object #" << ResourceID << "): " << std::endl;

	//	//	// Check Object Transform
	//	//	BOOL bHasTransform;
	//	//	hResult = pBuildItem->HasObjectTransform(&bHasTransform);
	//	//	if (hResult != S_OK) {
	//	//		std::cout << "could not get check object transform: " << std::hex << hResult << std::endl;
	//	//		return -1;
	//	//	}

	//	//	if (bHasTransform) {
	//	//		// Retrieve Transform
	//	//		hResult = pBuildItem->GetObjectTransform(&Transform);
	//	//		if (hResult != S_OK) {
	//	//			std::cout << "could not get object transform: " << std::hex << hResult << std::endl;
	//	//			return -1;
	//	//		}

	//	//		std::cout << "   Transformation:  [ " << Transform.m_fFields[0][0] << " " << Transform.m_fFields[0][1] << " " << Transform.m_fFields[0][2] << " " << Transform.m_fFields[0][3] << " ]" << std::endl;
	//	//		std::cout << "                    [ " << Transform.m_fFields[1][0] << " " << Transform.m_fFields[1][1] << " " << Transform.m_fFields[1][2] << " " << Transform.m_fFields[1][3] << " ]" << std::endl;
	//	//		std::cout << "                    [ " << Transform.m_fFields[2][0] << " " << Transform.m_fFields[2][1] << " " << Transform.m_fFields[2][2] << " " << Transform.m_fFields[2][3] << " ]" << std::endl;
	//	//	}
	//	//	else {
	//	//		std::cout << "   Transformation:  none" << std::endl;

	//	//	}

	//	//	// Retrieve Mesh Part Number Length
	//	//	std::wstring sPartNumber;
	//	//	DWORD nNeededChars;
	//	//	hResult = pBuildItem->GetPartNumber(nullptr, 0, &nNeededChars);
	//	//	if (hResult != S_OK)
	//	//		return hResult;

	//	//	// Retrieve Mesh Name
	//	//	if (nNeededChars > 0) {
	//	//		std::vector<wchar_t> pBuffer;
	//	//		pBuffer.resize(nNeededChars + 1);
	//	//		hResult = pBuildItem->GetPartNumber(&pBuffer[0], nNeededChars + 1, nullptr);
	//	//		pBuffer[nNeededChars] = 0;
	//	//		sPartNumber = std::wstring(&pBuffer[0]);
	//	//	}

	//	//	// Output Part number in local codepage
	//	//	std::string sAnsiPartNumber(sPartNumber.begin(), sPartNumber.end());
	//	//	std::cout << "   Part number:     \"" << sAnsiPartNumber << "\"" << std::endl;

	//	//	hResult = pBuildItemIterator->MoveNext(&pbHasNext);
	//	//	if (hResult != S_OK) {
	//	//		std::cout << "could not get next build item: " << std::hex << hResult << std::endl;
	//	//		return -1;
	//	//	}
	//	//}


	//	return 0;
	//}

private:
	HRESULT loadModelTexture(ILib3MFModel *pModel, const char *imgPath, const char *imgFile, eModelTexture2DType eType, DWORD *pTextureID);
	HRESULT loadModelTexture(ILib3MFModel *pModel, const char *imgPath, QByteArray *imgFile, eModelTexture2DType eType, DWORD *pTextureID);
	eModelTexture2DType getModelTextureType(string imgFile);
	CComPtr<ILib3MFModel> create_3mf(MeshDocument *md, CallBackPos *cb = 0);
	HRESULT addMesh(MeshDocument *md, ILib3MFModelMeshObject *pMesh, int objectCounter);
	HRESULT addComponent(MeshDocument *md, ILib3MFModelComponentsObject *pComponentsObject, int objectCounter);
	QString getName(ILib3MFModelMeshObject *pMeshObject, int objectCounter);
	QString getName(ILib3MFModelComponentsObject *pComponentsObject, int objectCounter);
	void getVertices(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject);
	void getFacets(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject);
	void getProperties(MeshModel *mm, ILib3MFModelMeshObject *pMeshObject);
	void loadDefaultBaseMaterials(MeshModel *mm, ILib3MFDefaultPropertyHandler *pDefaultPropertyHandler);
	void loadDefaultTexture(MeshModel *mm, ILib3MFDefaultPropertyHandler *pDefaultPropertyHandler);
	int meshColorPerMode(MeshModel *mm);
	//LPCWSTR* genName(int i);

	MeshDocument *md;
	QString projectName;
	QMap<int, Point3f> vertexID;
	QMap<int, std::string> meshTextures;
	QMap<int, QMap<int, Color4b>> baseMats;
	QMap<DWORD, MeshModel *> meshObjectPerID;
	QMap<DWORD, QList<MeshModel*>> meshComponentPerID;

};
inline uint qHash(const Point4<byte> &tag, uint seed) {
	return qHash(tag.V(), seed ^ 0xa03f);
}
#endif // THREEMF_MANIP_H