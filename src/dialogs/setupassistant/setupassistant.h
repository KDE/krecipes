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
#ifndef SETUPASSISTANT_H
#define SETUPASSISTANT_H

#include <KAssistantDialog>

/**
@author Unai Garro
*/

class WelcomePage;
class DBTypeSetupPage;
class PermissionsSetupPage;
class PSqlPermissionsSetupPage;
class ServerSetupPage;
class DataInitializePage;
class SavePage;
class SQLiteSetupPage;

class KPageWidgetItem;

enum DBType {SQLite, MySQL, PostgreSQL};

class SetupAssistant: public KAssistantDialog 
{
	Q_OBJECT
public:

	explicit SetupAssistant( QWidget *parent = 0, Qt::WFlags f = 0 );
	~SetupAssistant();
	void getOptions( bool &setupUser, bool &initializeData, bool &doUSDAImport );
	void getAdminInfo( bool &enabled, QString &adminUser, QString &adminPass, const QString &dbType );
	void getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass, int &port );

protected:
	virtual void next();
	virtual void accept( void );

private:
	// Widgets
	WelcomePage *welcomePage;
	DBTypeSetupPage *dbTypeSetupPage;
	PermissionsSetupPage *permissionsSetupPage;
	PSqlPermissionsSetupPage *pSqlPermissionsSetupPage;
	ServerSetupPage *serverSetupPage;
	DataInitializePage *dataInitializePage;
	SQLiteSetupPage *sqliteSetupPage;
	SavePage *savePage;
	// KPageWidgetItem's
	KPageWidgetItem *welcomePageItem;
	KPageWidgetItem *dbTypeSetupPageItem;
	KPageWidgetItem *permissionsSetupPageItem;
	KPageWidgetItem *pSqlPermissionsSetupPageItem;
	KPageWidgetItem *serverSetupPageItem;
	KPageWidgetItem *dataInitializePageItem;
	KPageWidgetItem *sqliteSetupPageItem;
	KPageWidgetItem *savePageItem;

private slots:
	void showPages( DBType );

};

#endif
