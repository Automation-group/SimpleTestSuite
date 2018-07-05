#include <QtGui>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"
#include <iostream>
#include <math.h>
#include <QStringList>
#include <QBoxLayout>
#include <QToolButton>

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
  m_expState(EsFree), m_temperatureExperiment(false), ui(new Ui_MainWindow) {
  ui->setupUi(this);
  init();
  openFiles();
  m_infoTimer.start(1000);
}

void MainWindow::init() {

  connect(&expTimer, SIGNAL(timeout()), this, SLOT(onExpTimer()));
  connect(&temperatureTimer, SIGNAL(timeout()), this, SLOT(onTemperatureTimer()));
  connect(&m_infoTimer, SIGNAL(timeout()), this, SLOT(showInfo()));
  
  ExperimentCtls::QIODevicePtr_t tmSerialPort = ExperimentCtls::getInstance()->getDevice("TestMachineSerialPort");
  testMachine = new QlfTOTestMachine(tmSerialPort.get());
  connect(testMachine, SIGNAL(pressTestButton()), this, SLOT(onTestButton_pressed()));
  
  int ident = testMachine -> ReadLoadCellIdent();
  loadCellRange = testMachine -> getLoadCellRange(ident);
  qDebug() << "Machine Ident" << testMachine->ReadMachineIdent();
  
  configurePlot();
  
  {
    bool temperatureExperiment = ExperimentCfg::getInstance()->getProperty("TemperatureExperiment").toBool();
    bool warmUpWithoutPattern = ExperimentCfg::getInstance()->getProperty("WarmUpWithoutPattern").toBool();
    
    ui->actionRunHeating->setEnabled(temperatureExperiment);
    ui->actionRunTemperatureCompensation->setEnabled(temperatureExperiment & !warmUpWithoutPattern);
    ui->actionLeadPuanson->setEnabled(temperatureExperiment & warmUpWithoutPattern);
    
    ui->actionRunHeating->setVisible(temperatureExperiment);
    ui->actionRunTemperatureCompensation->setVisible(temperatureExperiment & !warmUpWithoutPattern);
    ui->actionLeadPuanson->setVisible(temperatureExperiment & warmUpWithoutPattern);
  }

  if(ExperimentCfg::getInstance()->getProperty("TrueStrainRate").toBool()) {

    loadLeapAndTimes(
		     ExperimentCfg::getInstance()->getProperty("Leaps").toString(),
		     ExperimentCfg::getInstance()->getProperty("LeapTimes").toString()
		     );
    m_currentTrueSpeed = ExperimentCfg::getInstance()->getProperty("LoadingSpeed_1").toDouble();

  } else {
    ui->fLeaps->setVisible(false);
    m_currentTrueSpeed = 0.0;
  }

}

MainWindow::~MainWindow() {
  closeFiles();
  if (testMachine)
    delete testMachine;
}

void MainWindow::loadLeapAndTimes(const QString& leaps, const QString& times) {
  //  ui.fLeaps->layout()
  QStringList lps = leaps.split(';');
  QStringList tms = times.split(';');
  for(QStringList::iterator li = lps.begin(); li != lps.end(); ++li) {
    if(li->isEmpty()) continue;
    QGroupBox* gbox = new QGroupBox();
    gbox->setTitle(tr("Скачок х") + *li);
    ui->fLeaps->layout()->addWidget(gbox);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    gbox->setLayout(layout);
    for(QStringList::iterator ti = tms.begin(); ti != tms.end(); ++ti) {
      if(ti->isEmpty()) continue;
      QToolButton* tb = new QToolButton();
      tb->setText(*ti + tr(" с"));
      tb->setToolTip(tr("Длительность скачка в секудах"));
      tb->setProperty("leap", *li);
      tb->setProperty("leapTime", *ti);
      connect(tb, SIGNAL(clicked()), this, SLOT(onLeap()));
      layout->addWidget(tb);
    }
  }
  ui->fLeaps->layout()->addWidget(ui->gbLeapControl);
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::BottomToTop);
  ui->gbLeapControl->setLayout(layout);
  layout->addStretch();
}

