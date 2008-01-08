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

#ifndef SELECTUNITDIALOG_H
#define SELECTUNITDIALOG_H

#include <qwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qvbox.h>

#include <klistview.h>
#include <kdialogbase.h>

#include "datablocks/unit.h"

/**
@author Unai Garro
*/
class SelectUnitDialog : public KDialogBase
{
public:
	typedef enum OptionFlag { ShowEmptyUnit, HideEmptyUnit } ;

	SelectUnitDialog( QWidget* parent, const UnitList &unitList, OptionFlag = ShowEmptyUnit );

	~SelectUnitDialog();

	int unitID( void );

private:
	//Widgets
	QGroupBox *box;
	KListView *unitChooseView;
	OptionFlag m_showEmpty;

	void loadUnits( const UnitList &unitList );

};

#endif
