/*
 * ExperimentCtls.h
 *
 *  Created on: 25.11.2008
 *      Author: lexa
 */

#ifndef EXPERIMENTCTLS_H_
#define EXPERIMENTCTLS_H_

#include <QIODevice>
#include <QMap>
#include <boost/shared_ptr.hpp>
#include <QSettings>
#include <TemperatureSensor.h>
#include <PIDController.h>

class ExperimentCtls
{
public:
	typedef boost::shared_ptr<QIODevice> QIODevicePtr_t;
	typedef boost::shared_ptr<TemperatureSensor> TemperatureSensorPtr_t;
	typedef boost::shared_ptr<PIDController> PIDControllerPtr_t;

	static ExperimentCtls* getInstance();

	void addDevice(QIODevicePtr_t device, const QString& name);
	QIODevicePtr_t getDevice(const QString& name);

	void addSensor(TemperatureSensorPtr_t sensor, const QString& name);
	TemperatureSensorPtr_t getSensor(const QString& name);

	void addController(PIDControllerPtr_t controller, const QString& name);
	PIDControllerPtr_t getController(const QString& name);

	/*!
	 * Сенсор регистрирующий температуру образца
	 */
	TemperatureSensorPtr_t getPatternSensor() const { return m_patternSensor; }

	PIDControllerPtr_t getPrimaryController() const { return m_primaryController; }

	void loadSettings(QSettings& settings);
private:
	ExperimentCtls(const ExperimentCtls&);

	ExperimentCtls();
	~ExperimentCtls();

	TemperatureSensorPtr_t createSensor(const QString& confString);
	PIDControllerPtr_t createController(const QString& confString);
private:
	typedef QMap<QString, QIODevicePtr_t> DeviceMap_t;
	typedef QMap<QString, TemperatureSensorPtr_t> SensorMap_t;
	typedef QMap<QString, PIDControllerPtr_t> ControllerMap_t;

	DeviceMap_t m_deviceMap;
	SensorMap_t m_sensorMap;
	TemperatureSensorPtr_t m_patternSensor;
	ControllerMap_t m_controllerMap;
	PIDControllerPtr_t m_primaryController;
};

#endif /* EXPERIMENTCTLS_H_ */
