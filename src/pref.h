/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#ifndef _KRECIPESPREF_H_
#define _KRECIPESPREF_H_

#include <kdialogbase.h>
#include <klineedit.h>
#include <qframe.h>

class ServerPrefs;
class KrecipesPrefPageTwo;

class KrecipesPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KrecipesPreferences();

private:
    ServerPrefs *m_pageServer;
    KrecipesPrefPageTwo *m_pageTwo;
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

class KrecipesPrefPageTwo : public QFrame
{
    Q_OBJECT
public:
    KrecipesPrefPageTwo(QWidget *parent = 0);

};

#endif // _KRECIPESPREF_H_
