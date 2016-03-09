/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ratinglisteditor.h"

#include "dialogs/recipeinput/editratingdialog.h"
#include "dialogs/recipeinput/ratingdisplaywidget.h"
#include "backends/recipedb.h"
#include "datablocks/rating.h"

#include <KPushButton>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QFrame>
#include <QPointer>


class MyScrollArea : public QScrollArea
{

public:
	void resizeEvent(QResizeEvent * event)
	{
		Q_UNUSED( event );
		QList<RatingDisplayWidget*> widgets = findChildren<RatingDisplayWidget*>();
		foreach(RatingDisplayWidget * widget, widgets)
		{
			widget->resize( this->width(), widget->height() );
		}
		int delta;
		QScrollBar * scrollBar = verticalScrollBar();
		if ( scrollBar->isVisible() ) {
			delta = verticalScrollBar()->width();
		} else {
			delta = 0;
		}
		widget()->resize( this->width() - delta, widget()->height() );
	};

};

RatingListEditor::RatingListEditor( RatingList * ratingList, RecipeDB * db,
		QWidget * parent )
	: QWidget( parent ),
	m_ratingList( ratingList ),
	m_database( db )
{
	//General layout
	QVBoxLayout * layout = new QVBoxLayout( this );
	setLayout( layout );


	//The containers for the various RatingDisplayWidget's
	m_scrollArea = new MyScrollArea;
	m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	m_frame = new QFrame;
	m_ratingsLayout = new QVBoxLayout;
	m_frame->setLayout( m_ratingsLayout );

	m_scrollArea->setWidget( m_frame );

	layout->addWidget( m_scrollArea );


	//Add rating button
	m_addRatingButton = new KPushButton( i18nc("@action:button", "Add Rating...") );
	m_addRatingButton->setIcon( KIcon("list-add") );
	layout->addWidget( m_addRatingButton );
	m_addRatingButton->show();
	connect( m_addRatingButton, SIGNAL(clicked()), this, SLOT(addRating()) );


	//Display the rating list in the GUI
	refresh();
}

void RatingListEditor::refresh()
{
	clear();
	RatingList::iterator it;
	for ( it = m_ratingList->begin(); it != m_ratingList->end(); ++it ) {
		//Create the rating display widget
		RatingDisplayWidget *item = new RatingDisplayWidget( this );
		m_ratingListIteratorMap[item] = it;
		item->displayRating( *it );
		//Conect the signals
		connect(item, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
		connect(item, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));
		//Insert it in the scroll area
		m_ratingsLayout->addWidget( item );
		m_frame->resize( item->width(), m_frame->height() + item->height() );
	}
        m_scrollArea->resize( m_scrollArea->size() );
}

void RatingListEditor::showEvent( QShowEvent * event ) {
	Q_UNUSED( event );
	// This is needed to adjust properly the size of
	// the QFrame containing the various RatinDisplayWidget's
	m_scrollArea->resizeEvent( 0 );
}

void RatingListEditor::clear()
{
	//Clear all the rating widgets
	QList<RatingDisplayWidget*> widgets =
		m_frame->findChildren<RatingDisplayWidget*>();
	foreach(RatingDisplayWidget * widget, widgets)
	{
		delete widget;
	}
	m_frame->resize( 0, 0 );
}

void RatingListEditor::addRating()
{
	//Load the criteria list from the database
	ElementList criteriaList;
	m_database->loadRatingCriterion(&criteriaList);

	QPointer<EditRatingDialog> ratingDlg = new EditRatingDialog( criteriaList, this );
	if ( ratingDlg->exec() == QDialog::Accepted ) {

		//If the rating criteria doesn't exist, create it in the database
		//FIXME: This should be handled by RecipeDB when saving the recipe
		Rating r = ratingDlg->rating();
		foreach( RatingCriteria rc, r.ratingCriterias() ) {
			int criteria_id = m_database->findExistingRatingByName(rc.name());
			if ( criteria_id == RecipeDB::InvalidId ) {
				criteria_id = m_database->createNewRating(rc.name());
			}
			r.setIdOfRatingCriteria(rc.name(), criteria_id);
		}

		//Append the rating to the edited list
		m_ratingList->append(r);

		//Create the rating widget.
		RatingDisplayWidget *item = new RatingDisplayWidget( this );
		m_ratingListIteratorMap[item] = --(m_ratingList->end());
		item->displayRating( r );
		connect(item, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
		connect(item, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));

		//Insert the rating widget in the scroll area
		m_ratingsLayout->addWidget( item );
		m_frame->resize( m_frame->width(), m_frame->height() + item->height() );

		//Indicate that the recipe changed
		emit changed(); 
	}

	delete ratingDlg;
}

void RatingListEditor::editRating()
{
	RatingDisplayWidget *sender = (RatingDisplayWidget*)QObject::sender();
	RatingList::iterator rating_it = m_ratingListIteratorMap[sender];

	ElementList criteriaList;
	m_database->loadRatingCriterion(&criteriaList);

	QPointer<EditRatingDialog> ratingDlg = new EditRatingDialog (criteriaList,*rating_it,this);
	if ( ratingDlg->exec() == QDialog::Accepted ) {
		Rating r = ratingDlg->rating();

		//If the rating criteria doesn't exist, create it in the database
		//FIXME: This should be handled by RecipeDB when saving the recipe
		foreach ( RatingCriteria rc, r.ratingCriterias() ) {
			int criteria_id = m_database->findExistingRatingByName(rc.name());
			if ( criteria_id == -1 ) {
				criteria_id = m_database->createNewRating(rc.name());
			}
			r.setIdOfRatingCriteria(rc.name(), criteria_id);
		}

		//Update the rating values.
		*rating_it = r;

		//Show the changes in the GUI
		sender->displayRating( r );

		//Indicate that the recipe changed
		emit changed();
	}

	delete ratingDlg;
}

void RatingListEditor::removeRating()
{
	RatingDisplayWidget *sender = (RatingDisplayWidget*)QObject::sender();
	m_ratingList->erase( m_ratingListIteratorMap[sender] );

	disconnect(sender, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
	disconnect(sender, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));

        m_frame->resize( m_frame->width(), m_frame->height() - sender->height() );
        m_scrollArea->resize( m_scrollArea->size() );
	sender->deleteLater();

	emit changed(); //Indicate that the recipe changed
}

