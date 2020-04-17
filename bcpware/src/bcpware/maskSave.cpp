#include "maskSave.h"
#include <QDialogButtonbox>

maskSave::maskSave() :QDialog()
{
	initForm();
}
maskSave::maskSave(QWidget *parent, MeshModel *m, int capability, int defaultBits, RichParameterSet *par, GLArea* glar) : QDialog(parent), m(m),
mask(0), capability(capability), defaultBits(defaultBits), glar(glar)
{
	initForm();

}

void maskSave::initForm()
{
	cb1 = new QCheckBox("cb1");
	cb2 = new QCheckBox("cb2");
	
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->addWidget(cb1);
	l1->addWidget(cb2);
	

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	l1->addWidget(buttonBox);

	this->setLayout(l1);


}

maskSave::~maskSave()
{
}
