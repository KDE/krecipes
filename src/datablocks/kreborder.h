/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KREBORDER_H
#define KREBORDER_H
 
#include <qcolor.h>
#include <qstring.h>
 
//typedef enum KreBorderStyle { None = 0, Dotted, Dashed, Solid, Double, Groove, Ridge, Inset, Outset };

class KreBorder
{
public:
	KreBorder( int w = 1, const QString & s = "none", const QColor &c = Qt::black );

	int width;
	QString style;
	QColor color;
};

#endif //KREBORDER_H
