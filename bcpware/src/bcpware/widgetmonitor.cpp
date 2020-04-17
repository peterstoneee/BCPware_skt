#include "widgetmonitor.h"
#include "chipcommunication.h"
#include <future>
#include <string>
#include <exception>
#include <QMessagebox>
#include <QDebug>
#include "ui_printdialog.h"

WidgetMonitor::WidgetMonitor(RichParameterSet &printset,QWidget *parent)
	: QDialog(parent),
	white(255, 255, 255), cyan(0, 255, 255), magenta(255, 0, 255), yellow(255, 255, 0), binder(170, 180, 180, 150),
	inkFont("Consolas", 16, 8),
	cyanPixmap(NULL), cyanPainter(NULL), magentaPixmap(NULL), magentaPainter(NULL), yellowPixmap(NULL), yellowPainter(NULL),
	binderPixmap(NULL), binderPainter(NULL)
{
	//set ui
    //setupUi(this);
    ui = new Ui::PrintDialogFlow();
    WidgetMonitor::ui->setupUi(this);
    setParamTofilter(printset);

	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);


	/*connect( &tick, SIGNAL(timeout()), this, SLOT(updateInfo()) );
	tick.start(UPDATE_MSEC);*/
	


	if (ccbg.printerName)
        ui->lblPrinterSelect->setText(ccbg.printerName->c_str());
	updateInfo();

}

