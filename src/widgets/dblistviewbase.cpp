/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
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
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprogressdialog.h>
//Added by qt3to4:
#include <QKeyEvent>


//These two classes are used to identify the "Next" and "Prev" items, which are identified through rtti().  This also prevents renaming, even if it is enabled.
class PrevListViewItem : public Q3ListViewItem
{
public:
	PrevListViewItem( Q3ListView *parent ) : Q3ListViewItem(parent){}

	virtual int rtti() const { return PREVLISTITEM_RTTI; }

	QString text( int c ) const {
		if ( c == 0 ) {
			return QString("<< %1").arg(i18nc("Previous item", "Previous"));
		}
		else
			return QString();
	}
};

class NextListViewItem : public Q3ListViewItem
{
public:
	NextListViewItem( Q3ListView *parent, Q3ListViewItem *after ) : Q3ListViewItem(parent,after){}

	virtual int rtti() const { return NEXTLISTITEM_RTTI; }

	QString text( int c ) const {
		if ( c == 0 ) {
			return QString("%1 >>").arg(i18nc("Next item", "Next"));
		}
		else
			return QString();
	}
};

DBListViewBase::DBListViewBase( QWidget *parent, RecipeDB *db, int t ) : K3ListView(parent),
	database(db),
	curr_limit(-1),
	curr_offset(0),
	total(t),
	bulk_load(false),
	delete_me_later(0),
	m_progress(0),
	m_totalSteps(0)
{
	setSorting(-1);

	if ( curr_limit == -1 ) { //only use the default limit if a subclass hasn't given curr_limit its own value
		KConfigGroup config = KGlobal::config()->group( "Performance" );
		curr_limit = config.readEntry( "Limit", -1 );
	}

	connect(this,SIGNAL(executed(Q3ListViewItem*)),SLOT(slotDoubleClicked(Q3ListViewItem*)));
}

DBListViewBase::~DBListViewBase()
{
	delete m_progress;
}

void DBListViewBase::activatePrev()
{
	if ( curr_offset != 0 ) {
		curr_offset -= curr_limit;
		if ( curr_offset < 0 )
			curr_offset = 0;
	
		reload(ForceReload);
		emit prevGroupLoaded();
	}
}

void DBListViewBase::activateNext()
{
	curr_offset += curr_limit;

	reload(ForceReload);
	emit nextGroupLoaded();
}

void DBListViewBase::rename( Q3ListViewItem *it, int c )
{
	if ( it->rtti() == PREVLISTITEM_RTTI || it->rtti() == NEXTLISTITEM_RTTI ) {
		return;
	}

	K3ListView::rename(it,c);
}

void DBListViewBase::slotDoubleClicked( Q3ListViewItem *it )
{
	//we can't delete the item the was double clicked
	//and yet these functions will clear() the listview.
	//We'll take the item from the view so it isn't deleted
	//and delete it ourselves.
	delete delete_me_later; delete_me_later = 0;

	if ( it->rtti() == PREVLISTITEM_RTTI ) {
		delete_me_later = it;
		takeItem(it);
		activatePrev();
	}
	else if ( it->rtti() == NEXTLISTITEM_RTTI ) {
		delete_me_later = it;
		takeItem(it);
		activateNext();
	}
}

