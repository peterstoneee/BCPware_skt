/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
* My Extension to Meshlab
*
*  Copyright 2018 by 
*
* This file is part of Meshlab.
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//None of this should happen if we are compiling c, not c++
#ifdef __cplusplus

#include <GL/glew.h>

#include "CheckListWidget.h"
#include "../common/pluginmanager.h"
#include "../common/scriptinterface.h"
#include "command.h"
#include "../common/picaApplication.h"
#include "glarea.h"
#include "layerDialog.h"
#include "stdpardialog.h"
#include "xmlstdpardialog.h"
#include "xmlgeneratorgui.h"
#include "customizewidget.h"
#include "skt_function.h"
#include "SaveDialogTest.h"
#include "maskSave.h"
#include "transformPreview.h"
#include "printerJobInfo.h"
//#include "widgetmonitor.h"
#include "gui_launcher.h"
#include "my_thread.h"
#include "myview.h"
#include "saveproject.h"
//#include "CheckListWidget.h"
#include "printOption.h"
#include "meshcheck3.h"
#include "objectList.h"
#include "versionCheck.h"
#include "dockInfoWIdget.h"
#include "widgetStyleSheet.h"
#include "setting3DP.h"
#include "paletteAbout.h"
#include "ParamOp.h"


#include "login.h"

#include <QtScript>
#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include <QMdiSubWindow>
#include <QGroupBox>
#include <QtPrintSupport>
#include <qstylefactory.h>
#include <QGraphicsView>
#include <QIODevice>
#include <QScrollBar>

#include <QSettings>

#define MAXRECENTFILES 10

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;
class QNetworkAccessManager;
class QNetworkReply;
class QToolBar;
class QListWidget;
class QStackWidget;

class DockLeftWidget;
class Setting3DP;

//class DoubleSlider : public QSlider {
//	Q_OBJECT
//
//public:
//	DoubleSlider(QWidget *parent = 0) : QSlider(parent) {
//		connect(this, SIGNAL(valueChanged(int)),
//			this, SLOT(notifyValueChanged(int)));
//	}
//
//signals:
//	void doubleValueChanged(double value);
//
//	public slots:
//	void notifyValueChanged(int value) {
//		double doubleValue = value / 10.0;
//		emit doubleValueChanged(doubleValue);
//	}
//	void doubleSetValue(double value)
//	{
//		setValue(int(value*10));
//	}
//};



class JobEstimateDialog : public QDialog
{
public:
	JobEstimateDialog() : QDialog(0, Qt::Drawer/*, Qt::FramelessWindowHint*/) // hint is required on Windows
	{
		setWindowTitle("Material Usage Estimation");
		//QPushButton *button = new QPushButton("Some Button", this);
		setFixedSize(400, 300); // size of the background image
		//setAttribute(Qt::WA_TranslucentBackground);		

		//setStyleSheet("color: rgba(255, 255, 255, 255);");

		init();



	}
	void init()
	{
		
		QLabel *dialb = new QLabel();
		dialb->setText("Do you want to run the pre-printing estimation now? \nIt is crucial to know that you have sufficient printing material before starting the actual print");
			
		dialb->setWordWrap(true);
		//dialb->setStyleSheet("color: rgba(255, 255, 255, 255);");
	
		//dialb->setTextFormat(Qt::RichText);
		//QGridLayout *layout = new QGridLayout;


		QPushButton *disableBtn = new QPushButton("Skip");
		disableBtn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

		QPushButton *acceptBtn = new QPushButton("Estimate Now");
		acceptBtn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


		QHBoxLayout *tempHLayout = new QHBoxLayout;
		tempHLayout->addWidget(disableBtn);
		tempHLayout->addStretch(0);
		tempHLayout->addWidget(acceptBtn);		
		QFrame *tempFrame = new QFrame;
		tempFrame->setLayout(tempHLayout);


		//QDialogButtonBox  *buttonBox = new QDialogButtonBox(Qt::Horizontal);
		//buttonBox->addButton(disableBtn, QDialogButtonBox::RejectRole);
		//buttonBox->addButton(acceptBtn, QDialogButtonBox::AcceptRole);
		////buttonBox->setStyle(QDialogButtonBox::MacLayout);

		//buttonBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		//layout->addWidget(dialb, 0, 0, 1, 2);
		//layout->setVerticalSpacing(3); 
		//layout->addWidget(tempFrame, 2, 0, 1, 3);
		//setLayout(layout);
		QVBoxLayout *layout = new QVBoxLayout;
		layout->addWidget(dialb);
		layout->addStretch(0);
		layout->addWidget(tempFrame);
		setLayout(layout);

		//connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		//connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		connect(disableBtn, SIGNAL(clicked()), this, SLOT(reject()));
		connect(acceptBtn, SIGNAL(clicked()), this, SLOT(accept()));

	}
protected:
	


	public slots:


};





class GDPRDialog : public QDialog
{
public:
	GDPRDialog() : QDialog(0, Qt::FramelessWindowHint) // hint is required on Windows
	{
		//QPushButton *button = new QPushButton("Some Button", this);
		setFixedSize(500, 400); // size of the background image
		setAttribute(Qt::WA_TranslucentBackground);
		setStyleSheet("QDialog{\n"
			"background-color: rgb(255, 0, 0, 50);\n"
			"background:transparent;\n"
			"}\n"
			);



		init();



	}
	void init()
	{
		/*QString gdprlb(
			"We are making updates to our and ensure that they remain transparent for you.\n"
			"These updates will take effect on May 25th, 2018. We heighly recommend that you should\n"
			"read the documents carefully.The key updates in our Privacy Policy include :\n"
			"More focus on the controls we offer you over your personal data;\n"
			"more clarity about houw we prevent your data from harm and comply with the applicable laws.\n"
			"If you have and questions about this update, please reach out us at \" \n"
			"");*/
		/*QString gdprlb(
			"We've made some changes\n"
			"In order to help improve our products and service we collect statistics\n "
			"about how you use our software and printer.For more information\n"
			"please read our revised Privacy Policy."
			); */
		/*QString gdprlb(
			"<h1>We've made some changes.</h1>"
			"<h1>In order to help improve our products and service we collect statistics</h1>"
			"<h1>about how you use our software and printer.For more information</h1>"
			"<h1>please read our revised <a href=\"https://tw.dictionary.yahoo.com/\">Privacy Policy</a>.</h1>
			);*/
		QLabel *dialb = new QLabel();
		dialb->setText("<h1>We've made some changes.</h1>"
			"<h3>In order to help improve our products and service we collect statistics</h3>"
			"<h3>about how you use our software and printer.For more information</h3>"
			"<h3>please read our revised <a href=\"https://www.xyzprinting.com/en-US/company/privacy-policy\">Privacy Policy</a>.</h3>");
		dialb->setStyleSheet("color: rgba(255, 255, 255, 255);");
		dialb->setOpenExternalLinks(true);
		dialb->setTextFormat(Qt::RichText);

		QGridLayout *layout = new QGridLayout;



		QPushButton *disableBtn = new QPushButton("Disable");
		disableBtn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

		QPushButton *acceptBtn = new QPushButton("Accept");
		acceptBtn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

		QDialogButtonBox  *buttonBox = new QDialogButtonBox;
		buttonBox->addButton(disableBtn, QDialogButtonBox::RejectRole);
		buttonBox->addButton(acceptBtn, QDialogButtonBox::AcceptRole);
		buttonBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		layout->addWidget(dialb, 0, 0, 1, 2);
		layout->addWidget(buttonBox, 2, 0, 1, 3);
		setLayout(layout);


		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	}
protected:
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);

		/*QTextDocument doc;
		doc.setHtml("<b>Title</b><p>We are making updates to our and ensure that they remain transparent for you.</p>"
		"<p style=\"color:white;\" >These updates will take effect on May 25th, 2018. We heighly recommend that you should< / p>"
		"<p>read the documents carefully.The key updates in our Privacy Policy include :</p>"
		"<p>More focus on the controls we offer you over your personal data;</p>"
		"<p>more clarity about houw we prevent your data from harm and comply with the applicable laws.</p>"
		"<p>If you have and questions about this update, please reach out us at</p>"
		);*/

		//doc.drawContents(&painter, rect());


		painter.drawImage(QRectF(0, 0, 500, 400), QImage(":/images/icons/transparent.png"));
	}

	/*bool getFooBar() const
	{
	bool test = buttonBox->

	return test;
	}*/


	public slots:


};

