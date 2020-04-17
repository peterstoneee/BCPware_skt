#include "credits.h"

credits::credits(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle("Credit");
	ui.credit_icon->setPixmap(QPixmap(":/images/icons/credit_icon.png"));
}

credits::~credits()
{

}
