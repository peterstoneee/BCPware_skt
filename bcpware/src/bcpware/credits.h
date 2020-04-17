#ifndef CREDITS_H
#define CREDITS_H

#include <QDialog>
#include "ui_credits.h"

class credits : public QDialog
{
	Q_OBJECT

public:
	credits(QWidget *parent = 0);
	~credits();

private:
	Ui::credits ui;
};

#endif // CREDITS_H