WidgetMonitor::~WidgetMonitor()
{

}
bool WidgetMonitor::setParamTofilter(RichParameterSet &parlst){
    ui->slicetHeightL->setText(QString::number(parlst.getFloat("slice_height")));
    ui->pageNumL->setText(QString::number(parlst.getInt("end_page")));

    return true;
}
void WidgetMonitor::updateInfo()
{
	std::string funGETouts;
	static auto funcGET = [&funGETouts](const char *cmd) -> bool
	{
		return ccbg.sendGET(cmd, ChipCommunication_t::GETParamPairSet(), funGETouts);
	};

	static std::string json;
	static ChipCommunication_t::GETParamPairSet emptyParamSet;
	static Json::Features features;
	static Json::Reader reader(features); 
	static Json::Value root;

	//setup pixmap and painter for ink diagram if not setup
	if (!cyanPixmap)
	{
        cyanPixmap = new QPixmap(ui->lblCyanSurface->width() - 6, ui->lblCyanSurface->height() - 6);
		cyanPainter = new QPainter(cyanPixmap);
		cyanPainter->setFont(inkFont);
	}
	if (!magentaPixmap)
	{
        magentaPixmap = new QPixmap(ui->lblMagentaSurface->width() - 6, ui->lblMagentaSurface->height() - 6);
		magentaPainter = new QPainter(magentaPixmap);
		magentaPainter->setFont(inkFont);
	}
	if (!yellowPixmap)
	{
        yellowPixmap = new QPixmap(ui->lblYellowSurface->width() - 6, ui->lblYellowSurface->height() - 6);
		yellowPainter = new QPainter(yellowPixmap);
		yellowPainter->setFont(inkFont);
	}
	if (!binderPixmap)
	{
        binderPixmap = new QPixmap(ui->lblBinderSurface->width() - 6, ui->lblBinderSurface->height() - 6);
		binderPainter = new QPainter(binderPixmap);
		binderPainter->setFont(inkFont);
	}

	//get /status/system data
	if (!funcGET("status/system"))
	{
		showNothing();
		return;
	}
	json.swap(funGETouts);


	//get /status/ink data, and parse /status/system data
	std::future<bool> futStatusInk = std::async(std::launch::async, funcGET, "status/ink");
	reader.parse(json.c_str(), root);
    ui->lblSystemState->setText(root["system"]["SystemState"].asCString());
    ui->lblPrintState->setText(root["system"]["PrintState"].asCString());
    ui->lblMaintState->setText(root["system"]["MaintState"].asCString());
    ui->lblMaintActiveState->setText(root["system"]["MaintActiveState"].asCString());
    ui->lblMaintError->setText(root["system"]["MaintError"].asCString());
    ui->lblMaintDesc->setText(root["system"]["MaintDesc"].asCString());
	//job count
    ui->lblJobPageCount->setText( QString::number(root["system"]["JobPageCount"].asInt()) );
    ui->lblPageCount->setText( QString::number(root["system"]["PageCount"].asInt()) );
    ui->lblJobCount->setText( QString::number(root["system"]["JobCount"].asInt()) );
	//firmware version
//    ui->lblFirmwareVersion->setText(root["system"]["FirmwareVersion"].asCString());
//    ui->lblFirmwareBuildDate->setText(root["system"]["FirmwareBuildDate"].asCString());
	if (!futStatusInk.get())
	{
		showNothing();
		return;
	}
	json.swap(funGETouts);


	//get /job/status data, and parse /status/ink data
	std::future<bool> futJobStatus = std::async(std::launch::async, funcGET, "job/status");
	reader.parse(json.c_str(), root);
    ui->lblCyanStatus->setText(root["ink"]["CyanStatus"].asCString());
    ui->lblMagentaStatus->setText(root["ink"]["MagentaStatus"].asCString());
    ui->lblYellowStatus->setText(root["ink"]["YellowStatus"].asCString());
    ui->lblBinderStatus->setText(root["ink"]["BlackStatus"].asCString());
	//---------------------------------------------------------------
	//cyan
	static float cl, cs, cp;
	static int ch;
	if (root["ink"]["CyanLevel"].isString() || root["ink"]["CyanTankSize"].isString())
	{
		cyanPainter->fillRect(0, 0, cyanPixmap->width(), cyanPixmap->height(), white);
		cyanPainter->drawText(
			cyanPixmap->width() / 2 - 25,
			cyanPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString("NA")
		);
        ui->lblCyanSurface->setPixmap(*cyanPixmap);
	}
	else //(root["ink"]["CyanLevel"].isInt())
	{
		cl = root["ink"]["CyanLevel"].asInt();
		cs = root["ink"]["CyanTankSize"].asInt();
		cp = cl / cs;
		ch = cyanPixmap->height()*cp;
		cyanPainter->fillRect(0, cyanPixmap->height() - ch, cyanPixmap->width(), ch, cyan);
		cyanPainter->fillRect(0, 0, cyanPixmap->width(), cyanPixmap->height() - ch, white);
		cyanPainter->drawText(
			cyanPixmap->width() / 2 - 25,
			cyanPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString::number(int(cp * 100)).append('%')
		);
        ui->lblCyanSurface->setPixmap(*cyanPixmap);
	}
	//magenta
	static float ml, ms, mp;
	static int mh;
	if (root["ink"]["MagentaLevel"].isString() || root["ink"]["MagentaTankSize"].isString())
	{
		magentaPainter->fillRect(0, 0, magentaPixmap->width(), magentaPixmap->height(), white);
		magentaPainter->drawText(
			magentaPixmap->width() / 2 - 25,
			magentaPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString("NA")
		);
        ui->lblMagentaSurface->setPixmap(*magentaPixmap);
	}
	else
	{
		ml = root["ink"]["MagentaLevel"].asInt();
		ms = root["ink"]["MagentaTankSize"].asInt();
		mp = ml / ms;
		mh = magentaPixmap->height()*mp;
		magentaPainter->fillRect(0, magentaPixmap->height() - mh, magentaPixmap->width(), mh, magenta);
		magentaPainter->fillRect(0, 0, magentaPixmap->width(), magentaPixmap->height() - mh, white);
		magentaPainter->drawText(
			magentaPixmap->width() / 2 - 25,
			magentaPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString::number(int(mp * 100)).append('%')
			);
        ui->lblMagentaSurface->setPixmap(*magentaPixmap);
	}
	//yellow
	static float yl, ys, yp;
	static int yh;
	if (root["ink"]["YellowLevel"].isString() || root["ink"]["YellowTankSize"].isString())
	{
		yellowPainter->fillRect(0, 0, yellowPixmap->width(), yellowPixmap->height(), white);
		yellowPainter->drawText(
			yellowPixmap->width() / 2 - 25,
			yellowPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString("NA")
		);
        ui->lblYellowSurface->setPixmap(*yellowPixmap);
	}
	else
	{
		yl = root["ink"]["YellowLevel"].asInt();
		ys = root["ink"]["YellowTankSize"].asInt();
		yp = yl / ys;
		yh = yellowPixmap->height()*yp;
		yellowPainter->fillRect(0, yellowPixmap->height() - yh, yellowPixmap->width(), yh, yellow);
		yellowPainter->fillRect(0, 0, yellowPixmap->width(), yellowPixmap->height() - yh, white);
		yellowPainter->drawText(
			yellowPixmap->width() / 2 - 25,
			yellowPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString::number(int(yp * 100)).append('%')
		);
        ui->lblYellowSurface->setPixmap(*yellowPixmap);
	}
	//binder
	static float bl, bs, bp;
	static int bh;
	if (root["ink"]["BlackLevel"].isString() || root["ink"]["BlackTankSize"].isString())
	{
		binderPainter->fillRect(0, 0, binderPixmap->width(), binderPixmap->height(), white);
		binderPainter->drawText(
			binderPixmap->width() / 2 - 25,
			binderPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString("NA")
		);
        ui->lblBinderSurface->setPixmap(*binderPixmap);
	}
	else
	{
		bl = root["ink"]["BlackLevel"].asInt();
		bs = root["ink"]["BlackTankSize"].asInt();
		bp = bl / bs;
		bh = binderPixmap->height()*bp;
		binderPainter->fillRect(0, binderPixmap->height() - bh, binderPixmap->width(), bh, binder);
		binderPainter->fillRect(0, 0, binderPixmap->width(), binderPixmap->height() - bh, white);
		binderPainter->drawText(
			binderPixmap->width() / 2 - 25,
			binderPixmap->height() / 2 - 25,
			50, 50,
			Qt::AlignVCenter | Qt::AlignHCenter,
			QString::number(int(bp * 100)).append('%')
		);
        ui->lblBinderSurface->setPixmap(*binderPixmap);
	}
	//----------------------------------------------------------------
	if (!futJobStatus.get())
	{
		showNothing();
		return;
	}
	json.swap(funGETouts);


	//get job/pages data, and parse /job/status data
	std::future<bool> futJobPages = std::async(std::launch::async, funcGET, "job/pages");
	reader.parse(json.c_str(), root);
    ui->lblAvailable->setText(root["job-status"]["Available"].asCString());
    ui->lblResolution->setText(root["job-status"]["Resolution"].asCString());
	if (root["job-status"]["PageWidth"].isString())
        ui->lblPageWidth->setText(root["job-status"]["PageWidth"].asCString());
	else //(root["job-status"]["PageWidth"].isInt())
        ui->lblPageWidth->setText(QString::number(root["job-status"]["PageWidth"].asInt()));
	if (root["job-status"]["PageLength"].isString())
        ui->lblPageLength->setText(root["job-status"]["PageLength"].asCString());
	else //(root["job-status"]["PageLength"].isInt())
        ui->lblPageLength->setText( QString::number(root["job-status"]["PageLength"].asInt()) );
	if (!futJobPages.get())
	{
		showNothing();
		return;
	}
	json.swap(funGETouts);


	//parse job/pages data
	reader.parse(json.c_str(), root);
	if (root["job-pages"]["Printed"].isString())
        ui->lblPagePrinted->setText(root["job-pages"]["Printed"].asCString());
	else //(root["job-pages"]["Printed"].isInt())
        ui->lblPagePrinted->setText(QString::number(root["job-pages"]["Printed"].asInt()));
	if (root["job-pages"]["Total"].isString())
        ui->lblPageTotal->setText(root["job-pages"]["Total"].asCString());
	else //(root["job-pages"]["Total"].isInt())
        ui->lblPageTotal->setText(QString::number(root["job-pages"]["Total"].asInt()));

	//determinworkable
	determineWorkable();

	

}

