#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include <QtXml>

#include "meshmodel.h"
#include "meshlabdocumentxml.h"
#include <wrap/qt/shot_qt.h>

#include "QString"
#include "QFile"
#include "QDir"

bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit)
{
	QDir dir;
	dir.setPath(from_dir);

	from_dir += QDir::separator();
	to_dir += QDir::separator();

	foreach(QString copy_file, dir.entryList(QDir::Files))
	{
		QString from = from_dir + copy_file;
		QString to = to_dir + copy_file;

		if (QFile::exists(to))
		{
			if (replace_on_conflit)
			{
				if (QFile::remove(to) == false)
				{
					return false;
				}
			}
			else
			{
				continue;
			}
		}

		if (QFile::copy(from, to) == false)
		{
			return false;
		}
	}

	foreach(QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString from = from_dir + copy_dir;
		QString to = to_dir + copy_dir;

		if (dir.mkpath(to) == false)
		{
			return false;
		}

		if (copy_dir_recursive(from, to, replace_on_conflit) == false)
		{
			return false;
		}
	}

	return true;
}



bool createFolderSaveProject(MeshDocument &md, QStringList &path)
{
	//saveDialog in folder
	//create folder

	//QString foledername="test";
	QString foledername = path.at(0);
	qDebug() << "foldername" << foledername;
	QString savedir = path.at(1);
	qDebug() << "savedir" << savedir;
	//QDir saveFileDir("D:/model/MLP_project/");		
	QDir saveFileDir(savedir);
	qDebug() << "saveFileDir.currentPath()" << saveFileDir.path();
	saveFileDir.mkpath(foledername);
	saveFileDir.setPath(savedir + "/" + foledername);


	foreach(MeshModel *tmm, md.meshList)
	{
		//QString tmmCName = "/" + tmmFileinfo.fileName();//save the copied file path

		QString ms = tmm->fullName();
		QFileInfo tmmFileinfo(ms);//mesh path which is in computer

		QString tmmMtlName =  tmmFileinfo.baseName() + ".mtl";

		QDir tmmPath2(tmmFileinfo.absoluteDir());
		if (tmmPath2.exists(tmmMtlName))
		{
			QString mtlpath = tmmPath2.filePath(tmmMtlName);

			QFile::copy(mtlpath, saveFileDir.filePath(tmmMtlName));
		}		
		QFile::copy(ms, saveFileDir.filePath(tmmFileinfo.fileName()));
		foreach(string ss, tmm->cm.textures)
		{			
			QDir tmmpath(tmmFileinfo.absoluteDir());			
			if (tmmpath.exists(QString::fromStdString(ss)))
			{
				tmmpath.setPath(tmmFileinfo.absolutePath() + "/" + QString::fromStdString(ss));

				QString texturFullPath = tmmpath.cleanPath(tmmpath.absolutePath());//texture full path			
				QFileInfo tex(texturFullPath);
				QString tmmCName = "/" + tex.fileName();//texture file name

				QFile::copy(texturFullPath, saveFileDir.path().append(tmmCName));		
			}
		}
	}
	//QFile::copy("D:/model/model/stl/big_cube.stl", saveFileDir.currentPath().append("/big_cube.stl"));

	//copy mesh files
	//save new absolute path to project
	return true;


}

//sydny
bool createFolderSaveProject2(MeshDocument &md, QStringList path)
{
	//saveDialog in folder
	//create folder

	////QString foledername="test";
	QString foledername = path[0].section('/', -1).remove(".pip");
	QString savedir = path[0].remove(foledername).remove(".pip");
	qDebug() << "foldername" << foledername;
	qDebug() << "savedir" << savedir;
	//QDir saveFileDir("D:/model/MLP_project/");		
	QDir saveFileDir(savedir + "/");
	qDebug() << "saveFileDir.currentPath()" << saveFileDir.path();
	saveFileDir.mkpath(foledername);
	saveFileDir.setPath(savedir + "/" + foledername);


	foreach(MeshModel *tmm, md.meshList)
	{
		//QString tmmCName = "/" + tmmFileinfo.fileName();//save the copied file path

		QString ms = tmm->fullName();
		QFileInfo tmmFileinfo(ms);//mesh path which is in computer

		QString tmmMtlName = tmmFileinfo.baseName() + ".mtl";

		QDir tmmPath2(tmmFileinfo.absoluteDir());
		if (tmmPath2.exists(tmmMtlName))
		{
			QString mtlpath = tmmPath2.filePath(tmmMtlName);

			QFile::copy(mtlpath, saveFileDir.filePath(tmmMtlName));
		}
		QFile::copy(ms, saveFileDir.filePath(tmmFileinfo.fileName()));
		foreach(string ss, tmm->cm.textures)
		{
			QDir tmmpath(tmmFileinfo.absoluteDir());
			if (tmmpath.exists(QString::fromStdString(ss)))
			{
				tmmpath.setPath(tmmFileinfo.absolutePath() + "/" + QString::fromStdString(ss));

				QString texturFullPath = tmmpath.cleanPath(tmmpath.absolutePath());//texture full path			
				QFileInfo tex(texturFullPath);
				QString tmmCName = "/" + tex.fileName();//texture file name

				QFile::copy(texturFullPath, saveFileDir.path().append(tmmCName));
			}
		}
	}
	//QFile::copy("D:/model/model/stl/big_cube.stl", saveFileDir.currentPath().append("/big_cube.stl"));

	//copy mesh files
	//save new absolute path to project
	return true;
}


