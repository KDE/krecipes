/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef SERVERSETUPPAGE_H
#define SERVERSETUPPAGE_H

#include <QWidget>
#include <QString>

class QLabel;
class QCheckBox;
class KLineEdit;
class KIntNumInput;

class ServerSetupPage: public QWidget
{
public:
	// Methods
	ServerSetupPage( QWidget *parent );
	QString user( void ) const ;
	QString password( void ) const;
	QString dbName( void ) const;
	QString server( void ) const;
	QString client( void ) const;
	int port( void ) const;
	void getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass, int &port );
private:
	// Widgets
	QLabel *logo;
	QLabel *serverSetupText;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;
	QCheckBox *remoteServerCheckBox;
	KLineEdit *serverEdit;
	KLineEdit *clientEdit;
	KIntNumInput *portEdit;
};

#endif
