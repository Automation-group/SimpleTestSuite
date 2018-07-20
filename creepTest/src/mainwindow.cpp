#include <QtGui>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"
#include <iostream>
#include <math.h>

#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_map.h>

#include "ExperimentCtls.h"
#include "ExperimentCfg.h"

#include <QMutex>
#include <QWaitCondition>


#include "../ui_mainWindow.h"

MainWindow::MainWindow() :
	QMainWindow(0, 0), testMachine(0),
			m_expState(EsFree), ui(new Ui_MainWindow),
			m_loadSpeed(1.0), m_leadSpeed(1.0),
			m_mainCompensationSpeed(0.005), m_CompensationSpeed(0.010)
{
	ui->setupUi(this);
	init();
	openFiles();
	temperatureTimer.start(1000);
	compensationTimer.start(500);
}

void MainWindow::init()
{
	connect(&compensationTimer, SIGNAL(timeout()), this, SLOT(onCompensationTimer()));
	connect(&temperatureTimer, SIGNAL(timeout()), this, SLOT(onTemperatureTimer()));

	ExperimentCtls::QIODevicePtr_t tmSerialPort = ExperimentCtls::getInstance()->getDevice("TestMachineSerialPort");
	testMachine = new QlfTOTestMachine(tmSerialPort.get());
	connect(testMachine, SIGNAL(pressTestButton()), this, SLOT(onTestButton_pressed()));

	int ident = testMachine -> ReadLoadCellIdent();
	loadCellRange = testMachine -> getLoadCellRange(ident);
	qDebug() << "Machine Ident" << testMachine->ReadMachineIdent();

	configurePlot();
}

MainWindow::~MainWindow()
{
	closeFiles();
	if (testMachine)
		delete testMachine;
}

void sleep_ms(int delay)
{
	QWaitCondition cond;
	QMutex mutex;
	mutex.lock();
	cond.wait(&mutex, delay);
	mutex.unlock();
}

void MainWindow::configurePlot()
{
	{
		QwtSymbol symbol(QwtSymbol::Ellipse, QBrush(Qt::blue), QPen(
				Qt::darkBlue), QSize(7, 7));
		m_dataCurve = new QwtPlotCurve(tr("ε(t)"));
		m_dataCurve -> attach(ui->qwtpData);
		m_dataCurve -> setSymbol(symbol);
		m_dataCurve -> setData(m_data);

		m_dataCurve->setYAxis (QwtPlot::yLeft);
		m_dataCurve->setXAxis (QwtPlot::xBottom);
		/*
		for (int i = 0; i<1000;i++)
			m_data.data().push_back(QPointF(i,i/2.0));
			*/
	}

	ui->qwtpData -> setAxisTitle(QwtPlot::yLeft, tr("ε, %"));
	ui->qwtpData -> setAxisTitle(QwtPlot::xBottom, tr("t, c"));
	ui->qwtpData -> setAutoReplot(true);
	//ui->qwtpData -> setAxisScale (QwtPlot::yLeft, 0, 120.0, 20.0);

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid -> enableXMin(true);
	grid -> enableYMin(true);
	grid -> setMajPen(QPen(Qt::black, 0, Qt::DotLine));
	grid -> setMinPen(QPen(Qt::gray, 0, Qt::DotLine));
	grid -> attach(ui->qwtpData);

	bool temperatureExperiment = true;

	if (temperatureExperiment)
	{

		QwtPlotMarker* marker = new QwtPlotMarker();
		int expTemperature = ExperimentCfg::getInstance()->getProperty("ExpTemperature").toInt();
		marker -> setYValue(expTemperature);
		marker -> setLineStyle(QwtPlotMarker::HLine);
		marker -> setLinePen(QPen(Qt::darkRed, 2, Qt::SolidLine));
		marker -> setYAxis(QwtPlot::yRight);
		marker -> attach(ui->qwtpData);

		QwtSymbol symbol(QwtSymbol::Ellipse, QBrush(Qt::red), QPen(
				Qt::red), QSize(7, 7));

		QwtPlotCurve* m_tempCurve = new QwtPlotCurve(tr("T(t)"));

		m_tempCurve->setSymbol(symbol);
		m_tempCurve->setYAxis (QwtPlot::yRight);
		m_tempCurve->setXAxis (QwtPlot::xBottom);
		m_tempCurve->setPen(QPen(Qt::red));
		m_tempCurve->setData(m_temperatureData);
		m_tempCurve->attach(ui->qwtpData);

		ui->qwtpData->enableAxis(QwtPlot::yRight, true);
		ui->qwtpData-> setAxisTitle(QwtPlot::yRight, tr("T, °C"));
	}

	/*
	if (temperatureExperiment)
	{
		ui->qwtpData->enableAxis(QwtPlot::xTop, true);
		ui->qwtpData-> setAxisTitle(QwtPlot::xTop, tr("t, с"));
	}
	*/

	{
		QwtScaleMap* scaleMap = new QwtScaleMap;
		scaleMap->setPaintXInterval(0.0, 3.0);
		scaleMap->setScaleInterval(0.0, 1.0);
	}

	/*
	if (temperatureExperiment)
	{
		ui->qwtpData->enableAxis(QwtPlot::xBottom, false);
		ui->qwtpData->enableAxis(QwtPlot::yLeft, false);
	}
	*/
}

