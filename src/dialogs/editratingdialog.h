/***************************************************************************
*   Copyright (C) 2005 by Jason Kivlighn                                  *
*   jkivlighn@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef EDITRATINGDIALOG_H
#define EDITRATINGDIALOG_H

#include <qvariant.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class KDoubleSpinBox;
class QPushButton;
class KListView;
class QListViewItem;
class QTextEdit;
class QLineEdit;
class RatingWidget;

class Rating;
class RatingCriteria;

class EditRatingDialog : public KDialog
{
Q_OBJECT
	
public:
	EditRatingDialog( const Rating &, QWidget* parent = 0, const char* name = 0 );
	EditRatingDialog( QWidget* parent = 0, const char* name = 0 );
	~EditRatingDialog();
	
	QLabel* criteriaLabel;
	QComboBox* criteriaComboBox;
	QLabel* starsLabel;
	RatingWidget *starsWidget;
	QPushButton* addButton;
	KListView* criteriaListView;
	QLabel* commentsLabel;
	QTextEdit* commentsEdit;
	QLabel* raterLabel;
	QLineEdit* raterEdit;
	QPushButton* okButton;
	QPushButton* cancelButton;

	Rating rating() const;
	
protected:
	QVBoxLayout* EditRatingDialogLayout;
	QHBoxLayout* layout8;
	QHBoxLayout* layout2;
	QHBoxLayout* layout7;
	QSpacerItem* buttonsSpacer;
	
protected slots:
	virtual void languageChange();
	void slotAddRatingCriteria();
	void slotRemoveRatingCriteria();

private:
	void init();

	void loadRating( const Rating & );
	void addRatingCriteria( const RatingCriteria &rc );
};

#endif // EDITRATINGDIALOG_H
