/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <qdialog.h>
#include <qmap.h>

#include "../recipe.h"

class SetupDisplay;

/**
  * @author Jason Kivlighn
  */
class PageSetupDialog : public QDialog
{
Q_OBJECT

public:
	PageSetupDialog( QWidget *parent, const Recipe &sample );
	~PageSetupDialog(){}

protected:
	virtual void accept();
	void save();

private:
	SetupDisplay *setup_display;
};

#endif //PAGESETUPDIALOG_H
