/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ingredientmatcherdialog.h"

#include "datablocks/recipelist.h"
#include "widgets/ingredientlistview.h"
#include "elementlist.h"
#include "DBBackend/recipedb.h"
#include "widgets/krelistview.h"
#include "recipeactionshandler.h"

#include <qheader.h>
#include <qpainter.h>
#include <qstringlist.h>
#include <qlayout.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kglobal.h>

IngredientMatcherDialog::IngredientMatcherDialog(QWidget *parent,RecipeDB *db):QVBox(parent)
{
	// Initialize internal variables
	database=db;
	
	//Design the dialog
	setSpacing(10);
	
		// Ingredient list
	ingredientListView=new KreListView(this,i18n("Ingredients"),true,1);
	IngredientCheckListView *list_view = new IngredientCheckListView(ingredientListView,database);
	list_view->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	list_view->reload();
	ingredientListView->setListView(list_view);
		// Box to select allowed number of missing ingredients
	missingBox=new QHBox(this);
	missingNumberLabel=new QLabel(missingBox);
	missingNumberLabel->setText(i18n("Missing ingredients allowed:"));
	missingNumberSpinBox=new KIntSpinBox(missingBox);
	missingNumberSpinBox->setMinValue( -1 );
	missingNumberSpinBox->setSpecialValueText( i18n("Any") );
	
		// Found recipe list
	recipeListView=new KreListView(this,i18n("Matching Recipes"),false,1,missingBox);
	recipeListView->listView()->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	recipeListView->listView()->setAllColumnsShowFocus(true);
	recipeListView->listView()->addColumn(i18n("Title"));

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	recipeListView->listView()->addColumn( i18n("Id"), show_id ? -1 : 0 );

	recipeListView->listView()->addColumn(i18n("Missing Ingredients"));
	recipeListView->listView()->setSorting(-1);

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( recipeListView->listView(), database,  RecipeActionsHandler::Open|RecipeActionsHandler::Edit|RecipeActionsHandler::Export );

	KIconLoader il;
	QHBox *buttonBox=new QHBox(this);
	
	okButton=new QPushButton(buttonBox);
	okButton->setIconSet(il.loadIconSet("button_ok", KIcon::Small));
	okButton->setText(i18n("Find matching recipes"));

	//buttonBox->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton=new QPushButton(buttonBox);
	clearButton->setIconSet(il.loadIconSet("editclear", KIcon::Small));
	clearButton->setText(i18n("Clear recipe list"));

	// Connect signals & slots
	connect (okButton,SIGNAL(clicked()), this,SLOT(findRecipes()));
	connect (clearButton,SIGNAL(clicked()),recipeListView->listView(),SLOT(clear()));
	connect (actionHandler, SIGNAL(recipeSelected(int,int)), SIGNAL(recipeSelected(int,int)) );

}

IngredientMatcherDialog::~IngredientMatcherDialog()
{

}



void IngredientMatcherDialog::findRecipes(void)
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );

	RecipeList rlist;
	IngredientList ilist;
	database->loadRecipeDetails(&rlist,true,false,true);
		
	QListViewItem *qlv_it;
	
	// First make a list of the ingredients that we have
	for (qlv_it=ingredientListView->listView()->firstChild();qlv_it;qlv_it=qlv_it->nextSibling())
		{
		IngredientCheckListItem *il_it=(IngredientCheckListItem*) qlv_it;
		if (il_it->isOn())
			{
			Ingredient ing; ing.name=il_it->name(); ing.ingredientID=il_it->id();
			ilist.append(ing);
			}
		}
	
	// Clear the list
	recipeListView->listView()->clear();
	// Add the section header
	new SectionItem(recipeListView->listView(),i18n("Possible recipes with the specified ingredients"));
	
	// Now show the recipes with ingredients that are contained in the previous set
	// of ingredients
	RecipeList incompleteRecipes;
	QValueList <int> missingNumbers;
	QValueList <IngredientList> missingIngredients;
	
	RecipeList::Iterator it;
	for (it=rlist.begin();it!=rlist.end();++it)
		{
		IngredientList il=(*it).ingList;
		if ( il.isEmpty() ) continue;

		IngredientList missing;
		if (ilist.containsSubSet(il,missing))
			{
			new CustomRecipeListItem(recipeListView->listView(),*it);
			}
		else 
			{
			incompleteRecipes.append(*it);
			missingIngredients.append(missing);
			missingNumbers.append(missing.count());
			}
		}

	//Check if the user wants to show missing ingredients
	
	if (this->missingNumberSpinBox->value()==0){ KApplication::restoreOverrideCursor(); return; } //"None"
	
	
	
	
	// Classify recipes with missing ingredients in different lists by ammount
	QValueList<int>::Iterator nit;
	QValueList<IngredientList>::Iterator ilit;
	int missingNoAllowed = missingNumberSpinBox->value();
	
	if (missingNoAllowed==-1) // "Any"
	{
		for (nit=missingNumbers.begin();nit!=missingNumbers.end();++nit) if ((*nit)>missingNoAllowed) missingNoAllowed=(*nit);
	}
	
	
	for (int missingNo=1; missingNo<=missingNoAllowed; missingNo++)
	{
		nit=missingNumbers.begin();
		ilit=missingIngredients.begin();
		
		bool titleShownYet=false;
		
		for (it=incompleteRecipes.begin();it!=incompleteRecipes.end();++it,++nit,++ilit)
		{
			if ((*nit)==missingNo) 
				{	
					if (!titleShownYet)
					{
						new SectionItem(recipeListView->listView(),i18n("You are missing %1 ingredients for:").arg(missingNo));
						titleShownYet=true;
					}
				new CustomRecipeListItem(recipeListView->listView(),*it,*ilit);
				}
		}
	}

	KApplication::restoreOverrideCursor();
}

void IngredientMatcherDialog::reloadIngredients(void)
{
	((StdIngredientListView*)ingredientListView->listView())->reload();
}

void SectionItem::paintCell ( QPainter * p, const QColorGroup & /*cg*/, int column, int width, int /*align*/ )
{
	// Draw the section's deco
	p->setPen(KGlobalSettings::activeTitleColor());
	p->setBrush(KGlobalSettings::activeTitleColor());
	p->drawRect(0,0,width,height());
	
	// Draw the section's text
	if ( column==0 ) {
		QFont titleFont=KGlobalSettings::windowTitleFont ();
		p->setFont(titleFont);
	
		p->setPen(KGlobalSettings::activeTextColor());
		p->drawText(0,0,width,height(),Qt::AlignLeft|Qt::AlignVCenter,mText);
	}
}

#include "ingredientmatcherdialog.moc"