class CopyDialog : public QDialog
{
	Q_OBJECT
public:
	CopyDialog() : QDialog(0, Qt::WindowCloseButtonHint) // hint is required on Windows
	{
		setWindowTitle("Enter number: ");
		setFixedSize(150, 100); // size of the background image
		setModal(true);
		init();
		copyNum = 0;
	}
	QLineEdit *numLE;
	void init()
	{
		numLE = new QLineEdit();
		numLE->setValidator(new QIntValidator(0, 400, this));
		QGridLayout *layout = new QGridLayout;


		QPushButton *sendNowButton = new QPushButton("ok");
		sendNowButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		QPushButton *sendLaterButton = new QPushButton("cancel");
		sendLaterButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


		QDialogButtonBox  *buttonBox = new QDialogButtonBox;
		buttonBox->addButton(sendNowButton, QDialogButtonBox::AcceptRole);
		buttonBox->addButton(sendLaterButton, QDialogButtonBox::RejectRole);
		buttonBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		layout->addWidget(numLE, 0, 0, 1, 2);
		layout->addWidget(buttonBox, 2, 0, 1, 2);
		setLayout(layout);


		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		connect(numLE, SIGNAL(editingFinished()), this, SLOT(updatecopyNum()));
	}
protected:
	void paintEvent(QPaintEvent *event)
	{
	}
private:
	int copyNum;
	public slots:
	void updatecopyNum()
	{
		copyNum = numLE->text().toInt();
	}
public:
	int getcopyNum()
	{
		return copyNum;
	}

};

class PrintjobParam
{
public:
	PrintjobParam()
	{
		initParam();
	}
	void initParam()
	{
		printJobParam.addParam(new RichInt("print_dpi", 300, "dpi", ""));
		printJobParam.addParam(new RichFloat("slice_height", 0, "height_mm", ""));

		printJobParam.addParam(new RichBool("OL_Image", false, "save_Outline_Image", ""));//輪廓
		printJobParam.addParam(new RichBool("Cap_Image", false, "save_cap_Image", ""));	//膠水圖

		printJobParam.addParam(new RichBool("BLK_OUT_LINE", false, "BLK_OUT_LINE", ""));
		printJobParam.addParam(new RichBool("FaceColor", false, "FaceColor", ""));

		printJobParam.addParam(new RichFloat("useless_print", 0, "useless_print_des", ""));//犧牲條寬度
		printJobParam.addParam(new RichBool("generate_zx", false, "generate_zx_or_not", ""));//
		printJobParam.addParam(new RichBool("generate_final_picture", false, "generate_final_picture", ""));//
		printJobParam.addParam(new RichBool("mono_bool", false, "mono_bool", ""));//***20160322mono_bool


		printJobParam.addParam(new RichInt("start_page", 0, "start print pages"));


		printJobParam.addParam(new RichInt("end_page", 0, "end print pages"));


		printJobParam.addParam(new RichInt("per_zx_job", 10000, "number of zx pages", ""));//多少頁一個job
		printJobParam.addParam(new RichInt("send_page_num", 1, "send_page_num"));//幾頁send 一次	

		printJobParam.addParam(new RichBool("longpage_orshortpage", false, "longpage_orshortpage", ""));


		printJobParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));
		printJobParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));

		printJobParam.addParam(new RichBool("PRINT_START", false, "START PRINTING", ""));

		printJobParam.addParam(new RichBool("PLUS_SPITTON", true, "plus_Spitton", ""));
		printJobParam.addParam(new RichFloat("USELESS_PRINT", 0, "useless_print", ""));

		printJobParam.addParam(new RichBool("ADD_PATTERN", false, "useless_print", ""));
		printJobParam.addParam(new RichBool("Start_Printing_Estimate", false, "StartPrintEstimate", ""));

		printJobParam.addParam(new RichString("ZX_File_Path", "", "ZX_File_Path"));
		printJobParam.addParam(new RichString("COLOR_PROFILE_PATH", "", "COLOR_PROFILE_PATH"));

		printJobParam.addParam(new RichBool("Stiff_Print", false, "Stiff_Print", ""));//***20160322mono_bool

		printJobParam.addParam(new RichBool("dilate_Binder", false, "dilate_Binder", ""));//***20160322mono_bool

		printJobParam.addParam(new RichInt("dilate_Binder_Value", 1, "dilate_Binder_Value"));//	

		printJobParam.addParam(new RichBool("DYNAMIC_WIPE", false, "dynamic_Wipe", ""));//***20160322mono_bool

		printJobParam.addParam(new RichBool("COLOR_BINDING", false, "Color Binding", ""));//***20160322mono_bool

		printJobParam.addParam(new RichString("THUMBNAIL_FILE_PATH", "", "THUMBNAIL_FILE_PATH", ""));//***20160322mono_bool
		//printJobParam.addParam(new RichInt("SPITTON_MODE", 0, "Spitton mode", ""));//***

		//printJobParam.addParam(new RichFloat("WITNESS_WIDTH", 2, "Witness Width", ""));

	}
	void setCommonPrintValue(MeshDocument *md)
	{
		printJobParam.setValue("slice_height", FloatValue(md->p_setting.getbuildheight().value));

		int endPage = qCeil((md->bbox().max.Z() - md->bbox().min.Z()) / md->p_setting.getbuildheight().value) + 1;
		int startpage = md->p_setting.get_start_print_page();

		printJobParam.setValue("start_page", IntValue(md->p_setting.get_start_print_page()));
		printJobParam.setValue("end_page", IntValue(endPage));



		if (md->p_setting.print_pagesize == slice_program_setting::page_size::longPage_size)
		{
			int plus_pixel_val = md->p_setting.get_plus_print_length() * 300 / DSP_inchmm;
			float plus_cm_val = md->p_setting.get_plus_print_length();
			printJobParam.setValue("longpage_orshortpage", BoolValue(true));
			printJobParam.setValue("plus_pixel", IntValue(plus_pixel_val));//134*300/DSP_inchmm
			printJobParam.setValue("PLUS_MM", FloatValue(plus_cm_val));//134
		}
		else
		{
			printJobParam.setValue("longpage_orshortpage", BoolValue(false));
			printJobParam.setValue("plus_pixel", IntValue(0));//134*300/DSP_inchmm
			printJobParam.setValue("PLUS_MM", FloatValue(0));//134
		}

		printJobParam.setValue("USELESS_PRINT", FloatValue(md->p_setting.get_useless_print()));
		printJobParam.setValue("ADD_PATTERN", BoolValue(md->p_setting.get_add_pattern()));

		printJobParam.setValue("FaceColor", BoolValue(true));
	}
	void genDebugPic(bool t)
	{
		printJobParam.setValue("generate_final_picture", BoolValue(t));
	}
	void setGenZxParam(bool t)
	{
		printJobParam.setValue("generate_zx", BoolValue(t));//						
	}
	void setMono(bool t)
	{
		printJobParam.setValue("mono_bool", BoolValue(t));
	}
	void setPlusSpitton(bool t)
	{
		printJobParam.setValue("PLUS_SPITTON", BoolValue(t));
	}
	void setStiffPrint(bool t)
	{
		printJobParam.setValue("Stiff_Print", BoolValue(t));
	}
	void setDilateBinder(bool t)
	{
		printJobParam.setValue("dilate_Binder", BoolValue(t));
	}
	void setDialteBinderValue(int v)
	{
		printJobParam.setValue("dilate_Binder_Value", IntValue(v));
	}

	void setDynamicWipe(bool t)
	{
		printJobParam.setValue("DYNAMIC_WIPE", BoolValue(t));
	}

	void setColorBinding(bool t)
	{
		printJobParam.setValue("COLOR_BINDING", BoolValue(t));
	}
	void setUSELESS_PRINT(float t)
	{
		printJobParam.setValue("USELESS_PRINT", FloatValue(t));
	}


	void setPrintStart(bool t)
	{
		printJobParam.setValue("PRINT_START", BoolValue(t));
	}
	void setStartPrintEstimate(bool t)
	{
		printJobParam.setValue("Start_Printing_Estimate", BoolValue(t));
	}
	void setZXFilePath(QString path)
	{
		printJobParam.setValue("ZX_File_Path", StringValue(path));
	}
	void setColorProfile(QString path)
	{
		printJobParam.setValue("COLOR_PROFILE_PATH", StringValue(path));
	}
	/*void setSpittonMode(int Mode)
	{
		printJobParam.setValue("SPITTON_MODE", IntValue(Mode));
	}*/
	void setThumbnailPath(QString path)
	{
		printJobParam.setValue("THUMBNAIL_FILE_PATH", StringValue(path));
	}

	RichParameterSet& getprintJobParam(){ return printJobParam; }

	void setValueInParam(const QString name, const Value& val)
	{
		printJobParam.setValue(name, val);
	}
	void resetParam()
	{

	}
