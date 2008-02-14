/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef _KRECIPESPREF_H_
#define _KRECIPESPREF_H_

#include <kpagedialog.h>

#include <qmap.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class ServerPrefs;
class NumbersPrefs;
class ImportPrefs;
class PerformancePrefs;

class KIntNumInput;
class KLineEdit;
class KUrlRequester;

class Q3ButtonGroup;
class QCheckBox;
class QRadioButton;
class Q3VBoxLayout;
class QComboBox;

class KrecipesPreferences : public KPageDialog
{
	Q_OBJECT
public:
	KrecipesPreferences( QWidget *parent );

protected slots:
	void slotHelp();

private:
	ServerPrefs *m_pageServer;
	NumbersPrefs *m_pageNumbers;
	ImportPrefs *m_pageImport;
	PerformancePrefs *m_pagePerformance;

	QMap<int,QString> m_helpMap;

private slots:
	void saveSettings( void );
};


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

class PostgreSQLServerPrefs : public QWidget
{
public:
	PostgreSQLServerPrefs( QWidget *parent );

	void saveOptions( void );
private:
	// Internal Widgets
	KUrlRequester *dumpPathRequester;
	KUrlRequester *psqlPathRequester;

	KLineEdit *serverEdit;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;
	KIntNumInput *portEdit;
};

class SQLiteServerPrefs : public QWidget
{
	Q_OBJECT

public:
	SQLiteServerPrefs( QWidget *parent );

	void saveOptions( void );

private:
	// Internal Widgets
	KUrlRequester *dumpPathRequester;
	KUrlRequester *fileRequester;
};


class ServerPrefs : public QWidget
{
	Q_OBJECT
public:
	ServerPrefs( QWidget *parent = 0 );

	// Public Methods
	void saveOptions( void );
private:
	QWidget *serverWidget;
	QCheckBox *wizard_button;
};

class NumbersPrefs : public QWidget
{
	Q_OBJECT

public:
	NumbersPrefs( QWidget *parent = 0 );

	void saveOptions();

protected:
	Q3ButtonGroup* numberButtonGroup;
	QRadioButton* fractionRadioButton;
	QRadioButton* decimalRadioButton;

	Q3VBoxLayout* Form1Layout;
	Q3VBoxLayout* numberButtonGroupLayout;

	QCheckBox *abbrevButton;

protected slots:
	virtual void languageChange();

};

class ImportPrefs : public QWidget
{
	Q_OBJECT

public:
	ImportPrefs( QWidget *parent = 0 );

	void saveOptions();

protected:
	Q3VBoxLayout* Form1Layout;
	QCheckBox* overwriteCheckbox;
	QCheckBox* directImportCheckbox;

	QComboBox *clipBoardFormatComboBox;
};


class PerformancePrefs : public QWidget
{
	Q_OBJECT

public:
	PerformancePrefs( QWidget *parent = 0 );

	void saveOptions();

protected:
	Q3VBoxLayout* Form1Layout;
	QCheckBox* searchAsYouTypeBox;
	KIntNumInput* catLimitInput;
	KIntNumInput* limitInput;
};

#endif // _KRECIPESPREF_H_