bool WidgetMonitor::determineWorkable()
{
   /* qDebug()<<"determinworkable";
    if (ui->lblSystemState->text() == ""){ QMessageBox::information(this, "not ready", "NOT Ready"); return true; }
	else { QMessageBox::information(this, "NOT Ready", "NOT Ready"); return false; }*/
	return true;


}
void WidgetMonitor::showNothing()
{
    ui->lblSystemState->setText("-");
    ui->lblPrintState->setText("-");
    ui->lblMaintState->setText("-");
    ui->lblMaintActiveState->setText("-");
    ui->lblMaintError->setText("-");
    ui->lblMaintDesc->setText("-");
    ui->lblJobPageCount->setText("-");
    ui->lblPageCount->setText("-");
    ui->lblJobCount->setText("-");
//    ui->lblFirmwareVersion->setText("-");
//    ui->lblFirmwareBuildDate->setText("-");

    ui->lblCyanStatus->setText("-");
    ui->lblMagentaStatus->setText("-");
    ui->lblYellowStatus->setText("-");
    ui->lblBinderStatus->setText("-");

    ui->lblAvailable->setText("-");
    ui->lblResolution->setText("-");
    ui->lblPageWidth->setText("-");
    ui->lblPageLength->setText("-");

    ui->lblPagePrinted->setText("-");
    ui->lblPageTotal->setText("-");

	cyanPainter->fillRect(0, 0, cyanPixmap->width(), cyanPixmap->height(), white);
    ui->lblCyanSurface->setPixmap(*cyanPixmap);
	magentaPainter->fillRect(0, 0, magentaPixmap->width(), magentaPixmap->height(), white);
    ui->lblMagentaSurface->setPixmap(*magentaPixmap);
	yellowPainter->fillRect(0, 0, yellowPixmap->width(), yellowPixmap->height(), white);
    ui->lblYellowSurface->setPixmap(*yellowPixmap);
	binderPainter->fillRect(0, 0, binderPixmap->width(), binderPixmap->height(), white);
    ui->lblBinderSurface->setPixmap(*binderPixmap);
}

