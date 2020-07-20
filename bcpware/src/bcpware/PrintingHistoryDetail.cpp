#include "PrintingHistoryDetail.h"
#include "../common/define_some_parameter.h"
#include "./widgetStyleSheet.h"


CustomPie::CustomPie(float _value, QColor _pie_Color, float _startAngle, QString _strValue, QColor _bg_Color, int _radius, float _valueSum, bool _clockWise, QWidget *parent) :QWidget(parent),
bg_Color(_bg_Color), pie_Color(_pie_Color), radius(_radius), value(_value), valueSum(_valueSum), startAngle(_startAngle), strValue(_strValue), clockWise(_clockWise)
{
	/*bg_Color = QColor(233, 233, 233);
	pie_Color = QColor(255, 255, 0);
	radius = 25;
	value = 70;
	valueSum = 430;
	startAngle = 90;
	strValue = "abcd";
	clockWise = false;*/
}
void CustomPie::paintEvent(QPaintEvent *event)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);
	drawBG(&painter);
	drawPie(&painter);

}

void CustomPie::drawPie(QPainter *painter)
{
	QVector<float>values;
	values << value;
	QVector<QColor> colors;
	colors << pie_Color << QColor(0, 255, 0);

	painter->save();
	//int radius = 50;
	QRect rect(-radius, -radius, radius * 2, radius * 2);

	int count = 2;//labels.count();
	for (int i = 0; i < values.size(); ++i) {

		double value = values.at(i);
		double arcLength = value / valueSum * 360;
		if (clockWise)
			arcLength *= -1;
		double percent = value / valueSum * 100;
		QRect pieRect = rect;
		painter->setPen(Qt::NoPen);
		//        if (explodedIndex == i || explodedAll)
		//        {
		//            painter->setPen(borderColor);
		//            QPoint center = pieRect.center();
		//            int mid = startAngle + arcLength / 2;
		//            center += getOffsetPoint(mid);
		//            pieRect.moveCenter(center);
		//        }
		painter->setBrush(colors.at(i));
		painter->drawPie(pieRect, (startAngle * 16), arcLength * 16);

		//if (showPercent && percent > 7)
		QString textValue;
		if (true /*&& percent > 7*/)
		{
			textValue = QString("%1%2%3%").arg(strValue).arg(strValue.isEmpty() ? "" : "\n").arg(QString::number(percent, 'f', 2));
		}
		int mid = startAngle + arcLength / 2;
		int offset = 60;
		if (percent >= 50) { offset = 45; }
		else if (percent >= 30) { offset = 55; }
		else if (percent >= 15) { offset = 60; }
		QPoint p = QPoint(10, 25);//getOffsetPoint(mid, offset);
		QRect textRect; textRect.setX(p.x() - 40);
		textRect.setY(p.y() - 30);
		textRect.setWidth(110);
		textRect.setHeight(60);
		painter->setPen(Qt::black);
		//painter->drawRect(textRect);
		QFont font("Arial", 20, QFont::Bold);
		//font.setPixelSize(textValue.isEmpty() ? 20 : 17);
		painter->setFont(font);
		painter->setPen(QColor(0, 0, 0));
		painter->drawText(textRect, Qt::AlignCenter, textValue);
		//startAngle += arcLength;
	}
	painter->restore();
}

void CustomPie::drawBG(QPainter *painter)
{
	//int radius = 99;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(bg_Color);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();

}



