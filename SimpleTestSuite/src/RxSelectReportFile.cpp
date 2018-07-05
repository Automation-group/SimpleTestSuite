#include "RxSelectReportFile.h"
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include "ExperimentCfg.h"
#include "../ui_SelectReportFileWizardPage.h"

RxSelectReportFile::RxSelectReportFile()
  : m_ui(new Ui_SelectReportFileForm)
{
  m_ui->setupUi(this);
}

QString RxSelectReportFile::fileName() const
{
  return m_ui->leFileName -> text ();
}

void RxSelectReportFile::setFileName(const QString& fname)
{
  m_ui->leFileName -> setText(fname);
}

void RxSelectReportFile::on_tbSelectReportFile_clicked()
{
  QString dir = QFileInfo (m_ui->leFileName -> text ()).absolutePath() + QDir::separator();
  QString fileName = QFileDialog::getSaveFileName(this,
						  tr("Выберете файл эксперимента"), dir, tr("Файл отчета (*.l5rz)"));
  if (fileName.isEmpty())
    m_ui->leFileName -> setText(dir);
  else {
    if(!fileName.endsWith(".l5rz")) fileName += ".l5rz";
    m_ui->leFileName -> setText(fileName);
    ExperimentCfg::getInstance()->setReportFileName(fileName);
  }
}

void RxSelectReportFile::on_leFileName_textChanged(const QString& text)
{
  completeChanged ();
}

bool RxSelectReportFile::isComplete () const
{
  return !m_ui->leFileName->text().isEmpty() & !QFileInfo (m_ui->leFileName->text()).isDir();
}
