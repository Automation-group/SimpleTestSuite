/*
 * ExperimentCfg.h
 *
 *  Created on: 27.11.2008
 *      Author: lexa
 */

#ifndef EXPERIMENTCFG_H_
#define EXPERIMENTCFG_H_

#include <QString>
#include <QMap>
#include <QVariant>

/*!
 * Описание образца
 */
struct Pattern
{
	QString materialName;
	QString name;
	QString description;

	double lenght; // мм
	double width; // мм
	double thickness; // мм
	double workStress; // МПа
};

/*!
 * Контейнер параметров эксперимента
 */
class ExperimentCfg
{
public:
	typedef QMap<QString, QVariant> PropertyMap_t;

	~ExperimentCfg();

	static ExperimentCfg* getInstance();

    QString getReportFileName() const
    {
        return m_reportFileName;
    }

    void setReportFileName(const QString& reportFileName)
    {
        this->m_reportFileName = reportFileName;
    }

    QString getTempDir() const
    {
        return m_tempDir;
    }

    void setTempDir(const QString& tempDir)
    {
        this->m_tempDir = tempDir;
    }

    Pattern& pattern() { return m_pattern; }

    QVariant getProperty(const QString& pName) const
    {
    	return m_propertyMap[pName];
    }

    void setProperty(const QString& pName, const QVariant& value)
    {
    	m_propertyMap[pName] = value;
    }

private:
	ExperimentCfg();
	ExperimentCfg(const ExperimentCfg&);
private:
	Pattern m_pattern;
	QString m_reportFileName;
	QString m_tempDir;
	PropertyMap_t m_propertyMap;
};

#endif /* EXPERIMENTCFG_H_ */
