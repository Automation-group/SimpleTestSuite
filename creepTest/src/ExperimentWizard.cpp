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
	ui.dsbWorkStress->setValue(settings.value("dsbWorkStress").toDouble());

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
	settings.setValue("dsbWorkStress", ui.dsbWorkStress->value());

	settings.endGroup();
}

void PatternPage::saveExpCfg()
{
	Pattern& pattern = ExperimentCfg::getInstance()->pattern();
	pattern.description = ui.teDescription->toPlainText();
	pattern.lenght = ui.dsbSampleLength->value();
	pattern.workStress = ui.dsbWorkStress->value();
	//pattern.materialName =
	pattern.name = ui.leName->text();
	pattern.thickness = ui.dsbSampleThickness->value();
	pattern.width = ui.dsbSampleWidth->value();
}


ExperimentWizard::ExperimentWizard()
{
	PatternPage* patternPage = new PatternPage;
	addPage(patternPage);

	addPage(new RxSelectReportFile);
}

ExperimentWizard::~ExperimentWizard()
{
	// TODO Auto-generated destructor stub
}
