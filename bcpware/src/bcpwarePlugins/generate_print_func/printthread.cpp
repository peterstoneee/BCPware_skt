#include <QDebug>
#include "printthread.h"
#include <QMessageBox>
#include <QTimer>

PrintThread::PrintThread(QObject *parent) :
    QThread(parent)
{
	count = 0;
}
void PrintThread::run()
{
    qDebug()<<"From worker thread: "<<currentThreadId();
	//mutex1.lock();

//    while (1) {
//        {
//        mutex1.lock();
//        if (print_stop) break;
//        }
//        msleep(10);
//    }
//    mutex1.unlock();
	//QTimer timer;
	count++;
	qDebug() << "print_thread_count-----" << count;
	//timer.start(1000);


}
void PrintThread::page_count(int i)
{	
	//msleep();
	mutex1.lock();
	job_page_count=i;
	qDebug() << "job_page_count" << job_page_count;
	mutex1.unlock();
	
}
void PrintThread::pt_stop()
{
    qDebug()<<"call from main thread";
    mutex1.lock();
    print_stop = true;
    mutex1.unlock();
}
void PrintThread::getFile()
{

}
