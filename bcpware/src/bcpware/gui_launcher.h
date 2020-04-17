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
#ifndef GUI_LAUNCHER_H
#define GUI_LAUNCHER_H

#include <QWidget>
#include <QEvent>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QPushButton>
#include <QLayout>
#include <QTimer>
class mywidget:public QWidget
{
    Q_OBJECT
public :
    mywidget(QWidget *parent = 0):QWidget(parent)
    {
        count_number = 0;

        QHBoxLayout *buildFileLayout = new QHBoxLayout;
        bt1 = new QPushButton("sub_bt1");
        bt2 = new QPushButton("sub_bt2");
        buildFileLayout->addWidget(bt1);
        buildFileLayout->addWidget(bt2);
        setLayout(buildFileLayout);


        qDebug()<<"countnumber-----";
        /*connect(&timer,SIGNAL(timeout()),this,SLOT(plus_count()));
        timer.start(1000);*/

    }
    int count_number;
    QPushButton *bt1, *bt2;
    QTimer timer;
    void showmessage()
    {
        qDebug()<<"mywidget";
    }
public slots:
    void plus_count()
    {
        count_number++;
        qDebug()<<"countnumber-----"<<count_number;
        this->setWindowTitle(QString::number(count_number));
    }
    void showString()
    {
        qDebug()<<"ssssssssssssssssssss";

    }


};

//class gui_launcher : public QObject
//{
//
//public:
//	mywidget *w;
//    gui_launcher(QObject *parent=0 ):QObject(parent){}
//
////    gui_launcher(const gui_launcher &obj) {
////            w = new QWidget ;
////            *w = *obj.w;
////        }
//
//    // other components
//    //..
//public:
//    virtual bool event( QEvent *ev )
//    {
//        if( ev->type() == QEvent::User )
//        {
//			w = new mywidget;
//            w->show();
//            return true;
//        }
//        return false;
//    }
//};


template<typename mygui> class gui_launcher : public QObject
{

public:
    mygui *w;
    // other components
public:
    void add2Gui()
    {
        this->moveToThread( QApplication::instance()->thread() );
        QCoreApplication::postEvent( this, new QEvent( QEvent::User ) );
    }
    virtual bool event( QEvent *ev )
    {
        if( ev->type() == QEvent::User )
        {
            w = new mygui;
            w->show();
            return true;
        }
        return false;
    }

};



#endif // GUI_LAUNCHER_H
