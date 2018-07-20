#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include <QSettings>
#include <QTime>
#include <QFile>

#include <qextserialport.h>
#include <QlfTOTestMachine.h>
#include <boost/shared_ptr.hpp>
#include "ExpData.h"
#include "QcrTime.h"

class Ui_MainWindow;
class QwtPlotCurve;

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
private:
	QlfTOTestMachine* testMachine;

	QTimer compensationTimer;
	QTimer temperatureTimer;

	QcrTime expTime;

	double loadCellRange;

	/*!
	 * Сконфигурировать график
	 */
	void configurePlot();

	/*!
	 * Инициализация
	 */
	void init();

	/*!
	 * Открыть файлы данных
	 */
	void openFiles();

	/*!
	 * Закрыть файлы данных
	 */
	void closeFiles();

protected:
	void closeEvent(QCloseEvent* event);
private slots:
	void onCompensationTimer();
	void onTemperatureTimer();
	void on_actionSetZero_triggered();
	void on_actionStop_triggered();

	void on_actionLoad_triggered();
	void on_actionUnload_triggered();
	void on_actionLead_triggered();
	void on_actionCompensationBeforeLoad_triggered();
	void on_actionCompensationMain_triggered();
	void on_actionCompensationAfterUnload_triggered();

	/*!
	 * Нажата кнопка TEST
	 */
	void onTestButton_pressed();

private:

	enum ExpState
	{
		EsFree, // ничего не делается
		EsLeading, // нагружение
		EsCompensationBeforeLoad, // пргорев
		EsLoading, // нагружение
		EsCompensationMain,
		EsUnloading,
		EsCompensationAfterUnload
		//EsTemperatureCompensation // коменсация теплового расширения
	};

	ExpState m_expState;
	boost::shared_ptr<Ui_MainWindow> ui;
	ExpData m_data;
	ExpData m_temperatureData;
	QwtPlotCurve* m_dataCurve;
	QFile m_dataFile;

	const double m_loadSpeed; // мм/мин
	const double m_leadSpeed;
	const double m_mainCompensationSpeed;
	const double m_CompensationSpeed;
};

#endif /*MAINWINDOW_H_*/

