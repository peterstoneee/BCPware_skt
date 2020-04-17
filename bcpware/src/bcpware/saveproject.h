
#ifndef SAVEPROJECT_H
#define SAVEPROJECT_H

#include <QObject>
#include <QFileDialog>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include <QDialogButtonBox>



class SaveProject : public QDialog
{
 Q_OBJECT
public:
    //explicit SaveProject(QWidget *parent = 0);
     SaveProject(QWidget *parent = 0);
    ~SaveProject();
signals:


public slots:
     void browse();

private:
    QComboBox *projectNameComboBox;
    QComboBox *directoryComboBox;
	//QComboBox *projectFileName;

    QLabel *projectName;
    QLabel *savePosition;
	//QLabel *

	QDialogButtonBox *ok_cancel_btn_box;

    QPushButton *browseBtn;

    QComboBox *createComboBox(const QString &text = QString());
    QLabel *createLabel(const QString &text = QString());
    QPushButton *createButton(const QString &text, const char *member);
public :
	QStringList *returndata();

};

#endif // SAVEPROJECT_H
