/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEELEMENTDIALOG_H
#define CREATEELEMENTDIALOG_H

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qvbox.h>

#include <klineedit.h>
#include <kdialogbase.h>

/**
@author Unai Garro
*/
class CreateElementDialog : public KDialogBase
{
public:
	CreateElementDialog( QWidget *parent, const QString &text );
	~CreateElementDialog();
	QString newElementName( void );

private:
	//Widgets
	QGroupBox *box;
	KLineEdit *elementEdit;
};

#endif
