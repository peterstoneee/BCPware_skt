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
#include "transformPreview.h"


TransformPreview::TransformPreview(QWidget *parent) :QDialog(parent)
{
	/*tpTE = new QTextEdit;
	tpTE->setReadOnly(true);
	tpTE->setLineWrapMode(QTextEdit::NoWrap);

	tpPB = new QPushButton(tr("&Close"));
	connect(tpPB, SIGNAL(clicked()), this, SLOT(close()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(tpTE);
	layout->addWidget(tpPB);
	setLayout(layout);
*/
	setWindowTitle(tr("Preview"));
}

void TransformPreview::createContent()
{

}
void TransformPreview::setWindowFlags(Qt::WindowFlags flags)
{
	QWidget::setWindowFlags(flags);

	/*QString text;


	Qt::WindowFlags type = (flags & Qt::WindowType_Mask);
	if (type == Qt::Window) {
		text = "Qt::Window";
	}
	else if (type == Qt::Dialog) {
		text = "Qt::Dialog";
	}
	else if (type == Qt::Sheet) {
		text = "Qt::Sheet";
	}
	else if (type == Qt::Drawer) {
		text = "Qt::Drawer";
	}
	else if (type == Qt::Popup) {
		text = "Qt::Popup";
	}
	else if (type == Qt::Tool) {
		text = "Qt::Tool";
	}
	else if (type == Qt::ToolTip) {
		text = "Qt::ToolTip";
	}
	else if (type == Qt::SplashScreen) {
		text = "Qt::SplashScreen";
	}

	if (flags & Qt::MSWindowsFixedSizeDialogHint)
		text += "\n| Qt::MSWindowsFixedSizeDialogHint";
	if (flags & Qt::X11BypassWindowManagerHint)
		text += "\n| Qt::X11BypassWindowManagerHint";
	if (flags & Qt::FramelessWindowHint)
		text += "\n| Qt::FramelessWindowHint";
	if (flags & Qt::NoDropShadowWindowHint)
		text += "\n| Qt::NoDropShadowWindowHint";
	if (flags & Qt::WindowTitleHint)
		text += "\n| Qt::WindowTitleHint";
	if (flags & Qt::WindowSystemMenuHint)
		text += "\n| Qt::WindowSystemMenuHint";
	if (flags & Qt::WindowMinimizeButtonHint)
		text += "\n| Qt::WindowMinimizeButtonHint";
	if (flags & Qt::WindowMaximizeButtonHint)
		text += "\n| Qt::WindowMaximizeButtonHint";
	if (flags & Qt::WindowCloseButtonHint)
		text += "\n| Qt::WindowCloseButtonHint";
	if (flags & Qt::WindowContextHelpButtonHint)
		text += "\n| Qt::WindowContextHelpButtonHint";
	if (flags & Qt::WindowShadeButtonHint)
		text += "\n| Qt::WindowShadeButtonHint";
	if (flags & Qt::WindowStaysOnTopHint)
		text += "\n| Qt::WindowStaysOnTopHint";
	if (flags & Qt::CustomizeWindowHint)
		text += "\n| Qt::CustomizeWindowHint";

	tpTE->setPlainText(text);*/

}

//TransformPreview::~TransformPreview()
//{
//}
