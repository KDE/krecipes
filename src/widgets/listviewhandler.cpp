/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "listviewhandler.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>

ListViewHandler::ListViewHandler( QObject *parent, int t ) : QObject(parent),
  curr_offset(0),
  total(t)
{
	KConfig * config = KGlobal::config();config->setGroup( "Advanced" );
	curr_limit = config->readNumEntry( "Limit", -1 );
}

void ListViewHandler::increaseTotal()
{
	total++;
}

void ListViewHandler::decreaseTotal()
{
	total--;
}

bool ListViewHandler::eventFilter( QObject *, QEvent *e )
{
	if ( e->type() == QEvent::KeyPress ) {
		// special processing for key press
		QKeyEvent *k = (QKeyEvent *)e;
		if ( k->state() == Qt::ShiftButton ) {
			switch ( k->key() ) {
			case Qt::Key_N: {
				if ( curr_offset + curr_limit >= total )
					return true;

				curr_offset += curr_limit;

				kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
				emit reload(curr_limit,curr_offset);
				return true; // eat event
			}
			case Qt::Key_P: {
				curr_offset -= curr_limit;
				if ( curr_offset >= 0 ) {
					kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
					emit reload(curr_limit,curr_offset);
				}
				else
					curr_offset = 0;
				return true; // eat event
			}
			default: break;
			}
		}
	}
	
	return false; //pass the event on
}

#include "listviewhandler.moc"