void MainWindow::openFiles()
{
	qDebug() << "report file" << ExperimentCfg::getInstance()->getReportFileName();
	m_dataFile.setFileName(ExperimentCfg::getInstance()->getReportFileName());
	m_dataFile.open(QIODevice::WriteOnly);
}

void MainWindow::closeFiles()
{
	m_dataFile.close();
}

void MainWindow::on_actionSetZero_triggered()
{
	testMachine -> ZeroXhead();
	sleep_ms(500);
	testMachine->ZeroLoadCell();
	sleep_ms(500);
	expTime.restart();
}

void MainWindow::on_actionLoad_triggered()
{
	m_expState = EsLoading;
	testMachine -> SetSowtwareXheadVelocity(m_loadSpeed);
	sleep_ms(500);
	testMachine -> MoveXheadForward();
}

void MainWindow::on_actionUnload_triggered()
{
	m_expState = EsUnloading;

	testMachine -> SetSowtwareXheadVelocity(m_loadSpeed);
	sleep_ms(500);
	testMachine -> ReverseXhead();
}

void MainWindow::on_actionLead_triggered()
{
	m_expState = EsLeading;
	testMachine -> SetSowtwareXheadVelocity(m_leadSpeed);
	sleep_ms(500);
	testMachine -> MoveXheadForward();
}

void MainWindow::on_actionCompensationBeforeLoad_triggered()
{
	m_expState = EsCompensationBeforeLoad;
	testMachine -> SetSowtwareXheadVelocity(m_CompensationSpeed);
	sleep_ms(500);
}

void MainWindow::on_actionCompensationMain_triggered()
{
	m_dataFile.write(QByteArray("-----Compensation-----"));
	m_expState = EsCompensationMain;
	testMachine -> SetSowtwareXheadVelocity(m_mainCompensationSpeed);
	sleep_ms(500);
}

void MainWindow::on_actionCompensationAfterUnload_triggered()
{
	m_expState = EsCompensationAfterUnload;
	testMachine -> SetSowtwareXheadVelocity(m_CompensationSpeed);
	sleep_ms(500);
}


void MainWindow::on_actionStop_triggered()
{
	testMachine->HaltXhead();

	compensationTimer.stop();
	temperatureTimer.stop();
	closeFiles();
	m_expState = EsFree;
}

void MainWindow::onTestButton_pressed()
{
	/*
	if (m_expState == EsFree)
		ui->actionRunExperiment->triggered();
		*/
}

