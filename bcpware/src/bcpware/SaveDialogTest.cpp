#include "SaveDialogTest.h"
#include <QPushButton>

SaveDialogTest::SaveDialogTest() :QFileDialog()
{
	this->setOption(QFileDialog::DontUseNativeDialog,true);
	generateform();
}
SaveDialogTest::SaveDialogTest(QWidget *parent)
{
	this->setOption(QFileDialog::DontUseNativeDialog, true);
	generateform();
}

void SaveDialogTest::generateform()
{
	/*QVBoxLayout *l1 = new QVBoxLayout;
	bt1 = new QPushButton("bt1");
	l1->addWidget(bt1);
	this->setLayout(l1);

	this->layout()->addWidget(bt1);*/

}

SaveDialogTest::~SaveDialogTest()
{

}
