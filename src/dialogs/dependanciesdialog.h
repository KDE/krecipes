/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DEPENDANCIESDIALOG_H
#define DEPENDANCIESDIALOG_H

#include <q3groupbox.h>
#include <QLabel>
//Added by qt3to4:
#include <Q3ValueList>

#include <k3listview.h>
#include <K3ListBox>
#include <kdialog.h>

#include "datablocks/elementlist.h"

struct ListInfo {
	ElementList list;
	QString name;
};

/**
@author Unai Garro
*/
class DependanciesDialog: public KDialog
{
public:
	//Methods
	DependanciesDialog( QWidget *parent, const Q3ValueList<ListInfo> &lists, bool deps_are_deleted = true );
	DependanciesDialog( QWidget *parent, const ListInfo &list, bool deps_are_deleted = true );

	~DependanciesDialog();

	void setCustomWarning( const QString &msg ){ m_msg = msg; }

public slots:
	void accept();

private:
	//Widgets
	QLabel *instructionsLabel;

	bool m_depsAreDeleted;
	QString m_msg;

	// Methods
	void init( const Q3ValueList<ListInfo> &lists );
	void loadList( K3ListBox*, const ElementList &list );
};

#endif
