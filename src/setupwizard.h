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

#include <kwizard.h>
#include <qlabel.h>
#include <qlineedit.h>

/**
@author Unai Garro
*/

class WelcomePage;

class SetupWizard:public KWizard{
public:

     SetupWizard(QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
    ~SetupWizard();
private:
	// Widgets
	WelcomePage *welcomePage;



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

#endif
