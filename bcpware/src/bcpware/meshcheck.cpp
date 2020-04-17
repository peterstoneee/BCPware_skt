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
#include "meshcheck.h"
#include "ui_meshcheck.h"

MeshCheck::MeshCheck(MeshDocument *testmdi, RichParameterSet rps, MeshModel *m, Construction_Groove<Scalarm> cg, QWidget *parent) :
QDialog(parent), m(m), cg(cg), rps(rps), testmdi(testmdi),
ui(new Ui::MeshCheck)
{
	ui->setupUi(this);

	//***先轉為mm************************************//
	//Matrix44m scaleMatrix;
	//Matrix44m wholeTransformMatrix;
	//scaleMatrix.SetIdentity();
	//wholeTransformMatrix.SetIdentity();
	//Scalarm scale_all_offset = 0.1;

	//scaleMatrix.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
	//scaleMatrix.SetScale(*new Point3m(scale_all_offset, scale_all_offset, scale_all_offset));
	////***position
	//Matrix44m scale_Translate_BackTo_CenterMatrix;
	//Matrix44m scale_Translate_GoBackTo_OriginalMatrix;

	//Point3m meshCenter = m->cm.bbox.Center();
	//scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
	//scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
	//wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;

	//m->cm.Tr = wholeTransformMatrix * m->cm.Tr;
	//tri::UpdatePosition<CMeshO>::Matrix(m->cm, wholeTransformMatrix, true);
	//tri::UpdateBounding<CMeshO>::Box(m->cm);
	//m->UpdateBoxAndNormals();

	//***************************************//



	ui->scaleSlider->setRange(0, 100);
	ui->scaleSlider->setValue(1);

	//***20150518要先給值，不然unit_combobox_設置call changed 時length_unitmap會沒有值
	/*length_unit.insert("meter",100.0);
	length_unit.insert("centimeter",1.0);
	length_unit.insert("millimeter",0.1);
	length_unit.insert("inch",DSP_inchmm);
	transMatrix.SetIdentity();*/

	//***20160309
	/*length_unit.insert("meter", 1000.0);
	length_unit.insert("centimeter", 10.0);
	length_unit.insert("millimeter", 1.0);
	length_unit.insert("inch", 25.4);*/

	length_unit.insert("meter", 100);
	length_unit.insert("centimeter", 1.0);
	length_unit.insert("millimeter", 0.1);
	length_unit.insert("inch", DSP_inchmm);

	transMatrix.SetIdentity();



	/*ui->lengthL->setNum(m->cm.bbox.DimX()*10);
	ui->widthL->setNum(m->cm.bbox.DimZ()*10);
	ui->heightL->setNum(m->cm.bbox.DimY()*10);*/

	ui->lengthL->setNum(m->cm.bbox.DimX());
	ui->widthL->setNum(m->cm.bbox.DimZ());
	ui->heightL->setNum(m->cm.bbox.DimY());

	ui->unitCombobox->addItem("meter");
	ui->unitCombobox->addItem("centimeter");
	ui->unitCombobox->addItem("millimeter");
	ui->unitCombobox->addItem("inch");
	ui->unitCombobox->setCurrentText("centimeter");
	//on_unitCombobox_activated("millimeter");

	currentMeshBox = m->cm.bbox;

	//    ui->AutoScaleToFitGroove->setEnabled(false);
	//    ui->warningMessage->setVisible(false);
	ui->scaleSlider->setEnabled(false);


	comparetoCGbox.push_back(currentMeshBox.DimX() / cg.DimX());//***計算是否大於建構槽box
	comparetoCGbox.push_back(currentMeshBox.DimY() / cg.DimY());
	comparetoCGbox.push_back(currentMeshBox.DimZ() / cg.DimZ());
	qSort(comparetoCGbox.begin(), comparetoCGbox.end());

	msg = new QMessageBox(QMessageBox::Information, "Too Big!", "Plz Use Auto Scale to transform");
	QFont font = QFont();
	font.setPointSize(12);
	QPalette palette = QPalette();
	palette.setColor(QPalette::WindowText, Qt::red);
	msg->setFont(font);
	msg->setPalette(palette);

	//***scaleLabel***顯示最大倍數
	//ui->scaleLabel->setText(QString::number((cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim())), 'f', 5));
	ui->scaleLabel->setText(QString::number(1));

	if (comparetoCGbox.last()>1.0)
	{
		//ui->AutoScaleToFitGroove->setEnabled(true);
		ui->warningMessage->setVisible(true);
		//***messagebox

		msg->exec();
		//QMessageBox::information(this, "Too Big!", "Plz Use Auto Scale to transform");

	}
	else
	{
		//ui->AutoScaleToFitGroove->setEnabled(false);
		ui->warningMessage->setVisible(false);
	}

	//	MultiViewer_Container *mvcont = new MultiViewer_Container(this);//
	//	testwidget = new GLArea(mvcont, &rps);
	//    mvcont->meshDoc.addNewMesh(m->fullName(),m->label());
	//    qDebug()<<"m->fullName(),m->label()"<<m->fullName()<<m->label();
	//	testwidget->setFov(5);
	//	testwidget->vd = GLArea::viewDirection::top;
	//    testwidget->setFixedSize(200, 200);



}