private:
	RichParameterSet printJobParam;

};


class CustomToolButton : public QToolButton
{
	Q_OBJECT
public:
	explicit CustomToolButton(QWidget *parent = 0)
	{
		setPopupMode(QToolButton::MenuButtonPopup);
		QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(setDefaultAction(QAction*)));
	}

};



class DoubleSlider : public QScrollBar {
	Q_OBJECT

public:
	DoubleSlider(QWidget *parent = 0) : QScrollBar(parent) {
		connect(this, SIGNAL(valueChanged(int)), this, SLOT(notifyValueChanged(int)));
		setStyleSheet(WidgetStyleSheet::scrollBarStyleSheet());
		setInvertedAppearance(true);
		setInvertedControls(false);
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		setMinimumSize(61, 200);
	}

signals:
	void doubleValueChanged(double value);

	public slots:
	void notifyValueChanged(int value) {
		double doubleValue = value / 10.;
		emit doubleValueChanged(doubleValue);
	}
	void doubleSetValue(double value)
	{
		setValue(int(value * 100));
	}
	void doubleAdd5Step()
	{
		setValue(value() + 5 * singleStep());
	}
	void doubleMinus5Step()
	{
		setValue(value() - 5 * singleStep());
	}
};




class MainWindowSetting
{
public:

	static void initGlobalParameterSet(RichParameterSet* gblset);
	void updateGlobalParameterSet(RichParameterSet& rps);

	bool permeshtoolbar;
	static QString perMeshRenderingToolBar() { return "ToolBar"; }
};

class MainWindow : public QMainWindow, public MainWindowInterface
{
	Q_OBJECT

public:
	// callback function to execute a filter
	void executeFilter(QAction *action, RichParameterSet &srcpar, bool isPreview = false);
	//parexpval is a string map containing the parameter expression values set in the filter's dialog. 
	//These parameter expression values will be evaluated when the filter will start.
	void executeFilter(MeshLabXMLFilterContainer*, const QMap<QString, QString>& parexpval, bool  isPreview = false);
	//***20150604*****	
	void  executeEdit(QAction *, RichParameterSet & parset, bool dummy = false);
	void  updateTransformMenu();

	MainWindow();
	void init();
	static bool QCallBack(const int pos, const char * str);
	static bool QCallBack2(const int pos, const char * str);
	static bool QCallBackdata(const int layer, const int);
	//const QString appName() const {return tr("MeshLab v")+appVer(); }
	//const QString appVer() const {return tr("1.3.2"); }
	MainWindowSetting mwsettings;
signals:
	void dispatchCustomSettings(RichParameterSet& rps);
	void filterExecuted();
	void updateLayerTable();
	void receiveJsonSignal();
	void receiveExeSignal();

	private slots:
	GLArea* newProject(const QString& projName = QString());
	void saveProject();
	void saveProject2();
	void saveProjectAs();
	//***20150527
	//GLArea* newProject_v2(const QString& projName = QString());

	public slots:
	bool importMeshWithLayerManagement(QString fileName = QString());
	bool importRaster(const QString& fileImg = QString());
	bool openProject(QString fileName = QString());
	bool openProject2(QString fileName = QString());
	bool appendProject(QString fileName = QString());
	void updateCustomSettings();
	void reload();

	void delCurrentMesh();
	void delMeshes();
	void delCurrentRaster();
	void updateRenderMode();
	//***20150813****
	//void setCopyMesh();
	//void pastSelectMesh();	
	void toCenter();
	//*******

	//***20160608****
	void setCopyMeshes();
	void pasteSelectMeshes();
	//*******

	public slots:
	bool importMesh(QString fileName = QString());
	void testslot();
	private slots:
	//bool importMesh(QString fileName = QString());
	void endEdit();
	void updateDocumentScriptBindings();
	void loadAndInsertXMLPlugin(const QString& xmlpath, const QString& scriptname);
	void postFilterExecution(/*MeshLabXMLFilterContainer* mfc*/);
	//void evaluateExpression(const Expression& exp,Value** res);
	void updateProgressBar(const int pos, const QString& text);
	bool printFunction();
	bool printFunction352();
	bool printFunctionMono();
	bool printFunction2();
	bool print_test_page_function();
	bool GenZXFunc();
	void printJobEstimate();
	bool importFromProject(QString fileName = QString());

public:
	bool exportMesh(QString fileName, MeshModel* mod, const bool saveAllPossibleAttributes);
	bool exportMesh2(QString fileName, MeshModel* mod, const bool saveAllPossibleAttributes);
	bool loadMesh(const QString& fileName, MeshIOInterface *pCurrentIOPlugin, MeshModel* mm, int& mask, RichParameterSet* prePar);
	bool loadMeshWithStandardParams(QString& fullPath, MeshModel* mm);

	public slots:
	void updateMenus();
	void updateLoginData();
	void showLayerDlg(bool visible);

	private slots:
	//////////// Slot Menu File //////////////////////

	void reloadAllMesh();
	void openRecentMesh();
	void openRecentProj();
	bool saveAs(QString fileName = QString(), const bool saveAllPossibleAttributes = false);
	bool save(const bool saveAllPossibleAttributes = false);
	bool saveSnapshot();
	void changeFileExtension(const QString&);
	///////////Slot Menu Edit ////////////////////////
	void applyEditMode();
	void suspendEditMode();
	///////////Slot Menu Filter ////////////////////////
	void startFilter();
	void applyLastFilter();
	void runFilterScript();
	void showFilterScript();
	void showXMLPluginEditorGui();
	void showTooltip(QAction*);
	/////////// Slot Menu Render /////////////////////
	//void renderBbox();
	//void renderPoint();
	/*    void renderWire();
		void renderFlat();
		void renderFlatLine();
		void renderHiddenLines();
		void renderSmooth();
		void renderTexture();
		void setLight();
		void setDoubleLighting();
		void setFancyLighting();*/

	//void setColorMode(QAction *qa);
	//   void setColorNoneMode();
	//void setPerMeshColorMode();
	//void setPerVertexColorMode();
	//void setPerFaceColorMode();

	void applyRenderMode();
	//void applyColorMode();
	/* void toggleBackFaceCulling();
	void toggleSelectFaceRendering();
	void toggleSelectVertRendering();*/
	void applyDecorateMode();

	void switchOffDecorator(QAction*);
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	//void showLayerDlg(bool visible);
	void showRaster();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
	void updateSubFiltersMenu(const bool createmenuenabled, const bool validmeshdoc);
	void updateMenuItems(QMenu* menu, const bool enabled);
	void updateStdDialog();
	void updateXMLStdDialog();
	void enableDocumentSensibleActionsContainer(const bool enable);

	//void updatePerViewApplicationStatus();
	void setSplit(QAction *qa);
	void setUnsplit();
	void linkViewers();
	void viewFrom(QAction *qa);//設置ortho view
	void lock_View(bool);
	void selection_View(bool);
	void readViewFromFile();
	void viewFromCurrentMeshShot();
	void viewFromCurrentRasterShot();
	void copyViewToClipBoard();
	void pasteViewFromClipboard();
	///////////Slot PopUp Menu Handles /////////////////////
	void splitFromHandle(QAction * qa);
	void unsplitFromHandle(QAction * qa);

	///////////Slot Menu Preferences /////////////////
	void setCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void supportdia();
	void contactSupport();
	void aboutPlugins();
	void helpOnline();
	void helpOnscreen();
	void submitBug();
	void checkForUpdates(bool verboseFlag = true);
	void checkForUpdates_2();

	///////////Slot General Purpose ////////////////////////

	void dropEvent(QDropEvent * event);
	void dragEnterEvent(QDragEnterEvent *);
	void connectionDone(QNetworkReply *reply);
	void connectionDone2(QNetworkReply *reply);
	void sendHistory();