void MainWindow::onLeap() {
  
  if(sender()->property("leap").isNull() || sender()->property("leapTime").isNull()) return;

  if(!leaps.empty())
    leaps.back().bCancel->setEnabled(false);

  LeapRecord lrec;
  lrec.leap = sender()->property("leap").toDouble();
  lrec.leapTime = sender()->property("leapTime").toDouble();
  lrec.isUsing = false;
  lrec.prevTrueSpeed = m_currentTrueSpeed;
  lrec.bCancel = LeapRecord::bCancel_p_t(new QToolButton());
  lrec.bCancel->setText( tr("Скачок ") + QString::number(lrec.leap) + 
			 " (" + QString::number(lrec.leapTime) +tr(" сек)"));
  ui->gbLeapControl->layout()->addWidget(lrec.bCancel.get());
  connect(lrec.bCancel.get(), SIGNAL(clicked()), this, SLOT(onCancelLeap()));
  leaps.push_back(lrec);
  qDebug() << "Leap" << lrec.leap << lrec.leapTime;
}

void MainWindow::onCancelLeap() {
  if(leaps.empty()) return;
  
  if(leaps.back().bCancel.get() != sender()) return;

  m_currentTrueSpeed = leaps.back().prevTrueSpeed;
  leaps.pop_back();

  if(leaps.empty()) return;
  leaps.back().bCancel->setEnabled(true);
}

void sleep_ms(int delay) {
  QWaitCondition cond;
  QMutex mutex;
  mutex.lock();
  cond.wait(&mutex, delay);
  mutex.unlock();
}

void MainWindow::configurePlot() {
  {
    QwtSymbol symbol(QwtSymbol::Ellipse, QBrush(Qt::blue), QPen(
								Qt::darkBlue), QSize(7, 7));
    m_dataCurve = new QwtPlotCurve(tr("σ(ε)"));
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

	ui->qwtpData -> setAxisTitle(QwtPlot::yLeft, tr("σ, МПа"));
	ui->qwtpData -> setAxisTitle(QwtPlot::xBottom, tr("ε, %"));
	ui->qwtpData -> setAutoReplot(true);
	//ui->qwtpData -> setAxisScale (QwtPlot::yLeft, 0, 120.0, 20.0);

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid -> enableXMin(true);
	grid -> enableYMin(true);
	grid -> setMajPen(QPen(Qt::black, 0, Qt::DotLine));
	grid -> setMinPen(QPen(Qt::gray, 0, Qt::DotLine));
	grid -> attach(ui->qwtpData);

	bool temperatureExperiment = ExperimentCfg::getInstance()->getProperty("TemperatureExperiment").toBool();

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
		m_tempCurve->setXAxis (QwtPlot::xTop);
		m_tempCurve->setPen(QPen(Qt::red));
		m_tempCurve->setData(m_heatingData);
		m_tempCurve->attach(ui->qwtpData);

		m_tempCurve = new QwtPlotCurve(tr("T(t)"));		
		m_tempCurve->setSymbol(symbol);
		m_tempCurve->setYAxis (QwtPlot::yRight);
		m_tempCurve->setXAxis (QwtPlot::xTop);
		m_tempCurve->setPen(QPen(Qt::red));
		m_tempCurve->setData(m_temperatureData);
		m_tempCurve->attach(ui->qwtpData);

		ui->qwtpData->enableAxis(QwtPlot::yRight, true);
		ui->qwtpData-> setAxisTitle(QwtPlot::yRight, tr("T, °C"));
	}

	if (temperatureExperiment)
	{
		ui->qwtpData->enableAxis(QwtPlot::xTop, true);
		ui->qwtpData-> setAxisTitle(QwtPlot::xTop, tr("t, с"));
	}

	{
		QwtScaleMap* scaleMap = new QwtScaleMap;
		scaleMap->setPaintXInterval(0.0, 3.0);
		scaleMap->setScaleInterval(0.0, 1.0);
	}

	if (temperatureExperiment)
	{
		ui->qwtpData->enableAxis(QwtPlot::xBottom, false);
		ui->qwtpData->enableAxis(QwtPlot::yLeft, false);
	}
}

void MainWindow::openFiles() {
  m_dataFile.setFileName(ExperimentCfg::getInstance()->getTempDir() + "Data.csv");
  m_dataFile.open(QIODevice::WriteOnly);
  
  m_temperatureDataFile.setFileName(ExperimentCfg::getInstance()->getTempDir() + "TemperatureData.csv");
  m_temperatureDataFile.open(QIODevice::WriteOnly);
  
  m_warmupFile.setFileName(ExperimentCfg::getInstance()->getTempDir() + "Warmup.csv");
  m_warmupFile.open(QIODevice::WriteOnly);
}

void MainWindow::closeFiles() {
  m_dataFile.close();
  m_temperatureDataFile.close();
  m_warmupFile.close();
}

void MainWindow::on_actionSetZero_triggered() {
  testMachine->ZeroLoadCell();
}

