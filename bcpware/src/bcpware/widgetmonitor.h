#ifndef WIDGETMONITOR_H
#define WIDGETMONITOR_H

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDialog>

#include <common/interfaces.h>


#define UPDATE_MSEC 500
namespace Ui
{
    class PrintDialogFlow;
}

class WidgetMonitor : public QDialog
{
	Q_OBJECT

public:
    WidgetMonitor(RichParameterSet &printparameter,QWidget *parent = 0);
	~WidgetMonitor();

	/*variables used for ink diagram
	*/
private:
	const QColor white;
	const QColor cyan;
	const QColor magenta;
	const QColor yellow;
	const QColor binder;
	const QFont inkFont;
	QPixmap *cyanPixmap;
	QPainter *cyanPainter;
	QPixmap *magentaPixmap;
	QPainter *magentaPainter;
	QPixmap *yellowPixmap;
	QPainter *yellowPainter;
	QPixmap *binderPixmap;
	QPainter *binderPainter;
    RichParameterSet *printSet;

private:
	QTimer tick;
	private slots:
	void updateInfo();
	//void cboxPrintersSlot(int);

    void on_printOk_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
	void showNothing();
//public:
signals:
	void permition();

public slots:
	bool determineWorkable();
public :
    bool setParamTofilter(RichParameterSet &parlst);
	 
private:
    Ui::PrintDialogFlow *ui;



//public:
//    QGridLayout *gridLayout_2;
//        QGridLayout *gridLayout;
//        QLabel *label;
//        QLabel *lblPrinterSelect;
//        QGroupBox *groupBox;
//        QGridLayout *gridLayout_3;
//        QGroupBox *groupBox_5;
//        QGridLayout *gridLayout_5;
//        QLabel *label_2;
//        QLabel *label_6;
//        QLabel *lblJobPageCount;
//        QLabel *lblPageCount;
//        QLabel *label_10;
//        QLabel *lblJobCount;
//        QGroupBox *groupBox_3;
//        QGridLayout *gridLayout_4;
//        QLabel *label_3;
//        QLabel *label_7;
//        QLabel *label_5;
//        QLabel *lblSystemState;
//        QLabel *lblPrintState;
//        QLabel *lblMaintActiveState;
//        QLabel *label_11;
//        QLabel *label_23;
//        QLabel *lblMaintState;
//        QLabel *label_9;
//        QLabel *lblMaintError;
//        QLabel *lblMaintDesc;
//        QGroupBox *groupBox_6;
//        QGridLayout *gridLayout_8;
//        QGroupBox *groupBox_7;
//        QGridLayout *gridLayout_9;
//        QLabel *lblResolution;
//        QLabel *lblPageWidth;
//        QLabel *label_28;
//        QLabel *label_26;
//        QLabel *label_30;
//        QLabel *lblAvailable;
//        QLabel *lblPageLength;
//        QLabel *label_32;
//        QLabel *label_14;
//        QLabel *label_27;
//        QLabel *lblPagePrinted;
//        QLabel *lblPageTotal;
//        QGroupBox *groupBox_9;
//        QGridLayout *gridLayout_11;
//        QPushButton *pushButton;
//        QLabel *label_12;
//        QGroupBox *groupBox_2;
//        QGridLayout *gridLayout_7;
//        QLabel *lblCyanStatus;
//        QLabel *lblYellowStatus;
//        QLabel *label_21;
//        QLabel *label_22;
//        QLabel *lblYellowSurface;
//        QLabel *lblMagentaStatus;
//        QLabel *label_20;
//        QLabel *lblMagentaSurface;
//        QLabel *lblCyanSurface;
//        QLabel *lblBinderSurface;
//        QLabel *label_29;
//        QLabel *lblBinderStatus;
//        QSpacerItem *verticalSpacer;
//        QGroupBox *groupBox_4;
//        QGridLayout *gridLayout_6;
//        QLabel *lblFirmwareVersion;
//        QLabel *label_13;
//        QLabel *label_15;
//        QLabel *lblFirmwareBuildDate;


//	void setupUi(QWidget*);
//	void retranslateUi(QWidget*);
};

#endif // WIDGETMONITOR_H