//void WidgetMonitor::setupUi(QWidget *WidgetMonitor)
//{
//	if (WidgetMonitor->objectName().isEmpty())
//		WidgetMonitor->setObjectName(QStringLiteral("WidgetMonitor"));
//	WidgetMonitor->resize(830, 493);
//	WidgetMonitor->setMinimumSize(QSize(800, 450));
//	gridLayout_2 = new QGridLayout(WidgetMonitor);
//	gridLayout_2->setSpacing(12);
//	gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
//	gridLayout = new QGridLayout();
//	gridLayout->setObjectName(QStringLiteral("gridLayout"));
//	label = new QLabel(WidgetMonitor);
//	label->setObjectName(QStringLiteral("label"));
//	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
//	sizePolicy.setHorizontalStretch(0);
//	sizePolicy.setVerticalStretch(0);
//	sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
//	label->setSizePolicy(sizePolicy);
//	QFont font;
//	font.setBold(true);
//	font.setWeight(75);
//	label->setFont(font);

//	gridLayout->addWidget(label, 0, 0, 1, 1);

//	lblPrinterSelect = new QLabel(WidgetMonitor);
//	lblPrinterSelect->setObjectName(QStringLiteral("lblPrinterSelect"));

//	gridLayout->addWidget(lblPrinterSelect, 0, 1, 1, 1);


//	gridLayout_2->addLayout(gridLayout, 0, 0, 1, 2);

//	groupBox = new QGroupBox(WidgetMonitor);
//	groupBox->setObjectName(QStringLiteral("groupBox"));
//	QFont font1;
//	font1.setBold(true);
//	font1.setUnderline(false);
//	font1.setWeight(75);
//	groupBox->setFont(font1);
//	groupBox->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 2px solid gray;\n"
//		"    border-radius: 5px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_3 = new QGridLayout(groupBox);
//	gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
//	groupBox_5 = new QGroupBox(groupBox);
//	groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
//	groupBox_5->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 1px solid gray;\n"
//		"    border-radius: 9px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_5 = new QGridLayout(groupBox_5);
//	gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
//	label_2 = new QLabel(groupBox_5);
//	label_2->setObjectName(QStringLiteral("label_2"));

//	gridLayout_5->addWidget(label_2, 0, 0, 1, 1);

//	label_6 = new QLabel(groupBox_5);
//	label_6->setObjectName(QStringLiteral("label_6"));

//	gridLayout_5->addWidget(label_6, 1, 0, 1, 1);

//	lblJobPageCount = new QLabel(groupBox_5);
//	lblJobPageCount->setObjectName(QStringLiteral("lblJobPageCount"));
//	lblJobPageCount->setAlignment(Qt::AlignCenter);

//	gridLayout_5->addWidget(lblJobPageCount, 0, 1, 1, 1);

//	lblPageCount = new QLabel(groupBox_5);
//	lblPageCount->setObjectName(QStringLiteral("lblPageCount"));
//	lblPageCount->setAlignment(Qt::AlignCenter);

//	gridLayout_5->addWidget(lblPageCount, 1, 1, 1, 1);

//	label_10 = new QLabel(groupBox_5);
//	label_10->setObjectName(QStringLiteral("label_10"));

//	gridLayout_5->addWidget(label_10, 2, 0, 1, 1);

//	lblJobCount = new QLabel(groupBox_5);
//	lblJobCount->setObjectName(QStringLiteral("lblJobCount"));
//	lblJobCount->setAlignment(Qt::AlignCenter);

//	gridLayout_5->addWidget(lblJobCount, 2, 1, 1, 1);


//	gridLayout_3->addWidget(groupBox_5, 0, 1, 1, 1);

//	groupBox_3 = new QGroupBox(groupBox);
//	groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
//	groupBox_3->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 1px solid gray;\n"
//		"    border-radius: 9px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_4 = new QGridLayout(groupBox_3);
//	gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
//	label_3 = new QLabel(groupBox_3);
//	label_3->setObjectName(QStringLiteral("label_3"));
//	sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
//	label_3->setSizePolicy(sizePolicy);

//	gridLayout_4->addWidget(label_3, 0, 0, 1, 1);

//	label_7 = new QLabel(groupBox_3);
//	label_7->setObjectName(QStringLiteral("label_7"));
//	sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
//	label_7->setSizePolicy(sizePolicy);
//	label_7->setMinimumSize(QSize(0, 0));

//	gridLayout_4->addWidget(label_7, 7, 0, 1, 1);

//	label_5 = new QLabel(groupBox_3);
//	label_5->setObjectName(QStringLiteral("label_5"));
//	sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
//	label_5->setSizePolicy(sizePolicy);

