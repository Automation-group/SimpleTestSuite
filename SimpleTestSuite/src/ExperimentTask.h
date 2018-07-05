/*
 * ExperimentTask.h
 *
 *  Created on: 19.03.2010
 *      Author: lexa
 */

#ifndef EXPERIMENTTASK_H_
#define EXPERIMENTTASK_H_

#include <QString>
#include <QObject>

/*!
 * Экспериментальная задача, например подгрузка образца
 */
class ExperimentTask : QObject{
	Q_OBJECT
public:
	ExperimentTask();
	/*!
	 * Имя задачи
	 * @return имя задачи
	 */
	virtual const QString& getName() = 0;
	virtual ~ExperimentTask() = 0;

	/*!
	 * Подготовка к использованию
	 * @param gui
	 * @param testmachine
	 */
	virtual void use(void* gui, void* testmachine) = 0;
private Q_SLOTS:
Q_SIGNALS:
	void done();
};

#endif /* EXPERIMENTTASK_H_ */
