/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <qvbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <kwizard.h>
#include <klineedit.h>
#include <ktextedit.h>


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

enum DBType {SQLite,MySQL,PostgreSQL};

class SetupWizard:public KWizard{
Q_OBJECT
public:

     SetupWizard(QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
    ~SetupWizard();
    void getOptions(bool &setupUser, bool &initializeData, bool &doUSDAImport);
    void getAdminInfo(bool &enabled,QString &adminUser,QString &adminPass, const QString &dbType);
    void getServerInfo(bool &isRemote, QString &host, QString &client, QString &dbName,QString &user, QString &pass);
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

private slots:
	void save(void);
	void showPages(DBType);

};

class WelcomePage:public QWidget{
public:
	// Methods
	WelcomePage(QWidget *parent);

private:
	// Widgets
	QLabel *logo;
	QLabel *welcomeText;

};


class PermissionsSetupPage:public QWidget{
Q_OBJECT
public:
	// Methods
	PermissionsSetupPage(QWidget *parent);
	bool doUserSetup(void);
	bool useAdmin(void);
	void getAdmin(QString &adminName,QString &adminPass);
private:
	// Widgets
	QLabel *logo;
	QLabel *permissionsText;
	QCheckBox *noSetupCheckBox;
	QCheckBox *rootCheckBox;
	QLineEdit *userEdit;
	QLineEdit *passEdit;

private slots:
	void rootCheckBoxChanged(bool on);
	void noSetupCheckBoxChanged(bool on);

};

class PSqlPermissionsSetupPage:public QWidget{
Q_OBJECT
public:
	// Methods
	PSqlPermissionsSetupPage(QWidget *parent);
	bool doUserSetup(void);
	bool useAdmin(void);
	void getAdmin(QString &adminName,QString &adminPass);
private:
	// Widgets
	QLabel *logo;
	QLabel *permissionsText;
	QCheckBox *noSetupCheckBox;
	QCheckBox *rootCheckBox;
	QLineEdit *userEdit;
	QLineEdit *passEdit;

private slots:
	void rootCheckBoxChanged(bool on);
	void noSetupCheckBoxChanged(bool on);

};

class ServerSetupPage:public QWidget{
public:
	// Methods
	ServerSetupPage(QWidget *parent);
	QString user(void);
	QString password(void);
	QString dbName(void);
	QString server(void);
	QString client(void);
	void getServerInfo(bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass);
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
};


class SQLiteSetupPage:public QWidget{
Q_OBJECT

public:
	// Methods
	SQLiteSetupPage(QWidget *parent);
	QString dbFile() const;

private slots:
	void selectFile();

private:
	// Widgets
	QLabel *logo;
	QLabel *serverSetupText;
	KLineEdit *fileEdit;
};


class DataInitializePage:public QWidget{
public:
	// Methods
	DataInitializePage(QWidget *parent);
	bool doInitialization(void);
	bool doUSDAImport(void);

private:
	// Widgets
	QLabel *logo;
	QLabel *initializeText;
	QLabel *USDAImportText;
	QCheckBox *initializeCheckBox;
	QCheckBox *USDAImportCheckBox;

};

class SavePage:public QWidget{
public:
	// Methods
	SavePage(QWidget *parent);
private:
	// Widgets
	QLabel *logo;
	QLabel *saveText;

};

class DBTypeSetupPage:public QWidget{

Q_OBJECT

public:
	// Methods
	DBTypeSetupPage(QWidget *parent);
	int dbType(void);
private:
	// Widgets
	QLabel *dbTypeSetupText;
	QLabel *logo;
	QVButtonGroup *bg;
	QRadioButton *liteCheckBox;
	QRadioButton *mysqlCheckBox;
	QRadioButton *psqlCheckBox;
private slots:
	void setPages(int rb); // hides/shows pages given the radio button state
signals:
	void showPages(DBType);
};

#endif
