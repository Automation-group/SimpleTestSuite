#ifndef RXSELECTREPORTFILE_H_
#define RXSELECTREPORTFILE_H_

#include <QWizardPage>
#include <boost/shared_ptr.hpp>

class Ui_SelectReportFileForm;

class RxSelectReportFile : public QWizardPage
{
	Q_OBJECT
public:
	RxSelectReportFile();
	bool isComplete () const; 
	QString fileName() const;
	void setFileName(const QString& fname);
private Q_SLOTS:
	void on_tbSelectReportFile_clicked();
	void on_leFileName_textChanged(const QString& text);
private:
	boost::shared_ptr<Ui_SelectReportFileForm> m_ui;
};

#endif /*RXSELECTREPORTFILE_H_*/
