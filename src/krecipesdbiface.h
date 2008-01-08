/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef _KRECIPESDBIFACE_H_
#define _KRECIPESDBIFACE_H_

#include <QObject>

class KrecipesDBIface  
{
	Q_OBJECT
	Q_CLASSINFO("org.kde.krecipes", "BDIface")
public:

Q_SLOTS:
 	virtual Q_SCRIPTABLE void emptydata() = 0;
 	virtual Q_SCRIPTABLE bool backup(const QString &filename) = 0;

};

#endif // _KRECIPESIFACE_H_
