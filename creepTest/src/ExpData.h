/*
 * ExpData.h
 *
 *  Created on: 20.11.2008
 *      Author: lexa
 */

#ifndef EXPDATA_H_
#define EXPDATA_H_

#include <vector>
#include <qwt_data.h>
#include <boost/shared_ptr.hpp>
#include <QPointF>

class ExpData : public QwtData
{
public:
	typedef std::vector<QPointF> VectorData_t;

	ExpData()
		: m_data(new VectorData_t)
	{

	}

	const VectorData_t& data() const
	{
		return *m_data;
	}

	VectorData_t& data()
	{
		return *m_data;
	}

	QwtData* copy() const
	{
		return new ExpData(*this);
	}

	size_t size() const
	{
		return m_data->size();
	}

	double 	x (size_t i) const
	{
		return (*m_data)[i].x();
	}

	double 	y (size_t i) const
	{
		return (*m_data)[i].y();
	}

private:
	boost::shared_ptr<VectorData_t> m_data;
};

#endif /* EXPDATA_H_ */
