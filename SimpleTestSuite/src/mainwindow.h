#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include <QSettings>
#include <QTime>
#include <QFile>
#include <QList>
#include <QToolButton>

#include <qextserialport.h>
#include <QlfTOTestMachine.h>
#include <boost/shared_ptr.hpp>
#include "ExpData.h"

class Ui_MainWindow;
class QwtPlotCurve;

struct LeapRecord {
  double leap;
  double leapTime;
  double prevTrueSpeed;
  QTime timer;
  bool isUsing;
  typedef boost::shared_ptr<QToolButton> bCancel_p_t;
  bCancel_p_t bCancel;
};

class MainWindow: public QMainWindow
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();
private:
  QlfTOTestMachine* testMachine;

  QList<LeapRecord> leaps;

  QTimer expTimer;
  QTimer temperatureTimer;
  QTimer m_infoTimer;

  QTime expTime;

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

  /*!
   * Загрузить списки чкачков и времен скачков
   */
  void loadLeapAndTimes(const QString& leaps, const QString& times);

 protected:
  void closeEvent(QCloseEvent* event);
 private slots:
  void onExpTimer();
  void onTemperatureTimer();
  void on_actionSetZero_triggered();
  void on_actionRunExperiment_triggered();
  void on_actionRunHeating_triggered();
  void on_actionRunTemperatureCompensation_triggered();
  void on_actionLeadPuanson_triggered();
  void on_actionStop_triggered();

  /*!
   * Скачок
   */
  void onLeap();

  /*!
   * Отмена скачка
   */
  void onCancelLeap();

  /*!
   * Нажата кнопка TEST
   */
  void onTestButton_pressed();
  
  /*!
   * Показать текущее состоянии машины
   */
  void showInfo();
private:

  enum ExpState {
    EsFree, // ничего не делается
    EsWarmUp, // пргорев
    EsLoading // нагружение
    //EsTemperatureCompensation // коменсация теплового расширения
  };

  double m_currentTrueSpeed;
  ExpState m_expState;
  bool m_temperatureExperiment;
  boost::shared_ptr<Ui_MainWindow> ui;
  ExpData m_data;
  ExpData m_heatingData;
  ExpData m_temperatureData;
  QwtPlotCurve* m_dataCurve;
  QFile m_warmupFile;
  QFile m_dataFile;
  QFile m_temperatureDataFile;
  double m_currentTemperature;
};

#endif /*MAINWINDOW_H_*/