	///////////Solt Wrapper for QMdiArea //////////////////
	void wrapSetActiveSubWindow(QWidget* window);
	void scriptCodeExecuted(const QScriptValue& val, const int time, const QString& output);
	//change render mode
	void changeCurrentMeshRenderMode();
private:

	int longestActionWidthInMenu(QMenu* m, const int longestwidth);
	int longestActionWidthInMenu(QMenu* m);
	int longestActionWidthInAllMenus();
	void createStdPluginWnd(); // this one is
	void createXMLStdPluginWnd();
	void initGlobalParameters();
	void createActions();
	void createMenus();
	void initSearchEngine();
	void initItemForSearching(QAction* act);
	void initMenuForSearching(QMenu* menu);
	void fillFilterMenu();
	void fillDecorateMenu();
	void fillRenderMenu();
	void fillEditMenu();
	void createToolBars();
	void loadMeshLabSettings();
	void loadPicassoSettings();
	void loadFromFileToPicassoSettings();
	// void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();
	void updateRecentProjActions();
	//void saveRecentFileList(const QString &fileName);
	void saveRecentProjectList(const QString &projName);
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);

	void initDocumentMeshRenderState(MeshLabXMLFilterContainer* mfc);
	void initDocumentRasterRenderState(MeshLabXMLFilterContainer* mfc);

	QNetworkAccessManager *httpReq;
	QBuffer myLocalBuf;
	int idHost;
	int idGet;
	bool VerboseCheckingFlag;

	MeshlabStdDialog *stddialog;
	MeshLabXMLStdDialog* xmldialog;
	static QProgressBar *qb;
	//***20160624
	static QPushButton *s_pushbutton;
	static QPushButton *pause_button;

	static QLabel *statusLabel;
	QPushButton *unitStatus;

	QMdiArea *mdiarea, *mdiarea_xy;
	//LayerDialog *layerDialog;
	PluginGeneratorGUI* plugingui;
	QSignalMapper *windowMapper;


	//QMap<QThread*,Env*> envtobedeleted;

	/*
	Note this part should be detached from MainWindow just like the loading plugin part.

	For each running instance of meshlab, for the global params we have default (hardwired)
	values and current(saved,modified) values.
	At the start up the initGlobalParameterSet function (of decorations and of drea and of ... )
	is called with the empty RichParameterSet defaultGlobalParams (to collect the default values)
	At the start up the currentGlobalParams is filled with the values saved in the registry.
	*/

	RichParameterSet currentGlobalParams;
	RichParameterSet defaultGlobalParams;

	QByteArray toolbarState;								//stato delle toolbar e dockwidgets

	//QDir lastUsedDirectory;  //This will hold the last directory that was used to load/save a file/project in


	vcg::GLW::TextureMode getBestTextureRenderModePerMesh(const int meshid);
	void setBestTextureModePerMesh(RenderModeAction* textact, const int meshid, RenderMode& rm);
public:
	QColor color;
	QDir lastUsedDirectory;
	void saveRecentFileList(const QString &fileName);
	LayerDialog *layerDialog;
	Setting3DP *setting3DP2;
	//PrintOption *printOption_;
	PluginManager PM;

	MeshDocument *meshDoc() {
		assert(currentViewContainer());
		return &currentViewContainer()->meshDoc;
	}

	const RichParameterSet& currentGlobalPars() const { return currentGlobalParams; }
	RichParameterSet& currentGlobalPars() { return currentGlobalParams; }
	const RichParameterSet& defaultGlobalPars() const { return defaultGlobalParams; }

	GLArea *GLA() const {
		//	  if(mdiarea->currentSubWindow()==0) return 0;
		MultiViewer_Container *mvc = currentViewContainer();
		if (!mvc) return 0;
		GLArea *glw = qobject_cast<GLArea*>(mvc->currentView());
		return glw;
	}

	MultiViewer_Container* currentViewContainer() const {
		MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow());
		if (mvc) return mvc;
		if (mvc == 0 && mdiarea->currentSubWindow() != 0){
			mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow()->widget());
			if (mvc) return mvc;
		}
		QList<QMdiSubWindow *> subwinList = mdiarea->subWindowList();
		foreach(QMdiSubWindow *subwinPtr, subwinList)
		{
			MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(subwinPtr);
			if (mvc) return mvc;
			if (mvc == 0 && subwinPtr != 0){
				mvc = qobject_cast<MultiViewer_Container *>(subwinPtr->widget());
				if (mvc) return mvc;
			}
		}

		return 0;
	}


	void setHandleMenu(QPoint p, Qt::Orientation o, QSplitter *origin);

	const PluginManager& pluginManager() const { return PM; }

	static QStatusBar *&globalStatusBar()
	{
		static QStatusBar *_qsb = 0;
		return _qsb;
	}
	QMenu* meshLayerMenu() { return filterMenuMeshLayer; }
	QMenu* rasterLayerMenu() { return filterMenuRasterLayer; }
	void connectRenderModeActionList(QList<RenderModeAction*>& actlist);

