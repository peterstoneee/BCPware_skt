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
#include "printerJobInfo.h"
#include "skt_function.h"
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDesktopServices>

#include "xlsxdocument.h"

PrinterJobInfo::PrinterJobInfo(QWidget* p, MeshDocument *md, RichParameterSet *parlst, QVector<float> *_cmyUsage, printInfoMode _printInfoMode, bool _two_side_print) :
QDialog(p), CMYUsage(_cmyUsage), printInfoMode_Option(_printInfoMode), two_side_print(_two_side_print), param(parlst)
{
	te1 = new QTextEdit;
	frame1 = new QFrame;
	mdd = md;
	initPJI();
	createframe();
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	setWindowTitle("Estimate Usage");
	//mdd->p_setting.getbuildheight().value;

	te1->setReadOnly(true);
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));	
	//param = parlst;
}

void PrinterJobInfo::createframe()
{
	QGridLayout *qd1 = new QGridLayout(frame1);
	qd1->addWidget(te1);
	te1->setMinimumSize(QSize(300, 300));
	//te1->setseze(QSize(300, 300));
	QTextDocument *document = te1->document();

	QTextCharFormat charFormat;// = it.fragment().charFormat();
	charFormat.setFont(QFont("Arial", 10, QFont::Bold));
	QTextCharFormat charFormat2;// = it.fragment().charFormat();
	charFormat2.setFont(QFont("Arial", 10, QFont::Normal));
	//document->set
	QTextCursor cursor(te1->textCursor());

	/*QTextTableFormat tableFormat;
	tableFormat.setBorder(1);
	tableFormat.setCellPadding(2);
	tableFormat.setAlignment(Qt::AlignCenter);
	cursor.insertTable(1, 1, tableFormat);*/


	//document->setMaximumBlockCount(10);
	//***regular information
	/*cursor.insertText(pJI.currenttime.label);
	cursor.insertText(" : ");
	cursor.insertText(pJI.currenttime.currenttime.toString(QString("yyyy/MM/dd hh:mm:ss")));
	cursor.insertBlock();
	cursor.insertText("=================================");
	cursor.insertBlock();*/

	switch (printInfoMode_Option)
	{
	case PrintOptionMode:
	{

	}
		break;
	case EstimateMode:
		break;
	case PrintPrintingFile:
		break;
	}


	//***mesh information
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.modelNumber.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.modelNumber.value));
	cursor.insertText(pJI.modelNumber.unit);
	cursor.insertBlock();


	//***printJobInformation
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.title.label);
	cursor.insertText(pJI.estimatedBuildTime.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(pJI.estimatedBuildTime.time.toString(QString("hh:mm:ss")));
	cursor.insertBlock();

	//***numoflayer
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.numoflayer.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.numoflayer.value));
	cursor.insertText(pJI.numoflayer.unit);
	cursor.insertBlock();

	//***layerThickness
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.layerThickness.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.layerThickness.value));
	cursor.insertText(pJI.layerThickness.unit);
	cursor.insertBlock();

	//***estimatedPowerUsage
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.powerUsage.label);
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.powerUsage.value, 'f', 3));
	cursor.insertText(pJI.powerUsage.unit);
	cursor.insertBlock();

	//***estimatedBinderUsage
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.estimatedBinderUsage.label);	
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.estimatedBinderUsage.value, 'f', 3));
	cursor.insertText(pJI.estimatedBinderUsage.unit);
	cursor.insertBlock();

	//***estimated_cyan
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.estimated_cyan.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.estimated_cyan.value, 'f', 3));
	cursor.insertText(pJI.estimated_cyan.unit);
	cursor.insertBlock();

	//***estimated_magenta
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.estimated_magenta.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.estimated_magenta.value, 'f', 3));
	cursor.insertText(pJI.estimated_magenta.unit);
	cursor.insertBlock();

	//***estimated_yellow
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.estimated_yellow.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.estimated_yellow.value, 'f', 3));
	cursor.insertText(pJI.estimated_yellow.unit);
	cursor.insertBlock();

	//***wiperEstimateUsage
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.wiperEstimateUsage.label);	
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.wiperEstimateUsage.value));
	cursor.insertText(pJI.wiperEstimateUsage.unit);
	cursor.insertBlock();

	//***Post Process Usage
	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.postProcessUsage.label);
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.postProcessUsage.value, 'f', 3));
	cursor.insertText(pJI.postProcessUsage.unit);
	cursor.insertBlock();



	//cursor.insertText("=================================");
	cursor.insertText("                                 ");
	cursor.insertBlock();

	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.volumn.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.volumn.value, 'f', 2));
	//cursor.insertText(pJI.volumn.unit);
	cursor.insertHtml(" " + QString(" <font face=\"Arial\"size=\"3\"><b>cm<sup>3</sup></b></font>"));
	cursor.insertBlock();

	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.area.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.area.value, 'f', 2));
	//cursor.insertText(pJI.area.unit);
	cursor.insertHtml(" " + QString("<font face=\"Arial\"size=\"3\"><b>cm<sup>2</sup></b></font>"));
	cursor.insertBlock();

	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.totalNumOfFacet.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.totalNumOfFacet.value));
	cursor.insertText(pJI.totalNumOfFacet.unit);
	cursor.insertBlock();


	cursor.setCharFormat(charFormat2);
	cursor.insertText(pJI.totalNumOfVertices.label);
	//cursor.insertText(" : ");
	cursor.setCharFormat(charFormat);
	cursor.insertText(QString::number(pJI.totalNumOfVertices.value));
	cursor.insertText(pJI.totalNumOfVertices.unit);
	cursor.insertBlock();

	





	QPushButton *savelog = new QPushButton("Save(Txt)", this);
	QPushButton *exportExcelPB = new QPushButton("Save(xlsx)", this);
	QPushButton *closeButton = new QPushButton("Close", this);
	
	QHBoxLayout *tempHLayout = new QHBoxLayout;
	tempHLayout->addWidget(closeButton);
	tempHLayout->addStretch(0);
	tempHLayout->addWidget(exportExcelPB);
	tempHLayout->addWidget(savelog);
	QFrame *tempFrame = new QFrame;
	tempFrame->setLayout(tempHLayout);
	qd1->addWidget(tempFrame);

	/*qd1->addWidget(savelog);
	qd1->addWidget(exportExcelPB);
	qd1->addWidget(closeButton);*/
	qd1->setColumnMinimumWidth(3,20);

	connect(savelog, SIGNAL(clicked()), this, SLOT(saveLogslot()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeBtnSlot()));
	connect(exportExcelPB, SIGNAL(clicked()), this, SLOT(exportReportSlot()));





	this->setLayout(qd1);

}
void PrinterJobInfo::closeBtnSlot()
{
	close();
}
void PrinterJobInfo::saveLogslot()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Choose a file name"), ".",
		tr("txt (*.TXT )"));
	if (fileName.isEmpty())
		return;
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Dock Widgets"),
			tr("Cannot write file %1:\n%2.")
			.arg(fileName)
			.arg(file.errorString()));
		return;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	//out << te1->toHtml();
	out << te1->toPlainText();

	QApplication::restoreOverrideCursor();

	//statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}




