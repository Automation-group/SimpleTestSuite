/*
 * ExperimaentWizard_p.h
 *
 *  Created on: 26.11.2008
 *      Author: lexa
 */

#ifndef EXPERIMAENTWIZARD_P_H_
#define EXPERIMAENTWIZARD_P_H_

#include <QWizardPage>

#include "../ui_patternPage.h"
#include "../ui_expSettingsPage.h"

class ExperimentPage;

class PatternPage : public QWizardPage
{
	Q_OBJECT
public:
	PatternPage();
	~PatternPage();

	void loadSettings();
	void saveSettings();
	void saveExpCfg();
private:
	friend class ExperimentPage;
	Ui_patternPageForm ui;
};


class ExperimentPage : public QWizardPage
{
	Q_OBJECT
public:
	ExperimentPage(PatternPage* patternPage);
	~ExperimentPage();

	void loadSettings();
	void saveSettings();
	void saveExpCfg();

private Q_SLOTS:
	void on_dsbSpeed_mm_min_valueChanged (double d);
	void on_dsbSpeed_mm_sec_valueChanged (double d);
	void on_dsbSpeed_sec_1_valueChanged (double d);
	//void on_dsbSpeed_p_min_valueChanged (double d);
	//void on_dsbSpeed_p_sec_valueChanged (double d);
private:
	Ui_expSettingsPageForm ui;
	PatternPage* m_patternPage;
};

#endif /* EXPERIMAENTWIZARD_P_H_ */