//	gridLayout_4->addWidget(label_5, 2, 0, 1, 1);

//	lblSystemState = new QLabel(groupBox_3);
//	lblSystemState->setObjectName(QStringLiteral("lblSystemState"));
//	lblSystemState->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblSystemState, 0, 1, 1, 1);

//	lblPrintState = new QLabel(groupBox_3);
//	lblPrintState->setObjectName(QStringLiteral("lblPrintState"));
//	lblPrintState->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblPrintState, 2, 1, 1, 1);

//	lblMaintActiveState = new QLabel(groupBox_3);
//	lblMaintActiveState->setObjectName(QStringLiteral("lblMaintActiveState"));
//	lblMaintActiveState->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblMaintActiveState, 7, 1, 1, 1);

//	label_11 = new QLabel(groupBox_3);
//	label_11->setObjectName(QStringLiteral("label_11"));
//	sizePolicy.setHeightForWidth(label_11->sizePolicy().hasHeightForWidth());
//	label_11->setSizePolicy(sizePolicy);

//	gridLayout_4->addWidget(label_11, 9, 0, 1, 1);

//	label_23 = new QLabel(groupBox_3);
//	label_23->setObjectName(QStringLiteral("label_23"));

//	gridLayout_4->addWidget(label_23, 5, 0, 1, 1);

//	lblMaintState = new QLabel(groupBox_3);
//	lblMaintState->setObjectName(QStringLiteral("lblMaintState"));
//	lblMaintState->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblMaintState, 5, 1, 1, 1);

//	label_9 = new QLabel(groupBox_3);
//	label_9->setObjectName(QStringLiteral("label_9"));
//	sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
//	label_9->setSizePolicy(sizePolicy);

//	gridLayout_4->addWidget(label_9, 8, 0, 1, 1);

//	lblMaintError = new QLabel(groupBox_3);
//	lblMaintError->setObjectName(QStringLiteral("lblMaintError"));
//	lblMaintError->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblMaintError, 8, 1, 1, 1);

//	lblMaintDesc = new QLabel(groupBox_3);
//	lblMaintDesc->setObjectName(QStringLiteral("lblMaintDesc"));
//	lblMaintDesc->setAlignment(Qt::AlignCenter);

//	gridLayout_4->addWidget(lblMaintDesc, 9, 1, 1, 1);


//	gridLayout_3->addWidget(groupBox_3, 0, 0, 2, 1);


//	gridLayout_2->addWidget(groupBox, 1, 0, 5, 2);

//	groupBox_6 = new QGroupBox(WidgetMonitor);
//	groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
//	groupBox_6->setFont(font);
//	groupBox_6->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 2px solid gray;\n"
//		"    border-radius: 5px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_8 = new QGridLayout(groupBox_6);
//	gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
//	groupBox_7 = new QGroupBox(groupBox_6);
//	groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
//	groupBox_7->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 1px solid gray;\n"
//		"    border-radius: 9px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_9 = new QGridLayout(groupBox_7);
//	gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
//	lblResolution = new QLabel(groupBox_7);
//	lblResolution->setObjectName(QStringLiteral("lblResolution"));
//	lblResolution->setAlignment(Qt::AlignCenter);

//	gridLayout_9->addWidget(lblResolution, 1, 1, 1, 3);

//	lblPageWidth = new QLabel(groupBox_7);
//	lblPageWidth->setObjectName(QStringLiteral("lblPageWidth"));
//	lblPageWidth->setAlignment(Qt::AlignCenter);

//	gridLayout_9->addWidget(lblPageWidth, 2, 1, 1, 3);

//	label_28 = new QLabel(groupBox_7);
//	label_28->setObjectName(QStringLiteral("label_28"));

//	gridLayout_9->addWidget(label_28, 1, 0, 1, 1);

//	label_26 = new QLabel(groupBox_7);
//	label_26->setObjectName(QStringLiteral("label_26"));

//	gridLayout_9->addWidget(label_26, 0, 0, 1, 1);

//	label_30 = new QLabel(groupBox_7);
//	label_30->setObjectName(QStringLiteral("label_30"));

//	gridLayout_9->addWidget(label_30, 2, 0, 1, 1);

//	lblAvailable = new QLabel(groupBox_7);
//	lblAvailable->setObjectName(QStringLiteral("lblAvailable"));
//	lblAvailable->setAlignment(Qt::AlignCenter);

//	gridLayout_9->addWidget(lblAvailable, 0, 1, 1, 3);

//	lblPageLength = new QLabel(groupBox_7);
//	lblPageLength->setObjectName(QStringLiteral("lblPageLength"));
//	lblPageLength->setAlignment(Qt::AlignCenter);

//	gridLayout_9->addWidget(lblPageLength, 3, 1, 1, 3);

