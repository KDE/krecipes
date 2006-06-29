/***************************************************************************
*   Copyright (C) 2003-2006 Jason Kivlighn (jkivlighn@gmail.com)          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "similarcategoriesdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <klineedit.h>

#include "widgets/categorycombobox.h"
#include "backends/recipedb.h"

SimilarCategoriesDialog::SimilarCategoriesDialog( ElementList &list, QWidget* parent )
   : QDialog( parent, "SimilarCategoriesDialog", true ),
     m_elementList(list)
{
	SimilarCategoriesDialogLayout = new QVBoxLayout( this, 11, 6, "SimilarCategoriesDialogLayout"); 
	
	layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 
	
	layout4 = new QGridLayout( 0, 1, 1, 0, 6, "layout4"); 
	
	categoriesBox = new KLineEdit( this );
	
	layout4->addWidget( categoriesBox, 0, 1 );
	
	thresholdSlider = new QSlider( this, "thresholdSlider" );
	thresholdSlider->setValue( 40 );
	thresholdSlider->setOrientation( QSlider::Horizontal );
	
	layout4->addWidget( thresholdSlider, 1, 1 );
	
	thresholdLabel = new QLabel( this, "thresholdLabel" );
	
	layout4->addWidget( thresholdLabel, 1, 0 );
	
	categoryLabel = new QLabel( this, "categoryLabel" );
	
	layout4->addWidget( categoryLabel, 0, 0 );
	layout6->addLayout( layout4 );
	
	layout5 = new QVBoxLayout( 0, 0, 6, "layout5"); 
	
	searchButton = new QPushButton( this, "searchButton" );
	layout5->addWidget( searchButton );
	spacer4 = new QSpacerItem( 20, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout5->addItem( spacer4 );
	layout6->addLayout( layout5 );
	SimilarCategoriesDialogLayout->addLayout( layout6 );
	
	layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 
	
	layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 
	
	allLabel = new QLabel( this, "allLabel" );
	layout8->addWidget( allLabel );
	
	allListView = new KListView( this, "allListView" );
	allListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 1, allListView->sizePolicy().hasHeightForWidth() ) );
	layout8->addWidget( allListView );
	layout9->addLayout( layout8 );
	
	layout1 = new QVBoxLayout( 0, 0, 6, "layout1"); 
	
	removeButton = new QPushButton( this, "removeButton" );
	layout1->addWidget( removeButton );
	
	addButton = new QPushButton( this, "addButton" );
	layout1->addWidget( addButton );
	spacer1 = new QSpacerItem( 20, 61, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout1->addItem( spacer1 );
	layout9->addLayout( layout1 );
	
	layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 
	
	toMergeLabel = new QLabel( this, "toMergeLabel" );
	layout7->addWidget( toMergeLabel );
	
	toMergeListView = new KListView( this, "toMergeListView" );
	toMergeListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 1, toMergeListView->sizePolicy().hasHeightForWidth() ) );
	layout7->addWidget( toMergeListView );
	layout9->addLayout( layout7 );
	SimilarCategoriesDialogLayout->addLayout( layout9 );
	
	layout10 = new QHBoxLayout( 0, 0, 6, "layout10"); 
	spacer2 = new QSpacerItem( 310, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout10->addItem( spacer2 );
	
	mergeButton = new QPushButton( this, "mergeButton" );
	layout10->addWidget( mergeButton );
	
	cancelButton = new QPushButton( this, "cancelButton" );
	layout10->addWidget( cancelButton );
	SimilarCategoriesDialogLayout->addLayout( layout10 );
	languageChange();
	resize( QSize(573, 429).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );

	connect( searchButton, SIGNAL(clicked()), this, SLOT(findMatches()) );
	connect( mergeButton, SIGNAL(clicked()), this, SLOT(mergeMatches()) );
	connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
	connect( addButton, SIGNAL(clicked()), this, SLOT(addCategory()) );
	connect( removeButton, SIGNAL(clicked()), this, SLOT(removeCategory()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SimilarCategoriesDialog::~SimilarCategoriesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SimilarCategoriesDialog::languageChange()
{
	setCaption( i18n( "Similar Categories" ) );
	thresholdLabel->setText( i18n( "Threshold:" ) );
	categoryLabel->setText( i18n( "Category:" ) );
	searchButton->setText( i18n( "Search" ) );
	allLabel->setText( i18n( "Similar Categories:" ) );
	removeButton->setText( i18n( "<<" ) );
	addButton->setText( i18n( ">>" ) );
	toMergeLabel->setText( i18n( "Categories to Merge:" ) );
	mergeButton->setText( i18n( "Merge" ) );
	cancelButton->setText( i18n( "Cancel" ) );

	allListView->addColumn( i18n( "Category" ) );
	//allListView->addColumn( i18n( "Id" ) );
	toMergeListView->addColumn( i18n( "Category" ) );
	//toMergeListView->addColumn( i18n( "Id" ) );
}

/*****************************************************/
/*Function prototypes and libraries needed to compile*/
/*****************************************************/

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
int levenshtein_distance(const char *s,const char*t);
int minimum(int a,int b,int c);

