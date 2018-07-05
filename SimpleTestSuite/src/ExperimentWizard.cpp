/*
 * ExperimentWizard.cpp
 *
 *  Created on: 19.11.2008
 *      Author: lexa
 */

#include "ExperimentWizard.h"
#include <QWizardPage>
#include <QSettings>

#include "RxSelectReportFile.h"
#include "ExperimaentWizard_p.h"
#include "ExperimentCfg.h"
#include <QRegExpValidator>
#include <QRegExp>

PatternPage::PatternPage()
{
	ui.setupUi(this);
	loadSettings();
}

PatternPage::~PatternPage()
{
	saveExpCfg();
	saveSettings();
}

void PatternPage::loadSettings()
{
	QSettings settings;
	settings.beginGroup("PatternPage");

	//ui.leName->setText(settings.value("leName").toString());

	ui.dsbSampleLength->setValue(settings.value("dsbSampleLength").toDouble());
	ui.dsbSampleWidth->setValue(settings.value("dsbSampleWidth").toDouble());
	ui.dsbSampleThickness->setValue(settings.value("dsbSampleThickness").toDouble());
	ui.dsbSampleLengthL->setValue(settings.value("dsbSampleLengthL").toDouble());

	settings.endGroup();
}

void PatternPage::saveSettings()
{
	QSettings settings;
	settings.beginGroup("PatternPage");

	settings.setValue("leName", ui.leName->text());
	settings.setValue("teDescription", ui.teDescription->toPlainText());

	settings.setValue("dsbSampleLength", ui.dsbSampleLength->value());
	settings.setValue("dsbSampleWidth", ui.dsbSampleWidth->value());
	settings.setValue("dsbSampleThickness", ui.dsbSampleThickness->value());
	settings.setValue("dsbSampleLengthL", ui.dsbSampleLengthL->value());

	settings.endGroup();
}

void PatternPage::saveExpCfg()
{
	Pattern& pattern = ExperimentCfg::getInstance()->pattern();
	pattern.description = ui.teDescription->toPlainText();
	pattern.lenght = ui.dsbSampleLength->value();
	pattern.lenghtL = ui.dsbSampleLengthL->value();
	//pattern.materialName =
	pattern.name = ui.leName->text();
	pattern.thickness = ui.dsbSampleThickness->value();
	pattern.width = ui.dsbSampleWidth->value();
}

ExperimentPage::ExperimentPage(PatternPage* patternPage)
  : m_patternPage(patternPage) {
  ui.setupUi(this);
  ui.leLeaps->setValidator(
			   new QRegExpValidator(
						QRegExp("(?:\\d+\\.?\\d*;)*\\d+\\.?\\d*"),
						0
						)
			   );

  ui.leLeapTimes->setValidator(
			   new QRegExpValidator(
						QRegExp("(?:\\d+\\.?\\d*;)*\\d+\\.?\\d*"),
						0
						)
			   );
  loadSettings();
}

ExperimentPage::~ExperimentPage()
{
	saveExpCfg();
	saveSettings();
}

void ExperimentPage::loadSettings() {
  QSettings settings;
  settings.beginGroup("ExperimentPage");

  ui.dsbSpeed_mm_min->setValue(settings.value("dsbSpeed_mm_min").toDouble());
  ui.dsbSpeed_sec_1->setValue(settings.value("dsbSpeed_sec_1").toDouble());
  ui.spExpTemperature->setValue(settings.value("spExpTemperature").toInt());
  ui.sbWriteOnlyNSample->setValue(settings.value("sbWriteOnlyNSample").toInt());
  ui.cbTrueStrainRate->setChecked(true);// истинная скорость деформации, для выключения лишних полей
  ui.cbTrueStrainRate->setChecked(settings.value("cbTrueStrainRate").toBool());
  ui.leLeaps->setText(settings.value("leLeaps", "1.3;2;3").toString());
  ui.leLeapTimes->setText(settings.value("leLeapTimes", "10;20;30").toString());
  settings.endGroup();
}

void ExperimentPage::saveSettings() {
  
  QSettings settings;
  settings.beginGroup("ExperimentPage");

  settings.setValue("dsbSpeed_mm_min", ui.dsbSpeed_mm_min->value());
  settings.setValue("dsbSpeed_sec_1", ui.dsbSpeed_sec_1->value());
  settings.setValue("spExpTemperature", ui.spExpTemperature->value());
  settings.setValue("sbWriteOnlyNSample", ui.sbWriteOnlyNSample->value());
  settings.setValue("cbTrueStrainRate", ui.cbTrueStrainRate->isChecked());
  settings.setValue("leLeaps", ui.leLeaps->text());
  settings.setValue("leLeapTimes", ui.leLeapTimes->text());

  settings.endGroup();

  ExperimentCfg::getInstance()->setProperty("TemperatureExperiment", ui.cbTemperatureExperiment->isChecked ());
  ExperimentCfg::getInstance()->setProperty("WarmUpWithoutPattern", ui.rbWarmUpWithoutPattern->isChecked ());
  ExperimentCfg::getInstance()->setProperty("ExpTemperature", ui.spExpTemperature->value());

  ExperimentCfg::getInstance()->setProperty("TrueStrainRate", ui.cbTrueStrainRate->isChecked());
  ExperimentCfg::getInstance()->setProperty("LoadingSpeed_1", ui.dsbSpeed_sec_1->value());
  ExperimentCfg::getInstance()->setProperty("LoadingSpeed", ui.dsbSpeed_mm_min->value());
  ExperimentCfg::getInstance()->setProperty("WriteOnlyNSample", ui.sbWriteOnlyNSample->value());

  ExperimentCfg::getInstance()->setProperty("Leaps", ui.leLeaps->text());
  ExperimentCfg::getInstance()->setProperty("LeapTimes", ui.leLeapTimes->text());
}

void ExperimentPage::saveExpCfg() {

}

void ExperimentPage::on_dsbSpeed_sec_1_valueChanged (double d) {
  double Ltek = 1000.0/60.0/d;
  const double L0 = m_patternPage->ui.dsbSampleLength->value();
  double lmax = (Ltek - L0)/L0 * 100.0;
  ui.lMaxDeforamtion->setText(tr("Максимальная деформация - <span style=\"color:#ff0000;\"><b>") + 
			      QString::number(lmax) + "%</b></span>");
}

void ExperimentPage::on_dsbSpeed_mm_min_valueChanged (double d) {
  Q_EMIT ui.dsbSpeed_mm_sec->setValue(d/60.0);
}

void ExperimentPage::on_dsbSpeed_mm_sec_valueChanged (double d) {
  Q_EMIT ui.dsbSpeed_mm_min->setValue(d*60.0);
}

ExperimentWizard::ExperimentWizard() {
  PatternPage* patternPage = new PatternPage;
  addPage(patternPage);
  
  addPage(new ExperimentPage(patternPage));
  
  addPage(new RxSelectReportFile);
}

ExperimentWizard::~ExperimentWizard() {
  // TODO Auto-generated destructor stub
}