template<class T>
QString PrinterJobInfo::combinestring(QString ss, T tt)
{
	QString::number(tt);

}

void PrinterJobInfo::initPJI()
{
	//bool mdd = ;
	if (mdd->count_all_item() > 0) {

		switch (printInfoMode_Option)
		{
		case EstimateMode:
		{
							 pJI.title.label = "Print Summary \n";

							 //***regular information
							 //pJI.currenttime.label = "--currentTime";
							 //pJI.currenttime.currenttime = QDateTime::currentDateTime();

							 //***printer_job_information*************************************
							 pJI.layerThickness.label = "Slice Height \n";
							 pJI.layerThickness.value = mdd->p_setting.getbuildheight().value;
							 pJI.layerThickness.unit = " mm";

							 pJI.estimatedBuildTime.label = "Estimated Print Time \n";
							 pJI.estimatedBuildTime.time = estimatedbuildTime(mdd->selBBox().DimZ());

							 pJI.numoflayer.label = "Number Of Layer \n";
							 pJI.numoflayer.value = int((mdd->bbox().DimZ() / pJI.layerThickness.value))+2 + DSP_Blank_pages; //two_side_print ? (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) * 2 + 1 + DSP_Blank_pages) : (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages);
							 pJI.numoflayer.unit = " layers";

							 pJI.volumn.value = estimateVolumn();
							 pJI.estimatedBinderUsage.label = " \n Estimated Ink Usage \nBinder \n";
							 pJI.estimatedBinderUsage.value = estimateBinderInkUsage();
							 pJI.estimatedBinderUsage.unit = "ml";

							 //***mesh information
							 pJI.modelNumber.label = "Model Number \n";
							 //pJI.modelNumber.value = mdd->count_all_item();
							 pJI.modelNumber.value = mdd->count_sel_item();
							 //pJI.modelNumber.unit = " item";

							 pJI.powerUsage.label = "Powder Usage \n";
							 pJI.powerUsage.value = pJI.volumn.value*DSP_Power_Proportion;
							 pJI.powerUsage.unit = " g";

							 pJI.volumn.label = "Volume \n";
							 pJI.volumn.unit = " cubic centimeter";


							 pJI.area.label = "Surface Area \n";
							 pJI.area.value = estimateArea();
							 pJI.area.unit = QString("cm");

							 pJI.totalNumOfFacet.label = "Total Number of Facets \n";
							 pJI.totalNumOfFacet.value = mdd->selfn();							
							 //pJI.totalNumOfFacet.unit = " Facet";

							 pJI.totalNumOfVertices.label = "Total Number of Vertices \n";
							 pJI.totalNumOfVertices.value = mdd->selvn();							
							 //pJI.totalNumOfVertices.unit = " Vertices";

							 pJI.wiperEstimateUsage.label = "Estimate Wiper Count \n";
							 pJI.wiperEstimateUsage.value = estimateWiper();
							 pJI.wiperEstimateUsage.unit = " steps";

							 pJI.postProcessUsage.label = "Post Process Glu Usage \n";
							 pJI.postProcessUsage.value = estimatePostProcessUsage();
							 pJI.postProcessUsage.unit = " ml";

		}
			break;
		case PrintOptionMode:
		{
								pJI.title.label = "Print Summary \n";

								//***regular information
								//pJI.currenttime.label = "--currentTime";
								//pJI.currenttime.currenttime = QDateTime::currentDateTime();

								//***printer_job_information*************************************
								pJI.layerThickness.label = "Slice Height \n";
								pJI.layerThickness.value = mdd->p_setting.getbuildheight().value;
								pJI.layerThickness.unit = " mm";

								pJI.estimatedBuildTime.label = "Estimated Print Time \n";
								pJI.estimatedBuildTime.time = estimatedbuildTime(mdd->bbox().DimZ());



								pJI.numoflayer.label = "Number Of Layer \n";
								pJI.numoflayer.value = int(mdd->bbox().DimZ() / pJI.layerThickness.value) + +2 + DSP_Blank_pages; //two_side_print ? (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) * 2 + 1 + DSP_Blank_pages) : (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages);//(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages;
								pJI.numoflayer.unit = " layers";

								pJI.volumn.value = estimateVolumn();
								pJI.estimatedBinderUsage.label = " \n Estimated Ink Usage \nBinder \n";
								pJI.estimatedBinderUsage.value = estimateBinderInkUsage();
								pJI.estimatedBinderUsage.unit = "ml";

								//***mesh information
								pJI.modelNumber.label = "Model Number \n";
								pJI.modelNumber.value = mdd->count_all_item();
								//pJI.modelNumber.value = mdd->count_sel_item();
								//pJI.modelNumber.unit = " item";

								pJI.powerUsage.label = "Powder Usage \n";
								pJI.powerUsage.value = pJI.volumn.value*DSP_Power_Proportion;
								pJI.powerUsage.unit = " g";

								pJI.volumn.label = "Volume \n";

								pJI.volumn.unit = " cubic centimeter";

								pJI.area.label = "Surface Area \n";
								pJI.area.value = estimateArea();
								pJI.area.unit = QString("cm");

								pJI.totalNumOfFacet.label = "Total Number of Facets \n";
								pJI.totalNumOfFacet.value = mdd->fn();
								//pJI.totalNumOfFacet.unit = " Facet";

								pJI.totalNumOfVertices.label = "Total Number of Vertices \n";
								pJI.totalNumOfVertices.value = mdd->vn();
								//pJI.totalNumOfVertices.unit = " Vertices";

								pJI.wiperEstimateUsage.label = "Estimate Wiper Count \n";
								pJI.wiperEstimateUsage.value = estimateWiper();
								pJI.wiperEstimateUsage.unit = " steps";


								pJI.postProcessUsage.label = "Post Process Glu Usage \n";
								pJI.postProcessUsage.value = estimatePostProcessUsage();
								pJI.postProcessUsage.unit = " ml";

		}
			break;
		
		}
		



		if (CMYUsage)
		if (CMYUsage->size()>=3) {
			pJI.estimated_cyan.label = "Cyan \n";
			pJI.estimated_cyan.value = (*CMYUsage)[0];
			pJI.estimated_cyan.unit = "ml";
			pJI.estimated_magenta.label = "Magenta \n";
			pJI.estimated_magenta.value = (*CMYUsage)[1];
			pJI.estimated_magenta.unit = "ml";
			pJI.estimated_yellow.label = "Yellow \n";
			pJI.estimated_yellow.value = (*CMYUsage)[2];
			pJI.estimated_yellow.unit = "ml";
		}



	}
}
QTime PrinterJobInfo::estimatedbuildTime(float height)
{
	
	QTime qt(0, 0, 0);
	
	
	if (two_side_print)
	{
		//it's page, not layer
		double pageOneMinute = 2.8;
		qt = qt.addSecs(int(((height / pJI.layerThickness.value)*2 / pageOneMinute) * 60));
		//qt = qt.addSecs(10000);
		int totalPage = height / pJI.layerThickness.value;
		//totalPage
		
	}
		
	else
	{
		double pageOneMinute = 2.51;
		qt = qt.addSecs(int(((height / pJI.layerThickness.value) / pageOneMinute) * 60));
		//qt = qt.addSecs(10000);
	}
	
	
	
	
	return qt;

}
int PrinterJobInfo::estimateWiper()
{
	//DSP_pagesPerWipe
	int DSP_Midjob_Frequence = param->getInt("MIDJOB_FREQUENCY");
	int DSP_Midjob_Index = param->getInt("WIPER_INDEX");
	int DSP_Midjob_Clicks = param->getInt("WIPER_CLICK");
	
	if (DSP_Midjob_Index < 0 || DSP_Midjob_Frequence<0)return 0;
	

	int layer = getSliceLayer(two_side_print);// qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages;
	int result = layer / (DSP_Midjob_Frequence*DSP_Midjob_Index*2);
	return result;
}

