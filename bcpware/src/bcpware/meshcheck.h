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
#ifndef MESHCHECK_H
#define MESHCHECK_H

#include <QDialog>
#include "stdpardialog.h"
#include "../common/filterparameter.h"
#include<vcg/complex/complex.h>
#include <QMap>
#include <QVector>
#include "glarea.h"


namespace Ui {
class MeshCheck;
}

class MeshCheck : public QDialog
{
    Q_OBJECT

public:
	explicit MeshCheck(MeshDocument *testmdi, RichParameterSet rps, MeshModel *m, Construction_Groove<Scalarm> cg, QWidget *parent = 0);
    void AutoRotate(MeshModel *m);
    void AutoScale();
    ~MeshCheck();

private:
    Ui::MeshCheck *ui;
    
    Matrix44m transMatrix;
    MeshModel *m;
    Construction_Groove<Scalarm> cg;//***建構槽
    QMap<QString, Scalarm> length_unit;//***長度單位
    Box3m currentMeshBox;//***複製meshbox
    QVector<Scalarm> comparetoCGbox;
    RichParameterSet rps;
    GLArea *testwidget;
    MeshDocument *testmdi;
    QMessageBox *msg;
	Point3m scaleN;
public:
    StdParFrame *stdParFrame;
   // GLArea* glar;
private slots:
    //void on_unitCombobox_currentIndexChanged(int index);
    void on_buttonBox_accepted();
    void on_AutoScale_toggled(bool checked);
    void on_scaleSlider_valueChanged(int value);
    void on_unitCombobox_activated(const QString &arg1);



};

#endif // MESHCHECK_H
