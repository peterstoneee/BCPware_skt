#include "printbase.h"

//PrintBase::PrintBase(QObject *parent) :QObject(parent)
//{
//}
QList<MeshIOInterface::Format> PrintBase::importFormats() const
{
	QList<Format> formatList;

	return formatList;
}
QList<MeshIOInterface::Format> PrintBase::exportFormats()const
{
	QList<Format> formatList;	
	return formatList;
}
void PrintBase::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{}

bool PrintBase::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget * /*parent*/)
{
	

	return true;
}

bool PrintBase::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet & par, CallBackPos *cb, QWidget */*parent*/)
{
	
	return true;
}

//void PrintBase::initPreOpenParameter(const QString &formatName, const QString &/*filename*/, RichParameterSet &parlst)
//{
//	if (formatName.toUpper() == tr("PTX"))
//	{
//		parlst.addParam(new RichInt("meshindex", 0, "Index of Range Map to be Imported",
//			"PTX files may contain more than one range map. 0 is the first range map. If the number if higher than the actual mesh number, the import will fail"));
//		parlst.addParam(new RichBool("anglecull", true, "Cull faces by angle", "short"));
//		parlst.addParam(new RichFloat("angle", 85.0, "Angle limit for face culling", "short"));
//		parlst.addParam(new RichBool("usecolor", true, "import color", "Read color from PTX, if color is not present, uses reflectance instead"));
//		parlst.addParam(new RichBool("pointcull", true, "delete unsampled points", "Deletes unsampled points in the grid that are normally located in [0,0,0]"));
//		parlst.addParam(new RichBool("pointsonly", false, "Keep only points", "Import points a point cloud only, with radius and normals, no triangulation involved, isolated points and points with normals with steep angles are removed."));
//		parlst.addParam(new RichBool("switchside", false, "Swap rows/columns", "On some PTX, the rows and columns number are switched over"));
//		parlst.addParam(new RichBool("flipfaces", false, "Flip all faces", "Flip the orientation of all the triangles"));
//	}
//}

//void PrintBase::applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par)
//{
//	if (format.toUpper() == tr("STL"))
//	if (par.findParameter("Unify")->val->getBool())
//		tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
//}
//void PrintBase::initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par)
//{
//	if (format.toUpper() == tr("STL"))
//		par.addParam(new RichBool("Unify", true, "Unify Duplicated Vertices",
//		"The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified"));
//}
//void PrintBase::initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par)
//{
//	if (format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
//		par.addParam(new RichBool("Binary", true, "Binary encoding",
//		"Save the mesh using a binary encoding. If false the mesh is saved in a plain, readable ascii format"));
//	if (format.toUpper() == tr("STL"))
//		par.addParam(new RichBool("ColorMode", true, "Materialise Color Encoding",
//		"Save the color using a binary encoding according to the Materialise's Magic style (e.g. RGB coding instead of BGR coding)"));
//
//}
MESHLAB_PLUGIN_NAME_EXPORTER(PrintBase)