void MainWindow::on_actionRunExperiment_triggered() {
  m_infoTimer.stop();
  
  m_expState = EsLoading;

  expTime.restart();

  if (ExperimentCfg::getInstance()->getProperty("TemperatureExperiment").toBool()) {
    m_heatingData.data().clear();
    onTemperatureTimer();
    ui->qwtpData->enableAxis(QwtPlot::xBottom, true);
    ui->qwtpData->enableAxis(QwtPlot::yLeft, true);
  }

  testMachine->ToggleTEST_LED(false);
  testMachine -> ZeroXhead();
  sleep_ms(500);

  double loadingSpeed = ExperimentCfg::getInstance()->getProperty("LoadingSpeed").toDouble();
  if(ExperimentCfg::getInstance()->getProperty("TrueStrainRate").toBool()) {
    loadingSpeed = ExperimentCfg::getInstance()->getProperty("LoadingSpeed_1").toDouble();
    loadingSpeed *= ExperimentCfg::getInstance()->pattern().lenght * 60.0 /*в мм/мин*/;
  }

  testMachine -> SetSowtwareXheadVelocity(loadingSpeed);
  sleep_ms(500);
  qDebug() << "Loading speed" << loadingSpeed;
  testMachine -> MoveXheadForward();
  expTimer.start(10);
}

void MainWindow::on_actionStop_triggered()
{
  testMachine->HaltXhead();

  expTimer.stop();
  temperatureTimer.stop();
  closeFiles();
  
  QString m_tempDir = ExperimentCfg::getInstance()->getTempDir();
  QStringList fileList = QDir(m_tempDir).entryList(QDir::Files);
  fileList.push_front(ExperimentCfg::getInstance()->getReportFileName());
  
  QProcess process;
  process.setWorkingDirectory(m_tempDir);
  process.start ("zip", fileList);
  process.waitForFinished();

  qDebug() << "Zip File List" << fileList;
  m_expState = EsFree;
}

void MainWindow::onTestButton_pressed()
{
	/*
	if (m_expState == EsFree)
		ui->actionRunExperiment->triggered();
		*/
}

void MainWindow::on_actionRunHeating_triggered()
{
	m_infoTimer.stop();

	m_expState = EsWarmUp;
	expTime.restart();
	temperatureTimer.start(500);
}

void MainWindow::on_actionRunTemperatureCompensation_triggered()
{

}

void MainWindow::on_actionLeadPuanson_triggered() {

  int const plen = static_cast<int>(ExperimentCfg::getInstance()->pattern().lenght * 1000.0); // длина образца мкм
  int const offset = plen - 13700 - 200;
  
  if (offset <= 500) return;
  
  testMachine -> SetSowtwareXheadVelocity(1.0);
  
  testMachine -> ReverseXhead();
  while(true) {
    int lc, xh;
    testMachine->ReadLoadCellAndXhead(lc, xh);
    double t = expTime.elapsed()/1000.0;
    double force = loadCellRange * (double) lc / 30000.0;
    if (force < -1.0) break;
  }

  testMachine->HaltXhead();
  sleep_ms(500);
  
  testMachine -> SetSowtwareXheadVelocity(10.0);
  if (offset > 500)
    testMachine->RelativePositionalMove(offset);
}

void MainWindow::onTemperatureTimer()
{
	//ExperimentCtls::PIDControllerPtr_t controller
	ExperimentCtls::TemperatureSensorPtr_t sensor
		= ExperimentCtls::getInstance()->getPatternSensor();
	double t = expTime.elapsed()/1000.0;
	if (sensor)
	{
		double T = sensor->getTemperature();
		m_currentTemperature = T;

		if(m_expState == EsWarmUp)
			m_heatingData.data().push_back(QPointF(t, T));
		else
		{
			if (m_temperatureData.data().empty())
				m_temperatureData.data().push_back(QPointF(t, T));
			m_temperatureData.data().push_back(QPointF(t, T));
		}

		QIODevice* file = m_expState == EsWarmUp ? &m_warmupFile : &m_temperatureDataFile;
		QTextStream stream(file);
		stream << QString::number(t).replace(".", ",") << ";"
				<< QString::number(T).replace(".", ",") << "\n";

		ui->leTemperature->setText(QString::number(T));
	}
	else
		ui->leTemperature->setText(tr("Ошибка"));

	if(m_expState == EsWarmUp)
	{
		ui->qwtpData->replot();
		showInfo();
	}
}

