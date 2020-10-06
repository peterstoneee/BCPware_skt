#include "PrintingHistoryDetail.h"
#include "../common/define_some_parameter.h"
#include "./widgetStyleSheet.h"
#include <QRectF>
#include <QPointF>


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
		painter->drawArc(pieRect, (startAngle * 16), arcLength * 16);

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




CustomArc::CustomArc(QString _inktype, QVector<float> _value, QVector<QColor> _pie_Color, float _startAngle, QString _strValue, QColor _bg_Color, int _radius, float _valueSum, bool _clockWise, int _penWidth, QWidget *parent) : QWidget(parent),
bg_Color(_bg_Color), arc_Color(_pie_Color), radius(_radius), inputValue(_value), valueSum(_valueSum), startAngle(_startAngle), strValue(_strValue), clockWise(_clockWise), penWidth(_penWidth), inkType(_inktype)
{
	upperY = 100;
}


void CustomArc::drawArc(QPainter *painter)
{
	QVector<float> arcLength;
	arcLength<< (clockWise ? (inputValue.at(0)*-1 / valueSum * 360) : (inputValue.at(0) / valueSum * 360))
		<< (clockWise ? (inputValue.at(1)*-1 / valueSum * 360) : (inputValue.at(1) / valueSum * 360))
		<<(clockWise ? (inputValue.at(2)*-1 / valueSum * 360) : (inputValue.at(2) / valueSum * 360));
	

	double arcLength3 = 20 / valueSum * 360;
	if (clockWise)
		arcLength3 *= -1;

	/*QVector<float>values;
	values << value_1 << 20;*/
	/*QVector<QColor> colors;
	colors << pie_Color << QColor(0, 255, 0,100);*/
	QVector<QPen> pens;
	pens << QPen(arc_Color.at(0), 8, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) 
		<< QPen(arc_Color.at(1), 8, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin)
		<< QPen(arc_Color.at(2), 50, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);


	QVector<float> startAngles;
	startAngles << startAngle << startAngle << startAngle + arcLength.at(0) - arcLength.at(2);

	painter->save();
	//int radius = 50;
	//QRect rect(-radius, -radius, radius * 2, radius * 2);

	QVector<QRect> rects;
	int radius2 = radius - 15;
	
	rects << QRect(-radius, -radius - upperY, radius * 2, radius * 2)
		<< QRect(-radius2, -radius2 - upperY, radius2 * 2, radius2 * 2)
		<< QRect(-radius2, -radius2 - upperY, radius2 * 2, radius2 * 2);
	
	
	//painter->drawRect(rects.at(0));

	for (int i = 0; i < 1; i++)
	{
	int radius = 6;
	int offset2 = 123;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(arc_Color.at(1));
	painter->translate(0, -upperY);
	painter->rotate(-180 - arcLength.at(1));
	/*double degRotate =  arcLength.at(0);
	painter->rotate(degRotate);*/
	painter->drawEllipse(-radius, radius + offset2, radius * 2, radius * 2);
	painter->restore();
	}


	int count = 2;//labels.count();
	for (int i = 0; i < inputValue.size(); ++i) {

		double value = inputValue.at(i);
		/*double arcLength = value / valueSum * 360;
		if (clockWise)
			arcLength *= -1;*/
		double percent = value / valueSum * 100;
		QRect pieRect = rects.at(i);
		
		painter->setPen(pens.at(i));
		painter->setBrush(arc_Color.at(i));		
		painter->drawArc(pieRect, (startAngles.at(i) * 16), arcLength.at(i) * 16);
		

		



		//if (showPercent && percent > 7)
		QString textValue;
		if (true /*&& percent > 7*/ && i==2)
		{
			textValue = QString("%1%2%3%").arg(strValue).arg(strValue.isEmpty() ? "" : "\n").arg((percent>0) ? QString::number(-percent, 'f', 2) : QString::number(percent, 'f', 2));
		}
		//int mid = startAngle + arcLength / 2;
		int offset = 60;
		if (percent >= 50) { offset = 45; }
		else if (percent >= 30) { offset = 55; }
		else if (percent >= 15) { offset = 60; }
		QPoint p = QPoint(10, 25);//getOffsetPoint(mid, offset);
		QRect textRect;
		/*textRect.setX(p.x() - 40);
		textRect.setY(p.y() - 30);*/
		textRect.setX(-75);
		textRect.setY(-130);
		textRect.setWidth(150);
		textRect.setHeight(60);
		painter->setPen(Qt::black);
		//painter->drawRect(textRect);
		QFont font("Arial", 25, QFont::Bold);
		//font.setPixelSize(textValue.isEmpty() ? 20 : 17);
		painter->setFont(font);
		painter->setPen(QColor(0, 0, 0));
		if(i==2)painter->drawText(textRect, Qt::AlignCenter, textValue);
		//startAngle += arcLength;
	}
	painter->restore();
}

