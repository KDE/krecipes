/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreborder.h"

//typedef enum KreBorderStyle { None = 0, Dotted, Dashed, Solid, Double, Groove, Ridge, Inset, Outset };

KreBorder::KreBorder( int w, const QString & s, const QColor &c ) : width( w ), style( s ), color( c )
{
}
