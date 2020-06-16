/****************************************************************************
* BCPware
*
*  Copyright 2018 by
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
#ifndef WIDGET_STYLE_SHEET_H
#define WIDGET_STYLE_SHEET_H
#pragma once

#include <QString>

class WidgetStyleSheet
{
public:
	WidgetStyleSheet() {};
	~WidgetStyleSheet() {};
public:
	static QString WidgetStyleSheet::viewToolButtonStyleSheet(const QString & name = NULL, const QString & name2 = NULL)
	{
		const QString thisItemStyle
		(
			"QToolButton:enabled{\n"

			"   border: none;\n"
			"   background: url(" + name + ") top center no-repeat;\n"
			//"   padding-top: 200px;\n"
			"   width: 130px;\n"
			"   height :36px;\n"
			"   font: 9pt \"Arial\";\n"

			"   color: white;\n"
			"}\n"

			"QToolButton:disabled{"
			//"background-color:#dddddd;"
			"background: url(" + name2 + ") top center no-repeat; \n"
			"   border: none;\n"
			"   width: 130px;\n"
			"   height :36px;\n"
			"}"


			/*"QToolButton {\n"
			"   border: none;\n"
			"   background: url(" + name + "_normal.png) top center no-repeat;\n"
			"   padding-top: 200px;\n"
			"   width: 200px;\n"
			"   font: bold 14px;\n"
			"   color: red;\n"
			"}\n"
			"QToolButton:hover {\n"
			"   background: url(" + name + "_hover.png) top center no-repeat;\n"
			"   color: blue;\n"
			"}\n"
			"QToolButton:pressed {\n"
			"   background: url(" + name + "_pressed.png) top center no-repeat;\n"
			"   color: gray;\n}"*/
		);

		return thisItemStyle;
	}

	static QString WidgetStyleSheet::viewDockWidgetBottomTB(const QString & name = NULL, const QString & name2 = NULL)
	{
		const QString thisItemStyle
		(
			"QToolButton:enabled {\n"

			"   border: none;\n"
			"   background: url(" + name + ") top center no-repeat;\n"
			//"   padding-top: 200px;\n"
			"   width: 152px;\n"
			"   height :36px;\n"
			"   font: 9pt \"Arial\";\n"

			"   color: white;\n"
			"}\n"

			"QToolButton:disabled {\n"

			"   border: none;\n"
			"   background: url(" + name2 + ") top center no-repeat;\n"
			//"   padding-top: 200px;\n"
			"   width: 152px;\n"
			"   height :36px;\n"
			"   font: 9pt \"Arial\";\n"

			"   color: white;\n"
			"}\n"


			/*"QToolButton {\n"
			"   border: none;\n"
			"   background: url(" + name + "_normal.png) top center no-repeat;\n"
			"   padding-top: 200px;\n"
			"   width: 200px;\n"
			"   font: bold 14px;\n"
			"   color: red;\n"
			"}\n"
			"QToolButton:hover {\n"
			"   background: url(" + name + "_hover.png) top center no-repeat;\n"
			"   color: blue;\n"
			"}\n"
			"QToolButton:pressed {\n"
			"   background: url(" + name + "_pressed.png) top center no-repeat;\n"
			"   color: gray;\n}"*/
		);

		return thisItemStyle;
	}

	static QString WidgetStyleSheet::viewTabWidgetStyleSheet(const QString &name = NULL)
	{
		const QString thisItemStyle
		(
			//"QTabBar::tab:first : selected{	margin-left: 0;	margin-right: 0;}\n"
			//"QTabBar::tab:selected, QTabBar::tab:hover {}\n"
			//"QTabBar::tab{	border:0px solid #C4C4C3;	border-top-left-radius: 4px;	border-top-right-radius: 4px;	min-width: 8ex;padding: 2px;	}\n"
			"QTabBar::tab{"
			"border: 0px solid #C4C4C3;"
			"height:48px;"
			"width:48px;"
			"}"
			"QTabWidget::pane{ border: 1px solid #646464;}"
		);
		return thisItemStyle;
	}

	static QString WidgetStyleSheet::textStyleSheet(const QString &name = NULL)
	{
		const QString thisItemStyle
		(
			"QLabel{"
			"font: 9pt \"Arial\";"
			"border: 0px solid black"
			"}"


		);
		return thisItemStyle;
	}
	static QString WidgetStyleSheet::textStyleSheet2(const QString &name = NULL)
	{
		const QString thisItemStyle
		(
			"QLabel{"
			"font: 75 italic 9pt \"Arial\";"
			"color: rgb(0, 0, 255);"
			"}"
		);
		return thisItemStyle;
	}

	static QString WidgetStyleSheet::spinBoxStyleSheet(const QString &name = NULL)
	{
		const QString thisItemStyle
		(
			"QDoubleSpinBox{font: 10pt \"Arial\";"
			"background-color: rgb(237, 237, 237, 255)"
			"}"
			//"QDoubleSpinBox{background: url(\":/images/icons/ic_spinbox_line.png\") bottom center no-repeat;}\n"

			"QDoubleSpinBox{ border-bottom: 3px;	border-bottom-style: ridge;	border-bottom-color: rgb(44, 62, 80); }"
			"QDoubleSpinBox::up-button{ subcontrol-origin: border;	subcontrol-position: top right; 	width: 12px;height: 14px;"
			"border-image: url(:/images/icons/btn_triangle_on_2x.png) 1; border-width: 0px; }\n"

			//"QDoubleSpinBox::up-button:hover{border-image: url(:/images/icons/btn_triangle_up_on.png)1;}\n"
			//"QDoubleSpinBox::up-button:pressed{border-image:url(:/images/icons/btn_triangle_up_on.png)1;}\n"



			"QDoubleSpinBox{padding-left: 10px; border-width: 3;}\n"

			"QDoubleSpinBox::down-button{ subcontrol-origin: border;	subcontrol-position: bottom right; 	width: 12px; "
			"border-image: url(:/images/icons/btn_triangle_down_2x.png) 1; border-width: 0px; }\n"

			//"QDoubleSpinBox::up-button:hover{border-image: url(:/images/icons/btn_triangle_up_on.png)1;}\n"
			//"QDoubleSpinBox::down-button:pressed{border-image:url(:/images/icons/btn_triangle_down_on.png)1;}\n"
		);
		return thisItemStyle;
	}

	static QString WidgetStyleSheet::scrollBarStyleSheet(const QString &name = NULL)
	{
		//QString path1(":/images/icons/ic_drag_slider@2x.png");
		const QString thisItemStyle
		(
			//***scroll bar center picture
			"QScrollBar:vertical:enabled{"
			//"background-color: rgb(255, 255, 255);"
			"border: 0px solid grey;"
			"width: 15px;"
			"height: 250px;"
			"margin: 42px 0 42px 0;"
			"image: url(:/images/icons/ic_drag_slider.png);"

			"}"

			//***scroll bar center picture
			"QScrollBar:vertical:disabled{"
			//"background-color: rgb(255, 255, 255);"
			"border: 0px solid grey;"
			"width: 15px;"
			"margin: 42px 0 42px 0;"
			"image: url(:/images/icons/ic_drag_slider_disable.png);"

			"}"

			//***scroll bar dragger
			"QScrollBar::handle:vertical:enabled{"
			"image: url(:/images/icons/btn_dragger.png);"
			"min-height: 20px;"
			"}"
			//***scroll bar dragger
			"QScrollBar::handle:vertical:disabled{"
			"image: url(:/images/icons/btn_dragger_disable.png);"
			"min-height: 20px;"
			"}"

			//***arrow position
			"QScrollBar::add-line:vertical{"
			"border: 0px solid grey;"
			"height: 36px;"
			"subcontrol-position: top;"
			"subcontrol-origin: margin;"
			"}"

			//***arrow position
			"QScrollBar::sub-line:vertical{"
			"border: 0px solid grey;"
			"height: 36px;"
			"subcontrol-position: bottom;"
			"subcontrol-origin: margin;"
			"}"

			"QScrollBar::up-arrow:vertical:enabled{"
			"image: url(:/images/icons/btn_reduce_label.png);"
			"}"
			"QScrollBar::up-arrow:vertical:disabled{"
			"image: url(:/images/icons/btn_reduce_label_disable.png);"
			"}"

			"QScrollBar::down-arrow:vertical:enabled"
			"{image: url(:/images/icons/btn_add_label.png);}"
			"QScrollBar::down-arrow:vertical:disabled"
			"{image: url(:/images/icons/btn_add_label_disable.png);}"

			"QScrollBar::add-page:vertical, QScrollBar::sub-page : vertical{"
			"background: none;"
			"}"


		);
		return thisItemStyle;
	}

	static QString WidgetStyleSheet::checkBoxStyleSheet(const QString &name = NULL)
	{
		//QString path1(":/images/icons/ic_drag_slider@2x.png");
		const QString thisItemStyle
		(
			"QCheckBox{"
			"spacing: 5px;"
			"font: 9pt \"Arial\""
			"}"

			"QCheckBox::indicator{"
			"width: 16px;"
			"height: 16px;"
			"}"

			"QCheckBox::indicator:checked{"
			"image:url(:/images/icons/btn_checkbox_on_large_2x.png);"
			"}"

			"QCheckBox::indicator : checked : hover{"
			"image:url(:/images/icons/btn_checkbox_on_large_2x.png);"
			"}"

			"QCheckBox::indicator : checked : pressed{"
			"image:url(:/images/icons/btn_checkbox_on_large_2x.png);"
			"}"



		);
		return thisItemStyle;
	}
	static QString WidgetStyleSheet::tabWidgetStyleSheet(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			//	"QTabWidget::pane{ /* The tab widget frame */"
			//	"border-top: 2px solid #C2C7CB;"
			//	"}"

			//"QTabWidget::tab-bar{"
			//"left: 5px; /* move to the right by 5px */"
			//"}"

			///* Style the tab using the tab sub-control. Note that
			//it reads QTabBar _not_ QTabWidget */
			//"QTabBar::tab{"
			//"background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,"
			//"stop : 0 #E1E1E1, stop: 0.4 #DDDDDD,"
			//"  stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
			//"border: 2px solid #C4C4C3;"
			//"border-bottom-color: #C2C7CB; /* same as the pane color */"

			//"border-top-left-radius: 4px;"
			//"border-top-right-radius: 4px;"
			//"min-width: 8ex;"
			//"padding: 2px;"
			//"}"

			//"QTabBar::tab:selected, QTabBar::tab : hover{"
			//"background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,"
			//"stop : 0 #fafafa, stop: 0.4 #f4f4f4,"
			//"   stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
			//"}"

			"QTabBar::tab:selected{"
			"border-color: #9B9B9B;"
			"border-bottom-color: #C2C7CB; /* same as pane color */"
			"border-color: rgb(0, 255, 127);"
			//"background-color: rgb(39, 255, 255);"		
			"}"

			//"QTabBar::tab:!selected{"
			//"margin-top: 2px; /* make non-selected tabs look smaller */"
			//"}"

			///* make use of negative margins for overlapping tabs */
			//"QTabBar::tab : selected{"
			//	/* expand/overlap to the left and right by 4px */
			//	"margin-left: -4px;"
			//	"margin-right: -4px;"
			//	"}"

			//	"QTabBar::tab:first : selected{"
			//	"margin-left: 0; /* the first selected tab has nothing to overlap with on the left */"
			//	"}"

			//	"QTabBar::tab : last : selected{"
			//	"margin-right: 0; /* the last selected tab has nothing to overlap with on the right */"
			//	"}"

			//	"QTabBar::tab : only-one{"
			//	"margin: 0; /* if there is only one tab, we don't want overlapping margins */"



		);
		return thisItemStyle;
	}
	static QString WidgetStyleSheet::check_onoff_styleSheet(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			"QCheckBox{"
			"spacing: 5px;"
			"}"

			"QCheckBox::indicator{"
			"width: 60px;"
			"height: 20px;"
			"}"

			"QCheckBox::indicator:unchecked{"
			"image: url(:/images/icons/ic_toggle_off.png);"
			"}"

			/*QCheckBox::indicator:unchecked:hover {
			image: url(:/images/checkbox_unchecked_hover.png);
			}

			QCheckBox::indicator:unchecked:pressed {
			image: url(:/images/checkbox_unchecked_pressed.png);
			}*/

			"QCheckBox::indicator:checked{"
			"image: url(:/images/icons/ic_toggle_on.png);"
			"}"
		);
		return thisItemStyle;
	}
	static QString WidgetStyleSheet::menuStyleSheet(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			"QMenuBar{"
			//"background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 lightgray, stop : 1 darkgray);"
			"background-color: rgb(44, 62, 80);"
			//"color: rgb(255, 255, 255);"
			"}"

			"QMenuBar::item{"
			"spacing: 10px; /* spacing between menu bar items */"
			"padding: 8px 4px;"
			"background: transparent;"
			//"background-color:rgb(125, 125, 125);"
			"background-color:rgb(44, 62, 80);"
			"border-radius: 16px;"
			"color: rgb(255, 255, 255);"
			"font: 75 12pt \"Arial\";"
			"}"

			"QMenuBar::item:selected{ /* when selected using mouse or keyboard */"
			//"background: #a8a8a8;"
			//"background-color: rgb(255, 255, 255)"
			"color: rgb(255, 255, 255);"
			"}"

			"QMenuBar::item:pressed{"
			//"background: #888888;"
			"color: rgb(255, 255, 255);"
			"}"
			"QMenuBar::item:disabled{"
			//"color:#111111;"
			"color: rgb(128, 128, 128); "

			"}"

		);
		return thisItemStyle;
	}

	static QString WidgetStyleSheet::dockWidget(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			"QDockWidget{"
			"border: 1px solid lightgray;"
			"margin-top:0 px;"
			"color: rgb(255, 255, 255); "
			"font: 9pt \"Arial\";"
			//"titlebar-close-icon: url(close.png);"
			//"titlebar-normal-icon: url(undock.png);"
			"}"

			//"QDockWidget::title{"
			//
			//"text-align: left; /* align the text to the left */"
			//
			//"background: rgb(44, 62, 80);"
			//"padding-left: 5px;"
			//"}"
			//
		);
		return thisItemStyle;
	}
	static QString WidgetStyleSheet::grupBox(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			"QGroupBox::title{"
			"font: 9pt \"Arial\";"
			"border: 0px solid black"
			"}"


		);
		return thisItemStyle;
	}



	static QString WidgetStyleSheet::statusLabel(const QString &name = NULL)
	{

		const QString thisItemStyle
		(
			"QLabel{"
			"font: 9pt \"Arial\";"
			"border: 0px solid black"
			"color: rgb(255, 0, 0);"
			"}"


		);
		return thisItemStyle;
	}