private:
	//the xml filters run in a different thread. The xmlfiltertimer starts on executeFilter and stops on postFilterExecution
	//function linked to the thread finished signal.
	QTime xmlfiltertimer;
	WordActionsMapAccessor wama;
	//////// ToolBars ////////ToolBars 在此///////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	QToolBar *editToolBar;
	QToolBar *filterToolBar;
	QToolBar *editToolBar_pi;
	QToolBar *searchToolBar;
	QToolBar *viewDToolBar;
	QToolBar *settingToolBar_pi;
	QToolBar *ioToolBar;

	///////// Menus ///////////////
	QMenu *fileMenu;
	QMenu *filterMenu;
	QMenu* recentProjMenu;
	QMenu* recentFileMenu;

	QMenu *filterMenuSelect;
	QMenu *filterMenuClean;
	QMenu *filterMenuCreate;
	QMenu *filterMenuRemeshing;
	QMenu *filterMenuPolygonal;
	QMenu *filterMenuColorize;
	QMenu *filterMenuSmoothing;
	QMenu *filterMenuQuality;
	QMenu *filterMenuMeshLayer;
	QMenu *filterMenuRasterLayer;
	QMenu *filterMenuNormal;
	QMenu *filterMenuRangeMap;
	QMenu *filterMenuPointSet;
	QMenu *filterMenuSampling;
	QMenu *filterMenuTexture;
	QMenu *filterMenuCamera;


	QMenu *editMenu;
	QMenu *justify;
	QMenu *flipMenu;
	QMenu *landMenu;

	//Render Menu and SubMenu ////
	QMenu *shadersMenu;
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *colorModeMenu;
	QMenu *tmt;

	//View Menu and SubMenu //////
	QMenu *viewMenu;
	QMenu *trackBallMenu;
	QMenu *logMenu;
	QMenu *toolBarMenu;
	QMenu *changeViewMenu;
	//////////////////////////////
	QMenu *windowsMenu;
	QMenu *preferencesMenu;
	QMenu *helpMenu;
	QMenu *splitModeMenu;
	QMenu *viewFromMenu;

	QMenu *programmerTest;
	//////////// Split/Unsplit Menu from handle///////////
	QMenu *handleMenu;
	QMenu *splitMenu;
	QMenu *unSplitMenu;
	////////// Search Shortcut ////////////////
	QShortcut* searchShortCut;
	MyToolButton* searchButton;
	SearchMenu* searchMenu;
	////////// Print Menu ////////////////////
	QMenu *printMenu;
	////////// QToolButton view Menu ////////////////////
	QMenu *CustomToolButtonViewMenu;
	CustomToolButton *viewCustomToolButton;

	//////////// Actions Menu File ///////////////////////
	QAction *newProjectAct;
	QAction *openProjectAct, *appendProjectAct, *saveProjectAct, *saveProjectAsAct;
	QAction *importFromProjectAct;
	QAction *importMeshAct, *exportMeshAct, *exportMeshAsAct, *exportMeshAsAct2;
	QAction *importRasterAct;
	QAction *closeProjectAct;
	QAction *reloadMeshAct;
	QAction *reloadAllMeshAct;
	QAction *saveSnapshotAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *recentProjActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *exitAct;
	//////Filter
	QAction *lastFilterAct;
	QAction *runFilterScriptAct;
	QAction *showFilterScriptAct;
	QAction * showFilterEditAct;
	//***20150323_add_generate_slice_picture***
	QAction *generate_slice_FilterAct;

	/////////// Actions Menu Edit  /////////////////////
	QAction *suspendEditModeAct;
	/////////// Actions Menu Render /////////////////////
	QActionGroup *renderModeGroupAct;
	RenderModeAction *renderBboxAct;
	RenderModeAction *renderModePointsAct;
	RenderModeAction *renderModeWireAct;
	RenderModeAction *renderModeTexWireAct;//20150306新增
	RenderModeAction *renderModeOutlineTesselation;//20150319新增outline_tesselation
	//RenderModeAction *renderModeHiddenLinesAct;
	RenderModeAction *renderModeFlatLinesAct;
	RenderModeAction *renderModeFlatAct;
	RenderModeAction *renderModeSmoothAct;
	//RenderModeAction *renderModeTextureWedgeAct;
	//RenderModeAction *renderModeTextureWedgeAct;
	RenderModeAction *setDoubleLightingAct;
	RenderModeAction *setFancyLightingAct;
	RenderModeAction *setLightAct;
	RenderModeAction *backFaceCullAct;
	RenderModeAction *setSelectFaceRenderingAct;
	RenderModeAction *setSelectVertRenderingAct;

	QActionGroup *colorModeGroupAct;
	QActionGroup *piColorModeGroupAct;
	RenderModeAction *colorModeNoneAct;
	RenderModeAction *colorModePerMeshAct;
	RenderModeAction *colorModePerVertexAct;
	RenderModeAction *colorModePerFaceAct;
	///////////Actions Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	QAction *showLayerDlgAct;
	QAction *showObjectListAct;
	QAction *showRasterAct;
	QAction *resetViewAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *changeColorAC;
	//Sydny 01-19-2017
	QAction *showFunctionMenuAct;
	////////// Login ///////////
	QAction *nickname;
	QAction *username;
	QAction *logoutAct;
	QAction *status_Icon;
	QAction *status;
	QMenu *status_Arrow;
	//FrameAction * frame;

	QActionGroup *draw_mode_groupAct;
	QAction *wireFrameModeAct;
	QAction *pointModeAct;
	///////////Actions Menu Windows /////////////////////
	QAction *preparePrintintAct;
	QAction *showPrintDashBoardAct;
	QAction *testPrintAct;
	QAction *testPrintAct2;
	QAction *importSample3DModel;
	QAction *testPrintBORAct;
	QAction *estimateUsageAct;
	//QAction *

	///////////Actions Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAllAct;
	QAction *setSplitHAct;
	QAction *setSplitVAct;
	QActionGroup *setSplitGroupAct;
	QAction *setUnsplitAct;
	///////////Actions Menu Windows -> Split/UnSplit from Handle ////////////////////////
	QActionGroup *splitGroupAct;
	QActionGroup *unsplitGroupAct;

	QAction *splitUpAct;
	QAction *splitDownAct;

	QAction *unsplitUpAct;
	QAction *unsplitDownAct;

	QAction *splitRightAct;
	QAction *splitLeftAct;

	QAction *unsplitRightAct;
	QAction *unsplitLeftAct;

	///////////Actions Menu Windows -> View From ////////////////////////
	QActionGroup *viewFromGroupAct;//one checkable action in the action group
	QActionGroup *viewFromGroupActTB;
	QAction *viewTopAct;
	QAction *viewBottomAct;
	QAction *viewBottomActTB;
	QAction *viewLeftAct;
	QAction *viewRightAct;
	QAction *viewSideActTB;
	QAction *viewFrontAct;
	QAction *viewFrontActTB;
	QAction *viewBackAct;
	QAction *viewOriginalAct;
	QAction *viewOriginalActTB;
	QAction *lockViewAct;
	QAction *selectionModeSwitchAct;
	QAction *viewFromMeshAct;
	QAction *viewFromRasterAct;
	QAction *viewFromFileAct;

	QAction *landAct;
	QAction *landAllAct;
	QAction *genPrePrintingAct;
	QAction *toCenterAct;
	QAction *autoPackingAct;
	QAction *autoPackingAct2;
	QAction *overLapAct;
	QCheckBox *view_2dCB;
	QCheckBox *view_outline;
	QLabel *crossUnitLabel;

	///////////Actions Menu Windows -> Link/Copy/Paste View ////////////////////////
public:
	RenderModeAction *renderModeTextureWedgeAct;
	QAction *linkViewersAct;
	void setOverlapping(bool value)
	{
		overLapAct->setChecked(value);
		if (value == 0)
			overLapAct->setText("Allow Overlapping");
		else
			overLapAct->setText("Allow Overlapping");
	}
private:
	QAction *copyShotToClipboardAct;
	QAction *pasteShotFromClipboardAct;

	///////////Actions Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Actions Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	QAction *submitBugAct;
	QAction *onlineHelpAct;
	QAction *onscreenHelpAct;
	QAction *checkUpdatesAct;

	QAction *howToUseThisSWAct;
	QAction *printHistoryAct;
	QAction *supportResourceAct;
	QAction *picassoSupportSiteAct;
	QAction *contactSupportAct;
	QAction *aboutPicassoAct;
	////////////////////////////////////////////////////
	static QString getDecoratedFileName(const QString& name);

	void updateRenderToolBar(RenderModeAction* act);
public://**2015_2_11//開始加Tab_Menu
	QTabWidget *testTab;
	QWidget *testTab1;
	QWidget *testTab2;
public://***20150526***修改tabUI
	QTabWidget *pTabWidget;

	void buildwholeTabwidget();
	QWidget *createHomeTabWidget();
	QWidget *createViewTabWidget();
	QWidget *createTransformTabWidget();
	QWidget *createSettingTabWidget();
	QWidget *createToolsTabWidget();
	//QWidget *create
	QHBoxLayout *test_transform_layout();

	QWidget *homeTabWidget;
	QWidget *viewTabWidget;
	QWidget *transformTabWidget;
	QWidget *settingTabWidget;
	QWidget *toolsTabWidget;

public:
	void setGenericStyleSheet();
public://***20150527***新增函式
	void createMenus_v2();
	void createMenus_v3();

private:
	//***file Tab
	QToolBar *buildFileToolBar;
	QToolBar *partFileToolBar;
	QToolBar *buildToolBar;
	QToolBar *helpToolBar;
	//***view Tab
	QToolBar *viewToolBar;
	QToolBar *displayToolBar;
	QToolBar *layerToolBar;
	QToolBar *zoomToolBar;
	//***Transform Tab
	QToolBar *clipToolBar;
	QToolBar *editToolBar_v2;
	QToolBar *TranslateToolBar;
	QToolBar *RotateToolBar;
	QToolBar *ScaleToolBar;
	//***Tools Tab
	QToolBar *ToolToolBar;
	//***Setting Tab
	QToolBar *settingToolBar;
private:
	void createToolBars_v2();
private:
	//***home
	QToolButton *testbutton;
	QToolButton *newProjectToolButton;
	QToolButton *openProjectToolButton;
	QToolButton *saveProjectToolButton;
	QToolButton *importMeshToolButton;
	QToolButton *exportMeshToolButton;
	QToolButton *exportMeshAsToolButton;
	QToolButton *ImportRecentMeshToolButton;
	QToolButton *printImageFileButton;
	QToolButton *viewDirToolBarToolButton;

	QToolButton *helpToolButton;
	//***view
	QToolButton *cutToolButton;
	QToolButton *copyToolButton;
	QToolButton *pasteToolButton;
	QToolButton *topToolButton;
	QToolButton *bottomToolButton;
	QToolButton *leftToolButton;
	QToolButton *rightToolButton;
	QToolButton *frontToolButton;
	QToolButton *backToolButton;
	QToolButton *lockViewButton;
	QToolButton *topTagViewButton;

	QToolButton *ToolButton;


	QToolButton *isometricToolButton;
	QToolButton *projectToolButton;
	QToolButton *fastrenderToolButton;
	QToolButton *twoDToolButton;
	//***Tool??
	QToolButton *autoPackingToolButton;
	QToolButton *showLayerToolButton;
	QToolButton *butterflyToolButton;
	QToolButton *startbuildToolButton;
	QToolButton *autoLandingToolButton;
	//QToolButton *

	//***edit
	QToolButton *undoToolButton;
	QToolButton *redoToolButton;
	//***testrendermenubutton
	QToolButton *testRenderButton;

	//***setting
	QToolButton *printjobestimateButton;
	QToolButton *groove_settingBtn;
	QToolButton *sirius_settingBtn;

	void createQToolButton();
