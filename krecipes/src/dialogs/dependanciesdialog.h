/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DEPENDANCIESDIALOG_H
#define DEPENDANCIESDIALOG_H

#include <qgroupbox.h>
#include <qlabel.h>

#include <klistview.h>
#include <kdialogbase.h>

#include "datablocks/elementlist.h"

typedef struct ListInfo {
	ElementList list;
	QString name;
};

/**
@author Unai Garro
*/
class DependanciesDialog: public KDialogBase
{
public:
	//Methods
	DependanciesDialog( QWidget *parent, const QValueList<ListInfo> &lists, bool deps_are_deleted = true );
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
	void init( const QValueList<ListInfo> &lists );
	void loadList( KListBox*, const ElementList &list );
};

#endif
