/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef _KRECIPESDBIFACE_H_
#define _KRECIPESDBIFACE_H_

#include <dcopobject.h>

class KrecipesDBIface : virtual public DCOPObject
{
	K_DCOP
public:

k_dcop:
	virtual void emptyData() = 0;
	virtual void backup( const QString &filename ) = 0;
};

#endif // _KRECIPESIFACE_H_