float PrinterJobInfo::estimatePostProcessUsage()
{
	return pJI.area.value*DSP_postProcessGlu;//
}

float PrinterJobInfo::estimateVolumn()
{
	float total_volumn = 0;

	//foreach(MeshModel *bm, mdd->meshList)
	/*foreach(int mID, mdd->multiSelectID)
	{
	MeshModel *bm = mdd->getMesh(mID);
	if (mdd->isPrint_item(*bm))
	{
	if (SKT::volumeOfMesh<float>(*bm) < bm->cm.bbox.Volume())
	total_volumn += SKT::volumeOfMesh<float>(*bm);
	else
	total_volumn += bm->cm.bbox.Volume();
	}
	}*/
	if (printInfoMode_Option == printInfoMode::EstimateMode)
	{
		foreach(int i, mdd->multiSelectID)
			//foreach(MeshModel *bm, mdd->meshList)
		{
			MeshModel *bm = mdd->getMesh(i);
			if (mdd->isPrint_item(*bm))
			{
				if (SKT::volumeOfMesh<float>(*bm) < bm->cm.bbox.Volume())
					total_volumn += SKT::volumeOfMesh<float>(*bm);
				else
					total_volumn += bm->cm.bbox.Volume();
			}
		}
	}
	else if (printInfoMode_Option == printInfoMode::PrintOptionMode)
	{
		foreach(MeshModel *bm, mdd->meshList)
		{
			if (mdd->isPrint_item(*bm))
			{
				if (SKT::volumeOfMesh<float>(*bm) < bm->cm.bbox.Volume())
					total_volumn += SKT::volumeOfMesh<float>(*bm);
				else
					total_volumn += bm->cm.bbox.Volume();
			}
		}
	}
	return total_volumn / (DSP_cmmm*DSP_cmmm*DSP_cmmm);
}
float PrinterJobInfo::estimateArea()
{
	CMeshO::FaceIterator fi;
	double area = 0;
	if (printInfoMode_Option == printInfoMode::EstimateMode)
	{
		foreach(int i, mdd->multiSelectID)
		{
			MeshModel &ma = *mdd->getMesh(i);


			//MeshModel &m = *mdd->mm();
			vcg::Point3f va;

			for (fi = ma.cm.face.begin(); fi != ma.cm.face.end(); ++fi)
			{
				vcg::Point3f v0 = fi->V(1)->P() - fi->V(0)->P();
				vcg::Point3f v1 = fi->V(2)->P() - fi->V(0)->P();
				double temp = 1. / 2 * sqrt(SKT::Length<float>(v0)*SKT::Length<float>(v0)*SKT::Length<float>(v1)*SKT::Length<float>(v1)-(v0*v1)*(v0*v1));
				if (temp>0)
					area += temp;
				//SKT::CrossProduct<float>(v0, v1, va);
				//area += SKT::Length<float>(va);

			}
		}
	}
	else if (printInfoMode_Option == printInfoMode::PrintOptionMode)
	{
		foreach(MeshModel *ma, mdd->meshList)
		{
			
			//MeshModel &m = *mdd->mm();
			vcg::Point3f va;

			for (fi = ma->cm.face.begin(); fi != ma->cm.face.end(); ++fi)
			{
				vcg::Point3f v0 = fi->V(1)->P() - fi->V(0)->P();
				vcg::Point3f v1 = fi->V(2)->P() - fi->V(0)->P();
				double temp = 1. / 2 * sqrt(SKT::Length<float>(v0)*SKT::Length<float>(v0)*SKT::Length<float>(v1)*SKT::Length<float>(v1)-(v0*v1)*(v0*v1));
				if (temp>0)
					area += temp;
				/*SKT::CrossProduct<float>(v0, v1, va);
				area += SKT::Length<float>(va);*/

			}
		}
	}
	return area / 100;

}
float PrinterJobInfo::estimateBinderInkUsage()
{
	double sliceHeight = DSP_Estimate_Usage_Slice_Height;//0.1mm;
	double oneCCNozzles = 14000 * 10 / 222;
	double oneHundredLayers = 10 / sliceHeight;//***1cm/0.01cm assume slice height is 0.01cm
	double nozzleDrop = 1.2e-9;
	double binderChannel = 2;
	double oneCCbinderUsage = oneCCNozzles*oneCCNozzles*binderChannel*oneHundredLayers*nozzleDrop;

	int printHeadNozzle = 14080;//***640*11*2 = 14080
	double uselessPrintWidth = param->getFloat("SPITTOON_B");
	double printHeadLength = 223.52;// DSP_grooveX;//
	double inkDropVolumn = 1.2e-9;//ml
	float secondB = param->getFloat("SPITTOON_SECOND_B");

	int boxy_dim = qCeil((mdd->selBBox().max.Z() - mdd->selBBox().min.Z()) / sliceHeight) + 1;
	double dropsize = 1200 * 1e-12;
	//double uselessPrintUsage = printHeadNozzle * (printHeadNozzle*uselessPrintWidth / printHeadLength) * boxy_dim * inkDropVolumn;
	double uselessPrintUsage = 2 * (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidth * 1600 / 25.4)*boxy_dim*inkDropVolumn;
	if (two_side_print)
		uselessPrintUsage +=  2* (DSP_grooveY * 1600 / 25.4) * (secondB * 1600 / 25.4)*boxy_dim*inkDropVolumn;
	/*if (two_side_print)
		uselessPrintUsage+=*/
	//MaintUsage
	jobServiceCount prejob(200, 200, 200, 200, 200);
	jobServiceCount midjob(0, 0, 0, 0, 0);
	jobServiceCount postjob(200, 200, 200, 200, 200);
	
	double preJob = 0;
	double postJob = 0;

	double midJobDropDots = 14080 *800;	
	int DSP_Midjob_Frequence = param->getInt("MIDJOB_FREQUENCY");
	int DSP_Big_Midjob_Frequence_ = 128;
	int nMidJobFordouble_fuck_slow_wipe = (boxy_dim/ DSP_Big_Midjob_Frequence_);
	int nMidJobFordouble_fuck_slow_wipe2 = 2. * (boxy_dim / DSP_Big_Midjob_Frequence_) ;

	int nMidJob = (boxy_dim / DSP_Midjob_Frequence);

	int nMidJob2 = (boxy_dim / DSP_Midjob_Frequence);

	if (two_side_print)
	{
		nMidJob2 *= 2;
	}
	/*double maintsuckprejob = DSP_suck_preJob;
	double temp = midjob.maintUsage(jobServiceCount::inktype::k1);
	double maintmidjob = nMidJob *midjob.maintUsage(jobServiceCount::inktype::k1);
	double midwipe = nMidJob * DSP_wipe_midJob;
	double maintPostJob = postjob.maintUsage(jobServiceCount::inktype::k1);
	double wipePostJob = DSP_wipe_postJob * 2;*/

	maintBinder = uselessPrintUsage + 2 * (
		prejob.maintUsage(jobServiceCount::inktype::k1) + DSP_WIPER_B +
		nMidJob *midjob.maintUsage(jobServiceCount::inktype::k1) + nMidJob * DSP_WIPER_B
		+ postjob.maintUsage(jobServiceCount::inktype::k1) + DSP_WIPER_B 
		);	

	/*==================================
	detail
	==================================*/
	maint_Wiper_B = 2 * (DSP_WIPER_B + nMidJob2 * DSP_WIPER_B + DSP_WIPER_B);

	if (two_side_print)
	{
		maint_Spitton_B = 2 * 2 * (prejob.maintUsage(jobServiceCount::inktype::k1) +
			nMidJob2 *midjob.maintUsage(jobServiceCount::inktype::k1) +
			
			postjob.maintUsage(jobServiceCount::inktype::k1));

		maint_useless_B =  uselessPrintUsage;

		maint_Wiper_B = (DSP_WIPER_B + nMidJob2 * DSP_WIPER_B + DSP_WIPER_B) + nMidJobFordouble_fuck_slow_wipe2 * DSP_SLOW_WIPER_B;
		//***builder area
		binder_PrintUsage = 2*pJI.volumn.value * oneCCbinderUsage;
	}
	else
	{
		maint_Spitton_B = 2 * (prejob.maintUsage(jobServiceCount::inktype::k1) +
			nMidJob *midjob.maintUsage(jobServiceCount::inktype::k1) +
			nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_B +
			postjob.maintUsage(jobServiceCount::inktype::k1));

		maint_useless_B = uselessPrintUsage;

		maint_Wiper_B = (DSP_WIPER_B + nMidJob * DSP_WIPER_B + DSP_WIPER_B) + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_B;
		//***builder area
		binder_PrintUsage = pJI.volumn.value * oneCCbinderUsage;
	}
	
	return  maint_Wiper_B + maint_Spitton_B + maint_useless_B + binder_PrintUsage;

}
void PrinterJobInfo::updatePJI(Print_Job_Information *pji, MeshDocument *md)
{
	//estimatedbuildTime(md->bbox().DimZ());
}
Print_Job_Information PrinterJobInfo::getPJI()
{
	return pJI;
}
void PrinterJobInfo::setLayers(int pages)
{
	pJI.numoflayer.label = "Number Of Layer \n";
	pJI.numoflayer.value = pages;
	pJI.numoflayer.unit = " layers";
}
void PrinterJobInfo::setSMLayers(int startPage)
{
	if (printInfoMode_Option == printInfoMode::EstimateMode)
	{
		setLayers(qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages-startPage);

	}
	else if (printInfoMode_Option == printInfoMode::PrintOptionMode)
	{
		setLayers(qCeil(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages - startPage);
	}
}

int PrinterJobInfo::getSliceLayer(bool doublePrintLayer)
{
	
	if (doublePrintLayer)
		return (qCeil(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1)*2 + DSP_Blank_pages;
	else
		return (qCeil(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1) + DSP_Blank_pages;

}


void PrinterJobInfo::exportReportSlot()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Choose a file name"), ".",
		tr("xlsx (*.xlsx )"));
	if (fileName.isEmpty())
		return;

	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	//dir.setPath(temppath + "/xyzimage");
	QString reportSampleFile = PicaApplication::getRoamingDir() + "/" + DSP_REPORT_SAMPLE_FILE_NAME;
	QFile samplefile(reportSampleFile);
	if (QFile::exists(fileName))
	{
		bool result = QFile::remove(fileName);
	}	
	bool result = samplefile.copy(fileName);

	if (result)
	{
	QXlsx::Document xlsx(fileName);//
	xlsx.selectSheet("Job Cost");
	
	//***model Number
	xlsx.write("K6", pJI.modelNumber.value);
	//***Building Date
	xlsx.write("K4", QDateTime::currentDateTime().toString());
	//***estimated Print Time
	xlsx.write("K5", pJI.estimatedBuildTime.time);
	//***Build Height
	xlsx.write("K7", mdd->bbox().DimZ());
	//***slice height
	//xlsx.write("C8", pJI.layerThickness.value);
	//***powder Usage
	xlsx.write("E11", pJI.powerUsage.value);

	//CMYB
	xlsx.write("E9", pJI.estimatedBinderUsage.value);
	xlsx.write("E6", pJI.estimated_cyan.value);
	xlsx.write("E7", pJI.estimated_magenta.value);
	xlsx.write("E8", pJI.estimated_yellow.value);

	//***wiper count
	xlsx.write("E13", pJI.wiperEstimateUsage.value);
	//***pages
	//xlsx.write("E14", pJI.numoflayer.value);
	xlsx.write("E14", two_side_print ? (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) * 2 + 1 + DSP_Blank_pages) : (qCeil(mdd->selBBox().DimZ() / pJI.layerThickness.value) + 1 + DSP_Blank_pages));
	//***CA usate ml
	xlsx.write("E16", pJI.postProcessUsage.value);
	//***volumn
	xlsx.write("K8", pJI.volumn.value);
	//***volumn in3
	xlsx.write("K9", "=K8/16.387");
	//***surface Area
	xlsx.write("K10", pJI.area.value);
	xlsx.write("K11", "=K13/K8");
	xlsx.write("K12", "=K13/K9");

	xlsx.write("K14", "=H17/K6");

	/*xlsx.write("I11", maintBinder);
	xlsx.write("I12", CMYUsage[3]);
	xlsx.write("I13", CMYUsage[4]);
	xlsx.write("I14", CMYUsage[5]);*/

	if (CMYUsage->size() > 17 && false ){
	/*spitton*/
	xlsx.write("K19", (*CMYUsage)[6]);
	xlsx.write("K20", (*CMYUsage)[7]);
	xlsx.write("K21", (*CMYUsage)[8]);
	xlsx.write("K22", maint_Spitton_B);
	/*wiper*/
	xlsx.write("L19", (*CMYUsage)[9]);
	xlsx.write("L20", (*CMYUsage)[10]);
	xlsx.write("L21", (*CMYUsage)[11]);
	xlsx.write("L22", maint_Wiper_B);
	/*useless print*/
	xlsx.write("M19", (*CMYUsage)[12]);
	xlsx.write("M20", (*CMYUsage)[13]);
	xlsx.write("M21", (*CMYUsage)[14]);
	xlsx.write("M22", maint_useless_B);
	/*print usage*/
	xlsx.write("N19", (*CMYUsage)[15]);
	xlsx.write("N20", (*CMYUsage)[16]);
	xlsx.write("N21", (*CMYUsage)[17]);
	
	xlsx.write("N22", binder_PrintUsage);
	}

	bool saveresult = xlsx.saveAs(fileName);
	if (saveresult)
	{	
		QString temp = "file:///" + fileName;
		QDesktopServices::openUrl(QUrl(temp));
	}
	else
	{
		QMessageBox::information(this, tr("Failed to save file"), fileName);
	}
	}


	//QTextStream out(&file);
	//QApplication::setOverrideCursor(Qt::WaitCursor);
	////out << te1->toHtml();
	//out << te1->toPlainText();

	QApplication::restoreOverrideCursor();
}

