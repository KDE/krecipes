/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef MYSQLSERVERPREFS_H
#define MYSQLSERVERPREFS_H

#include <QWidget>

class KIntNumInput;
class KLineEdit;
class KUrlRequester;

class MySQLServerPrefs : public QWidget
{
public:
	MySQLServerPrefs( QWidget *parent );

	void saveOptions( void );
private:
	// Internal Widgets
	KUrlRequester *dumpPathRequester;
	KUrlRequester *mysqlPathRequester;

	KLineEdit *serverEdit;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;
	KIntNumInput *portEdit;
};

#endif
