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

#include "../widgets/dragarea.h"
#include "../widgets/sizehandle.h"

#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>

#include <cmath>

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : QDialog(parent,0,true)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	DragArea *dragArea = new DragArea(this);
	layout->addWidget(dragArea);

	title_box = new QLabel(sample.title,dragArea);
	instr_box = new QLabel(sample.instructions,dragArea);
	servings_box = new QLabel(QString(i18n("Servings: %1")).arg(sample.persons),dragArea);

	setFixedSize(500,600);

	loadSetup();
}

void PageSetupDialog::loadSetup()
{
	KConfig *config=kapp->config();
	config->setGroup("PageSetup");


}