//	label_32 = new QLabel(groupBox_7);
//	label_32->setObjectName(QStringLiteral("label_32"));
//	label_32->setBaseSize(QSize(0, 0));

//	gridLayout_9->addWidget(label_32, 3, 0, 1, 1);

//	label_14 = new QLabel(groupBox_7);
//	label_14->setObjectName(QStringLiteral("label_14"));

//	gridLayout_9->addWidget(label_14, 4, 0, 1, 1);

//	label_27 = new QLabel(groupBox_7);
//	label_27->setObjectName(QStringLiteral("label_27"));
//	sizePolicy.setHeightForWidth(label_27->sizePolicy().hasHeightForWidth());
//	label_27->setSizePolicy(sizePolicy);

//	gridLayout_9->addWidget(label_27, 4, 2, 1, 1);

//	lblPagePrinted = new QLabel(groupBox_7);
//	lblPagePrinted->setObjectName(QStringLiteral("lblPagePrinted"));
//	lblPagePrinted->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

//	gridLayout_9->addWidget(lblPagePrinted, 4, 1, 1, 1);

//	lblPageTotal = new QLabel(groupBox_7);
//	lblPageTotal->setObjectName(QStringLiteral("lblPageTotal"));

//	gridLayout_9->addWidget(lblPageTotal, 4, 3, 1, 1);


//	gridLayout_8->addWidget(groupBox_7, 0, 0, 2, 1);

//	groupBox_9 = new QGroupBox(groupBox_6);
//	groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
//	groupBox_9->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 1px solid gray;\n"
//		"    border-radius: 9px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_11 = new QGridLayout(groupBox_9);
//	gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
//	pushButton = new QPushButton(groupBox_9);
//	pushButton->setObjectName(QStringLiteral("pushButton"));

//	gridLayout_11->addWidget(pushButton, 0, 0, 1, 1);

//	label_12 = new QLabel(groupBox_9);
//	label_12->setObjectName(QStringLiteral("label_12"));
//	label_12->setAlignment(Qt::AlignCenter);

//	gridLayout_11->addWidget(label_12, 0, 1, 1, 1);


//	gridLayout_8->addWidget(groupBox_9, 2, 0, 1, 1);


//	gridLayout_2->addWidget(groupBox_6, 5, 2, 1, 1);

//	groupBox_2 = new QGroupBox(WidgetMonitor);
//	groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
//	groupBox_2->setFont(font);
//	groupBox_2->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 2px solid gray;\n"
//		"    border-radius: 5px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_7 = new QGridLayout(groupBox_2);
//	gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
//	lblCyanStatus = new QLabel(groupBox_2);
//	lblCyanStatus->setObjectName(QStringLiteral("lblCyanStatus"));
//	QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
//	sizePolicy1.setHorizontalStretch(0);
//	sizePolicy1.setVerticalStretch(0);
//	sizePolicy1.setHeightForWidth(lblCyanStatus->sizePolicy().hasHeightForWidth());
//	lblCyanStatus->setSizePolicy(sizePolicy1);
//	lblCyanStatus->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblCyanStatus, 2, 0, 1, 1);

//	lblYellowStatus = new QLabel(groupBox_2);
//	lblYellowStatus->setObjectName(QStringLiteral("lblYellowStatus"));
//	sizePolicy1.setHeightForWidth(lblYellowStatus->sizePolicy().hasHeightForWidth());
//	lblYellowStatus->setSizePolicy(sizePolicy1);
//	lblYellowStatus->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblYellowStatus, 2, 2, 1, 1);

//	label_21 = new QLabel(groupBox_2);
//	label_21->setObjectName(QStringLiteral("label_21"));
//	sizePolicy1.setHeightForWidth(label_21->sizePolicy().hasHeightForWidth());
//	label_21->setSizePolicy(sizePolicy1);
//	label_21->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(label_21, 1, 1, 1, 1);

//	label_22 = new QLabel(groupBox_2);
//	label_22->setObjectName(QStringLiteral("label_22"));
//	sizePolicy1.setHeightForWidth(label_22->sizePolicy().hasHeightForWidth());
//	label_22->setSizePolicy(sizePolicy1);
//	label_22->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(label_22, 1, 2, 1, 1);

//	lblYellowSurface = new QLabel(groupBox_2);
//	lblYellowSurface->setObjectName(QStringLiteral("lblYellowSurface"));
//	lblYellowSurface->setStyleSheet(QLatin1String("QLabel {\n"
//		"    border: 3px solid yellow;\n"
//		"    border-radius: 4px;\n"
//		"    padding: 0px;\n"
//		"}"));
//	lblYellowSurface->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblYellowSurface, 0, 2, 1, 1);

