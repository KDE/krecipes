/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pagesetupdialog.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>

#include <kiconloader.h>
#include <klocale.h>

#include "setupdisplay.h"

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : QDialog(parent,0,true)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	setup_display = new SetupDisplay(sample,this);
	layout->addWidget(setup_display);

	QHBox *buttonsBox = new QHBox(this);
	KIconLoader *il = new KIconLoader;
	QPushButton *okButton = new QPushButton(il->loadIconSet("ok",KIcon::Small),i18n("&OK"),buttonsBox);
	QPushButton *cancelButton = new QPushButton(il->loadIconSet("cancel",KIcon::Small),i18n("&Cancel"),buttonsBox);
	layout->addWidget(buttonsBox);

	connect( okButton, SIGNAL(clicked()), SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );

	adjustSize();
	setFixedSize(size());
}

void PageSetupDialog::accept()
{
	setup_display->save(); //save to this app's config file

	QDialog::accept();
}