static QString WidgetStyleSheet::viewTextStyleSheet(const QString & name = NULL, const QString & name2 = NULL)
	{
		const QString thisItemStyle
		(
			"QToolButton:enabled{\n"

			"   border: none;\n"
			"   background: url(" + name + ") top center no-repeat;\n"
			//"   padding-top: 200px;\n"
			"   width: 181px;\n"
			"   height :40px;\n"
			"   font: 12pt \"Arial\";\n"

			"   color: white;\n"
			"}\n"

			"QToolButton:disabled{"
			//"background-color:#dddddd;"
			"background: url(" + name2 + ") top center no-repeat; \n"
			"   border: none;\n"
			"   width: 181px;\n"
			"   height :40px;\n"
			"}"
		);

		return thisItemStyle;
	}



static QString WidgetStyleSheet::progressBarWidget(const QString & name = NULL, const QString & name2 = NULL)
	{
		const QString thisItemStyle
			(
"			QProgressBar {"
    "border: 2px solid grey;"
    "border-radius: 5px;"
	"background-color: rgb(237, 237, 237, 255);"
"color: rgb(255, 255, 255);"
"}"

"QProgressBar::chunk {"
    "background-color: rgb(0, 122, 204, 255);"
	"margin: 0px;"
    "width: 2px;}"
			"}"
			"QProgressBar{"
	"border: 2px solid grey;"
		"border-radius: 5px;"
		"text-align: center;	}"

			
			);

		return thisItemStyle;
	}


