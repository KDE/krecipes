/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "unitratio.h"
#include "klocale.h"

UnitRatio::UnitRatio()
{
uID1=-1;uID2=-1;ratio=-1;
}

UnitRatio::UnitRatio(UnitRatio &ur)
{
uID1=ur.uID1;uID2=ur.uID2;ratio=ur.ratio;
}

UnitRatio::~UnitRatio()
{
}


