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

#include <qlabel.h>
#include <qcheckbox.h>
#include <kwizard.h>
#include <klineedit.h>
#include <ktextedit.h>


/**
@author Unai Garro
*/

class WelcomePage;
class PermissionsSetupPage;
class ServerSetupPage;
class SavePage;

class SetupWizard:public KWizard{
Q_OBJECT
public:

     SetupWizard(QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
    ~SetupWizard();
    void getOptions(bool &setupUser, bool &initializeData);
private:
	// Widgets
	WelcomePage *welcomePage;
	PermissionsSetupPage *permissionsSetupPage;
	ServerSetupPage *serverSetupPage;
	SavePage *savePage;

private slots:
	void save(void);

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
	bool doUserSetup();
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
	QString server(void);
	QString user(void);
	QString password(void);
	QString dbName(void);
private:
	// Widgets
	QLabel *logo;
	KLineEdit *serverEdit;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;



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

#endif
