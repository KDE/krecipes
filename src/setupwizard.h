/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <qlabel.h>

#include <kwizard.h>
#include <klineedit.h>
#include <ktextedit.h>


/**
@author Unai Garro
*/

class WelcomePage;
class ServerSetupPage;
class SavePage;

class SetupWizard:public KWizard{
public:

     SetupWizard(QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
    ~SetupWizard();
private:
	// Widgets
	WelcomePage *welcomePage;
	ServerSetupPage *serverSetupPage;
	SavePage *savePage;

};

class WelcomePage:public QWidget{
public:
	// Methods
	WelcomePage(QWidget *parent);

private:
	// Widgets
	QLabel *logo;
	KTextEdit *welcomeText;

};

class ServerSetupPage:public QWidget{
public:
	// Methods
	ServerSetupPage(QWidget *parent);

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
	KTextEdit *saveText;

};

#endif