//void CustomArc::drawArc(QPainter *painter)
//{
//	QVector<float>values;
//	values << value_1;
//	QVector<QColor> colors;
//	colors << pie_Color << QColor(0, 255, 0);
//	QVector<QPen> pens;
//	pens << QPen(Qt::green, 10, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) << QPen(Qt::red, 30, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
//
//	double arcLength = value_1 / valueSum * 360;
//	if (clockWise)
//		arcLength *= -1;
//
//	QVector<float> startAngles;
//	startAngles << startAngle << startAngle + arcLength - 20;
//
//	painter->save();
//	//int radius = 50;
//	QRect rect(-radius, -radius, radius * 2, radius * 2 );
//	int radius2 = 99;	
//	QRect rect2(-radius2, -radius2, radius2 * 2 , radius2 * 2 );
//
//	int count = 2;//labels.count();
//	for (int i = 0; i < values.size(); ++i) {
//
//		double value = values.at(i);
//		double arcLength = value / valueSum * 360;
//		if (clockWise)
//			arcLength *= -1;
//		double percent = value / valueSum * 100;
//		QRect pieRect = rect;
//		QPen pen(Qt::green, penWidth, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
//		painter->setPen(pen);
//		
//		painter->setBrush(colors.at(i));
//		//painter->drawPie(pieRect, (startAngle * 16), arcLength * 16);
//		painter->drawArc(pieRect, (startAngle * 16), arcLength * 16);
//		pen.setWidth(30);
//		pen.setColor(QColor(255,0,0,100));
//		painter->setPen(pen);
//		painter->drawArc(rect2, ((startAngle+arcLength-20) * 16), 50 * 16);
//
//		//if (showPercent && percent > 7)
//		QString textValue;
//		if (true /*&& percent > 7*/)
//		{
//			textValue = QString("%1%2%3%").arg(strValue).arg(strValue.isEmpty() ? "" : "\n").arg(QString::number(percent, 'f', 2));
//		}
//		int mid = startAngle + arcLength / 2;
//		int offset = 60;
//		if (percent >= 50) { offset = 45; }
//		else if (percent >= 30) { offset = 55; }
//		else if (percent >= 15) { offset = 60; }
//		QPoint p = QPoint(10, 25);//getOffsetPoint(mid, offset);
//		QRect textRect; textRect.setX(p.x() - 40);
//		textRect.setY(p.y() - 30);
//		textRect.setWidth(110);
//		textRect.setHeight(60);
//		painter->setPen(Qt::black);
//		//painter->drawRect(textRect);
//		QFont font("Arial", 20, QFont::Bold);
//		//font.setPixelSize(textValue.isEmpty() ? 20 : 17);
//		painter->setFont(font);
//		painter->setPen(QColor(0, 0, 0));
//		painter->drawText(textRect, Qt::AlignCenter, textValue);
//		//startAngle += arcLength;
//	}
//	painter->restore();
//}
void CustomArc::paintEvent(QPaintEvent *event){
	int width =  this->width();
	int height =  this->height();
	int side = height;// qMin(width, height);
	float scaleValue = side / 600.;
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); 
	painter.drawRect(QRect(0, 0, width, height));

	painter.setFont(QFont("Arial", 14));
	painter.drawText(QPointF(10, 30), inkType);

	painter.translate(width / 2, height / 2);
	painter.scale(side / 600.0, side / 600.0);
	painter.setPen(Qt::black);
	//painter.drawRect(QRect(-762, -300, 1500, 600));
	//painter.drawRect(QRect(-width * 2 / scaleValue, height * 2 / scaleValue, width / scaleValue, height/scaleValue));
	drawBG(&painter);
	drawArc(&painter);
	drawInfo(&painter);
	
}
void CustomArc::drawBG(QPainter *painter){
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(bg_Color);
	painter->drawEllipse(-radius, -radius - upperY, radius * 2, radius * 2);
	painter->restore();

}
void CustomArc::drawInfo(QPainter *painter)
{
	int textX = -130;
	int textY = 80;
	int textWidth = 100;
	int textHeight = 60;

	int rectX = textX + textWidth;
	int rectY = textY;
	int rectWidth = 40;
	int rectHeight = 50;

	int text2X = rectX + rectWidth+20;
	int text2Y = textY;
	int text2Width = 115;
	int text2Height = 60;


	painter->setPen(Qt::black);
	/*painter->drawRect(QRect(textX, textY, textWidth, textHeight));
	painter->drawRect(QRect(textX, textY + 60, textWidth, textHeight));
	painter->drawRect(QRect(textX, textY + 120, textWidth, textHeight));*/
	QFont font("Arial", 20, QFont::Bold);
	//font.setPixelSize(textValue.isEmpty() ? 20 : 17);
	painter->setFont(font);
	painter->setPen(QColor(0, 0, 0));
	painter->drawText(QRect(textX, textY, textWidth, textHeight), Qt::AlignLeft, "Start");
	painter->drawText(QRect(textX, textY + 60, textWidth, textHeight), Qt::AlignLeft, "End");
	painter->drawText(QRect(textX, textY + 120, textWidth, textHeight), Qt::AlignLeft, "Used");

	painter->fillRect(QRect(rectX, rectY, rectWidth, rectHeight), QBrush(arc_Color.at(0)));
	painter->fillRect(QRect(rectX, rectY + 60, rectWidth, rectHeight), QBrush(arc_Color.at(1)));
	painter->fillRect(QRect(rectX, rectY + 120, rectWidth, rectHeight), QBrush(arc_Color.at(2)));

	/*painter->drawRect(QRect(text2X, text2Y, text2Width, text2Height));
	painter->drawRect(QRect(text2X, text2Y + 60, text2Width, text2Height));
	painter->drawRect(QRect(text2X, text2Y + 120, text2Width, text2Height));*/

	//QString("%1%2%3%").arg(strValue).arg(strValue.isEmpty() ? "" : "\n").arg(QString::number(percent, 'f', 2));

	painter->drawText(QRect(text2X, text2Y, text2Width, text2Height), Qt::AlignLeft, (inputValue.size()>2) ? QString("%1%").arg(QString::number(inputValue.at(0), 'f', 2) ): "");
	painter->setPen(QColor(44, 135, 24));
	painter->drawText(QRect(text2X, text2Y + text2Height, text2Width, text2Height), Qt::AlignLeft, (inputValue.size()>2) ? QString("%1%").arg(QString::number(inputValue.at(1), 'f', 2) ): "");
	painter->setPen(QColor(255,0,0));
	painter->drawText(QRect(text2X, text2Y + text2Height * 2, text2Width, text2Height), Qt::AlignLeft, (inputValue.size()>2) ? QString("-%1%").arg(QString::number(inputValue.at(2), 'f', 2)) : "");



	painter->restore();
}
void CustomArc::drawCircle(QPainter *painter)
{
	




}