void DBListViewBase::keyPressEvent( QKeyEvent *k )
{
	if ( k->state() == Qt::ShiftModifier ) {
		switch ( k->key() ) {
		case Qt::Key_N: {
			if ( curr_offset + curr_limit >= total || curr_limit == -1 ) {
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

	K3ListView::keyPressEvent(k);
}

void DBListViewBase::reload( ReloadFlags flag )
{
	if ( flag == ForceReload || (!firstChild() && flag == Load) || (firstChild() && flag == ReloadIfPopulated) ) {
		KApplication::setOverrideCursor( Qt::WaitCursor );

		//m_progress = new KProgressDialog(this,0,QString(),i18n("Loading..."),true);
		//m_progress->setAllowCancel(false);
		//m_progress->progressBar()->setPercentageVisible(false);
		//m_progress->progressBar()->setTotalSteps( m_totalSteps );
		//m_progress->show();
		//kapp->processEvents();

		//reset some things
		clear();
		lastElementMap.clear();
	
		bulk_load=true;
		load(curr_limit,curr_offset);
		bulk_load=false;
	
		if ( curr_limit != -1 && curr_offset + curr_limit < total )
			new NextListViewItem(this,lastElementMap[0]);
	
		if ( curr_offset != 0 )
			new PrevListViewItem(this);

		//delete m_progress; m_progress = 0;
	
		KApplication::restoreOverrideCursor();
	}
}

void DBListViewBase::setTotalItems(int i)
{
	m_totalSteps = i;
	if ( m_progress ) {
		m_progress->progressBar()->setMaximum( m_totalSteps );
	}
}

void DBListViewBase::createElement( Q3ListViewItem *it )
{
	Q_ASSERT(it);

	Q3ListViewItem *lastElement;
	QMap<Q3ListViewItem*,Q3ListViewItem*>::iterator map_it = lastElementMap.find(it->parent());
	if ( map_it != lastElementMap.end() ) {
		lastElement = map_it.value();
	}
	else
		lastElement = 0;

	if ( bulk_load ) { //this can be much faster if we know the elements are already in order
		if ( lastElement ) it->moveItem(lastElement);
		lastElementMap.insert(it->parent(),it);
		if ( m_progress ) { m_progress->progressBar()->setValue(m_progress->progressBar()->value() +1); }
	}
	else {
		if ( lastElement == 0 ) {
			lastElementMap.insert(it->parent(),it);
		}
		else {
			
			int c = 0;//FIXME: the column used should be variable (set by a subclass)

			if ( it->parent() == 0 ) {
				//start it out below the "Prev" item... currently it will be at firstChild()
				if ( firstChild()->nextSibling() && 
				( firstChild()->nextSibling()->rtti() == PREVLISTITEM_RTTI || 
				firstChild()->nextSibling()->rtti() == 1006 ) ) { //A hack to skip the Uncategorized item
					it->moveItem( firstChild()->nextSibling() );
				}
			}

			if ( QString::localeAwareCompare(it->text(c),lastElement->text(c)) >= 0 ) {
				it->moveItem(lastElement);
				lastElementMap.insert(it->parent(),it);
			}
			else {
				Q3ListViewItem *last_it = 0;

				for ( Q3ListViewItem *search_it = it; search_it; search_it = search_it->nextSibling() ) {
					if ( search_it->rtti() == NEXTLISTITEM_RTTI ) {
						it->moveItem(lastElement);
						lastElementMap.insert(it->parent(),it);
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

void DBListViewBase::removeElement( Q3ListViewItem *it, bool delete_item )
{
	total--;
	if ( !it ) return;

	Q3ListViewItem *lastElement = lastElementMap[it->parent()];
	if ( it == lastElement ) {
		for ( Q3ListViewItem *search_it = (it->parent())?it->parent()->firstChild():firstChild(); search_it->nextSibling(); search_it = search_it->nextSibling() ) {
			if ( it == search_it->nextSibling() ) {
				lastElementMap.insert(it->parent(),search_it);
				lastElement = search_it;
				break;
			}
		}

		if ( lastElement == it || lastElement->rtti() == PREVLISTITEM_RTTI ) { //there are no more items in the view if this happens
			if ( firstChild() && firstChild()->rtti() == PREVLISTITEM_RTTI ) {
				activatePrev();
				it = 0; //keep 'delete it' below from segfault'ing
			}
			else if ( lastElement->nextSibling() && lastElement->nextSibling()->rtti() == NEXTLISTITEM_RTTI ) {
				reload();
				it = 0; //keep 'delete it' below from segfault'ing
			}
			else //the list is now empty, there is no last element
				lastElementMap.remove(it->parent());
		}
	}

	if ( delete_item )
		delete it;
}

bool DBListViewBase::handleElement( const QString &name )
{
	total++;

	Q3ListViewItem *lastElement = lastElementMap[0];

	int c = 0;//FIXME: the column used should be variable (set by a subclass)

	int child_count = childCount();
	if ( child_count == 0 ) return true;

	if ( firstChild()->rtti() == PREVLISTITEM_RTTI || firstChild()->rtti() == 1006 ){ child_count--; } //"Prev" item
	if ( child_count == 0 ) return true;

	if ( lastElement->nextSibling() ){ child_count--; } //"Next" item

	if ( curr_limit != -1 && child_count >= curr_limit ) {
		Q3ListViewItem *firstElement = firstChild();
		if (firstElement->rtti() == PREVLISTITEM_RTTI || firstElement->rtti() == 1006 ) {
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
			if ( lastElement->nextSibling() == 0 )
				new NextListViewItem(this,lastElement);

			return false;
		}
		else {
			removeElement(lastElement);
		}
	}

	return true;
}

#include "dblistviewbase.moc"