void MainWindow::onExpTimer()
{
  static bool temperatureExperiment = false;
  static bool firstRun = true;
  static bool trueStrainRate = false; //испытания с истинной скоростью деформации
  static int writeOnlyNSample = 1;
  static double trueLoadingSpeed = 0.01;//  истинная скорость деформации

  if (firstRun) {
    writeOnlyNSample = ExperimentCfg::getInstance()->getProperty("WriteOnlyNSample").toInt();
    temperatureExperiment = ExperimentCfg::getInstance()->getProperty("TemperatureExperiment").toBool();
    trueStrainRate = ExperimentCfg::getInstance()->getProperty("TrueStrainRate").toBool();
    trueLoadingSpeed = ExperimentCfg::getInstance()->getProperty("LoadingSpeed_1").toDouble();
    m_currentTrueSpeed = trueLoadingSpeed;
    qDebug() << "WriteOnlyNSample" << writeOnlyNSample;
    firstRun = false;
  }

  static int callCounter = 0;

  double t = expTime.elapsed()/1000.0;

  int lc, xh;
  testMachine->ReadLoadCellAndXhead(lc, xh);
  double force = loadCellRange * (double) lc / 30000.0;
  double stress = force / (ExperimentCfg::getInstance()->pattern().width
			   * ExperimentCfg::getInstance()->pattern().thickness);
  double epsilon = (double) xh / ( ExperimentCfg::getInstance()->pattern().lenght * 1000.0) * 100.0;

  if (trueStrainRate && (!(callCounter % 10)) ) { // испытания с истинной скоростью деформации
    double Ltek = ExperimentCfg::getInstance()->pattern().lenght + static_cast<double>(xh)/1000.0 /*в мм*/;
    if (leaps.empty()) 
      m_currentTrueSpeed = trueLoadingSpeed; // если нет скачков
    else {
      LeapRecord& lpr = leaps.back();
      if(lpr.isUsing) { // если прыжок применен
	double t_elapsed = static_cast<double>(lpr.timer.elapsed())/1000.0; // сек
	if(t_elapsed > lpr.leapTime) {
	  // пора выключать прыжок
	  m_currentTrueSpeed = lpr.prevTrueSpeed;
	  leaps.pop_back();
	  if (!leaps.empty()) leaps.back().bCancel->setEnabled(true);

	} else if (!(callCounter % 100) && lpr.bCancel) { // обновить информацию на кнопке
	  lpr.bCancel->setText( tr("Скачок ") + QString::number(lpr.leap) + 
				" (" + QString::number(lpr.leapTime - t_elapsed) +tr(" сек)"));
	}
      } else {
	lpr.isUsing = true;
	m_currentTrueSpeed *= lpr.leap;
	lpr.timer.restart();
      }
    }

    double loadingSpeed = m_currentTrueSpeed * Ltek;
    loadingSpeed *= 60.0 /*в мм/мин*/;
    qDebug() << "cur speed" << t << m_currentTrueSpeed << loadingSpeed;
    testMachine -> SetSowtwareXheadVelocity(loadingSpeed);
    //sleep_ms(500);
  }

  if (!(callCounter % writeOnlyNSample)) {

    if(temperatureExperiment)
      m_temperatureData.data().back() = QPointF(t, m_currentTemperature);

    m_data.data().push_back(QPointF(epsilon, stress));
    QTextStream stream(&m_dataFile);
    // 1.Data.csv — данные эксперимента: t с, ε %, σ МПа.
    stream << QString::number(t).replace(".", ",") << ";"
	   << QString::number(epsilon).replace(".", ",") << ";"
	   << QString::number(stress).replace(".", ",") << "\n";
    //stream.flush();
  }

  if (!(callCounter % 100)) { // каждый десятый
    ui->leForce->setText(QString::number(force));
    ui->leStress->setText(QString::number(stress));
    ui->leDeformation->setText(QString::number(xh));
    ui->leDeformationInPercent->setText(QString::number(epsilon));
    
    ui->qwtpData->replot();
  }

  ++callCounter;
}


void MainWindow::showInfo()
{
	
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

	static bool tempExp = ExperimentCfg::getInstance()->getProperty("TemperatureExperiment").toBool();

	if (m_expState == EsFree && tempExp)
	{

		ExperimentCtls::TemperatureSensorPtr_t sensor =
				ExperimentCtls::getInstance()->getPatternSensor();
		if (sensor)
			ui->leTemperature->setText(QString::number(sensor->getTemperature()));
		else
			ui->leTemperature->setText(tr("Ошибка"));
	}
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	if (m_expState == EsFree)
		event -> accept();
	else
		event -> ignore();

}