//void PrinterJobInfo::exportReportSlot()
//{
//	QString fileName = QFileDialog::getSaveFileName(this,
//		tr("Choose a file name"), ".",
//		tr("xlsx (*.xlsx )"));
//	if (fileName.isEmpty())
//		return;
//
//	QString temppath = PicaApplication::getRoamingDir();
//	QDir dir(temppath);//backup
//	//dir.setPath(temppath + "/xyzimage");
//	QString reportSampleFile = PicaApplication::getRoamingDir() + "/" + DSP_REPORT_SAMPLE_FILE_NAME_TWO;
//
//	QFile samplefile(reportSampleFile);
//	if (QFile::exists(fileName))
//	{
//		bool result = QFile::remove(fileName);
//	}
//	bool result = samplefile.copy(fileName);
//
//	if (result)
//	{
//		QXlsx::Document xlsx(fileName);//
//		xlsx.selectSheet("PartPro 350");
//
//		//***model Number
//		xlsx.write("B4", pJI.modelNumber.value);
//		//***Building Date
//		//xlsx.write("K4", QDateTime::currentDateTime().toString());
//		//***estimated Print Time
//		xlsx.write("B5", pJI.estimatedBuildTime.time);
//		//***Build Height
//		xlsx.write("K7", mdd->bbox().DimZ());
//		//***slice height
//		//xlsx.write("C8", pJI.layerThickness.value);
//		//***powder Usage
//		xlsx.write("B17", pJI.powerUsage.value);
//
//		//CMYB
//		xlsx.write("B10", pJI.estimatedBinderUsage.value);
//		xlsx.write("B11", pJI.estimated_cyan.value);
//		xlsx.write("B12", pJI.estimated_magenta.value);
//		xlsx.write("B13", pJI.estimated_yellow.value);
//
//		//***wiper count
//		xlsx.write("B15", pJI.wiperEstimateUsage.value);
//		//***pages
//		xlsx.write("B6", pJI.numoflayer.value);
//		//***CA usage ml
//		xlsx.write("B16", pJI.postProcessUsage.value);
//		//***volumn
//		xlsx.write("B17", pJI.volumn.value);
//		//***volumn in3
//		//xlsx.write("K9", "=K8/16.387");
//		//***surface Area
//		xlsx.write("B18", pJI.area.value);
//		/*xlsx.write("K11", "=K13/K8");
//		xlsx.write("K12", "=K13/K9");*/
//
//		//xlsx.write("K14", "=H17/K6");
//
//
//		int DSP_Midjob_Frequence = param->getInt("MIDJOB_FREQUENCY");
//		int DSP_Midjob_Index = param->getInt("WIPER_INDEX");
//		int DSP_Midjob_Clicks = param->getInt("WIPER_CLICK");
//
//
//		xlsx.write("F29", DSP_Midjob_Frequence);
//		//xlsx.write("F30", "=H17/K6");
//		xlsx.write("F31", DSP_Midjob_Frequence*DSP_Midjob_Index*2);
//
//		/*
//			spitton first page
//		*/
//		float fuck1_page_C = param->getFloat("SPITTOON_C");
//		float fuck1_page_M = param->getFloat("SPITTOON_M");
//		float fuck1_page_Y = param->getFloat("SPITTOON_Y");
//		float fuck1_page_B = param->getFloat("SPITTOON_B");
//		float fuck2_page_CMY = 0; 
//		float fuck2_page_B = 0; 
//		if (two_side_print)
//		{
//			fuck2_page_CMY = param->getFloat("SPITTOON_SECOND_CMY");
//			fuck2_page_B = param->getFloat("SPITTOON_SECOND_B");
//		}
//
//		xlsx.write("C41", fuck1_page_C);
//		xlsx.write("E41", fuck1_page_B);
//
//		xlsx.write("C42", fuck2_page_CMY);
//
//		xlsx.write("E42", fuck2_page_B);
//
//
//
//		/*xlsx.write("I11", maintBinder);
//		xlsx.write("I12", CMYUsage[3]);
//		xlsx.write("I13", CMYUsage[4]);
//		xlsx.write("I14", CMYUsage[5]);*/
//
//		if (CMYUsage->size() > 17){
//			/*spitton*/
//			xlsx.write("B34", (*CMYUsage)[6]);
//			xlsx.write("B35", (*CMYUsage)[7]);
//			xlsx.write("B36", (*CMYUsage)[8]);
//			xlsx.write("B37", maint_Spitton_B);
//			/*wiper*/
//			xlsx.write("C34", (*CMYUsage)[9]);
//			xlsx.write("C35", (*CMYUsage)[10]);
//			xlsx.write("C36", (*CMYUsage)[11]);
//			xlsx.write("C37", maint_Wiper_B);
//			/*useless print*/
//			xlsx.write("D34", (*CMYUsage)[12]);
//			xlsx.write("D35", (*CMYUsage)[13]);
//			xlsx.write("D36", (*CMYUsage)[14]);
//			xlsx.write("D37", maint_useless_B);
//			/*print usage*/
//			xlsx.write("E34", (*CMYUsage)[15]);
//			xlsx.write("E35", (*CMYUsage)[16]);
//			xlsx.write("E36", (*CMYUsage)[17]);
//			xlsx.write("E37", binder_PrintUsage);
//		}
//
//		bool saveresult = xlsx.saveAs(fileName);
//		if (saveresult)
//		{
//			QString temp = "file:///" + fileName;
//			QDesktopServices::openUrl(QUrl(temp));
//		}
//		else
//		{
//			QMessageBox::information(this, tr("Failed to save file"), fileName);
//		}
//	}
//
//
//	//QTextStream out(&file);
//	//QApplication::setOverrideCursor(Qt::WaitCursor);
//	////out << te1->toHtml();
//	//out << te1->toPlainText();
//
//	QApplication::restoreOverrideCursor();
//}

