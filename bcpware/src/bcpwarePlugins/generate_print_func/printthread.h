#ifndef PRINTTHREAD_H
#define PRINTTHREAD_H

#include <QThread>
#include <QMutex>
class PrintThread : public QThread
{
    Q_OBJECT
public:
    explicit PrintThread(QObject *parent = 0);
    bool print_stop;
	int job_page_count;
	int count;
    void pt_stop();
signals:


	public slots :
		void page_count(int i);

private:
    void run();
    QMutex mutex1;
	void getFile();
};

#endif // PRINTTHREAD_H