MeshCheck::~MeshCheck()
{
	delete ui;
}

void MeshCheck::AutoRotate(MeshModel *m)
{

}
void MeshCheck::AutoScale()
{

}


void MeshCheck::on_unitCombobox_activated(const QString &arg1)
{
	qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim()))0" << currentMeshBox.DimValue(currentMeshBox.MaxDim());
	scaleN = { 1, 1, 1 };
	transMatrix.SetIdentity();

	qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim()))1" << currentMeshBox.DimValue(currentMeshBox.MaxDim());
	comparetoCGbox.clear();
	//***關掉自動scale
	ui->AutoScale->setChecked(false);

	scaleN.Scale(length_unit.value(arg1), length_unit.value(arg1), length_unit.value(arg1));

	transMatrix.SetScale(scaleN);
	currentMeshBox = m->cm.bbox;//***不知道為什麼要移到下面來才執行的到
	currentMeshBox.min *= length_unit.value(arg1);
	currentMeshBox.max *= length_unit.value(arg1);
	qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim()))2" << currentMeshBox.DimValue(currentMeshBox.MaxDim());
	//	if (ui->AutoScale->isChecked())
	//	{
	//		int x = 100 * (currentMeshBox.DimValue(currentMeshBox.MaxDim()) / cg.DimValue(cg.MinDim()));
	//		ui->scaleSlider->setValue(x);
	//	}

	//***show current length(mm)中間那一行
	/* ui->lengthL_2->setText(QString::number(m->cm.bbox.DimX()*scaleN.X()));
	ui->widthL_2->setText(QString::number(m->cm.bbox.DimZ()*scaleN.Z()));
	ui->heightL_2->setText(QString::number(m->cm.bbox.DimY()*scaleN.Y()));*/



	//***mesh各軸除以建構槽各軸
	comparetoCGbox.push_back(currentMeshBox.DimX() / cg.DimX());
	comparetoCGbox.push_back(currentMeshBox.DimY() / cg.DimY());
	comparetoCGbox.push_back(currentMeshBox.DimZ() / cg.DimZ());
	qSort(comparetoCGbox.begin(), comparetoCGbox.end());



	//***scaleslider設為1
	ui->scaleSlider->setValue(1);

	if (comparetoCGbox.last()>1.0)
	{
		ui->warningMessage->setVisible(true);
		//QMessageBox::information(this, "Too Big!", "Plz Use Auto Scale to transform");
		msg->exec();
	}
	else
	{
		ui->warningMessage->setVisible(false);

	}
	//***scaleLabel***顯示最大倍數
	//ui->scaleLabel->setText(QString::number((cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim())), 'f', 5));
	ui->scaleLabel->setText(QString::number(1));
	qDebug() << "ui->scaleLabel->text1" << ui->scaleLabel->text();


	/*ui->lengthL->setNum(m->cm.bbox.DimX() * 10);
	ui->widthL->setNum(m->cm.bbox.DimZ() * 10);
	ui->heightL->setNum(m->cm.bbox.DimY() * 10);*/

	ui->lengthL->setNum(m->cm.bbox.DimX());
	ui->widthL->setNum(m->cm.bbox.DimZ());
	ui->heightL->setNum(m->cm.bbox.DimY());


	qDebug() << "ui->scaleLabel->text2" << ui->scaleLabel->text();
}