PrintingHistoryDetail::PrintingHistoryDetail(int i, QWidget *parent) : QDialog(parent), ui(new Ui::HistoryDetailDia)//, inPHI(_phiDetail)
{
	ui->setupUi(this);

	QVariant _dmHistory, _sliceSetting;
	QVariant _currentID, _id_History;
	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()))
	{
		if (ParamOp::extractVariantTest(_id_History, _dmHistory, QString::number(i)))
		{
			itemMap = _id_History.toMap();
			if (ParamOp::extractVariantTest(_sliceSetting, _id_History, "SLICE_SETTING"))
			{
				sliceSettingList = _sliceSetting.toList();
			}
		}
	}

	const int w = ui->frame->width();
	const int h = ui->frame->height();
	ui->thumbnailLb->setPixmap(QPixmap(itemMap.value("JOB_THUMBNAIL_NAME").toString()).scaled(w, h, Qt::KeepAspectRatio));


	initConsumeTab();
	initSliceTab();


}
void PrintingHistoryDetail::initSliceTab()
{
	QVBoxLayout *vb1 = new QVBoxLayout;
	QStringList labelList;
	labelList << "Layer Height" << "Add hex pattern" << "Start point offset" << "Draw line as rectangle" << "Second layer on/off" << "Second layer binder percent"
		<< "Second layer shell binder percent" << "Binder inward offset on/off" << "Binder inward offset value" << "Bottom binder lighter thick" << "Bottom binder lighter %"
		<< "Horizontal line width" << "Vertical line width" << "Color profile on/off" << "Color profile path" << "Spitton M width"
		<< "Spitton C width" << "Spitton Y width" << "Second page spitton CMY width" << "Second page spitton Binder width" << "Fan speed rpm"
		<< "IR on/off" << "Mid-job frequency" << "Wiper index" << "Wiper click" << "Post-heating On/off"
		<< "Post-Heating minutes" << "Pump value";
	for (int i = 0; i < 20; i++)
	{
		QMetaType::Type tempType = static_cast<QMetaType::Type>(sliceSettingList.at(i).type());
		switch (tempType)
		{
		case QMetaType::Double:
		case QMetaType::Float:
			vb1->addWidget(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toDouble(), 'g', 4)));
			break;
		case QMetaType::QString:
			vb1->addWidget(factoryLabel(labelList.at(i), sliceSettingList.at(i).toString()));
			break;
		case QMetaType::Int:
			vb1->addWidget(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toInt(), 'g', 0)));
			break;
		case QMetaType::Bool:
			vb1->addWidget(factoryLabel(labelList.at(i), sliceSettingList.at(i).toBool() ? "true" : "false"));

		}

	}


	//vb1->addWidget(factoryLabel("Color profile", sliceSettingList.at(14).toString()));



	QWidget *spacerWidget = new QWidget(this);//***ªÅ¥Õspacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);
	vb1->addWidget(spacerWidget);

	ui->scrollAreaWidgetContents_2->setLayout(vb1);
}
void PrintingHistoryDetail::initConsumeTab()
{

	float cStartLevel = itemMap.value("CYAN_START_LEVEL").toFloat();
	float cEndLevel = itemMap.value("CYAN_END_LEVEL").toFloat() == 0 ? cStartLevel : itemMap.value("CYAN_END_LEVEL").toFloat();
	float cUsed = cStartLevel - cEndLevel;
	CustomPie *cyanStartpie = new CustomPie(cStartLevel, QColor(Qt::cyan));
	CustomPie *cyanEndpie = new CustomPie(cEndLevel, QColor(Qt::cyan));
	CustomPie *cyanUsed = new CustomPie(cUsed, QColor(Qt::darkCyan), 90 + (cEndLevel * 360 / 100));
	QHBoxLayout *chl = new QHBoxLayout();
	chl->addWidget(cyanStartpie);
	chl->addWidget(cyanEndpie);
	chl->addWidget(cyanUsed);

	ui->cyanGB->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
	ui->cyanGB->setLayout(chl);

	float mStartLevel = itemMap.value("MAGENTA_START_LEVEL").toFloat();
	float mEndLevel = itemMap.value("MAGENTA_END_LEVEL").toFloat() == 0 ? mStartLevel : itemMap.value("MAGENTA_END_LEVEL").toFloat();
	float mUsed = mStartLevel - mEndLevel;
	CustomPie *magentaStartpie = new CustomPie(mStartLevel, QColor(Qt::magenta));
	CustomPie *magentaEndpie = new CustomPie(mEndLevel, QColor(Qt::magenta));
	CustomPie *magentaUsed = new CustomPie(mUsed, QColor(Qt::darkMagenta), 90 + (mEndLevel * 360 / 100));
	QHBoxLayout *mhL = new QHBoxLayout();
	mhL->addWidget(magentaStartpie);
	mhL->addWidget(magentaEndpie);
	mhL->addWidget(magentaUsed);

	ui->magentaGB->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
	ui->magentaGB->setLayout(mhL);

	float yStartLevel = itemMap.value("YELLOW_START_LEVEL").toFloat();
	float yEndLevel = itemMap.value("YELLOW_END_LEVEL").toFloat() == 0 ? yStartLevel : itemMap.value("YELLOW_END_LEVEL").toFloat();
	float yUsed = yStartLevel - yEndLevel;
	CustomPie *yellowStartpie = new CustomPie(yStartLevel, QColor(Qt::yellow));
	CustomPie *yellowEndpie = new CustomPie(yEndLevel, QColor(Qt::yellow));
	CustomPie *yellowUsed = new CustomPie(yUsed, QColor(Qt::darkYellow), 90 + (yEndLevel * 360 / 100));
	QHBoxLayout *yhl = new QHBoxLayout();
	yhl->addWidget(yellowStartpie);
	yhl->addWidget(yellowEndpie);
	yhl->addWidget(yellowUsed);
	ui->yellowGB->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
	ui->yellowGB->setLayout(yhl);

	float bStartLevel = itemMap.value("BINDER_START_LEVEL").toFloat();
	float bEndLevel = itemMap.value("BINDER_END_LEVEL").toFloat() == 0 ? bStartLevel : itemMap.value("BINDER_END_LEVEL").toFloat();
	float bUsed = bEndLevel == 0 ? bStartLevel : (bStartLevel - bEndLevel);
	CustomPie *binderStartpie = new CustomPie(bStartLevel, QColor(128, 128, 128));
	CustomPie *binderEndpie = new CustomPie(bEndLevel, QColor(128, 128, 128));
	CustomPie *binderUsed = new CustomPie(bUsed, QColor(Qt::darkGray), 90 + (bEndLevel * 360 / 100));
	QHBoxLayout *bhl = new QHBoxLayout();
	bhl->addWidget(binderStartpie);
	bhl->addWidget(binderEndpie);
	bhl->addWidget(binderUsed);
	ui->binderGB->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
	ui->binderGB->setLayout(bhl);
	//wiper
	QVBoxLayout *bhwiper = new QVBoxLayout();
	int wiperStart = itemMap.value("WIPES_AT_START").toInt();
	int wiperEnd = itemMap.value("WIPES_AT_END").toInt();
	int wiperUsed = wiperStart - wiperEnd;

	bhwiper->addWidget(factoryLabel("Wiper at start", QString::number(wiperStart)));
	bhwiper->addWidget(factoryLabel("Wiper at End", QString::number(wiperEnd)));
	bhwiper->addWidget(factoryLabel("Wiper used", QString::number(wiperUsed)));
	ui->wiperGB->setLayout(bhwiper);
	//printhead
	QVBoxLayout *bhPrinthead = new QVBoxLayout();	
	int printheadStart = itemMap.value("PRINTHEAD_PAGE_START").toInt();
	int printheadEnd = itemMap.value("PRINTHEAD_PAGE_END").toInt();
	int printheadUsed = printheadEnd - printheadStart;

	bhPrinthead->addWidget(factoryLabel("Printhead ID", itemMap.value("PRINTHEAD_ID").toString()));
	bhPrinthead->addWidget(factoryLabel("Printhead at start", QString::number(printheadStart)));
	bhPrinthead->addWidget(factoryLabel("Printhead at End", QString::number(printheadEnd)));
	bhPrinthead->addWidget(factoryLabel("Printhead used", QString::number(printheadUsed)));
	ui->printheadGB->setLayout(bhPrinthead);
}

QWidget *PrintingHistoryDetail::factoryLabel(QString _decription, QString _value)
{
	QFrame *frr = new QFrame;
	QHBoxLayout * hbl = new QHBoxLayout();
	QLabel *descriptionLB = new QLabel(_decription);
	QLabel *valueLB = new QLabel(_value);
	valueLB->setWordWrap(true);
	hbl->addWidget(descriptionLB);
	hbl->addWidget(valueLB);
	frr->setLayout(hbl);
	return frr;
}

PrintingHistoryDetail::~PrintingHistoryDetail()
{

}


InformationWidget::InformationWidget(QWidget *parent) :QWidget(parent)
{}
void InformationWidget::paintEvent(QPaintEvent *)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);



}