public:
	//***20150601
	QVector<GenericParamGroupbox *> genGroupbox;
	GenericParamGroupbox *genTranGroupbox;
	GenericParamGroupbox *genRotateGroupbox;
	GenericParamGroupbox *genScaleGroupbox;
	CustomizeGroupBox *ptranslate;
	CustomizeGroupBox *protate;
	CustomizeGroupBox *pscale;
	void createTransformGroupBox();

	bool meshCheckSwitch;
	Point3m ScaleN;
	bool redoOccured;
	int gdprResult;

private:
	bool firstImport;
	Matrix44m transMatrix;

	void connectEditAction();
	//***20150803設置建構槽大小dialog
	QAction *groove_settingAct;
	QAction *sirius_settingAct;
	GenericParamDialog *genStdialog;

	///***20150813 增加
	QAction *topTagAction;
	QAction *genZXAct;
	QAction *threeDPrintAct;
	QAction *threeDPrintAct2;
	QAction *printjobestimateAct;
	QAction *changeRendertypeAct[3];
	//bool meshCheckSwitch;//檢查mesh的開關
	int vtNumberLimit;
	int fcNumberLimit;
	bool md_num_limit;
	bool pointCloudSwitch;

	//QAction *zoomInAct;
	//QAction *zoomOutAct;

public:
	bool fastRenderSwitch;
	void delOl(QString a, QString b);
	void delOl(MeshModel::meshsort);
	void delSelectedOl(MeshModel::meshsort);
	int getFCNumberLimit();

	//***20151013***自動填滿filenameList
	//---------------------QDir-----file_name-stringlist--取幾張,=使用meshDoc()的bbox().DimZ()
	bool fill_fileNameList(QString, QString, QStringList &, int x = 0);

	void setMdNumLimit(bool smdl){
		md_num_limit = smdl;
	}
	bool getMdNumLimit(){ return md_num_limit; }
	void setpointCloudSwitch(bool smdl){ pointCloudSwitch = smdl; }
	bool getpointCloudSwitch()
	{
		return pointCloudSwitch;
	}
	void setRadarModeSwitch(bool srm){ radarSwitchAct->setChecked(srm); }
	bool getRadarModeSwitch()
	{
		return radarSwitchAct->isChecked();
	}


	void setFcNumberLimit(int tl){
		fcNumberLimit = tl;
	}
	bool alreadyload;

	public slots:
	void setTopTagView(bool);
	void openDashboard(bool);

	//***20150901***printer
private:
	QImage *q2;
	QStringList fileNameList;
	QStringList fileNameList2;
	bool testReadZX(QString);
	bool testReadZip(QString);
	//bool alreadyload;
	private slots:
	void printPainterFunction(QPrinter *);
	//***20150903
	void executeSirius();
	QString getRoamingDir();
	public slots:
	void startsetting();
	void startsetting2();
	//***************************
	void emitconnect();
	//***20160303
	//void monitor_isfinished(int);
public:
	void createSliceBar();
	//**20160308
	QWidget *testdialog;
	void testslicefunc(MeshDocument &, float);
	TransformPreview *TPreview;
	static FILE *dbgff;
	//WidgetMonitor widget_p_monitor;
	void wrapperexecutefilter(RichParameterSet);
	//***20160517_two_window_in_different_thread
	//gui_launcher<mywidget> gl;
	//OtherThread otherT1;
	void sendZXfile(QString);
	void sendBORfile(QString);
	QMenu *rightmouse_contextMenu;
protected:
	void contextMenuEvent(QContextMenuEvent *event);
private:
	QAction *copyAct;
	QAction *pasteAct;
	QAction *deleteAct;
	QAction *moveToCenterAct;

	QAction *tempPicassoJRPrintingAct;
	QAction *testDMSlicerAct;
	QAction *testFuncAct;
	QAction *testFuncAct2;
	QAction *detectOverlapping;
	QAction *testFunc_clearErrorAct;
	QAction *testLoad3mfAct3;
	QAction *testFuncReadZX;
	QAction *selectAllAct;
	QAction *selectNoneAct;

	//SYDNY 07/11/2017
	QAction *copyAct_right;
	QAction *pasteAct_right;
	QAction *pasteManyAct_right;
	QAction *deleteAct_right;
	QAction *newProjectAct_right;
	QAction *landAct_right;

	QAction *justifyFront;
	QAction *justifyBack;
	QAction *justifyRight;
	QAction *justifyLeft;
	QAction *justifyTop;
	QAction *justifyBottom;

	QAction *flipByXYPlane;
	QAction *flipByYZPlane;
	QAction *flipByXZPlane;

	QAction *pointCloudSwitchAct;
	QAction *radarSwitchAct;
	QAction *chip_Gap_Mark_gird_Switch_Act;

	//QAction *resetViewAct;

	//QAction *
	public slots:
	void move_to_center_ground();
	void executeSliceFunc(double);
	void testFuncFunc();
	void genByDMSlicer();
	void copyNumber();
	void detectOverlappingFunc();
	void testFuncFunc2();
	void testClearSiuiusError();
	bool testload3mf(QString fileName = QString());
	bool testReadPrintingFile();

	void selectAllSlot();
	void selectNoneSlot();
	void transferFilterNameSlot();
	void genPrePrintingSlot();
	//Sydny 01-19-2017
	void showFunctionMenu();
	void logout();

	void tempPicassoJRPrinting();
	void view_outlineSlot();
public:
	//***20160517***test_qgraphicscene
	QGraphicsScene scene;
	MyView *view;
	//view.show

	bool isReset;

	void createQDockWidget2();
	void createQDockWidget3();

	QDockWidget *createQDockWidget4();
	QTabWidget *createDockTabWidget();
	QWidget *createDockViewTabWidget();
	QWidget *createDockTranslateTabWidget();
	QWidget *createDockRotateTabWidget();
	QWidget *createDockScaleTabWidget();
	//QWidget *createDockCrossSection();
	QWidget *createDockCrossSection2();
	QWidget *createDockInformationWidget();
	DockInfoWIdget *createDockInformationWidget2();
	void createWhereIsSlicePosition();

	QDockWidget *dock;
	QTabWidget *dockTabWidget;
	QWidget *dockTabViewWidget;
	QWidget *dockTabTranslateWidget;
	QWidget *dockTabRotateWidget;
	QWidget *dockTabScaleWidget;
	QWidget *dockCrossSectionWidget;
	QWidget *dockInformationWidget;
	DockInfoWIdget *dockInformationWidget2;
	//QWidget *deleteTabWidget;

	QLabel *originalNameLabel;
	QLabel *buildHeightLabel;
	QLabel *dimensionLabel;
	QLabel *numOfverticesLabel;
	QLabel *numOfFaceLabel;
	QLabel *originalName;
	QLabel *buildHeight;
	QLabel *dimension;
	QLabel *numOfvertices;
	QLabel *numOfFace;
	QLabel *layerLabel;

	QDoubleSpinBox *sliceSpinBox;
	DoubleSlider *crossSlider;
	QToolButton *up5;
	QToolButton *down5;
	float meshDocZ;

	inline QUndoStack *setUndoStack() { return currentViewContainer()->undoStack; }
	int undoLimit;


private:
	QUndoGroup *undoGroup;
	QUndoView *undoView;
	QMap<int, QString> resetNameList;
	QMap<int, Point3f> resetScaleN;
	QList<int> resetListID;
	QString internationlization();

signals:
	void slicePositionChanged(double);
	void updateMeshChanged();

	public slots:

	void matrix_changed(MeshModel *sm, const Matrix44m &tt_matrix);
	void matrix_changed_2(QSet<int> multiSelectID, const Matrix44m &execute_matrix);
	void set_cancel_filter();
	void set_pause_filter();
	//void setlayerLabel(double);
	void setSliceSpinBox(double);
	void setSliderPosition(double);
	void changeTabIcon(int);
	void dockTabClicked(int);


	//void scale_changed(MeshModel *sm, const Point3f &tt_matrix);
	//void rotate_changed(MeshModel *rm, const Matrix44m &r_matrix);
	void scale_changed(QSet<int> multiSelectID, const Point3f &scale_rate);
	void rotate_changed(QSet<int> multiSelectID, Point3d _rotation, QList<Point3f> _old_center);
	void packing_occur(QSet<int> multiSelectID,QMap<int, Point3f> savePos);
	void packing_rotate_occur(QSet<int> multiSelectID,QMap<int, Point3f> savePos, QMap<int, Point3d> firstRotation, QMap<int, Point3d> secondRotation);

	void zoomInSlot();
	void zoomOutSlot();
	void showObjectList(bool);
	void downloadStatus(qint64, qint64);
	void changeMeshColor();