//SYDNY 07/26/2017
static QString WidgetStyleSheet::check_onoff_styleSheet2(const QString &name = NULL)
{

	const QString thisItemStyle
	(
		"QCheckBox{"
		"spacing: 5px;"
		"}"

		"QCheckBox::indicator{"
		"width: 60px;"
		"height: 20px;"
		"}"

		"QCheckBox::indicator:unchecked{"
		"image: url(:/images/icons/hidden_on.png);"
		"}"

		/*QCheckBox::indicator:unchecked:hover {
		image: url(:/images/checkbox_unchecked_hover.png);
		}

		QCheckBox::indicator:unchecked:pressed {
		image: url(:/images/checkbox_unchecked_pressed.png);
		}*/

		"QCheckBox::indicator:checked{"
		"image: url(:/images/icons/hidden_off.png);"
		"}"
	);
	return thisItemStyle;
}

static QString WidgetStyleSheet::groupBoxStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QGroupBox{"
		"font: 14px \"Arial\";"
		"background-color: #ffffff;"
		"border: 1px solid #707070;"
		"border-radius: 1px;"
		"margin-top: 5ex;" /* leave space at the top for the title */
		"}"

		"QGroupBox::title{"
		"color:#2c3d50;"
		"subcontrol-origin: margin;"
		"subcontrol-position: top left; "/* position at the top center */
		"padding: 0 3px;"
		"}"
		);
	return thisItemStyle;
}

static QString WidgetStyleSheet::settingLabelStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QLabel { "
	"color : #2c3d50; "
	"font: 14px \"Arial\";"
	"   width: 130px;\n"
	"   height :44px;\n"
	"}"
		);
	return thisItemStyle;
}
static QString WidgetStyleSheet::settingLabelchangedStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QLabel { "
		"color : #2d00ff; "
		"font: 14px \"Arial\";"
		"   width: 130px;\n"
		"   height :44px;\n"
		"}"
		);
	return thisItemStyle;
}
static QString WidgetStyleSheet::settingSpinBoxStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QSpinBox{font: 14px \"Arial\";"

		"}"
		);
	return thisItemStyle;
}

static QString WidgetStyleSheet::settingComboBoxStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QLabel {  color : blue; }"
		);
	return thisItemStyle;
}

static QString WidgetStyleSheet::settingCheckBoxStyleSheet(const QString &name = NULL)
{
	const QString thisItemStyle
		(

		"QLabel {  color : blue; }"
		);
	return thisItemStyle;
}

};







#endif