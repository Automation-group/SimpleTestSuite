/*
 * lfUtils.h
 *
 *  Created on: 25.11.2008
 *      Author: lexa
 */

#ifndef LFUTILS_H_
#define LFUTILS_H_

#include <QString>
#include <qextserialbase.h>

namespace lfUtils
{
	PortSettings createPortSettings(const QString& confString);
}

#endif /* LFUTILS_H_ */
