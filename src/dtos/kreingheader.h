/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREINGHEADER_H
#define KREINGHEADER_H

#include "kreelement.h"

class KreIngHeader: public KreElement {

public:
	KreIngHeader();
	KreIngHeader( const QVariant & id, const QString & name );

};

#endif