void MeshCheck::on_buttonBox_accepted()
{
	//***20150828***更新Tr***************
	Matrix44m scale_Translate_BackTo_CenterMatrix;
	Matrix44m scale_Translate_GoBackTo_OriginalMatrix;
	Point3m meshCenter = m->cm.bbox.Center();
	scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
	scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
	transMatrix = scale_Translate_GoBackTo_OriginalMatrix*transMatrix*scale_Translate_BackTo_CenterMatrix;
	m->cm.Tr = transMatrix * m->cm.Tr;
	//***********************************

	tri::UpdatePosition<CMeshO>::Matrix(m->cm, transMatrix, true);
	tri::UpdateBounding<CMeshO>::Box(m->cm);
	//m->cm.Tr.SetIdentity();
	m->UpdateBoxAndNormals();
}

void MeshCheck::on_AutoScale_toggled(bool checked)//自動scale_checkbox
{
	if (checked)
	{
		//***建構槽可印最大
		//scaleN*=1.0/comparetoCGbox.last();
		scaleN = { 1, 1, 1 };
		scaleN = scaleN *(cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim()))*1/100;
		qDebug() <<"cg.DimValue(cg.MinDim())" <<cg.DimValue(cg.MinDim());
		qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim())" << currentMeshBox.DimValue(currentMeshBox.MaxDim());

		//scaleN *= (cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim()) * 1 / 100.;
		transMatrix.SetScale(scaleN);
		ui->AutoScale->setCheckable(true);
		ui->warningMessage->setVisible(false);
		//ui->scaleSlider->setValue(100* ( cg.DimValue(cg.MinDim()/currentMeshBox.DimValue(currentMeshBox.MaxDim()))));
		ui->scaleSlider->setValue(1);

		//***show current length(mm)
		/*ui->lengthL_2->setText(QString::number(m->cm.bbox.DimX()*scaleN.X()));
		ui->widthL_2->setText(QString::number(m->cm.bbox.DimZ()*scaleN.Z()));
		ui->heightL_2->setText(QString::number(m->cm.bbox.DimY()*scaleN.Y()));*/



		/*ui->lengthL->setText(QString::number(m->cm.bbox.DimX() * 10));
		ui->widthL->setText(QString::number(m->cm.bbox.DimZ() * 10));
		ui->heightL->setText(QString::number(m->cm.bbox.DimY() * 10));*/

		ui->lengthL->setText(QString::number(m->cm.bbox.DimX()));
		ui->widthL->setText(QString::number(m->cm.bbox.DimZ()));
		ui->heightL->setText(QString::number(m->cm.bbox.DimY()));


		//qDebug() << "QString::number(m->cm.bbox.DimY() * 10)" << ui->heightL->text();
		//***scaleLabel
		//ui->scaleLabel->setText(QString::number((cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim())), 'f', 5));
		ui->scaleLabel->setText(QString::number((cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim()))*1 / 100., 'f', 5));

	}
	else
	{
		//***slider的數值比例大小
		scaleN = { 1, 1, 1 };
		currentMeshBox = m->cm.bbox;
		transMatrix.SetIdentity();
		currentMeshBox.min *= length_unit.value(ui->unitCombobox->currentText());
		currentMeshBox.max *= length_unit.value(ui->unitCombobox->currentText());
		qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim()))3" << currentMeshBox.DimValue(currentMeshBox.MaxDim());
		scaleN.Scale(length_unit.value(ui->unitCombobox->currentText()), length_unit.value(ui->unitCombobox->currentText()), length_unit.value(ui->unitCombobox->currentText()));
		transMatrix.SetScale(scaleN);
		//ui->scaleSlider->setValue(100* ( currentMeshBox.DimValue(currentMeshBox.MaxDim())/cg.DimValue(cg.MinDim())));

		//***show current length(mm)
		/*ui->lengthL_2->setText(QString::number(m->cm.bbox.DimX()*scaleN.X()));
		ui->widthL_2->setText(QString::number(m->cm.bbox.DimZ()*scaleN.Z()));
		ui->heightL_2->setText(QString::number(m->cm.bbox.DimY()*scaleN.Y()));*/


		/*ui->lengthL->setText(QString::number(m->cm.bbox.DimX() * 10));
		ui->widthL->setText(QString::number(m->cm.bbox.DimZ() * 10));
		ui->heightL->setText(QString::number(m->cm.bbox.DimY() * 10));
		*/
		ui->lengthL->setText(QString::number(m->cm.bbox.DimX()));
		ui->widthL->setText(QString::number(m->cm.bbox.DimZ()));
		ui->heightL->setText(QString::number(m->cm.bbox.DimY()));

		//qDebug() << "QString::number(m->cm.bbox.DimY() * 10)" << ui->heightL->text();
		//***scaleLabel
		//ui->scaleLabel->setText(QString::number(0));
	}
}



