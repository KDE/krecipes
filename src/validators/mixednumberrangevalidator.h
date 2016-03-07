/***************************************************************************
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MIXEDNUMBERRANGEVALIDATOR_H
#define MIXEDNUMBERRANGEVALIDATOR_H

#include <QValidator>


class MixedNumberRangeValidator : public QValidator
{
public:
	virtual QValidator::State validate ( QString & input, int & pos ) const;
};


#endif