CustomBar::CustomBar(QWidget *parent) :QWidget(parent){}
void CustomBar::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(0, 50);
	drawRuler(&painter);
	drawBarBG(&painter);
	drawBar(&painter);
	
	
}
void CustomBar::drawBG(QPainter *painter)
{
	


}
void CustomBar::drawRuler(QPainter *painter)
{
	int space = 1;
	int maxValue = 400;
	int minValue = 0;
	int shortStep = 4;
	int longStep = 40;
	int precision = 0;

	int widthh = this->width();

	painter->save();
	painter->setPen(Qt::black);
	double initX = space + 20;
	double initY = space;
	QPointF leftPot(initX, initY);
	QPointF rightPot(width() - space, initY);
	painter->drawLine(leftPot, rightPot);

	double length = width() - 2 * space;
	double increment = length / (maxValue - minValue);

	int longLineLen = 10;
	int shortLineLen = 7;
	for (int i = maxValue; i >= minValue; i = i - shortStep)
	{
		if (i%longStep == 0)
		{
			/*QPointF leftPot(initX+longLineLen, initY);
			QPointF rightPot(initX, initY);
			painter->drawLine(leftPot, rightPot);*/

			QPointF botPot(initX, initY);
			QPointF topPot(initX, initY - longLineLen);
			painter->drawLine(botPot, topPot);

			QString strValue = QString("%1").arg((double)(400-i), 0, 'f', precision);
			double fontWidth = painter->fontMetrics().width(strValue);
			double fontHeight = painter->fontMetrics().height();
			QPointF textPot(initX - fontWidth + 15, initY + fontHeight);
			painter->drawText(textPot, strValue);
		}
		else
		{
			if (i % (longStep / 2) == 0)
			{
				shortLineLen = 7;
			}
			else
				shortLineLen = 4;
		}
		/*QPointF leftPot(initX + shortLineLen, initY);
		QPointF rightPot(initX, initY);
		painter->drawLine(leftPot, rightPot);*/

		QPointF botPot(initX, initY);
		QPointF topPot(initX, initY - shortLineLen);
		painter->drawLine(botPot, topPot);

		initX += increment * shortStep;
	}
	painter->restore();

}
void CustomBar::drawBar(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	double barWidth = barRect.width();
	double increment = (double)barWidth / (400);

	double initX = 300 * increment;
	QPointF topLeftPot(barRect.topLeft().x(),barRect.topLeft().y());
	QPointF bottomRightPot(barRect.bottomLeft().x() + initX, barRect.bottomLeft().y());
	QRectF currentRect(topLeftPot,bottomRightPot);

	double initX2 = 20 * increment;
	QPointF topLeftPot2(currentRect.bottomRight().x() - initX2, currentRect.topLeft().y());
	
	QRectF currentRect2(topLeftPot2, currentRect.bottomRight());

	QLinearGradient bgGradient(barRect.topLeft(), barRect.bottomRight());
	bgGradient.setColorAt(0.0, Qt::red);
	//bgGradient.setColorAt(0.5, Qt::yellow);
	bgGradient.setColorAt(1.0, QColor(128,0,0));



	painter->setBrush(bgGradient);
	//painter->setBrush(QColor(0, 255, 0));
	painter->drawRect(currentRect);

	painter->setBrush(QColor(128,255,128));
	painter->drawRect(currentRect2);
	painter->restore();


}
void CustomBar::drawBarBG(QPainter *painter)
{
	int space = 0;
	painter->save();
	painter->setPen(Qt::NoPen);
	double initY = space + 20+15;
	double initX = space + 20 + 15;
	QPointF topLeftPot(space+20, initY);
	QPointF bottomRightPot(width() - space, 50-space);
	barRect = QRectF(topLeftPot,bottomRightPot);
	painter->setBrush(Qt::lightGray);
	painter->drawRect(barRect);
	painter->restore();
}

CustomBarSecond::CustomBarSecond(QWidget *parent) : QWidget(parent)
{
	//wiperOrPrintHead = false;
}
CustomBarSecond::CustomBarSecond(bool _wOrP, int _minBarValue, int _maxBarValue, QVariant _startValue, QVariant _endValue, QVariant _usedValue, QVariant _printHeadID, QWidget *parent)
	: QWidget(parent), wiperOrPrintHead(_wOrP), minBarValue(_minBarValue), maxBarValue(_maxBarValue), startValue(_startValue), endValue(_endValue), usedValue(_usedValue), printHeadID(_printHeadID.toString())
{
	_startValue.isNull() ? startValue = QVariant() : startValue = _startValue;
	_endValue.isNull() ? endValue = QVariant() : endValue = _endValue;
	_usedValue.isNull() ? usedValue = QVariant() : usedValue = _usedValue;
		
}
void CustomBarSecond::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(0,5);
	drawBarBG(&painter);
	drawBar(&painter);
	drawInfo(&painter);
}