public:
	void createUndoView();
	QAction *unDoAct;
	QAction *reDoAct;
	static bool testGetstring();//***20160701
	static QMap<int, int> dynamicWipe;
	static QMap<int, int> finalDynamicPage;
	//static QVector<int> dynamicSliceLayer;
	int countJobWipe(int);

	TransformWidget *transformwidgett1;
	RotateWidget_Customized *rotate_widget_cus;
	ScaleWidget_Customized *scale_widget_cus;
	QString operateString(QString, int);
	void updateMenuForCommand();
	ObjectList *objList;

	/*QThread *filterThread;
	QGLWidget* filterWidget;*/
private:
	static bool cancel_filter;
	static bool pause_filter;
	static bool filter_process_running;

	//static QMessageBox 
	//protected:
	//	void mousePressEvent(QMouseEvent *event);
private:
	DockLeftWidget *fw1;
	QListWidget *listWidget1;
	QStackedWidget *stackWidget1;

	bool updateOrNot(QString json);
	QString saveFileName(const QUrl &);
	bool saveToDisk(const QString &, QIODevice *);
	QList<QNetworkReply *> currentDownloads;
	void startSendURL(QUrl);

protected:
	void closeEvent(QCloseEvent *event);

private:
	float sliceH;
	QMap<int, int> sliceId;
	//float cmyusage[18];
	QVector<float> cmyusage;
	int vboTest;
	bool logInFUNC;

	QSet<int> rollerPage;
	void recordRollerPage();
	QStringList rollerPageToServer;
	bool filterResult;




public:
	QProcess *DBProcess;
	//float *getCmyusage();
	void getCmyusage();
	QMainWindow *w3ptr;

public:
	std::vector<GLfloat> vertices_test;
	std::vector<GLfloat> normal_test;
	float vbodataptr[108];
	float vbonormalptr[108];


	//SYDNY 
public:
	float old_Total;
	float new_Total = -1;
	float old_Total2;
	float new_Total2 = -1;
};







//***20150522 測試tabWidget*****
class FileTab :public QWidget
{
	Q_OBJECT
public:
	explicit FileTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};
class TransformTab : public QWidget
{
	Q_OBJECT
public:
	explicit TransformTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};
class viewTab : public QWidget
{
	Q_OBJECT
public:
	explicit viewTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};
class ToolTab : public QWidget
{
	Q_OBJECT
public:
	explicit ToolTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};
class settingTab : public QWidget
{
	Q_OBJECT
public:
	explicit settingTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};
//***********
///TestFloatWidget
class DockLeftWidget :public QWidget
{
	Q_OBJECT
public:
	DockLeftWidget(QWidget *parent = 0);
	~DockLeftWidget();
private:
	QToolButton *bt1;
	QToolButton *bt2;
	QToolButton *bt3;
	QAction *btac1;
	QAction *btac2;
	QAction *btac3;
	QActionGroup *acGroup;
	void createAction();
public:
	QVector<GenericParamGroupbox *> genGroupbox;
	MainWindow *mw();
	public slots:
	void bt1show(bool);
	void showGroupbox(QAction *);
protected:
	void mousePressEvent(QMouseEvent *event);


};

/// Event filter that is installed to intercept the open events sent directly by the Operative System
class FileOpenEater : public QObject
{
	Q_OBJECT

public:
	FileOpenEater(MainWindow *_mainWindow)
	{
		mainWindow = _mainWindow;
		noEvent = true;
	}

	MainWindow *mainWindow;
	bool noEvent;

protected:

	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (event->type() == QEvent::FileOpen) {
			qDebug() << "eventEater";
			noEvent = false;
			QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
			if (QFileInfo(fileEvent->file()).suffix() == ".3mf")
			{		
				mainWindow->openProject2(fileEvent->file());
			}
			else
			{
				mainWindow->importMeshWithLayerManagement(fileEvent->file());
				qDebug("event fileopen %s", qPrintable(fileEvent->file()));
			}
			return true;
		}
		else {
			// standard event processing
			return QObject::eventFilter(obj, event);
		}
	}
};


class MainWindow2 : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow2(MainWindow *_wptr, QWidget *parent = 0) :wptr(_wptr), QMainWindow(parent)
	{
		//        QString s(
		//                   "background:url(:images/btn_triangle_down.png);"
		//                   "background-repeat: no-repeat;"
		//                   );
		resize(QSize(100, 100));
		setAttribute(Qt::WA_TranslucentBackground);
		//QPixmap bkgnd(":images/icons/ic_check_power_on_2x.png");
		/*QPixmap bkgnd(":images/icons/bcp_icon.png");
		bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
		QPalette palette;
		palette.setBrush(QPalette::Background, bkgnd);
		this->setPalette(palette);*/

		setWindowFlags(Qt::FramelessWindowHint);


		setWindowOpacity(0.0);
		a1 = new QPropertyAnimation(this, "windowOpacity"); // As top level window, uses this one.
		a1->setDuration(3000);
		a1->setStartValue(0.0);
		a1->setEndValue(1.0);
		a1->setEasingCurve(QEasingCurve::Linear);
		a1->start(QPropertyAnimation::DeleteWhenStopped);

		connect(a1, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
			this, SLOT(on_animation_stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)));
		logINFunc = false;
	}
private:
	MainWindow *wptr;
	QPropertyAnimation *a1;
	bool logINFunc;
	private slots:
	void on_animation_stateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
	{
		/**/
		if (newState != QAbstractAnimation::Running && false)
		{
			if (wptr->gdprResult == QDialog::Accepted)
			{
				if (a1->direction() == QAbstractAnimation::Forward)
				{
					//a1->setDirection(QAbstractAnimation::Backward);
					//a1->start();

					//SYDNY
					QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);

					if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
					{
						QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
						xyzReg.remove("xyzsettings");
					}

					wptr->show();
					this->close();


					//SYDNY 07/27/2017 

					//QString sURL = "http://service.xyzprinting.com/XYZPrinting/Members/REQUESTLOGIN/JSON";

					//// create custom temporary event loop on stack
					//QEventLoop eventLoop;

					//// "quit()" the event-loop, when the network request "finished()"
					//QNetworkAccessManager mgr;
					//QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

					//// the HTTP request
					//QNetworkRequest geturl;
					//geturl.setUrl(QUrl(sURL));
					////geturl.setRawHeader("EMAIL", username.toAscii());
					//geturl.setRawHeader("key", "033bd94b1168d7e4f0d644c3c95e35bf");
					//QNetworkReply *reply = mgr.get(geturl);
					//eventLoop.exec();
					//QString rep = reply->readAll();
					//bool result = reply->bytesAvailable();

					QNetworkAccessManager nam;
					QNetworkRequest req(QUrl("http://www.google.com"));
					QNetworkReply *reply = nam.get(req);
					QEventLoop loop;
					connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();
					QString rep = reply->readAll();


					if (rep == NULL)
					{
						if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
						{
							while (rep == NULL)
							{//while loop
								QMessageBox::StandardButton ret = QMessageBox::question(
									this, tr("No Internet Connection"), tr("Skip Login?\nYes, Skip Login.\nNo, Need Internet."),
									QMessageBox::Yes | QMessageBox::No,
									QMessageBox::No);

								if (ret == QMessageBox::Yes)
								{
									wptr->updateLoginData();
									this->close();
									break;
								}
								if (ret == QMessageBox::No)
								{
									QString sURL = "http://service.xyzprinting.com/XYZPrinting/Members/REQUESTLOGIN/JSON";

									// create custom temporary event loop on stack
									QEventLoop eventLoop;

									// "quit()" the event-loop, when the network request "finished()"
									QNetworkAccessManager mgr;
									QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

									// the HTTP request
									QNetworkRequest geturl;
									geturl.setUrl(QUrl(sURL));
									//geturl.setRawHeader("EMAIL", username.toAscii());
									geturl.setRawHeader("key", "033bd94b1168d7e4f0d644c3c95e35bf");
									QNetworkReply *reply = mgr.get(geturl);
									eventLoop.exec();
									rep = reply->readAll();

									QNetworkAccessManager nam;
									QNetworkRequest req(QUrl("http://www.google.com"));
									QNetworkReply *replyg = nam.get(req);
									QEventLoop loop;
									connect(replyg, SIGNAL(finished()), &loop, SLOT(quit()));
									loop.exec();
									QString repgoogle = replyg->readAll();


									if (repgoogle != NULL)
									{
										//SYDNY 
										if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
										{
											login logindialog(this);
											logindialog.setWindowFlags(Qt::Tool);
											int r = logindialog.exec();

											if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
											{
												if (r == 0)
												{
													wptr->close();
													this->close();
												}
											}
											else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
											{
												if (r == 0)
												{
													wptr->updateLoginData();
												}
											}
										}
										else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
										{
											wptr->updateLoginData();
										}
									}
									else
									{
										repgoogle = reply->readAll();
									}
								}
							}//while loop

						}
						else
						{
							wptr->updateLoginData();
						}
					}

					else
					{
						//SYDNY 
						if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
						{
							login logindialog(this);
							logindialog.setWindowFlags(Qt::Tool);
							int r = logindialog.exec();

							if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
							{
								if (r == 0)
								{
									wptr->close();
									this->close();
								}
							}
							else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
							{
								if (r == 0)
								{
									wptr->updateLoginData();
									this->close();
								}
							}
						}
						else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
						{
							wptr->updateLoginData();
							this->close();
						}
					}
				}

			}
			else
			{
				wptr->show();
				wptr->updateLoginData();
				this->close();
			}

			//wptr->show();
			//this->~MainWindow2();
		}
		else{
			wptr->show();			
			this->close();
		}

	}
