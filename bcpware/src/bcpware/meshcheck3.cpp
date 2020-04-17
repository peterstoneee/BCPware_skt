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
#include "meshcheck3.h"

meshCheck3::meshCheck3(MeshModel *m, Construction_Groove<Scalarm> _cg, GLArea *_gla, QWidget *parent)
	: QDialog(parent)
{
	checkModel = m;
	cg = _cg;
	gla = _gla;

	ui.setupUi(this);
	init();

	setWindowTitle("Resize object with different unit");
}

void meshCheck3::init()
{
	ui.xRawLabel->setNum(checkModel->cm.bbox.DimX());
	ui.yRawLabel->setNum(checkModel->cm.bbox.DimY());
	ui.zRawLabel->setNum(checkModel->cm.bbox.DimZ());

	scaleN = { 0, 0, 0 };

	dimensionGroup = new QButtonGroup;
	dimensionGroup->addButton(ui.mmCB, 1);
	dimensionGroup->addButton(ui.cmCB, 2);
	dimensionGroup->addButton(ui.mCB, 3);
	dimensionGroup->addButton(ui.inCB, 4);
	ui.mmCB->setChecked(true);
	dimensionGroup->setExclusive(true);
	
	ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Change Unit"));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void meshCheck3::getAccept()
{
	switch (dimensionGroup->checkedId())
	{
	case 1: scaleN.Import(Point3f(1.0, 1.0, 1.0)); break;
	case 2: scaleN.Import(Point3f(10.0, 10.0, 10.0)); break;
	case 3: scaleN.Import(Point3f(1000.0, 1000.0, 1000.0)); break;
	case 4: scaleN.Import(Point3f(25.4, 25.4, 25.4)); break;
	}

	Matrix44m scale_Translate_BackTo_CenterMatrix;
	Matrix44m scale_Translate_GoBackTo_OriginalMatrix;
	Point3m meshCenter = checkModel->cm.bbox.Center();
	transMatrix.SetScale(scaleN);
	scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));
	scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);
	transMatrix = scale_Translate_GoBackTo_OriginalMatrix*transMatrix * scale_Translate_BackTo_CenterMatrix;
	checkModel->cm.Tr = transMatrix * checkModel->cm.Tr;

	tri::UpdatePosition<CMeshO>::Matrix(checkModel->cm, transMatrix, true);
	tri::UpdateBounding<CMeshO>::Box(checkModel->cm);

	checkModel->UpdateBoxAndNormals();

	accept();
}