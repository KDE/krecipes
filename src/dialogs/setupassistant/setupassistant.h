/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef SETUPASSISTANT_H
#define SETUPASSISTANT_H

#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <kassistantdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <KIntNumInput>


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

class WelcomePage: public QWidget
{
public:
	// Methods
	WelcomePage( QWidget *parent );

private:
	// Widgets
	QLabel *logo;
	QLabel *welcomeText;

};


class PermissionsSetupPage: public QWidget
{
	Q_OBJECT
public:
	// Methods
	PermissionsSetupPage( QWidget *parent );
	bool doUserSetup( void );
	bool useAdmin( void );
	void getAdmin( QString &adminName, QString &adminPass );
private:
	// Widgets
	QLabel *logo;
	QLabel *permissionsText;
	QCheckBox *noSetupCheckBox;
	QCheckBox *rootCheckBox;
	KLineEdit *userEdit;
	KLineEdit *passEdit;

private slots:
	void rootCheckBoxChanged( bool on );
	void noSetupCheckBoxChanged( bool on );

};

class PSqlPermissionsSetupPage: public QWidget
{
	Q_OBJECT
public:
	// Methods
	PSqlPermissionsSetupPage( QWidget *parent );
	bool doUserSetup( void );
	bool useAdmin( void );
	void getAdmin( QString &adminName, QString &adminPass );
private:
	// Widgets
	QLabel *logo;
	QLabel *permissionsText;
	QCheckBox *noSetupCheckBox;
	QCheckBox *rootCheckBox;
	KLineEdit *userEdit;
	KLineEdit *passEdit;

private slots:
	void rootCheckBoxChanged( bool on );
	void noSetupCheckBoxChanged( bool on );

};

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


class SQLiteSetupPage: public QWidget
{
	Q_OBJECT

public:
	// Methods
	SQLiteSetupPage( QWidget *parent );
	QString dbFile() const;

private slots:
	void selectFile();

private:
	// Widgets
	QLabel *logo;
	QLabel *serverSetupText;
	KLineEdit *fileEdit;
};


class DataInitializePage: public QWidget
{
public:
	// Methods
	DataInitializePage( QWidget *parent );
	bool doInitialization( void );
	bool doUSDAImport( void );
	void setUSDAImport( bool import );

private:
	// Widgets
	QLabel *logo;
	QLabel *initializeText;
	QLabel *USDAImportText;
	QCheckBox *initializeCheckBox;
	QCheckBox *USDAImportCheckBox;

};

class SavePage: public QWidget
{
public:
	// Methods
	SavePage( QWidget *parent );
private:
	// Widgets
	QLabel *logo;
	QLabel *saveText;

};

class DBTypeSetupPage: public QWidget
{

	Q_OBJECT

public:
	// Methods
	DBTypeSetupPage( QWidget *parent );
	int dbType( void );
private:
	// Widgets
	QLabel *dbTypeSetupText;
	QLabel *logo;
	QGroupBox *bg;
	QRadioButton *liteCheckBox;
	QRadioButton *mysqlCheckBox;
	QRadioButton *psqlCheckBox;
private slots:
	void setSQLitePages();
	void setMySQLPages();
	void setPostgreSQLPages();
signals:
	void showPages( DBType );
};

#endif