void CustomBarSecond::drawBar(QPainter *painter)
{
	if (wiperOrPrintHead){
		painter->save();
		painter->setPen(Qt::NoPen);
		double barWidth = barRect.width();
		double increment = (double)barWidth / (400);

		double initX = 300 * increment;
		QPointF topLeftPot(barRect.topLeft().x(), barRect.topLeft().y());
		QPointF bottomRightPot(barRect.bottomLeft().x() + initX, barRect.bottomLeft().y());
		QRectF currentRect(topLeftPot, bottomRightPot);

		double initX2 = 20 * increment;
		QPointF topLeftPot2(currentRect.bottomRight().x() - initX2, currentRect.topLeft().y());

		QRectF currentRect2(topLeftPot2, currentRect.bottomRight());

		QLinearGradient bgGradient(barRect.topLeft(), barRect.bottomRight());
		//bgGradient.setColorAt(0.0, Qt::red);
		//bgGradient.setColorAt(0.5, Qt::yellow);
		//bgGradient.setColorAt(1.0, QColor(0, 131, 236));


		//painter->setBrush(bgGradient);
		painter->setBrush(QColor(0, 131, 236));
		painter->drawRect(currentRect);

		painter->setBrush(QColor(0, 89, 159));
		painter->drawRect(currentRect2);
		painter->restore();
	}
	else
	{
		painter->save();
		painter->setPen(Qt::NoPen);
		double barWidth = barRect.width();
		double increment = (double)barWidth / (400);

		double initX = 300 * increment;
		QPointF topLeftPot(barRect.topLeft().x(), barRect.topLeft().y());
		QPointF bottomRightPot(barRect.bottomLeft().x() + initX, barRect.bottomLeft().y());
		QRectF currentRect(topLeftPot, bottomRightPot);

		double initX2 = 20 * increment;
		QPointF bottomRight2(currentRect.bottomRight().x() + initX2, currentRect.bottomLeft().y());

		QRectF currentRect2(currentRect.topRight(), bottomRight2);

		
		painter->setBrush(QColor(0, 131, 236));
		painter->drawRect(currentRect);

		painter->setBrush(QColor(0, 89, 159));
		painter->drawRect(currentRect2);
		painter->restore();

	}

}
void CustomBarSecond::drawBarBG(QPainter *painter)
{
	int space = 0;
	painter->save();
	painter->setPen(Qt::NoPen);
	double initX = space /*+ 20 + 15*/;
	double initY = 150;
	int backgroundheight = 20;

	//Rect
	QPointF topLeftPot(space , initY);
	QPointF bottomRightPot(width() - space, initY + backgroundheight);
	barRect = QRectF(topLeftPot, bottomRightPot);

	painter->setBrush(Qt::lightGray);
	painter->drawRect(barRect);

	int minValueX = 0;
	int minValueY = initY + backgroundheight + 5;
	int initWidth = 20;
	int initHeight = 20;
	QFont font("Arial", 8, QFont::Bold);
	QString strValue;
	int space2 = 5;
	QPointF textPot;
	//font.setPixelSize(textValue.isEmpty() ? 20 : 17);



	painter->setFont(QFont("Arial", 8, QFont::Bold));
	painter->setPen(QColor(0, 0, 0));
	strValue = QString("%1").arg(minBarValue);;
	painter->drawText(QRect(minValueX, minValueY, initWidth, initHeight), Qt::AlignLeft, strValue);

	strValue = QString("%1").arg(maxBarValue);;
	double fontWidth3 = painter->fontMetrics().width(strValue);
	double fontHeight3 = painter->fontMetrics().height();
	textPot = QPointF(barRect.bottomRight().x() - fontWidth3, barRect.bottomRight().y() + fontHeight3 + space2);
	painter->drawText(textPot, strValue);
	//painter->drawText(QRect(barRect.bottomRight().x() - 25, barRect.bottomRight().y() + 5, 25, barRect.bottomRight().y() + 5 + 20), Qt::AlignLeft, "400");





	painter->setFont(QFont("Arial", 16, QFont::Bold));
	if (!wiperOrPrintHead)
		usedValue.isNull() ? strValue = "NA": strValue = QString("+%1").arg(usedValue.toInt());
	else
		usedValue.isNull() ? strValue = "NA" : strValue = QString("%1").arg(usedValue.toInt());
	double fontWidth = painter->fontMetrics().width(strValue);
	double fontHeight = painter->fontMetrics().height();
	textPot = QPointF(barRect.topRight().x() - fontWidth, barRect.topRight().y() - space2);
	painter->drawText(textPot, strValue);
	
	
	painter->setFont(QFont("Arial", 8, QFont::Bold));
	strValue = "Used";
	double fontWidth2 = painter->fontMetrics().width(strValue);
	double fontHeight2 = painter->fontMetrics().height();
	textPot = QPointF(barRect.topRight().x() - fontWidth2, barRect.topRight().y() - fontHeight);
	painter->drawText(textPot, strValue);
	

	painter->restore();
}
void CustomBarSecond::drawInfo(QPainter *painter)
{
	/*title*/
	/*start end*/
	/*Used*/
	QString strValue;
	int space2 = 5;
	QPointF textPot;

	int idY = 15;
	int rectX = 0;


	/*strValue = "Wiper";
	painter->setFont(QFont("Arial", 12, QFont::Bold));
	double typeWidth = painter->fontMetrics().width(strValue);
	double typeHeight = painter->fontMetrics().height();
	textPot = QPointF(0, typeHeight);
	painter->drawText(textPot, strValue);*/

	strValue = "ID";
	painter->setFont(QFont("Arial", 12, QFont::Bold));
	double idWidth = painter->fontMetrics().width(strValue);
	double idHeight = painter->fontMetrics().height();
	textPot = QPointF(0, idY + idHeight);
	if (!wiperOrPrintHead)painter->drawText(textPot, strValue);
	textPot = QPointF(space2 + idWidth, idY + idHeight);
	if (!wiperOrPrintHead)painter->drawText(textPot, printHeadID);

	int rectY = idY + idHeight + space2 + 20;
	int rectWidth = 20;
	int rectHeight = 20;

	int textX = rectWidth + 5;
	int textY = rectY;
	int textWidth = 40;
	int textHeight = 20;

	int text2X = textX +textWidth + 5;
	int text2Y = rectY;
	int text2Width = 40;
	int text2Height = 20;

	



	QVector<int> inputValue;
	inputValue << 200 << 250;
	painter->setFont(QFont("Arial", 10, QFont::Bold));
	strValue = "Start";
	double fontWidth = painter->fontMetrics().width(strValue);
	double fontHeight = painter->fontMetrics().height();
	textPot = QPointF(rectWidth + space2, rectY + fontHeight);
	painter->drawText(textPot, strValue);

	textPot = QPointF(rectWidth + 2*space2 + fontWidth, rectY + fontHeight);
	//painter->drawText(textPot, (startValue.toInt() >= 0) ? QString(" %1").arg(QString::number(startValue.toInt(), 'f', 0)) : "");
	painter->drawText(textPot, startValue.isNull() ? strValue = "NA" : strValue = QString("%1").arg(startValue.toInt()));
	
	strValue = "End";
	textPot = QPointF(rectWidth + space2 , rectY + 2 * fontHeight + space2);
	painter->drawText(textPot, strValue);

	textPot = QPointF(rectWidth + 2 * space2 + fontWidth, rectY + 2 * fontHeight + space2);
	//painter->drawText(textPot, (endValue.toInt() >= 0) ? QString(" %1").arg(QString::number(endValue.toInt(), 'f', 0)) : "");
	painter->drawText(textPot, endValue.isNull() ? strValue = "NA" : strValue = QString("%1").arg(endValue.toInt()));




	painter->setPen(Qt::black);
	/*painter->drawRect(QRect(textX, textY, textWidth, textHeight));
	painter->drawRect(QRect(textX, textY + 25, textWidth, textHeight));*/
	
	//QFont font("Arial", 10, QFont::Bold);
	////font.setPixelSize(textValue.isEmpty() ? 20 : 17);
	//painter->setFont(font);
	//painter->setPen(QColor(0, 0, 0));
	//painter->drawText(QRect(textX, textY, textWidth, textHeight), Qt::AlignLeft, "Start");
	//painter->drawText(QRect(textX, textY + 25, textWidth, textHeight), Qt::AlignLeft, "End");
	

	painter->fillRect(QRect(rectX, rectY, rectWidth, rectHeight), QBrush(QColor(0, 89, 159)));
	painter->fillRect(QRect(rectX, rectY + 25, rectWidth, rectHeight), QBrush(QColor(0, 131, 236)));
	

	/*painter->drawRect(QRect(text2X, text2Y, text2Width, text2Height));
	painter->drawRect(QRect(text2X, text2Y + 25, text2Width, text2Height));*/
	

	//QString("%1%2%3%").arg(strValue).arg(strValue.isEmpty() ? "" : "\n").arg(QString::number(percent, 'f', 2));
	


	/*painter->drawText(QRect(text2X, text2Y, text2Width, text2Height), Qt::AlignLeft, (inputValue.size()>2) ? QString("%1%").arg(QString::number(inputValue.at(0), 'f', 2)) : "");
	painter->setPen(QColor(44, 135, 24));
	painter->drawText(QRect(text2X, text2Y + text2Height, text2Width, text2Height), Qt::AlignLeft, (inputValue.size()>2) ? QString("%1%").arg(QString::number(inputValue.at(1), 'f', 2)) : "");*/
	

	
	



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

	if (itemMap.value("JOB_THUMBNAIL_NAME").toString().isEmpty())
	{
		QPixmap *thumbnailp = new QPixmap();
		bool abcd = thumbnailp->load(":/images/icons/history_nan_pic.png");
		ui->thumbnailLb->setPixmap(thumbnailp->scaled(w, h, Qt::KeepAspectRatio));
	}
	else
		ui->thumbnailLb->setPixmap(QPixmap(itemMap.value("JOB_THUMBNAIL_NAME").toString()).scaled(w, h, Qt::KeepAspectRatio));


	
	
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

	initConsumeTab();
	initSliceTab();
	initSummary();


}
void PrintingHistoryDetail::initSliceTab()
{
	const QString tabStyleSheet
		(

		"QTabBar::tab:first{"
		"color: #868686;"
		"border: 0px;"
		/*"border-top-color: #ffffff;"
		"border-left-color: #ffffff;"
		"border-right-color: #ffffff;"
		"border-bottom-color: #5cbcca;"
		"background-color: rgb(255, 255, 255);"*/
		"min-width: 40ex;"
		"width: 150;"
		"height: 40;"
		"padding: 12px;"
		"font: 8pt \"Arial\";"	
		"image:url(:/images/icons/btn_slice_settings_normal.png);"
		"}"

		"QTabBar::tab:first:hover{"
		"color: #2c3d50;"
		"border: 0px solid;"		
		"image:url(:/images/icons/btn_slice_settings_hover.png);"
		"}"
		"QTabWidget::pane{"
		"border: 0px solid #C2C7CB;"
		"}"

		"QTabBar::tab:first:selected{"
		"color: #2c3d50;"
		"border: 0px solid;"
		/*"border-top-color: #ffffff;"
		"border-left-color: #ffffff;"
		"border-right-color: #ffffff;"
		"border-bottom-color: #5cbcca;"*/
		"image:url(:/images/icons/btn_slice_settings_selected.png);"
		"}"
		"QTabWidget::pane{"
		"border: 0px solid #C2C7CB;"		
		"}"
		/*===========================================================================================================*/
		"QTabBar::tab:last{"
		"color: #868686;"
		"border: 0px;"		
		"min-width: 40ex;"
		"width: 150;"
		"height: 40;"
		"padding: 12px;"
		"font: 8pt \"Arial\";"
		"image:url(:/images/icons/btn_printer_normal.png);"
		"}"

		"QTabBar::tab:last:hover{"
		"color: #2c3d50;"
		"border: 0px solid;"
		"image:url(:/images/icons/btn_printer_hover.png);"
		"}"
		"QTabWidget::pane{"
		"border: 0px solid #C2C7CB;"
		"}"

		"QTabBar::tab:last:selected{"
		"color: #2c3d50;"
		"border: 0px solid;"		
		"image:url(:/images/icons/btn_printer_selected.png);"
		"}"
		"QTabWidget::pane{"
		"border: 0px solid #C2C7CB;"
		"}"



		);

	ui->tabWidget->setStyleSheet(tabStyleSheet);




	QVBoxLayout *vb1 = new QVBoxLayout;
	QVector<QGroupBox *>gbSort;
	QVector<QVBoxLayout *>vbSort;
	gbSort.push_back(new QGroupBox("General"));
	gbSort.push_back(new QGroupBox("Slice Parameter"));
	gbSort.push_back(new QGroupBox("Maintenance"));
	//gbSort.push_back(new QGroupBox("Maintenance"));

	vbSort.push_back(new QVBoxLayout());
	vbSort.push_back(new QVBoxLayout());
	vbSort.push_back(new QVBoxLayout());
	//vbSort.push_back(new QVBoxLayout());
	
	QVector<QWidget *> sliceSettingW;
	QStringList labelList;
	labelList << "Layer Height" << "Add hex pattern" << "Start point offset" << "Draw line as rectangle" << "Second layer on/off" << "Second layer binder percent"
		<< "Second layer shell binder percent" << "Binder inward offset on/off" << "Binder inward offset value" << "Bottom binder lighter thick" << "Bottom binder lighter %"
		<< "Horizontal line width" << "Vertical line width" << "Color profile on/off" << "Color profile path" << "Spitton M width"
		<< "Spitton C width" << "Spitton Y width" << "Second page spitton CMY width" << "Second page spitton Binder width" << "Fan speed rpm"
		<< "IR on/off" << "Mid-job frequency" << "Wiper index" << "Wiper click" << "Post-heating On/off"
		<< "Post-Heating minutes" << "Pump value";
	for (int i = 0; i < sliceSettingList.size(); i++)
	{
		QMetaType::Type tempType = static_cast<QMetaType::Type>(sliceSettingList.at(i).type());
		switch (tempType)
		{
		case QMetaType::Double:
		case QMetaType::Float:
			sliceSettingW.push_back(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toDouble(), 'g', 4)));
			//vb1->addWidget(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toDouble(), 'g', 4)));
			break;
		case QMetaType::QString:
			sliceSettingW.push_back(factoryLabel(labelList.at(i), sliceSettingList.at(i).toString()));
			//vb1->addWidget(factoryLabel(labelList.at(i), sliceSettingList.at(i).toString()));
			break;
		case QMetaType::Int:
			sliceSettingW.push_back(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toInt(), 'g', 0)));
			//vb1->addWidget(factoryLabel(labelList.at(i), QString::number(sliceSettingList.at(i).toInt(), 'g', 0)));
			break;
		case QMetaType::Bool:
			sliceSettingW.push_back(factoryLabel(labelList.at(i), sliceSettingList.at(i).toBool() ? "true" : "false"));
			//vb1->addWidget(factoryLabel(labelList.at(i), sliceSettingList.at(i).toBool() ? "true" : "false"));
		}
	}
	for (int i = 0; i < sliceSettingW.size(); i++)
	{
		if (i < 4)
		{
			vbSort[0]->addWidget(sliceSettingW.at(i));			
		}
		else if (i < 15)
		{
			vbSort[1]->addWidget(sliceSettingW.at(i));

		}
		/*else if (i < 20)
		{
			vbSort[2]->addWidget(sliceSettingW.at(i));
		}*/
		else if (i < 28)
		{
			vbSort[2]->addWidget(sliceSettingW.at(i));
		}
		if (i == sliceSettingW.size() - 1)
		{
			for (int x = 0; x < gbSort.size(); x++)
			{
				gbSort.at(x)->setLayout(vbSort[x]);
			}
			
		}


	}

	for (int x = 0; x < gbSort.size(); x++)
	{
		vb1->addWidget(gbSort[x]);
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
	float cUsed = (cStartLevel>=cEndLevel) ? cStartLevel - cEndLevel: 0;

	float mStartLevel = itemMap.value("MAGENTA_START_LEVEL").toFloat();
	float mEndLevel = itemMap.value("MAGENTA_END_LEVEL").toFloat() == 0 ? mStartLevel : itemMap.value("MAGENTA_END_LEVEL").toFloat();
	float mUsed = (mStartLevel>=mEndLevel) ? mStartLevel - mEndLevel:0;

	float yStartLevel = itemMap.value("YELLOW_START_LEVEL").toFloat();
	float yEndLevel = itemMap.value("YELLOW_END_LEVEL").toFloat() == 0 ? yStartLevel : itemMap.value("YELLOW_END_LEVEL").toFloat();
	float yUsed = (yStartLevel>=yEndLevel) ? yStartLevel - yEndLevel:0;

	float bStartLevel = itemMap.value("BINDER_START_LEVEL").toFloat();
	float bEndLevel = itemMap.value("BINDER_END_LEVEL").toFloat() == 0 ? bStartLevel : itemMap.value("BINDER_END_LEVEL").toFloat();
	float bUsed = (bStartLevel>=bEndLevel) ? yStartLevel - yEndLevel : 0;


	//CustomArc *cyanStartpie = new CustomArc(cStartLevel, QColor(Qt::cyan),90,QString(),QColor(233,233,233),99,100,false,10);
	CustomArc *cyanArc = new CustomArc("C",QVector<float>() << cStartLevel << cEndLevel << cUsed, QVector<QColor>() << QColor(0, 128, 128) << QColor(0, 200, 200) << QColor(0, 255, 255, 100));
	CustomArc *magentaArc = new CustomArc("M", QVector<float>() << mStartLevel << mEndLevel << mUsed, QVector<QColor>() << QColor(128, 0, 128) << QColor(200, 0, 200) << QColor(255, 0, 255, 100));
	CustomArc *yellowArc = new CustomArc("Y", QVector<float>() << yStartLevel << yEndLevel << yUsed, QVector<QColor>() << QColor(128, 128, 0) << QColor(200, 200, 0) << QColor(255, 255, 0, 100));
	CustomArc *binderArc = new CustomArc("B", QVector<float>() << bStartLevel << bEndLevel << bUsed, QVector<QColor>() << QColor(128, 128, 128) << QColor(200, 200, 200) << QColor(222, 222, 222, 100));


	CustomPie *cyanEndpie = new CustomPie(cEndLevel, QColor(Qt::cyan));
	CustomPie *cyanUsed = new CustomPie(cUsed, QColor(Qt::darkCyan), 90 + (cEndLevel * 360 / 100));
	QHBoxLayout *chl = new QHBoxLayout();

	
	chl->addWidget(cyanArc);
	chl->addWidget(magentaArc);
	chl->addWidget(yellowArc);
	chl->addWidget(binderArc);

	//chl->addWidget(cyanEndpie);
	//chl->addWidget(cyanUsed);	

	ui->cyanGB->setStyleSheet(WidgetStyleSheet::groupHistoryDetailBoxStyleSheet());
	ui->cyanGB->setLayout(chl);

	/*CustomPie *magentaStartpie = new CustomPie(mStartLevel, QColor(Qt::magenta));
	CustomPie *magentaEndpie = new CustomPie(mEndLevel, QColor(Qt::magenta));
	CustomPie *magentaUsed = new CustomPie(mUsed, QColor(Qt::darkMagenta), 90 + (mEndLevel * 360 / 100));
	QHBoxLayout *mhL = new QHBoxLayout();
	mhL->addWidget(magentaStartpie);
	mhL->addWidget(magentaEndpie);
	mhL->addWidget(magentaUsed);

	ui->magentaGB->setStyleSheet(WidgetStyleSheet::groupHistoryDetailBoxStyleSheet());
	ui->magentaGB->setLayout(mhL);

	CustomPie *yellowStartpie = new CustomPie(yStartLevel, QColor(Qt::yellow));
	CustomPie *yellowEndpie = new CustomPie(yEndLevel, QColor(Qt::yellow));
	CustomPie *yellowUsed = new CustomPie(yUsed, QColor(Qt::darkYellow), 90 + (yEndLevel * 360 / 100));
	QHBoxLayout *yhl = new QHBoxLayout();
	yhl->addWidget(yellowStartpie);
	yhl->addWidget(yellowEndpie);
	yhl->addWidget(yellowUsed);
	ui->yellowGB->setStyleSheet(WidgetStyleSheet::groupHistoryDetailBoxStyleSheet());
	ui->yellowGB->setLayout(yhl);

	CustomPie *binderStartpie = new CustomPie(bStartLevel, QColor(128, 128, 128));
	CustomPie *binderEndpie = new CustomPie(bEndLevel, QColor(128, 128, 128));
	CustomPie *binderUsed = new CustomPie(bUsed, QColor(Qt::darkGray), 90 + (bEndLevel * 360 / 100));
	QHBoxLayout *bhl = new QHBoxLayout();
	bhl->addWidget(binderStartpie);
	bhl->addWidget(binderEndpie);
	bhl->addWidget(binderUsed);
	ui->binderGB->setStyleSheet(WidgetStyleSheet::groupHistoryDetailBoxStyleSheet());
	ui->binderGB->setLayout(bhl);*/
	//wiper
	qDebug() << itemMap.value("WIPES_AT_END");
	if (itemMap.value("WIPES_AT_END").isNull())
	{
		qDebug() << "isnull";
	}

	QVBoxLayout *bhwiper = new QVBoxLayout();
	QVariant wiperStart = itemMap.value("WIPES_AT_START").toString().isEmpty() ? QVariant() : itemMap.value("WIPES_AT_START");
	QVariant wiperEnd = itemMap.value("WIPES_AT_END").toString().isEmpty() ? QVariant() : itemMap.value("WIPES_AT_END").toInt();
	QVariant wiperUsed;
	if (!(wiperStart.isNull() || wiperEnd.isNull()))
		wiperUsed = wiperEnd.toInt() - wiperStart.toInt();
	else
		wiperUsed = QVariant();
	

	CustomBarSecond *wiperBar = new CustomBarSecond(true, 0, 400, wiperStart, wiperEnd, wiperUsed);

	//bhwiper->addWidget(factoryLabel("Wiper at start", QString::number(wiperStart)));
	//bhwiper->addWidget(factoryLabel("Wiper at End", QString::number(wiperEnd)));
	//bhwiper->addWidget(factoryLabel("Wiper used", QString::number(wiperUsed)));
	bhwiper->addWidget(wiperBar);
	//bhwiper->addWidget(wiperBar);


	const QString thisItemStyle
		(

		"QGroupBox{"
		"font: 20px \"Arial\";"
		"background-color: #ffffff;"
		"border: 1px solid #707070;"
		"border-radius: 1px;"
		"margin-top: 1ex;" /* leave space at the top for the title */
		"}"

		"QGroupBox::title{"
		
		"color:#2c3d50;"
		"subcontrol-origin: padding;"
		"subcontrol-position: top left; "/* position at the top center */
		"padding: 5px 5px;"
		"color: #2c3d50; "
		"}"
		);

	//ui->wiperGB->setStyleSheet(WidgetStyleSheet::groupHistoryDetailBoxStyleSheet());
	ui->wiperGB->setStyleSheet(thisItemStyle);
	ui->wiperGB->setLayout(bhwiper);
	//printhead

	QVBoxLayout *bhPrinthead = new QVBoxLayout();	
	itemMap.value("PRINTHEAD_PAGE_START").toString().isEmpty() ? QVariant() : itemMap.value("PRINTHEAD_PAGE_START");
	QVariant printheadStart = itemMap.value("PRINTHEAD_PAGE_START").toString().isEmpty() ? QVariant() : itemMap.value("PRINTHEAD_PAGE_START");
	QVariant printheadEnd = itemMap.value("PRINTHEAD_PAGE_END").toString().isEmpty() ? QVariant() : itemMap.value("PRINTHEAD_PAGE_END"); 
	QVariant printheadId = itemMap.value("PRINTHEAD_ID");
	QVariant printheadUsed;
	if (!(printheadStart.isNull() || printheadEnd.isNull()))
		printheadUsed = printheadEnd.toInt() - printheadStart.toInt();
	CustomBarSecond *printHeadInfo = new CustomBarSecond(false, 0, 10000, printheadStart, printheadEnd, printheadUsed, printheadId);

	/*bhPrinthead->addWidget(factoryLabel("Printhead ID", itemMap.value("PRINTHEAD_ID").toString()));
	bhPrinthead->addWidget(factoryLabel("Printhead at start", QString::number(printheadStart)));
	bhPrinthead->addWidget(factoryLabel("Printhead at End", QString::number(printheadEnd)));
	bhPrinthead->addWidget(factoryLabel("Printhead used", QString::number(printheadUsed)));*/
	bhPrinthead->addWidget(printHeadInfo);
	ui->printheadGB->setStyleSheet(thisItemStyle);
	ui->printheadGB->setLayout(bhPrinthead);
}
void PrintingHistoryDetail::initSummary()
{


	QFileInfo projectInfo(itemMap.value("PROJECT_NAME").toString());
	QString projectNameSt = projectInfo.fileName();
	QLabel *projectNameLb = new QLabel("Project Name", this);
	QLabel *projectNameFILb = new QLabel(projectNameSt.isEmpty() ? "NA" : projectNameSt, this);
	projectNameLb->setStyleSheet(WidgetStyleSheet::settingLabelStyleSheet());

	QString started_atSt = itemMap.value("START_TIME").toString();
	QLabel *startTimeLb = new QLabel("Started at", this);
	QLabel *startTimeValueLb = new QLabel(started_atSt.isEmpty() ? "NA" : started_atSt, this);
	startTimeLb->setStyleSheet(WidgetStyleSheet::settingLabelStyleSheet());

	QString finishAtSt = itemMap.value("BUILD_COMPLETED_ON").toString();
	QLabel *endTimeLb = new QLabel("Finished Time",this);
	QLabel *endTimeValueLb = new QLabel(finishAtSt.isEmpty() ? "NA" : finishAtSt, this);
	endTimeLb->setStyleSheet(WidgetStyleSheet::settingLabelStyleSheet());

	QDateTime endTime = QDateTime::fromString(itemMap.value("BUILD_COMPLETED_ON").toString(), Qt::ISODate);
	QDateTime startTime = QDateTime::fromString(itemMap.value("START_TIME").toString(), Qt::ISODate);
	int elapsedSeconds = startTime.secsTo(endTime);
	QString elapsedTimeSt = QDateTime::fromMSecsSinceEpoch(elapsedSeconds * 1000).toUTC().toString("hh %1 : mm %2 : ss %3").arg("hr.").arg("min.").arg("sec.");

	QLabel *elapsedTimeLb = new QLabel("Printing Time", this);
	QLabel *elapsedTimeValueLb = new QLabel(elapsedTimeSt.isEmpty() ? "NA" : elapsedTimeSt, this);
	elapsedTimeLb->setStyleSheet(WidgetStyleSheet::settingLabelStyleSheet());


	

	QVBoxLayout *vlayout = new QVBoxLayout();
	vlayout->addWidget(projectNameLb);
	vlayout->addWidget(projectNameFILb);

	vlayout->addWidget(startTimeLb);
	vlayout->addWidget(startTimeValueLb);

	vlayout->addWidget(endTimeLb);
	vlayout->addWidget(endTimeValueLb);

	vlayout->addWidget(elapsedTimeLb);
	vlayout->addWidget(elapsedTimeValueLb);
	
	QSpacerItem *sitm = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	vlayout->addSpacerItem(sitm);

	



	ui->frame_3->setLayout(vlayout);

}


void setElidedText(QLabel* label, const QString &text){
	QFontMetrics metrix(label->font());
	int width = label->width() - 5;
	QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
	label->setText(clippedText);
}
QWidget *PrintingHistoryDetail::factoryLabel(QString _decription, QString _value)
{
	QFrame *frr = new QFrame;
	QHBoxLayout * hbl = new QHBoxLayout();
	QLabel *descriptionLB = new QLabel(_decription);
	QLabel *valueLB = new QLabel(this);
	valueLB->setToolTip(_value);
	setElidedText(valueLB, _value);
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
{
}
void InformationWidget::paintEvent(QPaintEvent *)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);



}

