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
	Ui_patternPageForm ui;
};


#endif /* EXPERIMAENTWIZARD_P_H_ */
