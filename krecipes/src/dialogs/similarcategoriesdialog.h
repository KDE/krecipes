/***************************************************************************
*   Copyright (C) 2003-2006 Jason Kivlighn (jkivlighn@gmail.com)          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SIMILARCATEGORIESDIALOG_H
#define SIMILARCATEGORIESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qvaluevector.h>

#include "datablocks/elementlist.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QSlider;
class QLabel;
class QPushButton;
class QListViewItem;

class KListView;
class KLineEdit;

class RecipeDB;

class SimilarCategoriesDialog : public QDialog
{
Q_OBJECT
	
public:
	SimilarCategoriesDialog( ElementList &, QWidget* parent = 0 );
	~SimilarCategoriesDialog();

	QValueList<int> matches() const;
	QString element() const;

	KLineEdit* categoriesBox;
	QSlider* thresholdSlider;
	QLabel* thresholdLabel;
	QLabel* categoryLabel;
	QPushButton* searchButton;
	QLabel* allLabel;
	KListView* allListView;
	QPushButton* removeButton;
	QPushButton* addButton;
	QLabel* toMergeLabel;
	KListView* toMergeListView;
	QPushButton* mergeButton;
	QPushButton* cancelButton;
	
protected:
	QVBoxLayout* SimilarCategoriesDialogLayout;
	QHBoxLayout* layout6;
	QGridLayout* layout4;
	QVBoxLayout* layout5;
	QSpacerItem* spacer4;
	QHBoxLayout* layout9;
	QVBoxLayout* layout8;
	QVBoxLayout* layout1;
	QSpacerItem* spacer1;
	QVBoxLayout* layout7;
	QHBoxLayout* layout10;
	QSpacerItem* spacer2;
	
protected slots:
	virtual void languageChange();
	void findMatches();
	void mergeMatches();
	void addCategory();
	void removeCategory();

private:
	ElementList m_elementList;

};

#endif // SIMILARCATEGORIESDIALOG_H