bool MeshDocumentToXMLFile(MeshDocument &md, QString filename, bool onlyVisibleLayers)
{
	md.setFileName(filename);
	qDebug() << "filename" << filename;
	QFileInfo fi(filename);
	QDir tmpDir = QDir::current();
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	QDomDocument doc = MeshDocumentToXML(md, onlyVisibleLayers);
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream, 1);
	file.close();
	QDir::setCurrent(tmpDir.absolutePath());
	return true;
}

QDomElement Matrix44mToXML(Matrix44m &m, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QString Row[4];
	for (int i = 0; i < 4; ++i)
		Row[i] = QString("%1 %2 %3 %4 \n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

	QDomText nd = doc.createTextNode("\n" + Row[0] + Row[1] + Row[2] + Row[3]);
	matrixElem.appendChild(nd);

	return matrixElem;
}

//SYDNY 02/17/2017 added new class that will get the color values from each mesh
QDomElement ColorToXML(Color4b &m, QDomDocument &doc)
{
		QDomElement colorElem = doc.createElement("Color");
		QDomText cd = doc.createTextNode(QString::number(m.X()) + " " + QString::number(m.Y()) + " " + QString::number(m.Z()) + " " + QString::number(m.W()));
		colorElem.appendChild(cd);
		return colorElem;
} 

//SYDNY 04/27/2017
QDomElement ColorToXMLWhite(Color4b &m, QDomDocument &doc)
{
	QDomElement colorElem = doc.createElement("Color");
	QDomText cd = doc.createTextNode(QString::number(m.W()) + " " + QString::number(m.W()) + " " + QString::number(m.W()) + " " + QString::number(m.W()));
	colorElem.appendChild(cd);
	return colorElem;
}

QDomElement MeshModelToXML(MeshModel *mp, QDomDocument &doc)
{
	QDomElement meshElem = doc.createElement("PiMesh");
	meshElem.setAttribute("label", mp->label());
	//meshElem.setAttribute("filename", "./" + mp->label());//***20160616_test save project
	qDebug() << "mp->label()" << mp->label();
	QFileInfo fin(mp->relativePathName());
	QString fname = fin.fileName();
	//meshElem.setAttribute("filename", mp->relativePathName());//***20160616  backup relative path
	meshElem.setAttribute("filename", "./" + fname);//***20160616  backup relative path
	meshElem.appendChild(Matrix44mToXML(mp->cm.Tr, doc));
	//SYDNY 02/17/2017 added an element that will be generated on the .pip file
	if (mp->hasDataMask(MeshModel::MM_COLOR))
	{
		meshElem.appendChild(ColorToXML(mp->cm.C(), doc));
	}
	if (!mp->hasDataMask(MeshModel::MM_COLOR))
	{
		meshElem.appendChild(ColorToXMLWhite(mp->cm.C(), doc));
	}
	return meshElem;
}

bool MeshDocumentFromXML(MeshDocument &md, QString filename)
{
	QFile qf(filename);
	QFileInfo qfInfo(filename);
	QDir tmpDir = QDir::current();
	QDir::setCurrent(qfInfo.absoluteDir().absolutePath());
	if (!qf.open(QIODevice::ReadOnly))
		return false;

	QString project_path = qfInfo.absoluteFilePath();

	QDomDocument doc("MeshLabDocument");    //It represents the XML document
	if (!doc.setContent(&qf))
		return false;

	QDomElement root = doc.documentElement();

	QDomNode node;

	node = root.firstChild();

	//Devices
	while (!node.isNull()){
		if (QString::compare(node.nodeName(), "MeshGroup") == 0)
		{
			QDomNode mesh; QString filen, label;
			mesh = node.firstChild();
			while (!mesh.isNull()){
				//return true;
				filen = mesh.attributes().namedItem("filename").nodeValue();
				label = mesh.attributes().namedItem("label").nodeValue();
				MeshModel* mm = md.addNewMesh(filen, label);
				QDomNode tr = mesh.firstChild();
				QDomNode tr2 = mesh.lastChild();

				if (!tr.isNull() && QString::compare(tr.nodeName(), "MLMatrix44") == 0)
				{
					vcg::Matrix44f trm;
					if (tr.childNodes().size() == 1)
					{
						QStringList values = tr.firstChild().nodeValue().split(" ", QString::SkipEmptyParts);
						for (int y = 0; y < 4; y++)
							for (int x = 0; x < 4; x++)
								md.mm()->cm.Tr[y][x] = values[x + 4 * y].toFloat(); 
					}
				}

				//SYDNY 02/20/2017 added new node for color values.
				if (!tr2.isNull() && QString::compare(tr2.nodeName(), "Color") == 0)
				{
					vcg::Color4b crm;
					if (tr.childNodes().size() == 1)
					{
						QStringList values = tr2.lastChild().nodeValue().split(" ", QString::SkipEmptyParts);
						if (!values.isEmpty())
						{
							for (int x = 0; x < 4; x++)
								md.mm()->cm.C()[x] = values[x].toFloat();
						}
					}
				}
				mesh = mesh.nextSibling();
			}
		}
		// READ IN POINT CORRESPONDECES INCOMPLETO!!
		else if (QString::compare(node.nodeName(), "RasterGroup") == 0)
		{
			QDomNode raster; QString filen, label;
			raster = node.firstChild();
			while (!raster.isNull())
			{
				//return true;
				md.addNewRaster();
				QString labelRaster = raster.attributes().namedItem("label").nodeValue();
				md.rm()->setLabel(labelRaster);
				QDomNode sh = raster.firstChild();
				ReadShotFromQDomNode(md.rm()->shot, sh);

				QDomElement el = raster.firstChildElement("Plane");
				while (!el.isNull())
				{
					QString filen = el.attribute("fileName");
					QFileInfo fi(filen);
					QString sem = el.attribute("semantic");
					QString nm = fi.absoluteFilePath();
					md.rm()->addPlane(new Plane(fi.absoluteFilePath(), Plane::RGBA));
					el = node.nextSiblingElement("Plane");
				}
				raster = raster.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	QDir::setCurrent(tmpDir.absolutePath());
	qf.close();
	return true;
}


QDomElement Pisymbol(QDomDocument &doc)
{
	QDomElement piElem = doc.createElement("pisym");
	return piElem;
}

QDomElement RasterModelToXML(RasterModel *mp, QDomDocument &doc)
{
	QDomElement rasterElem = doc.createElement("MLRaster");
	rasterElem.setAttribute("label", mp->label());
	rasterElem.appendChild(WriteShotToQDomNode(mp->shot, doc));
	for (int ii = 0; ii < mp->planeList.size(); ++ii)
		rasterElem.appendChild(PlaneToXML(mp->planeList[ii], mp->par->pathName(), doc));
	return rasterElem;
}

QDomElement PlaneToXML(Plane* pl, const QString& basePath, QDomDocument& doc)
{
	QDomElement planeElem = doc.createElement("Plane");
	QDir dir(basePath);
	planeElem.setAttribute("fileName", dir.relativeFilePath(pl->fullPathFileName));
	planeElem.setAttribute("semantic", pl->semantic);
	return planeElem;
}

QDomDocument MeshDocumentToXML(MeshDocument &md, bool onlyVisibleLayers)//***save MeshDoc to XML
{
	QDomDocument ddoc("PalettrDocument");

	QDomElement root = ddoc.createElement("PaletteProject");
	ddoc.appendChild(root);
	QDomElement mgroot = ddoc.createElement("MeshGroup");

	foreach(MeshModel *mmp, md.meshList)
	{
		//if ()
		if (((!onlyVisibleLayers) || (mmp->visible)) && mmp->getMeshSort() == MeshModel::meshsort::print_item)
		{
			QDomElement meshElem = MeshModelToXML(mmp, ddoc);
			mgroot.appendChild(meshElem);
		}
	}
	//mgroot.appendChild(Pisymbol(ddoc));//***20160325...nonsense
	root.appendChild(mgroot);

	QDomElement rgroot = ddoc.createElement("RasterGroup");

	foreach(RasterModel *rmp, md.rasterList)
	{
		QDomElement rasterElem = RasterModelToXML(rmp, ddoc);
		rgroot.appendChild(rasterElem);
	}

	root.appendChild(rgroot);




	//    tag.setAttribute(QString("name"),(*ii).first);
	//    RichParameterSet &par=(*ii).second;
	//    QList<RichParameter*>::iterator jj;
	//    RichParameterXMLVisitor v(doc);
	//    for(jj=par.paramList.begin();jj!=par.paramList.end();++jj)
	//    {
	//      (*jj)->accept(v);
	//      tag.appendChild(v.parElem);
	//    }
	//    root.appendChild(tag);
	//  }
	//
	return ddoc;
}

