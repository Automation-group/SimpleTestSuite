/*
 * QcrTime.h
 *
 *  Created on: 20.01.2009
 *      Author: lexa
 */

#ifndef QCRTIME_H_
#define QCRTIME_H_

#include <QDateTime>

class QcrTime
{
public:
	QcrTime();
	virtual ~QcrTime();

	double elapsed() const;
	void restart();
	void start();
private:
	QDateTime m_startTime;
};

#endif /* QCRTIME_H_ */
