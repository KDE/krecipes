/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef _KRECIPESIFACE_H_
#define _KRECIPESIFACE_H_

#include <qlist.h>

class KrecipesIface 
{
	Q_CLASSINFO("D-Bus Interface", "org.kde.krecipes")
public:
	virtual ~KrecipesIface();
public Q_SLOTS:
	virtual Q_SCRIPTABLE QDBusInterface currentDatabase() const = 0;
 	virtual Q_SCRIPTABLE void reload() = 0;
 	     
 	virtual Q_SCRIPTABLE void exportRecipes( const Q3ValueList<int> &ids ) = 0;

};

#endif // _KRECIPESIFACE_H_
