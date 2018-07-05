#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>


#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"
#include "ExperimentWizard.h"
#include "STSConfig.h"
#include <QDir>
#include <QtDebug>
#include "ExperimentCtls.h"
#include "ExperimentCfg.h"

int main(int argc, char *argv[])
{
	// аппроксимация кривой нагружения f(x) = ln(x + 1.0)∙(1.0 − tanh((x−500)))
	QApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QCoreApplication::setOrganizationName("Lab5");
    QCoreApplication::setOrganizationDomain("metals.dom");
    QCoreApplication::setApplicationName("SimpleTestSuite");

	QString config_filename = QString(STSCONFIG_FILENAME);
	qDebug() << "Config file:" << config_filename;

	if(!QDir().exists(config_filename))
		config_filename = "SimpleTestSuite.conf";

	QSettings settings(config_filename, QSettings::IniFormat);
	ExperimentCtls::getInstance()->loadSettings(settings);

	qDebug() << "Using config file:" << config_filename;

	{
		ExperimentWizard wizard;
		if (wizard.exec() == QDialog::Rejected)
			return 0;
	}

	{
		QString tempDir = QDir::homePath() + QString::fromUtf8("/.lab5/SimpleTestSuite/temp/") +
			QDateTime::currentDateTime().toString("hh.mm_dd.MM.yyyy") + "/";
		tempDir =  QDir::toNativeSeparators(tempDir);
		QDir().mkpath(tempDir);

		ExperimentCfg::getInstance()->setTempDir(tempDir);
	}

	MainWindow mainWin;
	mainWin.showMaximized ();
	return app.exec();
}