void MeshCheck::on_scaleSlider_valueChanged(int value)//拉霸
{
	qDebug() << "currentMeshBox.DimValue(currentMeshBox.MaxDim()))4" << currentMeshBox.DimValue(currentMeshBox.MaxDim());
	//***(建構槽最小邊/mesh最大邊)*value/100.0
	ui->scaleLabel->setText(QString::number((cg.DimValue(cg.MinDim()) / currentMeshBox.DimValue(currentMeshBox.MaxDim()))*value / 100., 'f', 5));
	scaleN = { 1, 1, 1 };
	transMatrix.SetIdentity();
	scaleN.Scale(ui->scaleLabel->text().toFloat(), ui->scaleLabel->text().toFloat(), ui->scaleLabel->text().toFloat());
	scaleN.Scale(length_unit.value(ui->unitCombobox->currentText()), length_unit.value(ui->unitCombobox->currentText()), length_unit.value(ui->unitCombobox->currentText()));
	transMatrix.SetScale(scaleN);

	//***show current length(mm)
	/*ui->lengthL_2->setText(QString::number(m->cm.bbox.DimX()*scaleN.X()));
	ui->widthL_2->setText(QString::number(m->cm.bbox.DimZ()*scaleN.Z()));
	ui->heightL_2->setText(QString::number(m->cm.bbox.DimY()*scaleN.Y()));*/

	ui->lengthL->setText(QString::number(m->cm.bbox.DimX()*scaleN.X()));
	ui->widthL->setText(QString::number(m->cm.bbox.DimZ()*scaleN.Z()));
	ui->heightL->setText(QString::number(m->cm.bbox.DimY()*scaleN.Y()));
	//qDebug() << "QString::number(m->cm.bbox.DimY() * 10)3" << ui->heightL->text();
	//qDebug()<<"currentMeshBox.DimValue(currentMeshBox.MaxDim()))5"<<currentMeshBox.DimValue(currentMeshBox.MaxDim());
	qDebug() << "ui->scaleLabel->text3" << ui->scaleSlider->value();
}




