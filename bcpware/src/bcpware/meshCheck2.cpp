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
#include "meshCheck2.h"




MeshCheck2::MeshCheck2(MeshDocument *testmdi, RichParameterSet rps, MeshModel *m, Construction_Groove<Scalarm> _cg, GLArea *_gla, QWidget *parent) :QDialog(parent),
checkModel(m), mdd(testmdi), ui(new Ui::meshCheckDlg), cg(_cg), gla(_gla)
{
	ui->setupUi(this);
	init();
}

void MeshCheck2::init()
{
	originalLWH = { checkModel->cm.bbox.DimX(), checkModel->cm.bbox.DimY(), checkModel->cm.bbox.DimZ() };
	ui->x_dimension->setNum(checkModel->cm.bbox.DimX());
	ui->y_dimension->setNum(checkModel->cm.bbox.DimY());
	ui->z_dimension->setNum(checkModel->cm.bbox.DimZ());



	scaleN = { 0, 0, 0 };

	dimensionGroup = new QButtonGroup;
	dimensionGroup->addButton(ui->mmCHB, 1);
	dimensionGroup->addButton(ui->meterCHB, 2);
	dimensionGroup->addButton(ui->cmCHB, 3);
	dimensionGroup->addButton(ui->inchCHB, 4);
	ui->cmCHB->setChecked(true);
	dimensionGroup->setExclusive(true);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(dimensionGroup, SIGNAL(buttonClicked(int)), this, SLOT(changeDimension(int)));


	ui->maxMmLA->setText(QString("%1 * %2 * %3").arg(cg.DimX()*10).arg(cg.DimY()*10).arg(cg.DimZ()*10));
	ui->maxCmLA->setText(QString("%1 * %2 * %3").arg(cg.DimX()).arg(cg.DimY()).arg(cg.DimZ()));
	ui->maxMeterLA->setText(QString("%1 * %2 * %3").arg(cg.DimX()/100).arg(cg.DimY()/100).arg(cg.DimZ()/100));
	ui->maxInchLA->setText(QString("%1 * %2 * %3").arg(cg.DimX()/DSP_inchmm, 0, 'f', 2).arg(cg.DimY()/DSP_inchmm, 0, 'g', 2).arg(cg.DimZ()/DSP_inchmm, 0, 'f', 2));



	//connect(dimensionGroup, SIGNAL(buttonClicked(int)), this, SLOT());

}

void MeshCheck2::changeDimension(int choose_unit)
{
	
	QPalette temppalette;// = ui->maxMmLA->palette();
	//temppalette.setColor(ui->maxMmLA->backgroundRole(), Qt::red);
	temppalette.setColor(QPalette::Foreground, Qt::blue);

	QPalette temppalette2;// = ui->maxMmLA->palette();
	//temppalette.setColor(ui->maxMmLA->backgroundRole(), Qt::red);
	temppalette2.setColor(QPalette::Foreground, Qt::black);
	
	switch (choose_unit)
	{
	case 1:
	{
			 /* ui->x_dimension->setNum((originalLWH*10).X());
			  ui->y_dimension->setNum((originalLWH*10).Y());
			  ui->z_dimension->setNum((originalLWH*10).Z());*/

			  ui->maxMmLA->setPalette(temppalette);
			  ui->maxMeterLA->setPalette(temppalette2);
			  ui->maxCmLA->setPalette(temppalette2);
			  ui->maxInchLA->setPalette(temppalette2);

	}
		break;
	case 2:
	{
			  /*ui->x_dimension->setNum((originalLWH).X());
			  ui->y_dimension->setNum((originalLWH).Y());
			  ui->z_dimension->setNum((originalLWH).Z());*/

			  ui->maxMmLA->setPalette(temppalette2);
			  ui->maxMeterLA->setPalette(temppalette);
			  ui->maxCmLA->setPalette(temppalette2);
			  ui->maxInchLA->setPalette(temppalette2);
			
	}
		break;
	case 3:
	{
			  /*ui->x_dimension->setNum((originalLWH*0.01).X());
			  ui->y_dimension->setNum((originalLWH*0.01).Y());
			  ui->z_dimension->setNum((originalLWH*0.01).Z());*/

			  ui->maxMmLA->setPalette(temppalette2);
			  ui->maxMeterLA->setPalette(temppalette2);
			  ui->maxCmLA->setPalette(temppalette);
			  ui->maxInchLA->setPalette(temppalette2);
	}
		break;
	case 4:
	{
			  /*ui->x_dimension->setNum((originalLWH/DSP_inchmm).X());
			  ui->y_dimension->setNum((originalLWH/DSP_inchmm).Y());
			  ui->z_dimension->setNum((originalLWH/DSP_inchmm).Z());*/

			  ui->maxMmLA->setPalette(temppalette2);			  
			  ui->maxMeterLA->setPalette(temppalette2);
			  ui->maxCmLA->setPalette(temppalette2);
			  ui->maxInchLA->setPalette(temppalette);
	}
		break;
	}
	

	

}
void MeshCheck2::getAccept()
{
	
	switch (dimensionGroup->checkedId())
	{
	case unit::cetimeters:
	{
							 scaleN = { 1, 1, 1 };

	}break;
	case unit::meters:
	{
						 scaleN.Import(Point3f(100, 100, 100));
	}break;
	case unit::milimeters:
	{
							 scaleN.Import(Point3f(0.1, 0.1, 0.1));

	}break;
	case unit::inch:
	{
					   scaleN.Import(Point3f(DSP_inchmm, DSP_inchmm, DSP_inchmm));
	}break;

	}

	Matrix44m scale_Translate_BackTo_CenterMatrix;
	Matrix44m scale_Translate_GoBackTo_OriginalMatrix;
	Point3m meshCenter = checkModel->cm.bbox.Center();
	transMatrix.SetScale(scaleN);
	scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
	scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
	transMatrix = scale_Translate_GoBackTo_OriginalMatrix*transMatrix*scale_Translate_BackTo_CenterMatrix;
	checkModel->cm.Tr = transMatrix * checkModel->cm.Tr;
	//***********************************

	tri::UpdatePosition<CMeshO>::Matrix(checkModel->cm, transMatrix, true);
	tri::UpdateBounding<CMeshO>::Box(checkModel->cm);
	//m->cm.Tr.SetIdentity();

	checkModel->UpdateBoxAndNormals();


	accept();
}
MeshCheck2::~MeshCheck2()
{
}