public:

	void closeEvent(QCloseEvent *event)
	{

	}
	~MainWindow2()
	{

	}
protected:
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);

		painter.drawImage(QRectF(0, 0, 100, 100), QImage(":/images/icons/bcp_icon.png"));
	}



};





class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0)
	{
		setWindowTitle(tr("progress"));
		layout = new QHBoxLayout(this);
		dlgBtn = new QPushButton(tr("open"));
		layout->addWidget(dlgBtn);

		connect(dlgBtn, SIGNAL(clicked()), this, SLOT(openDlg()));
	}
	private slots:
	void openDlg()
	{
		QProgressDialog dlg(tr("copy"), tr("cancel"), 0, 50000, this);
		dlg.setWindowTitle(tr("progressbar"));
		dlg.setWindowModality(Qt::WindowModal); //
		dlg.show();
		for (int i = 0; i < 50000; i++){
			dlg.setValue(i + 1);
			QCoreApplication::processEvents(); //
			if (dlg.wasCanceled()){
				break;
			}
		}
	}
private:
	QPushButton *dlgBtn;
	QHBoxLayout *layout;
};


class OpWidget : public QDialog
{
	Q_OBJECT
public:
	OpWidget(QWidget *parent = 0) :QDialog(0, Qt::FramelessWindowHint)
	{
		//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
		setFixedSize(QSize(500, 500));

		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);

		//setAttribute(Qt::WA_PaintOnScreen);
		//setAttribute(Qt::WA_TransparentForMouseEvents);
		/*setStyleSheet("QDialog{\n"
			"background-color: rgb(255, 0, 0, 50);\n"
			"background:transparent;\n"
			"}\n"
			);*/
	}
	void paintEvent(QPaintEvent*)
	{
		QPainter ppp(this);
		ppp.drawImage(QRectF(0, 0, 500, 500), QImage(":/images/icons/btn_move_on_2x.png"));
		//ppp.setOpacity(0.5);
		//QString text = "Some foo goes here";
		//QFontMetrics metrics(ppp.font());
		//resize(metrics.size(0, text));
		//QBrush bbrr;
		////bb.setStyle(Qt::Dense6Pattern);
		//
		//bbrr.setColor(Qt::darkBlue);
		//ppp.setBrush(bbrr);
		//ppp.drawText(rect(), Qt::AlignCenter, text);
		//ppp.drawRect(0, 0, 200, 200);
		//ppp.setBackground(Qt::darkBlue);

	}
};


//class HeatDialog : public QDialog
//{
//	Q_OBJECT
//public:
//	CopyDialog() : QDialog(0, Qt::WindowCloseButtonHint) // hint is required on Windows
//	{
//		setWindowTitle("Enter number: ");
//		setFixedSize(150, 100); // size of the background image
//		setModal(true);
//		init();
//		copyNum = 0;
//	}
//	QLineEdit *numLE;
//	void init()
//	{
//		numLE = new QLineEdit();
//		numLE->setValidator(new QIntValidator(0, 400, this));
//		QGridLayout *layout = new QGridLayout;
//
//
//		QPushButton *sendNowButton = new QPushButton("ok");
//		sendNowButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
//		QPushButton *sendLaterButton = new QPushButton("cancel");
//		sendLaterButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
//
//
//		QDialogButtonBox  *buttonBox = new QDialogButtonBox;
//		buttonBox->addButton(sendNowButton, QDialogButtonBox::AcceptRole);
//		buttonBox->addButton(sendLaterButton, QDialogButtonBox::RejectRole);
//		buttonBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
//		layout->addWidget(numLE, 0, 0, 1, 2);
//		layout->addWidget(buttonBox, 2, 0, 1, 2);
//		setLayout(layout);
//
//
//		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
//		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
//		connect(numLE, SIGNAL(editingFinished()), this, SLOT(updatecopyNum()));
//	}
//protected:
//	void paintEvent(QPaintEvent *event)
//	{
//	}
//private:
//	int copyNum;
//	public slots:
//	void updatecopyNum()
//	{
//		copyNum = numLE->text().toInt();
//	}
//public:
//	int getcopyNum()
//	{
//		return copyNum;
//	}
//
//};



//class MainWindow3 : public QMainWindow
//{
//	Q_OBJECT
//public:
//	explicit MainWindow3(MainWindow *_wptr, QWidget *parent = 0) :wptr(_wptr), QMainWindow(parent)
//	{
//		//        QString s(
//		//                   "background:url(:images/btn_triangle_down.png);"
//		//                   "background-repeat: no-repeat;"
//		//                   );
//		resize(QSize(100, 100));
//
//		QPixmap bkgnd(":images/icons/ic_check_power_on_2x.png");
//		bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
//		QPalette palette;
//		palette.setBrush(QPalette::Background, bkgnd);
//		this->setPalette(palette);
//
//
//		//setWindowFlags(Qt::FramelessWindowHint);
//
//
//		setWindowOpacity(0.0);
//		a1 = new QPropertyAnimation(this, "windowOpacity"); // As top level window, uses this one.
//		a1->setDuration(4000);
//		a1->setStartValue(0.0);
//		a1->setEndValue(1.0);
//		a1->setEasingCurve(QEasingCurve::Linear);
//		a1->start(QPropertyAnimation::DeleteWhenStopped);
//
//		connect(a1, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
//			this, SLOT(on_animation_stateChanged(QAbstractAnimation::State, QAbstractAnimation::State))
//			);
//
//		bt1 = new QPushButton("process");
//		connect(bt1, SIGNAL(clicked()), this, SLOT(bt1clicked()));
//		this->setCentralWidget(bt1);
//
//
//
//
//	}
//signals:
//	void stopfilter();
//private:
//	MainWindow *wptr;
//	QPushButton *bt1;
//	QPropertyAnimation *a1;
//private slots:
//	void bt1clicked()
//	{
//		/*wptr->set_cancel_filter();
//		wptr->QCallBack(0, "");*/
//	}
//	void on_animation_stateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
//	{
//		if (newState != QAbstractAnimation::Running)
//		{
//
//			if (a1->direction() == QAbstractAnimation::Forward) {
//				//a1->setDirection(QAbstractAnimation::Backward);
//				//a1->start();
//
//				wptr->show();
//				this->close();
//			}
//			//wptr->show();
//			//this->~MainWindow2();
//		}
//
//	}
//public:
//
//	void closeEvent(QCloseEvent *event)
//	{
//
//	}
//	~MainWindow3()
//	{
//
//	}
//};



#endif
#endif
