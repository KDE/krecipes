/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECIPESPREF_H
#define KRECIPESPREF_H

#include <kpagedialog.h>

#include <qmap.h>
//Added by qt3to4:
#include <QVBoxLayout>

namespace Sonnet {
	class ConfigWidget;
}

class ServerPrefs;
class NumbersPrefs;
class ImportPrefs;
class PerformancePrefs;
class SpellCheckingPrefs;

class KIntNumInput;
class KLineEdit;
class KUrlRequester;

class QGroupBox;
class QCheckBox;
class QRadioButton;
class QVBoxLayout;
class KComboBox;

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
	SpellCheckingPrefs *m_pageSpellChecking;

	QMap<QString,QString> m_helpMap;

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
	KUrlRequester *oldPathRequester;
	KUrlRequester *newPathRequester;
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
	QGroupBox* numberButtonGroup;
	QRadioButton* fractionRadioButton;
	QRadioButton* decimalRadioButton;

	QVBoxLayout* Form1Layout;
	QVBoxLayout* numberButtonGroupLayout;

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

	KComboBox *clipBoardFormatComboBox;
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

class SpellCheckingPrefs : public QWidget
{
	Q_OBJECT
public:
	SpellCheckingPrefs( QWidget *parent = 0 );

	void saveOptions();
protected:
	Sonnet::ConfigWidget *m_confPage;
};

#endif // KRECIPESPREF_H
