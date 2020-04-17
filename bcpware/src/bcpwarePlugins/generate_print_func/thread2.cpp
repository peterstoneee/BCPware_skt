#include "thread2.h"
#include "sample_filtergpu.h"
#include <QDebug>
#include <QMessageBox>

Thread2::Thread2(QObject* parent) :QThread(parent)
{
	i = 0;
}


Thread2::~Thread2()
{
	//qDebug() << "thread_activated";
}

void Thread2::run()
{
	i++;
	qDebug() << "thread222_activated";
	//aa->progress->setValue(i);
	aa->cond.wakeOne();
	
}
