#include "customizewidget.h"


CustomizeWidget::CustomizeWidget(QWidget *parent) :
    QWidget(parent)
{
}


CustomizeGroupBox::CustomizeGroupBox(const QString &title, QWidget *p):QGroupBox(title,p)
{
    setStyleSheet("QGroupBox{	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,	stop : 0 #E0E0E0, stop: 1 #FFFFFF);}"
            "QGroupBox{ border: 1px solid gray;}"
            "QGroupBox{border-radius: 5px;}"
            "QGroupBox{margin-bottom: 2ex; /* leave space at the top for the title */}"
            "QGroupBox::title{subcontrol-origin: margin; subcontrol-position: bottom center;padding: 0 3px;	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 #FFAECE, stop: 1 #FFFFFF);}"
			);
	//setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
}