void MainWindow::onTemperatureTimer()
{
	//ExperimentCtls::PIDControllerPtr_t controller
	ExperimentCtls::TemperatureSensorPtr_t sensor
		= ExperimentCtls::getInstance()->getPatternSensor();

	double t = expTime.elapsed()/1000.0;
	double T = 0.0;
	if (sensor)
	{
		double T = sensor->getTemperature();
		ui->leTemperature->setText(QString::number(T));
	}
	else
		ui->leTemperature->setText(tr("Ошибка"));

	int lc, xh;
	testMachine->ReadLoadCellAndXhead(lc, xh);
	double force = loadCellRange * (double) lc / 30000.0;
	double stress = force / (ExperimentCfg::getInstance()->pattern().width
			* ExperimentCfg::getInstance()->pattern().thickness);
	double epsilon = (double) xh / ( ExperimentCfg::getInstance()->pattern().lenght * 1000.0) * 100.0;

	ui->leForce->setText(QString::number(force));
	ui->leStress->setText(QString::number(stress));
	ui->leDeformation->setText(QString::number(xh));
	ui->leDeformationInPercent->setText(QString::number(epsilon));

	static int counter = 59;
	counter ++;
	if (counter == 60)
	{
		m_temperatureData.data().push_back(QPointF(t, T));
		m_data.data().push_back(QPointF(t, epsilon));
		counter = 0;

		{
			QTextStream stream(&m_dataFile);

			// 1.Data.csv — данные эксперимента: t с, ε %, σ МПа, T.
			stream << QString::number(t).replace(".", ",") << ";"
					<< QString::number(epsilon).replace(".", ",") << ";"
					<< QString::number(stress).replace(".", ",") << ";"
					<< QString::number(T).replace(".", ",") << "\n";
			//stream.flush();
		}

	}

}

void MainWindow::onCompensationTimer()
{
	int lc = testMachine->ReadLoadCellValue();
	double force = loadCellRange * (double) lc / 30000.0;

	const double leadForce = 10.0;
	const double inertZone = 5.0;

	static const double loadForce =
		ExperimentCfg::getInstance()->pattern().workStress *
		ExperimentCfg::getInstance()->pattern().width *
		ExperimentCfg::getInstance()->pattern().thickness;

	switch (m_expState)
	{
	case EsLeading:
	{
		if (force > leadForce)
		{
			testMachine->HaltXhead();
			m_expState = EsFree;
		}
		break;
	}

	case EsLoading:
	{
		if (force > loadForce)
		{
			testMachine->HaltXhead();
			m_expState = EsFree;

			//sleep_ms(500);
			//testMachine -> ZeroXhead();
			//sleep_ms(500);
		}
		break;
	}

	case EsUnloading:
	{
		if (force < leadForce)
		{
			testMachine->HaltXhead();
			m_expState = EsFree;
		}
		break;
	}

	case EsCompensationMain:
	{
		if (force < (loadForce - inertZone))
		{
			// подгрузить
			testMachine -> MoveXheadForward();
		}

		if (force > loadForce)
		{
			// остановить
			testMachine->HaltXhead();
		}
	}

	case EsCompensationBeforeLoad:
	{
		if (force < leadForce)
		{
			// нагрузить
			testMachine -> MoveXheadForward();
		}

		if (force > (leadForce + inertZone))
		{
			// остановить
			testMachine->HaltXhead();
		}
	}

	case EsCompensationAfterUnload:
	{
		if (force > leadForce)
		{
			// разгрузить
			testMachine -> ReverseXhead();
		}

		if (force < (leadForce - inertZone))
		{
			// остановить
			testMachine->HaltXhead();
		}
	}
	}

}


void MainWindow::closeEvent(QCloseEvent* event)
{
	if (m_expState == EsFree)
		event -> accept();
	else
		event -> ignore();

}