//	lblMagentaStatus = new QLabel(groupBox_2);
//	lblMagentaStatus->setObjectName(QStringLiteral("lblMagentaStatus"));
//	sizePolicy1.setHeightForWidth(lblMagentaStatus->sizePolicy().hasHeightForWidth());
//	lblMagentaStatus->setSizePolicy(sizePolicy1);
//	lblMagentaStatus->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblMagentaStatus, 2, 1, 1, 1);

//	label_20 = new QLabel(groupBox_2);
//	label_20->setObjectName(QStringLiteral("label_20"));
//	sizePolicy1.setHeightForWidth(label_20->sizePolicy().hasHeightForWidth());
//	label_20->setSizePolicy(sizePolicy1);
//	label_20->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(label_20, 1, 0, 1, 1);

//	lblMagentaSurface = new QLabel(groupBox_2);
//	lblMagentaSurface->setObjectName(QStringLiteral("lblMagentaSurface"));
//	lblMagentaSurface->setStyleSheet(QLatin1String("QLabel {\n"
//		"    border: 3px solid magenta;\n"
//		"    border-radius: 4px;\n"
//		"    padding: 0px;\n"
//		"}"));
//	lblMagentaSurface->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblMagentaSurface, 0, 1, 1, 1);

//	lblCyanSurface = new QLabel(groupBox_2);
//	lblCyanSurface->setObjectName(QStringLiteral("lblCyanSurface"));
//	lblCyanSurface->setStyleSheet(QLatin1String("QLabel {\n"
//		"    border: 3px solid cyan;\n"
//		"    border-radius: 4px;\n"
//		"    padding: 0px;\n"
//		"}"));
//	lblCyanSurface->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblCyanSurface, 0, 0, 1, 1);

//	lblBinderSurface = new QLabel(groupBox_2);
//	lblBinderSurface->setObjectName(QStringLiteral("lblBinderSurface"));
//	lblBinderSurface->setStyleSheet(QLatin1String("QLabel {\n"
//		"    border: 3px solid rgba(170, 180, 180, 150);\n"
//		"    border-radius: 4px;\n"
//		"    padding: 0px;\n"
//		"}"));
//	lblBinderSurface->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblBinderSurface, 0, 3, 1, 1);

//	label_29 = new QLabel(groupBox_2);
//	label_29->setObjectName(QStringLiteral("label_29"));
//	sizePolicy1.setHeightForWidth(label_29->sizePolicy().hasHeightForWidth());
//	label_29->setSizePolicy(sizePolicy1);
//	label_29->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(label_29, 1, 3, 1, 1);

//	lblBinderStatus = new QLabel(groupBox_2);
//	lblBinderStatus->setObjectName(QStringLiteral("lblBinderStatus"));
//	sizePolicy1.setHeightForWidth(lblBinderStatus->sizePolicy().hasHeightForWidth());
//	lblBinderStatus->setSizePolicy(sizePolicy1);
//	lblBinderStatus->setAlignment(Qt::AlignCenter);

//	gridLayout_7->addWidget(lblBinderStatus, 2, 3, 1, 1);


//	gridLayout_2->addWidget(groupBox_2, 6, 0, 1, 1);

//	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

//	gridLayout_2->addItem(verticalSpacer, 6, 1, 1, 1);

//	groupBox_4 = new QGroupBox(WidgetMonitor);
//	groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
//	groupBox_4->setStyleSheet(QLatin1String("QGroupBox {\n"
//		"    border: 1px solid gray;\n"
//		"    border-radius: 9px;\n"
//		"    margin-top: 0.5em;\n"
//		"}\n"
//		"QGroupBox::title {\n"
//		"    subcontrol-origin: margin;\n"
//		"    left: 10px;\n"
//		"    padding: 0 3px 0 3px;\n"
//		"}"));
//	gridLayout_6 = new QGridLayout(groupBox_4);
//	gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
//	lblFirmwareVersion = new QLabel(groupBox_4);
//	lblFirmwareVersion->setObjectName(QStringLiteral("lblFirmwareVersion"));
//	lblFirmwareVersion->setAlignment(Qt::AlignCenter);

//	gridLayout_6->addWidget(lblFirmwareVersion, 0, 1, 1, 1);

//	label_13 = new QLabel(groupBox_4);
//	label_13->setObjectName(QStringLiteral("label_13"));

//	gridLayout_6->addWidget(label_13, 0, 0, 1, 1);

//	label_15 = new QLabel(groupBox_4);
//	label_15->setObjectName(QStringLiteral("label_15"));

//	gridLayout_6->addWidget(label_15, 1, 0, 1, 1);

