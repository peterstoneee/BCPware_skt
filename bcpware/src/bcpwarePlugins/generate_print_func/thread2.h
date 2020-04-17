#pragma once
#include <QThread>
#include <QProgressDialog>

class ExtraSampleGPUPlugin;
class Thread2:public QThread 
{
	Q_OBJECT
public:
	 explicit Thread2(QObject *parent = 0);
	~Thread2();
	void run();
public:
	QProgressDialog *pgd;
	ExtraSampleGPUPlugin *aa;
	int i;

};

