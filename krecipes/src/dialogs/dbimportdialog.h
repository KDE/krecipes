/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef DBIMPORTDIALOG_H
#define DBIMPORTDIALOG_H

#include <kdialogbase.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class Q3ButtonGroup;
class QRadioButton;
class Q3WidgetStack;
class QWidget;
class QLineEdit;
class QLabel;
class QPushButton;

class KUrlRequester;
class KIntNumInput;

/**
@author Jason Kivlighn
*/

class DBImportDialog: public KDialogBase
{
	Q_OBJECT
public:
	DBImportDialog( QWidget *parent = 0, const char *name = 0 );

	QString dbType() const;
	void serverParams( QString &host, QString &user, QString &pass, int &port, QString &table ) const;
	QString dbFile() const;

private:
	// Widgets
	Q3ButtonGroup* dbButtonGroup;
	QRadioButton* liteRadioButton;
	QRadioButton* mysqlRadioButton;
	QRadioButton* psqlRadioButton;
	Q3WidgetStack* paramStack;
	QWidget* serverPage;
	QLineEdit* nameEdit;
	QLabel* passwordLabel;
	QLineEdit* hostEdit;
	QLineEdit* passwordEdit;
	QLineEdit* userEdit;
	QLabel* userLabel;
	QLabel* hostLabel;
	QLabel* nameLabel;
	QLabel* portLabel;
	KIntNumInput *portEdit;
	QWidget* sqlitePage;
	KUrlRequester* sqliteDBRequester;

	Q3VBoxLayout* dbButtonGroupLayout;
	Q3VBoxLayout* serverPageLayout;
	Q3GridLayout* layout5;
	Q3VBoxLayout* serverPageLayout_2;

protected slots:
	void languageChange();
	void switchDBPage( int id );

};

#endif
