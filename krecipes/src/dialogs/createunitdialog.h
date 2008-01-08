/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEUNITDIALOG_H
#define CREATEUNITDIALOG_H

#include <kdialogbase.h>
#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <q3vbox.h>

#include "datablocks/unit.h"

class KComboBox;
class KLineEdit;

/**
@author Unai Garro
*/
class CreateUnitDialog : public KDialogBase
{
Q_OBJECT

public:
	CreateUnitDialog( QWidget *parent, const QString &name = QString::null, const QString &plural = QString::null, const QString &name_abbrev = QString::null, const QString &plural_abbrev = QString::null, bool newUnit = true );
	~CreateUnitDialog();
	Unit newUnit( void );

protected slots:
	void nameAbbrevTextChanged(const QString &);

private:
	//Widgets
	Q3GroupBox *box;
	KLineEdit *nameEdit;
	KLineEdit *pluralEdit;
	KLineEdit *nameAbbrevEdit;
	KLineEdit *pluralAbbrevEdit;
	KComboBox *typeComboBox;
};

#endif