bool PrinterJobInfo::createReport()
{

	return 0;
}

void PrinterJobInfo::genCSVFile()
{
	//Print_Job_Information pjii = pji.getPJI();
	bool hasHeader = hasCSVHeader();


	QString fileName = PrinterJobInfo::docPath() + PicaApplication::appName() + "/printInfoCSV.csv";

	if (fileName.isEmpty())

		return;



	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{

		std::cerr << "Cannot open file for writing: "

			<< qPrintable(file.errorString()) << std::endl;

		return;

	}

	QTextStream out(&file);

	if (!hasHeader){

	out << "Build Time,"//******************1
		<< "Project Name,"//****************2
		<< "Model Number,"//****************3
		<< "Estimated Print Time,"//********4
		<< "Number of Layer,"//*************5
		<< "Slice Height,"//****************6
		<< "Estimate Powder Usage,"//****************7
		<< "Estimate Ink Usage : Binder,"//*8
		<< "Estimate Ink Usage : Cyan,"//***9
		<< "Estimate Ink Usage : Magenta,"//10
		<< "Estimate Ink Usage : Yellow,"//*11
		<< "Estimate Wiper Usage,"//********12
		<< "Estimate Post Process Glu Usage,"//**13
		<< "Total Number of Facets,"//******14
		//*****After Printing============================================================================================================================***************
		<< "Start Printing Time,"//**********15
		<< "End Printing Time,"//************16
		//<<
		<< "\n"
		;
	}
	out << QDateTime::currentDateTime().toString() << ","//**********************1
		<< "" << ","//proejectName //************************************************2
		<< pJI.modelNumber.value << ","//***************************************3
		<< pJI.estimatedBuildTime.time.toString() << ","//**********************4
		<< (qCeil(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1) + DSP_Blank_pages << ","//***************************************5
		<< pJI.layerThickness.value << ","//*******************************************************************************************6
		<< pJI.powerUsage.value << ","//***********************************************************************************************7
		<< pJI.estimatedBinderUsage.value << ","//*************************************************************************************8
		<< pJI.estimated_cyan.value << ","//*******************************************************************************************9
		<< pJI.estimated_magenta.value << ","//****************************************************************************************10
		<< pJI.estimated_yellow.value << ","//*****************************************************************************************11
		<< pJI.wiperEstimateUsage.value << ","//***************************************************************************************12
		<< pJI.postProcessUsage.value << ","//*****************************************************************************************13
		<< pJI.totalNumOfFacet.value << ","//******************************************************************************************14
		<< ",\n"		
		;


	//QString sName = ui->lineEdit->text();
	/*out << tr("¡G,") << sName << ",\n";
	out << tr("¡G,") << iAge << ",\n";*/

	//out << tr("1,") << tr("2,") << tr("3,") << tr("4,") << ",\n"; 





	//for (int i = 0; i < row; i++)
	//{
	//	for (int col = 0; col < 4; col++)
	//	{
	//		QString string = ui->tableWidget->item(i, col)->text();
	//		out << string << ",";// 
	//	}
	//	out << "\n";
	//}



	file.close();
}

void PrinterJobInfo::writeAtLastLine()
{
	//Print_Job_Information pjii = pji.getPJI();
	bool hasHeader = hasCSVHeader();


	QString fileName = PrinterJobInfo::docPath() + PicaApplication::appName() + "/printInfoCSV.csv";

	if (fileName.isEmpty())

		return;



	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{

		std::cerr << "Cannot open file for writing: "

			<< qPrintable(file.errorString()) << std::endl;

		return;

	}

	QTextStream out(&file);

	
	out << "08070509"<<","
		<< "08070510"<<","		
		<< ",\n"		
		;




	file.close();
}


bool PrinterJobInfo::hasCSVHeader()
{
	QString fileName = PrinterJobInfo::docPath() + PicaApplication::appName() + "/printInfoCSV.csv";
	if (fileName.isEmpty())
		return false;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{

		std::cerr << "Cannot open file for reading: "

			<< qPrintable(file.errorString()) << std::endl;

		return false;

	}
	QStringList list;

	list.clear();

	QTextStream in(&file);
	QString fileLine = in.readLine();
	list = fileLine.split(",", QString::SkipEmptyParts);
	
	if (list.contains("End Printing Time"))
		return true;
	else
		return false;


	file.close();




}

PrinterJobInfo::~PrinterJobInfo()
{

}
