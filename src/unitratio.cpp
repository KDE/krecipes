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
ingID1=-1;ingID2=-1;ratio=-1;
}

UnitRatio::UnitRatio(UnitRatio &ur)
{
ingID1=ur.ingID1;ingID2=ur.ingID2;ratio=ur.ratio;
}

UnitRatio::~UnitRatio()
{
}