/****************************************/
/*Implementation of Levenshtein distance*/
/****************************************/

int levenshtein_distance(const char *s,const char*t)
/*Compute levenshtein distance between s and t*/
{
  //Step 1
  int k,i,j,n,m,cost,*d,distance;
  n=strlen(s); 
  m=strlen(t);
  if(n!=0&&m!=0)
  {
    d=(int*)malloc((sizeof(int))*(m+1)*(n+1));
    m++;
    n++;
    //Step 2	
    for(k=0;k<n;k++)
	d[k]=k;
    for(k=0;k<m;k++)
      d[k*n]=k;
    //Step 3 and 4	
    for(i=1;i<n;i++)
      for(j=1;j<m;j++)
	{
        //Step 5
        if(s[i-1]==t[j-1])
          cost=0;
        else
          cost=1;
        //Step 6			 
        d[j*n+i]=minimum(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
      }
    distance=d[n*m-1];
    free(d);
    return distance;
  }
  else 
    return -1; //a negative return value means that one or both strings are empty.
}

int minimum(int a,int b,int c)
/*Gets the minimum of three values*/
{
  int min=a;
  if(b<min)
    min=b;
  if(c<min)
    min=c;
  return min;
}

/** @return an array of adjacent letter pairs contained in the input string */
QStringList letterPairs(const QString& str) {
	int numPairs = str.length()-1;
	QStringList pairs;
	for (int i=0; i<numPairs; i++) {
		pairs << str.mid(i,2);
	}
	return pairs;
}

/** @return an ArrayList of 2-character Strings. */
QValueList<QStringList> wordLetterPairs(const QString &str) {
	QValueList<QStringList> allPairs;
	// Tokenize the string and put the tokens/words into an array
	QStringList words = QStringList::split("\\s",str);
	// For each word
	for (int w=0; w < words.count(); w++) {
		// Find the pairs of characters
		QStringList pairsInWord = letterPairs(words[w]);
		for (int p=0; p < pairsInWord.count(); p++) {
			allPairs.append(pairsInWord[p]);
		}
	}
	return allPairs;
}

/** @return lexical similarity value in the range [0,1] */
double compareStrings(const QString &str1, const QString &str2) {
	QValueList<QStringList> pairs1 = wordLetterPairs(str1.upper());
	QValueList<QStringList> pairs2 = wordLetterPairs(str2.upper());
	int intersection = 0;
	int size_union = pairs1.count() + pairs2.count();
	for (int i=0; i<pairs1.count(); i++) {
		QStringList pair1=pairs1[i];
		for(int j=0; j<pairs2.count(); j++) {
			QStringList pair2=pairs2[j];
			if (pair1 == pair2) {
				intersection++;
				pairs2.remove( pairs2.at(j) );
				break;
			}
		}
	}
	return (2.0*intersection)/size_union;
}



#include <kdebug.h>

#if 0
void RecipeActionsHandler::mergeSimilar()
{
	QPtrList<QListViewItem> items = parentListView->selectedItems();
	if ( items.count() > 1 )
		KMessageBox::sorry( kapp->mainWidget(), i18n("Please select only one category."), QString::null );
	else if ( items.count() == 1 && items.at(0)->rtti() == 1001 ) {
		CategoryListItem * cat_it = ( CategoryListItem* ) items.at(0);
		QString name = cat_it->categoryName();
		const double max_allowed_distance = 0.60;
		const int length = name.length();
		ElementList categories;
		database->loadCategories( &categories );

		ElementList matches;
		for ( ElementList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
			#if 0
			if ( levenshtein_distance(name.latin1(),(*it).name.latin1())/double(QMAX(length,(*it).name.length())) >= max_allowed_distance ) {
			#else
			if ( compareStrings(name,(*it).name) >= max_allowed_distance ) {
			#endif
				kdDebug()<<(*it).name<<" matches"<<endl;
				if ( cat_it->categoryId() != (*it).id )
					matches.append(*it);
			}
		}

		
		for ( ElementList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
			database->mergeCategories(cat_it->categoryId(),(*it).id);
		}

	}
	else //either nothing was selected or a recipe was selected
		KMessageBox::sorry( kapp->mainWidget(), i18n("No recipes selected."), i18n("Edit Recipe") );
}
#endif

void SimilarCategoriesDialog::findMatches()
{
	allListView->clear();
	toMergeListView->clear();

	const double threshold = (100 - thresholdSlider->value())/100.0;
	const QString name = categoriesBox->text();
	const int length = name.length();

	for ( ElementList::const_iterator it = m_elementList.begin(); it != m_elementList.end(); ++it ) {
		//kdDebug()<<(*it).name<<" (result/threshold): "<<compareStrings(name,(*it).name)<<"/"<<threshold<<endl;
		#if 0
		if ( levenshtein_distance(name.latin1(),(*it).name.latin1())/double(QMAX(length,(*it).name.length())) >= max_allowed_distance ) {
		#else
		if ( compareStrings(name,(*it).name) >= threshold ) {
		#endif
			kdDebug()<<(*it).name<<" matches"<<endl;
			//if ( id != (*it).id ) {
				(void) new QListViewItem(allListView,(*it).name,QString::number((*it).id));
				(void) new QListViewItem(toMergeListView,(*it).name,QString::number((*it).id));
			//}
		}
	}
}

void SimilarCategoriesDialog::mergeMatches()
{
	if ( !toMergeListView->firstChild() ) {
		KMessageBox::sorry( this, i18n("No categories selected to merge."), QString::null );
		return;
	}

	//const int id = categoriesBox->id(categoriesBox->currentItem());
	//for ( QListViewItem *item = toMergeListView->firstChild(); item; item = item->nextSibling() ) {
	//	m_database->mergeCategories(id,item->text(1).toInt());
	//}

	allListView->clear();
	//toMergeListView->clear();

	QDialog::accept();
}

QValueList<int> SimilarCategoriesDialog::matches() const
{
	QValueList<int> ids;
	for ( QListViewItem *item = toMergeListView->firstChild(); item; item = item->nextSibling() ) {
		ids << item->text(1).toInt();
	}

	return ids;
}

QString SimilarCategoriesDialog::element() const
{
	return categoriesBox->text();
}

void SimilarCategoriesDialog::addCategory()
{
	QListViewItem *item = allListView->selectedItem();
	if ( item )
	{
		//make sure it isn't already in the list
		for ( QListViewItem *search_it = toMergeListView->firstChild(); search_it; search_it = search_it->nextSibling() ) {
			if ( search_it->text(0) == item->text(0) )
				return;
		}

		(void) new QListViewItem(toMergeListView,item->text(0),item->text(1));
	}
}

void SimilarCategoriesDialog::removeCategory()
{
	QListViewItem *item = toMergeListView->selectedItem();
	if ( item )
		delete item;
}
