/*
 * QcrTime.cpp
 *
 *  Created on: 20.01.2009
 *      Author: lexa
 */

#include "QcrTime.h"

QcrTime::QcrTime()
	: m_startTime(QDateTime::currentDateTime())
{

}

double timeToDouble(QDateTime dt)
{
	QTime time = dt.time();
	QDate date = dt.date();

	double result = time.msec() + time.second() * 1000.0;
	result += time.minute() * 60.0 * 1000.0;
	result += time.hour() * 60.0 * 60.0 * 1000.0;

	double hours = date.day() * 24.0;
	result += hours * 60.0 * 60.0 * 1000.0;
	return result;
}

double QcrTime::elapsed() const
{
	return timeToDouble(QDateTime::currentDateTime()) - timeToDouble(m_startTime);
}

void QcrTime::restart()
{
	m_startTime = QDateTime::currentDateTime();
}

void QcrTime::start()
{
	restart();
}


QcrTime::~QcrTime()
{
	// TODO Auto-generated destructor stub
}
