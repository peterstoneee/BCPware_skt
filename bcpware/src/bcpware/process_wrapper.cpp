#include "process_wrapper.h"

process_wrapper::process_wrapper(QObject *parent) :
    QObject(parent)
{
	qp1 = new QProcess(this);
}
process_wrapper::process_wrapper(QProcess *pp) //: qp1(new QProcess(pp))
{
	qp1 = new QProcess(this);
}
void process_wrapper::startt(const QString & program, const QStringList & arguments)
{
	qp1->start(program, arguments);
	qp1->write("dir \n");
	//this->start(program);
}
