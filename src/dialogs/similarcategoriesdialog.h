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
#include <q3valuevector.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3ValueList>
#include <Q3HBoxLayout>
#include <QLabel>

#include "datablocks/elementlist.h"

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QSlider;
class QLabel;
class QPushButton;

class K3ListView;
class KLineEdit;


class SimilarCategoriesDialog : public QDialog
{
Q_OBJECT
	
public:
	SimilarCategoriesDialog( ElementList &, QWidget* parent = 0 );
	~SimilarCategoriesDialog();

	Q3ValueList<int> matches() const;
	QString element() const;

	KLineEdit* categoriesBox;
	QSlider* thresholdSlider;
	QLabel* thresholdLabel;
	QLabel* categoryLabel;
	QPushButton* searchButton;
	QLabel* allLabel;
	K3ListView* allListView;
	QPushButton* removeButton;
	QPushButton* addButton;
	QLabel* toMergeLabel;
	K3ListView* toMergeListView;
	QPushButton* mergeButton;
	QPushButton* cancelButton;
	
protected:
	Q3VBoxLayout* SimilarCategoriesDialogLayout;
	Q3HBoxLayout* layout6;
	Q3GridLayout* layout4;
	Q3VBoxLayout* layout5;
	QSpacerItem* spacer4;
	Q3HBoxLayout* layout9;
	Q3VBoxLayout* layout8;
	Q3VBoxLayout* layout1;
	QSpacerItem* spacer1;
	Q3VBoxLayout* layout7;
	Q3HBoxLayout* layout10;
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
