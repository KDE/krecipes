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
#include <kcursor.h>
#include <kglobal.h>
#include <klocale.h>

#define PREVLISTITEM_RTTI 1002
#define NEXTLISTITEM_RTTI 1003


//These two classes are used to identify the "Next" and "Prev" items, which are identified through rtti().  This also prevents renaming, even if it is enabled.
class PrevListViewItem : public QListViewItem
{
public:
	PrevListViewItem( QListView *parent ) : QListViewItem(parent){}

	virtual int rtti() const { return PREVLISTITEM_RTTI; }

	QString text( int c ) const {
		if ( c == 0 ) {
			return QString("<< %1").arg(i18n("Prev"));
		}
		else
			return QString::null;
	}
};

class NextListViewItem : public QListViewItem
{
public:
	NextListViewItem( QListView *parent, QListViewItem *after ) : QListViewItem(parent,after){}

	virtual int rtti() const { return NEXTLISTITEM_RTTI; }

	QString text( int c ) const {
		if ( c == 0 ) {
			return QString("%1 >>").arg(i18n("Next"));
		}
		else
			return QString::null;
	}
};

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

	connect(this,SIGNAL(executed(QListViewItem*)),SLOT(slotDoubleClicked(QListViewItem*)));
}

void DBListViewBase::activatePrev()
{
	if ( curr_offset != 0 ) {
		curr_offset -= curr_limit;
		if ( curr_offset < 0 )
			curr_offset = 0;
	
		reload();
		emit prevGroupLoaded();
	}
}

void DBListViewBase::activateNext()
{
	curr_offset += curr_limit;

	reload();
	emit nextGroupLoaded();
}

void DBListViewBase::rename( QListViewItem *it, int c )
{
	if ( it->rtti() == PREVLISTITEM_RTTI || it->rtti() == NEXTLISTITEM_RTTI ) {
		return;
	}

	KListView::rename(it,c);
}

void DBListViewBase::slotDoubleClicked( QListViewItem *it )
{
	if ( it->rtti() == PREVLISTITEM_RTTI ) {
		activatePrev();
	}
	else if ( it->rtti() == NEXTLISTITEM_RTTI ) {
		activateNext();
	}
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

			kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
			activateNext();
			k->accept();
			break;
		}
		case Qt::Key_P: {
			kapp->processEvents(); //if auto-repeating, user won't otherwise see change in the listview
			activatePrev();
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
	KApplication::setOverrideCursor( KCursor::waitCursor() );

	clear(); kapp->processEvents();

	lastElement = 0;

	bulk_load=true;
	load(curr_limit,curr_offset);
	bulk_load=false;

	if ( curr_limit != -1 && curr_offset + curr_limit < total )
		new NextListViewItem(this,lastElement);

	if ( curr_offset != 0 )
		new PrevListViewItem(this);

	KApplication::restoreOverrideCursor();
}

void DBListViewBase::createElement( QListViewItem *it )
{
	Q_ASSERT(it);

	if ( bulk_load ) { //this can be much faster if we know the elements are already in order
		if ( lastElement ) it->moveItem(lastElement);
		lastElement = it;
	}
	else {
		if ( lastElement == 0 ) {
			lastElement = it;
		}
		else {
			int c = 0;//FIXME: the column used should be variable (set by a subclass)

			//start it out below the "Prev" item... currently it will be at firstChild()
			if ( firstChild()->nextSibling() && firstChild()->nextSibling()->rtti() == PREVLISTITEM_RTTI ) {
				it->moveItem( firstChild()->nextSibling() );
			}

			if ( QString::localeAwareCompare(it->text(c),lastElement->text(c)) >= 0 ) {
				it->moveItem(lastElement);
				lastElement = it;
			}
			else {
				QListViewItem *last_it = 0;

				for ( QListViewItem *search_it = it; search_it; search_it = search_it->nextSibling() ) {
					if ( search_it->rtti() == NEXTLISTITEM_RTTI ) {
						it->moveItem(lastElement);
						lastElement = it;
					}
					else if ( QString::localeAwareCompare(it->text(c),search_it->text(c)) < 0 ) { //we assume the list is sorted, as it should stay
						if ( last_it ) it->moveItem(last_it);
						break;
					}
					last_it = search_it;
				}
			}
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

	int child_count = childCount();
	if ( firstChild()->rtti() == PREVLISTITEM_RTTI ){ child_count--; } //"Prev" item
	if ( lastElement->nextSibling() ){ child_count--; } //"Next" item

	if ( curr_limit != -1 && child_count >= curr_limit ) {
		QListViewItem *firstElement = firstChild();
		if (firstElement->rtti() == PREVLISTITEM_RTTI) {
			firstElement = firstElement->nextSibling();
		}
		else if ( name < firstElement->text(c) ) { //provide access to this new element if we need to
			new PrevListViewItem(this);
			curr_offset++;
			return false;
		}

		if ( name < firstElement->text(c) ) {
			curr_offset++;
			return false;
		}
		else if ( name >= lastElement->text(c) ) {
			return false;
		}
	}
		
	return true;
}

#include "dblistviewbase.moc"
