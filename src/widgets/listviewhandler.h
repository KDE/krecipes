/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef LISTVIEWHANDLER_H
#define LISTVIEWHANDLER_H

#include <qobject.h>

class ListViewHandler : public QObject
{
Q_OBJECT

public:
	ListViewHandler( QObject *, int total );

	void emitReload(){ emit reload(curr_limit,curr_offset); }

public slots:
	void increaseTotal();
	void decreaseTotal();

signals:
	void reload(int limit,int offset);

protected:
	bool eventFilter( QObject *o, QEvent *e );

private:
	int curr_limit;
	int curr_offset;
	int total;
};

#endif //LISTVIEWHANDLER_H
