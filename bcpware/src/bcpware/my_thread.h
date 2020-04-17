#ifndef MY_THREAD_H
#define MY_THREAD_H

#include <QThread>
#include <QCoreApplication>
#include <QApplication>


#include "gui_launcher.h"


class OtherThread : public QThread
{
    Q_OBJECT
public:
    gui_launcher<mywidget> gl;
    explicit OtherThread(QObject *parent = 0):QThread(parent)
    {
        count = 0;
        
    }
    int count;
signals:
    void sendMessage(QString msg);
private:
    void run()
    {
        //qDebug() << "From OtherThread : " << currentThreadId();
        count++;
//		//connect(&workt1,SIGNAL(sendMessage(QString)),gl.w,SLOT(showString(QString)));
//		gl.moveToThread(QApplication::instance()->thread());
//		// send it event which will be posted from main thread
//		QCoreApplication::postEvent(&gl, new QEvent(QEvent::User));


        //emit sendMessage(QString( "otherthread"));

        exec();
    }

public slots:

};


#endif // MY_THREAD_H
