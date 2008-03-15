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


class KrecipesDBIface  
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.kde.krecipes")
public:
	virtual ~KrecipesDBIface(){};
public Q_SLOTS:
	//KDE4 port
	//Q_SCRIPTABLE virtual void emptydata() = 0;
	Q_SCRIPTABLE void emptydata();
 	Q_SCRIPTABLE virtual bool backup(const QString &filename)=0;

};

#endif // _KRECIPESIFACE_H_
