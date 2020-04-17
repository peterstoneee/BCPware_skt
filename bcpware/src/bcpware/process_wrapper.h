#ifndef PROCESS_WRAPPER_H
#define PROCESS_WRAPPER_H

#include <QObject>
#include <QProcess>


class process_wrapper : public QObject,public QProcess
{
    Q_OBJECT
public:
    explicit process_wrapper(QObject *parent = 0);
	process_wrapper(QProcess *pp =0 );
	void startt(const QString & program, const QStringList & arguments);
private:
    QProcess *qp1;

signals:

public slots:

};


//QProcess *g_process = NULL;
//class ProcOut : public QObject
//{
//	Q_OBJECT
//public:
//	ProcOut(QObject *parent = NULL);
//	virtual ~ProcOut() {};
//
//	public slots:
//	void readyRead();
//	void finished();
//};
//
//ProcOut::ProcOut(QObject *parent /* = NULL */) :
//QObject(parent)
//{}
//
//void ProcOut::readyRead()
//{
//	if (!g_process)
//		return;
//
//	QTextStream out(stdout);
//	out << g_process->readAllStandardOutput() << endl;
//}
//
//void ProcOut::finished()
//{
//	//QCoreApplication::exit(0);
//}

#endif // PROCESS_WRAPPER_H
