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
#include "elementlist.h"
#include "DBBackend/recipedb.h"
#include "widgets/krelistview.h"
#include "recipeactionshandler.h"

#include <qheader.h>
#include <qpainter.h>
#include <qstringlist.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <klocale.h>

IngredientMatcherDialog::IngredientMatcherDialog(QWidget *parent,RecipeDB *db):QVBox(parent)
{
	// Initialize internal variables
	database=db;
	
	//Design the dialog
	setSpacing(10);
	
		// Ingredient list
	ingredientListView=new KreListView(this,i18n("Ingredients"),true,1);
	ingredientListView->listView()->setAllColumnsShowFocus(true);
	ingredientListView->listView()->addColumn("*");
	ingredientListView->listView()->addColumn(i18n("Ingredient"));
		// Box to select allowed number of missing ingredients
	missingBox=new QHBox(this);
	missingNumberLabel=new QLabel(missingBox);
	missingNumberLabel->setText(i18n("Missing ingredients allowed:"));
	missingNumberCombo=new KComboBox(missingBox);
	QStringList optionsList;
	optionsList.append(i18n("None"));
	optionsList+="1";
	optionsList+="2";
	optionsList+="3";
	optionsList+=i18n("any");
	missingNumberCombo->insertStringList(optionsList);
	
		// Found recipe list
	recipeListView=new KreListView(this,i18n("Matching Recipes"),false,1,missingBox);
	recipeListView->listView()->setAllColumnsShowFocus(true);
	recipeListView->listView()->addColumn(i18n("Title"));
	recipeListView->listView()->addColumn(i18n("Missing Ingredients"));
	recipeListView->listView()->setSorting(-1);

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( recipeListView->listView(), database, 0, -1, -1,  RecipeActionsHandler::Open|RecipeActionsHandler::Edit|RecipeActionsHandler::SaveAs );

	KIconLoader il;
	QHBox *buttonBox=new QHBox(this);
	
	okButton=new QPushButton(buttonBox);
	okButton->setIconSet(il.loadIconSet("button_ok", KIcon::Small));
	okButton->setText(i18n("Find matching recipes"));
	
	clearButton=new QPushButton(buttonBox);
	clearButton->setIconSet(il.loadIconSet("editclear", KIcon::Small));
	clearButton->setText(i18n("Clear recipe list"));
	
	// Load the data
	reloadIngredients();
	
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
		IngredientListItem *il_it=(IngredientListItem*) qlv_it;
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
		IngredientList missing;
		if (ilist.containsSubSet(il,missing))
			{
			new RecipeListItem(recipeListView->listView(),*it);
			}
		else 
			{
			incompleteRecipes.append(*it);
			missingIngredients.append(missing);
			missingNumbers.append(missing.count());
			}
		}

	//Check if the user wants to show missing ingredients
	
	if (this->missingNumberCombo->currentItem()==0){ KApplication::restoreOverrideCursor(); return; } //"None"
	
	
	
	
	// Classify recipes with missing ingredients in different lists by ammount
	QValueList<int>::Iterator nit;
	QValueList<IngredientList>::Iterator ilit;
	int missingNoAllowed;
	
	if (missingNumberCombo->currentItem()!=4) missingNoAllowed=missingNumberCombo->currentText().toInt(); //"1..3"
	else  // "Any"
		{
		missingNoAllowed=0;
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
				new RecipeListItem(recipeListView->listView(),*it,*ilit);
				}
		}
	}

	KApplication::restoreOverrideCursor();
}

void IngredientMatcherDialog::reloadIngredients(void)
{

	ingredientListView->listView()->clear();
	ElementList ingredientList;
	database->loadIngredients(&ingredientList);
	
	ElementList::Iterator it;
	
	for (it=ingredientList.begin();it!=ingredientList.end();++it)
		{
		Element ingredient=*it;
		new IngredientListItem(ingredientListView->listView(),ingredient);
		}
}

void SectionItem::paintCell ( QPainter * p, const QColorGroup & /*cg*/, int column, int /*width*/, int /*align*/ )
{
	if ( column == 0 ) //we only need to do this once
	{
		int totalWidth=listView()->columnWidth(0)+listView()->columnWidth(1);
		
		QPixmap sectionPm(totalWidth,height()); QPainter painter(&sectionPm);
		
		// Draw the section's deco
		painter.setPen(KGlobalSettings::activeTitleColor());
		painter.setBrush(KGlobalSettings::activeTitleColor());
		painter.drawRect(0,0,totalWidth,height());
		
		// Draw the section's text
		
		QFont titleFont=KGlobalSettings::windowTitleFont ();
		painter.setFont(titleFont);
		
		painter.setPen(KGlobalSettings::activeTextColor());
		painter.drawText(0,0,totalWidth,height(),Qt::AlignHCenter|Qt::AlignVCenter,mText);
		
		// Paint only this cell (causes trouble while resizing)
		
		/*if (column==0) bitBlt(p->device(), 0, 0, &textPm,0,0,width,height());
		else if (column==1) bitBlt(p->device(),listView()->columnWidth(0),0,&textPm,listView()->columnWidth(0),0,width,height());*/
		
		// Paint full row
		bitBlt(p->device(), 0, listView()->contentsToViewport(QPoint(0,itemPos())).y(), &sectionPm,0,0,totalWidth,height());
	}
}

#include "ingredientmatcherdialog.moc"
