/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#include "pref.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>

KrecipesPreferences::KrecipesPreferences()
    : KDialogBase(TreeList, i18n("Krecipes Preferences"),
                  Help|Default|Ok|Apply|Cancel, Ok)
{
    // this is the base class for your preferences dialog.  it is now
    // a Treelist dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    QFrame *frame;
    frame = addPage(i18n("Server Settings"), i18n("Database Server Options"));
    m_pageOne = new ServerPrefs(frame);

    frame = addPage(i18n("Appearance"), i18n("Customize Krecipes Appearance"));
    m_pageTwo = new KrecipesPrefPageTwo(frame);
}

ServerPrefs::ServerPrefs(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);
    propertyNameEdit=new KLineEdit(this);
    new QLabel(i18n("Add something here"), this);
}

KrecipesPrefPageTwo::KrecipesPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}
#include "pref.moc"
