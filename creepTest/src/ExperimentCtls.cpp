/*
 * ExperimentCtls.cpp
 *
 *  Created on: 25.11.2008
 *      Author: lexa
 */

#include "ExperimentCtls.h"
#include <qextserialport.h>
#include "lfUtils.h"
#include <QRegExp>
#include <QtDebug>
#include <QStringList>
#include <QlfSensors.h>
#include <QlfPIDControllers.h>
#include <algorithm>
#include <boost/bind.hpp>

ExperimentCtls::ExperimentCtls()
{

}

ExperimentCtls::~ExperimentCtls()
{
	using namespace boost;
	std::for_each(m_deviceMap.begin(), m_deviceMap.end(),
			bind(&QIODevice::close, _1)
			);
}

void ExperimentCtls::addDevice(QIODevicePtr_t device, const QString& name)
{
	m_deviceMap.insert(name, device);
}

ExperimentCtls::QIODevicePtr_t ExperimentCtls::getDevice(const QString& name)
{
	return m_deviceMap[name];
}

void ExperimentCtls::addSensor(TemperatureSensorPtr_t sensor, const QString& name)
{
	m_sensorMap.insert(name, sensor);
}

ExperimentCtls::TemperatureSensorPtr_t ExperimentCtls::getSensor(const QString& name)
{
	return m_sensorMap[name];
}

void ExperimentCtls::addController(PIDControllerPtr_t controller, const QString& name)
{
	m_controllerMap.insert(name, controller);
}

ExperimentCtls::PIDControllerPtr_t ExperimentCtls::getController(const QString& name)
{
	return m_controllerMap[name];
}

ExperimentCtls::TemperatureSensorPtr_t ExperimentCtls::createSensor(const QString& confString)
{
	QString type;
	{
		QRegExp regexp("type:(\\S*)");
		if (regexp.indexIn(confString) != -1)
			type = regexp.cap(1);
	}

	QIODevicePtr_t sPort;
	{
		QRegExp regexp("serialport:(\\S*)");
		if (regexp.indexIn(confString) != -1)
			sPort = getDevice(regexp.cap(1));
	}

	if (type == "Metakon")
	{
		int dev = 0;
		{
			QRegExp regexp("dev:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				dev = regexp.cap(1).toInt();
		}

		int cha = 0;
		{
			QRegExp regexp("cha:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				cha = regexp.cap(1).toInt();
		}

		int reg = 0;
		{
			QRegExp regexp("reg:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				reg = regexp.cap(1).toInt();
		}

		return TemperatureSensorPtr_t(new RNetSensor(sPort.get(), dev, cha, reg));
	}

	if (type == "EuroTherm")
	{
		int dev = 0;
		{
			QRegExp regexp("dev:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				dev = regexp.cap(1).toInt();
		}

		int reg = 0;
		{
			QRegExp regexp("reg:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				reg = regexp.cap(1).toInt();
		}

		return TemperatureSensorPtr_t(new EuroThermSensor(sPort.get(), dev, reg));
	}


	if (type == "TestSensor")
	{
		return TemperatureSensorPtr_t(new TestSensor);
	}

	return TemperatureSensorPtr_t();
}

ExperimentCtls::PIDControllerPtr_t ExperimentCtls::createController(const QString& confString)
{
	QString type;
	{
		QRegExp regexp("type:(\\S*)");
		if (regexp.indexIn(confString) != -1)
			type = regexp.cap(1);
	}

	QIODevicePtr_t sPort;
	{
		QRegExp regexp("serialport:(\\S*)");
		if (regexp.indexIn(confString) != -1)
			sPort = getDevice(regexp.cap(1));
	}

	if (type == "Metakon")
	{
		int dev = 0;
		{
			QRegExp regexp("dev:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				dev = regexp.cap(1).toInt();
		}

		int cha = 0;
		{
			QRegExp regexp("cha:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				cha = regexp.cap(1).toInt();
		}

		return PIDControllerPtr_t(new QlfMetakonPIDController(sPort.get(), dev, cha));
	}

	if (type == "EuroTherm")
	{
		int dev = 0;
		{
			QRegExp regexp("dev:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				dev = regexp.cap(1).toInt();
		}

		return PIDControllerPtr_t(new QlfEuroThermPIDController(sPort.get(), dev));
	}

	if (type == "TestController")
	{
		return PIDControllerPtr_t(new QlfTestPIDController());
	}

	return PIDControllerPtr_t();
}

void ExperimentCtls::loadSettings(QSettings& settings)
{
	{
		settings.beginGroup("SerialPorts");
		QStringList keys = settings.allKeys();
		for(QStringList::const_iterator i = keys.begin(); i != keys.end(); ++i)
		{
			QString value = settings.value(*i).toString();
			QRegExp regexp("name:(\\S*)");

			if (regexp.indexIn(value) == -1) continue;
			QString portName = regexp.cap(1);
			PortSettings portSettings = lfUtils::createPortSettings(value);
			QIODevicePtr_t sPort (new QextSerialPort(portName, portSettings));
			addDevice(sPort, *i);
			qDebug() << "port name:" << portName;
		}
		settings.endGroup();

		using namespace boost;
		std::for_each(m_deviceMap.begin(), m_deviceMap.end(),
				bind(&QIODevice::open, _1, QIODevice::ReadWrite | QIODevice::Unbuffered)
				);
	}

	{
		settings.beginGroup("TemperatureSensors");
		QStringList keys = settings.allKeys();
		for(QStringList::const_iterator i = keys.begin(); i != keys.end(); ++i)
		{
			QString value = settings.value(*i).toString();
			addSensor(createSensor(value), *i);
		}
		settings.endGroup();
	}

	{
		settings.beginGroup("TemperatureControllers");
		QStringList keys = settings.allKeys();
		for(QStringList::const_iterator i = keys.begin(); i != keys.end(); ++i)
		{
			QString value = settings.value(*i).toString();
			addController(createController(value), *i);
		}
		settings.endGroup();
	}

	{
		//settings.beginGroup("General");

		m_patternSensor = getSensor(settings.value("PatternTemperatureSensor").toString());
		m_primaryController = getController(settings.value("PrimaryController").toString());
		//settings.endGroup();
	}
}

ExperimentCtls* ExperimentCtls::getInstance()
{
	static ExperimentCtls expCtls;
	return &expCtls;
}
