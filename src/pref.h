/*
* Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
*/

#ifndef _KRECIPESPREF_H_
#define _KRECIPESPREF_H_

#include <kdialogbase.h>
#include <klineedit.h>
#include <qframe.h>

class ServerPrefs;
class NumbersPrefs;
class ImportPrefs;

class QButtonGroup;
class QCheckBox;
class QRadioButton;
class QVBoxLayout;

class KrecipesPreferences : public KDialogBase
{
	Q_OBJECT
public:
	KrecipesPreferences( QWidget *parent );

private:
	ServerPrefs *m_pageServer;
	NumbersPrefs *m_pageNumbers;
	ImportPrefs *m_pageImport;

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
	KLineEdit *serverEdit;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;
};

class PostgreSQLServerPrefs : public QWidget
{
public:
	PostgreSQLServerPrefs( QWidget *parent );

	void saveOptions( void );
private:
	// Internal Widgets
	KLineEdit *serverEdit;
	KLineEdit *usernameEdit;
	KLineEdit *passwordEdit;
	KLineEdit *dbNameEdit;
};

class SQLiteServerPrefs : public QWidget
{
	Q_OBJECT

public:
	SQLiteServerPrefs( QWidget *parent );

	void saveOptions( void );

private slots:
	void selectFile();

private:
	// Internal Widgets
	KLineEdit *fileEdit;
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
};

#endif // _KRECIPESPREF_H_
