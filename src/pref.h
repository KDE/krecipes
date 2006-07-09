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

#include <kdialogbase.h>

class ServerPrefs;
class NumbersPrefs;
class ImportPrefs;
class PerformancePrefs;

class KIntNumInput;
class KLineEdit;
class KURLRequester;

class QButtonGroup;
class QCheckBox;
class QRadioButton;
class QVBoxLayout;
class QComboBox;

class KrecipesPreferences : public KDialogBase
{
	Q_OBJECT
public:
	KrecipesPreferences( QWidget *parent );

private:
	ServerPrefs *m_pageServer;
	NumbersPrefs *m_pageNumbers;
	ImportPrefs *m_pageImport;
	PerformancePrefs *m_pagePerformance;

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
	KURLRequester *dumpPathRequester;
	KURLRequester *mysqlPathRequester;

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
	KURLRequester *dumpPathRequester;
	KURLRequester *psqlPathRequester;

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
	KURLRequester *dumpPathRequester;
	KURLRequester *fileRequester;
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
	QButtonGroup* numberButtonGroup;
	QRadioButton* fractionRadioButton;
	QRadioButton* decimalRadioButton;

	QVBoxLayout* Form1Layout;
	QVBoxLayout* numberButtonGroupLayout;

	KLineEdit *ingredientEdit;
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
	QVBoxLayout* Form1Layout;
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
	QVBoxLayout* Form1Layout;
	QCheckBox* searchAsYouTypeBox;
	KIntNumInput* catLimitInput;
	KIntNumInput* limitInput;
};

#endif // _KRECIPESPREF_H_
