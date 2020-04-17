#pragma once

#include <QtWidgets>
#include <QGLWidget>

class MyView:public QGraphicsView
{
	
public:
	MyView()
	{
		setWindowTitle(tr("helloworld"));

	}
	//~MyView();

public slots:
virtual void setViewport(QWidget *viewport)
{
	/*QGLWidget *glWidget = qobject_cast<QGLWidget*>(viewport);
	if (glWidget)
		glWidget->updateGL();*/

}

};

