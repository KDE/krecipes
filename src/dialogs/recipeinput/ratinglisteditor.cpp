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
#include "widgets/kwidgetlistbox.h"
#include "backends/recipedb.h"
#include "datablocks/rating.h"

#include <KVBox>
#include <KPushButton>

#include <QPointer>


RatingListEditor::RatingListEditor( RatingList * ratingList, RecipeDB * db,
		QWidget * parent )
	: QWidget( parent ),
	m_ratingList( ratingList ),
	m_database( db )
{
	QVBoxLayout * layout = new QVBoxLayout( this );
	setLayout( layout );

	m_ratingListDisplayWidget = new KWidgetListbox;
	layout->addWidget( m_ratingListDisplayWidget );

	m_addRatingButton = new KPushButton( i18nc("@action:button", "Add Rating...") );
	m_addRatingButton->setIcon( KIcon("list-add") );
	layout->addWidget( m_addRatingButton );

	connect( m_addRatingButton, SIGNAL(clicked()), this, SLOT(addRating()) );

	refresh();
}

void RatingListEditor::refresh()
{
	m_ratingListDisplayWidget->clear();
	RatingList::iterator it;
	for ( it = m_ratingList->begin(); it != m_ratingList->end(); ++it ) {
		//Create the rating display widget
		RatingDisplayWidget *item = new RatingDisplayWidget( this );
		item->rating_it = it;
		item->displayRating( *it );
		//Conect the signals
		connect(item, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
		connect(item, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));
		//Insert it in the widget list
		m_ratingListDisplayWidget->insertItem(item);
	}
	m_ratingListDisplayWidget->ensureCellVisible(0,0);
}

void RatingListEditor::clear()
{
	m_ratingListDisplayWidget->clear();
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
		item->rating_it = --(m_ratingList->end());
		item->displayRating( r );
		connect(item, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
		connect(item, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));

		//Insert the rating widget in the list
		m_ratingListDisplayWidget->insertItem(item,0);

		//Indicate that the recipe changed
		emit changed(); 
	}

	delete ratingDlg;
}

void RatingListEditor::editRating()
{
	RatingDisplayWidget *sender = (RatingDisplayWidget*)QObject::sender();

	ElementList criteriaList;
	m_database->loadRatingCriterion(&criteriaList);

	QPointer<EditRatingDialog> ratingDlg = new EditRatingDialog (criteriaList,*sender->rating_it,this);
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
		(*sender->rating_it) = r;

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
	m_ratingList->erase(sender->rating_it);

	disconnect(sender, SIGNAL(editButtonClicked()), this, SLOT(editRating()));
	disconnect(sender, SIGNAL(removeButtonClicked()), this, SLOT(removeRating()));

	//FIXME: sender is removed but never deleted (sender->deleteLater() doesn't work)
	m_ratingListDisplayWidget->removeItem(sender);

	emit changed(); //Indicate that the recipe changed
}