//	lblFirmwareBuildDate = new QLabel(groupBox_4);
//	lblFirmwareBuildDate->setObjectName(QStringLiteral("lblFirmwareBuildDate"));
//	lblFirmwareBuildDate->setAlignment(Qt::AlignCenter);

//	gridLayout_6->addWidget(lblFirmwareBuildDate, 1, 1, 1, 1);


//	gridLayout_2->addWidget(groupBox_4, 6, 2, 1, 1);


//	retranslateUi(WidgetMonitor);

//	QMetaObject::connectSlotsByName(WidgetMonitor);
//} // setupUi

//void WidgetMonitor::retranslateUi(QWidget *WidgetMonitor)
//{
//	WidgetMonitor->setWindowTitle(QApplication::translate("WidgetMonitor", "Monitor", 0));
//	label->setText(QApplication::translate("WidgetMonitor", "Printer: ", 0));
//	lblPrinterSelect->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox->setTitle(QApplication::translate("WidgetMonitor", "System", 0));
//	groupBox_5->setTitle(QApplication::translate("WidgetMonitor", "Count", 0));
//	label_2->setText(QApplication::translate("WidgetMonitor", "Page Count in Job :", 0));
//	label_6->setText(QApplication::translate("WidgetMonitor", "Page Count :", 0));
//	lblJobPageCount->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblPageCount->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_10->setText(QApplication::translate("WidgetMonitor", "Job Count :", 0));
//	lblJobCount->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox_3->setTitle(QApplication::translate("WidgetMonitor", "Status", 0));
//	label_3->setText(QApplication::translate("WidgetMonitor", "System State : ", 0));
//	label_7->setText(QApplication::translate("WidgetMonitor", "Maintenance Active State :", 0));
//	label_5->setText(QApplication::translate("WidgetMonitor", "Print State :", 0));
//	lblSystemState->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblPrintState->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblMaintActiveState->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_11->setText(QApplication::translate("WidgetMonitor", "Maintenance Description :", 0));
//	label_23->setText(QApplication::translate("WidgetMonitor", "Maintenance State :", 0));
//	lblMaintState->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_9->setText(QApplication::translate("WidgetMonitor", "Maintenance Error :", 0));
//	lblMaintError->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblMaintDesc->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox_6->setTitle(QApplication::translate("WidgetMonitor", "Job", 0));
//	groupBox_7->setTitle(QApplication::translate("WidgetMonitor", "Status", 0));
//	lblResolution->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblPageWidth->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_28->setText(QApplication::translate("WidgetMonitor", "Resolution :", 0));
//	label_26->setText(QApplication::translate("WidgetMonitor", "Available :", 0));
//	label_30->setText(QApplication::translate("WidgetMonitor", "Page Width :", 0));
//	lblAvailable->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblPageLength->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_32->setText(QApplication::translate("WidgetMonitor", "Page Length :", 0));
//	label_14->setText(QApplication::translate("WidgetMonitor", "Pages :", 0));
//	label_27->setText(QApplication::translate("WidgetMonitor", "/", 0));
//	lblPagePrinted->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblPageTotal->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox_9->setTitle(QApplication::translate("WidgetMonitor", "Cancel", 0));
//	pushButton->setText(QApplication::translate("WidgetMonitor", "Do it !", 0));
//	label_12->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox_2->setTitle(QApplication::translate("WidgetMonitor", "Ink", 0));
//	lblCyanStatus->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	lblYellowStatus->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_21->setText(QApplication::translate("WidgetMonitor", "Magenta", 0));
//	label_22->setText(QApplication::translate("WidgetMonitor", "Yellow", 0));
//	lblYellowSurface->setText(QString());
//	lblMagentaStatus->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_20->setText(QApplication::translate("WidgetMonitor", "Cyan", 0));
//	lblMagentaSurface->setText(QString());
//	lblCyanSurface->setText(QString());
//	lblBinderSurface->setText(QString());
//	label_29->setText(QApplication::translate("WidgetMonitor", "Binder", 0));
//	lblBinderStatus->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	groupBox_4->setTitle(QApplication::translate("WidgetMonitor", "Firmware", 0));
//	lblFirmwareVersion->setText(QApplication::translate("WidgetMonitor", "-", 0));
//	label_13->setText(QApplication::translate("WidgetMonitor", "Firmware Version :", 0));
//	label_15->setText(QApplication::translate("WidgetMonitor", "Firmware Build Date :", 0));
//	lblFirmwareBuildDate->setText(QApplication::translate("WidgetMonitor", "-", 0));
//} // retranslateUi

void WidgetMonitor::on_printOk_clicked()
{
    accept();
	 
}

void WidgetMonitor::on_buttonBox_accepted()
{
    accept();
}

void WidgetMonitor::on_buttonBox_rejected()
{
    reject();
}
