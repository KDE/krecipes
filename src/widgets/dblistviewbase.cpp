/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dblistviewbase.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>

DBListViewBase::DBListViewBase( QWidget *parent, RecipeDB *db, int t ) : KListView(parent),
  database(db),
  curr_offset(0),
  total(t),
  bulk_load(false),
  lastElement(0)
{
	setSorting(-1);

	KConfig * config = KGlobal::config();config->setGroup( "Advanced" );
	curr_limit = config->readNumEntry( "Limit", -1 );
}

void DBListViewBase::keyPressEvent( QKeyEvent *k )
{
	if ( k->state() == Qt::ShiftButton ) {
		switch ( k->key() ) {
		case Qt::Key_N: {
			if ( curr_offset + curr_limit >= total ) {
				k->accept();
				return;
			}

			curr_offset += curr_limit;

			kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
			reload();
			k->accept();
			break;
		}
		case Qt::Key_P: {
			curr_offset -= curr_limit;
			if ( curr_offset >= 0 ) {
				kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
				reload();
			}
			else
				curr_offset = 0;
			k->accept();
			break;
		}
		default: break;
		}
	}

	k->ignore();
}

void DBListViewBase::reload()
{
	clear();
	lastElement = 0;

	bulk_load=true;
	load(curr_limit,curr_offset);
	bulk_load=false;

	if ( curr_offset + curr_limit < total )
		new QListViewItem(this,lastElement,"Next >>"); //FIXME: Note to self: i18n'ize this when I'm ready

	if ( curr_offset != 0 )
		new QListViewItem(this,"Prev <<"); //FIXME: Note to self: i18n'ize this when I'm ready
	
}

void DBListViewBase::createElement( QListViewItem *it )
{
	Q_ASSERT(it);

	if ( bulk_load ) { //this can be much faster if we know the elements are already in order
		if ( lastElement ) it->moveItem(lastElement);
		lastElement = it;
	}
	else {
		int c = 0;//FIXME: the column used should be variable (set by a subclass)

		QListViewItem *last_it = 0;
		
		//skip the "Next" item
		for ( QListViewItem *search_it = firstChild()->nextSibling(); search_it; search_it = search_it->nextSibling() ) {
			if ( search_it == lastElement->nextSibling() ) {
				it->moveItem(lastElement);
				lastElement = it;
			}
			else if ( it->text(c) < search_it->text(c) ) { //we assume the list is sorted, as it should stay
				if ( last_it ) it->moveItem(last_it);
				if ( search_it == it ) lastElement=it;
				break;
			}
			last_it = search_it;
		}
	}
}

void DBListViewBase::removeElement( QListViewItem *it )
{
	if ( !it ) return;

	if ( it == lastElement ) {
		for ( QListViewItem *search_it = firstChild(); search_it->nextSibling(); search_it = search_it->nextSibling() ) {
			if ( lastElement == search_it->nextSibling() )
				lastElement = search_it; 
		}
	}
	delete it;
}

void DBListViewBase::elementCreated()
{
	total++;
}

void DBListViewBase::elementRemoved()
{
	total--;
}

bool DBListViewBase::handleElement( const QString &name )
{
	int c = 0;//FIXME: the column used should be variable (set by a subclass)

	if ( childCount()-2 >= curr_limit ) {
		if ( name < firstChild()->text(c) || name >= lastElement->text(c) ) {
			return false;
		}
		else
			return true;
	}
	else
		return true;
}

#include "dblistviewbase.moc"
