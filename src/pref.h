/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#ifndef _KRECIPESPREF_H_
#define _KRECIPESPREF_H_

#include <kdialogbase.h>
#include <klineedit.h>
#include <qframe.h>

class ServerPrefs;
class KrecipesPrefPageTwo;
class UnitsPrefs;

class QButtonGroup;
class QRadioButton;
class QVBoxLayout;

class KrecipesPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KrecipesPreferences(QWidget *parent);

private:
    ServerPrefs *m_pageServer;
    KrecipesPrefPageTwo *m_pageTwo;
    UnitsPrefs *m_pageUnits;
private slots:
    void saveSettings(void);
};

class ServerPrefs : public QWidget
{
    Q_OBJECT
public:
    ServerPrefs(QWidget *parent = 0);

    // Public Methods
    void saveOptions(void);
private:
    // Internal Widgets
    KLineEdit *serverEdit;
    KLineEdit *usernameEdit;
    KLineEdit *passwordEdit;
    KLineEdit *dbNameEdit;


};

class KrecipesPrefPageTwo : public QWidget
{
    Q_OBJECT
public:
    KrecipesPrefPageTwo(QWidget *parent = 0);

};

class UnitsPrefs : public QWidget
{
Q_OBJECT

public:
	UnitsPrefs(QWidget *parent = 0);

	void saveOptions();

protected:
	QButtonGroup* numberButtonGroup;
	QRadioButton* fractionRadioButton;
	QRadioButton* decimalRadioButton;

	QVBoxLayout* Form1Layout;
	QVBoxLayout* numberButtonGroupLayout;

protected slots:
	virtual void languageChange();
};

#endif // _KRECIPESPREF_H_
