/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dietwizarddialog.h"
#include "DBBackend/recipedb.h"
#include "dietviewdialog.h"
#include "editbox.h"

#include <qbitmap.h>
#include <qheader.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <kapp.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "propertycalculator.h"

DietWizardDialog::DietWizardDialog(QWidget *parent,RecipeDB *db):QVBox(parent)
{
// Initialize internal variables
database=db;
mealNumber=1;
dayNumber=1;
dietRList=new RecipeList();

//Design the dialog
setSpacing(10);
// Options Box
optionsBox=new QHBox(this);

	daysSliderBox=new QVGroupBox(i18n("Number of days"),optionsBox);
	dayNumberLabel=new QLabel(daysSliderBox);
	dayNumberLabel->setText("- 1 -");
	dayNumberLabel->setAlignment(Qt::AlignHCenter);
	dayNumberSelector=new QSlider(daysSliderBox);

	dayNumberSelector->setOrientation(Qt::Horizontal);
	dayNumberSelector->setRange(1,10);
	dayNumberSelector->setSteps(1,1);
	dayNumberSelector->setTickmarks(QSlider::Below);
	dayNumberSelector->setFixedWidth(100);

	mealsSliderBox=new QVGroupBox(i18n("Meals per day"),optionsBox);
	mealNumberLabel=new QLabel(mealsSliderBox);
	mealNumberLabel->setText("- 1 -");
	mealNumberLabel->setAlignment(Qt::AlignHCenter);
	mealNumberSelector=new QSlider(mealsSliderBox);

	mealNumberSelector->setOrientation(Qt::Horizontal);
	mealNumberSelector->setRange(1,10);
	mealNumberSelector->setSteps(1,1);
	mealNumberSelector->setTickmarks(QSlider::Below);
	mealNumberSelector->setFixedWidth(100);

// Tabs
mealTabs=new QTabWidget(this);
mealTabs->setMargin(20);

// Button bar
KIconLoader il;

buttonBox=new QHBox(this);

okButton=new QPushButton(buttonBox);
okButton->setIconSet(il.loadIconSet("button_ok", KIcon::Small));
okButton->setText(i18n("Create the diet"));

// Create Tabs
newTab(i18n("Meal 1"));

// Initialize data
reload();

// Signals & Slots
connect(mealNumberSelector,SIGNAL(valueChanged(int)),this,SLOT(changeMealNumber(int)));
connect(dayNumberSelector,SIGNAL(valueChanged(int)),this,SLOT(changeDayNumber(int)));
connect(okButton,SIGNAL(clicked()),this,SLOT(createDiet()));
}


DietWizardDialog::~DietWizardDialog()
{
delete dietRList;
}

void DietWizardDialog::reload(void)
{

// Load the lists
database->loadCategories(&categoriesList);
database->loadProperties(&propertyList); // Loads the properties of all the ingredients
int pgcount=0;
for (MealInput *tab=(MealInput *) (mealTabs->page(pgcount));pgcount<mealTabs->count(); pgcount++)
	tab->reload(categoriesList,propertyList);

//Fill in the caches from the database
database->loadUnitRatios(&cachedUnitRatios);
database->loadProperties(&cachedIngredientProperties,-1);
}

void DietWizardDialog::newTab(const QString &name)
{
mealTab=new MealInput(mealTabs);
mealTab->reload(categoriesList,propertyList);
mealTabs->addTab(mealTab,name);
mealTabs->setCurrentPage(mealTabs->indexOf(mealTab));
}

void DietWizardDialog::changeMealNumber(int mn)
{
mealNumberLabel->setText(QString(i18n("- %1 -")).arg(mn));
if (mn>mealNumber)
	{

	 while (mealNumber!=mn)
	 {
	 	mealNumber++;
		newTab(i18n("Meal %1").arg(mealNumber));

	 }
	 }
else if (mn<mealNumber)
	{

	 while (mealNumber!=mn)
	 {
	 	mealNumber--;
		delete mealTabs->page(mealTabs->count()-1);
	 }
	 }
}

void DietWizardDialog::changeDayNumber(int dn)
{

if (dn<7)
	{
	 dayNumber=dn;
	 dayNumberLabel->setText(QString(i18n("- %1 -")).arg(dn));
	}
else if (dn==7)
	{
	dayNumber=7;
	dayNumberLabel->setText(QString(i18n("- 1 week -")));
	}
else
	{
	dayNumber=(dn-6)*7;
	dayNumberLabel->setText(QString(i18n("- %1 weeks -")).arg(dn-6));
	}
}

void DietWizardDialog::createDiet(void)
{
RecipeList rlist;
dietRList->clear();
// Get the whole list of recipes, detailed
database->loadRecipeDetails(&rlist,true,true);

int recipes_left;


QValueList <RecipeList::Iterator> tempRList; // temporal iterator list so elements can be removed without reloading them again from the DB

bool alert=false;

for (int day=0;day<dayNumber;day++) // Create the diet for the number of days defined by the user
{
	populateIteratorList(rlist,&tempRList); // temporal iterator list so elements can be removed without reloading them again from the DB
	recipes_left=rlist.count();
	for (int meal=0;meal<mealNumber;meal++)
	{
	int dishNo=( (MealInput*)(mealTabs->page(meal)) )->dishNo();

		for (int dish=0;dish<dishNo;dish++)
		{
			bool found=false;
			while ((!found) && recipes_left)
			{
				int random_index=(int)((float)(kapp->random())/(float)RAND_MAX*recipes_left);
				QValueList<RecipeList::Iterator>::Iterator iit=tempRList.at(random_index); // note that at() retrieves an iterator to the iterator list, so we need to use * in order to get the RecipeList::Iterator
				RecipeList::Iterator rit=*iit;
				if (found=(((!categoryFiltering(meal,dish)) ||checkCategories(*rit,meal,dish)) && checkConstraints(*rit,meal,dish))) // Check that the recipe is inside the constraint limits and in the categories specified
				{
				dietRList->append(*rit);// Add recipe to the diet list
				}

				// Remove this analized recipe from teh list
				tempRList.remove(iit);
				recipes_left--;

			}
			if (!found) alert=true;
		}
	}
}

if (alert) KMessageBox::information(this,i18n("I could not create a full diet list given the constraints. Either the recipe list is too short or the constraints are too demanding. "));

else // show the resulting diet
	{

	// make a list of dishnumbers
	QValueList<int> dishNumbers;

	for (int meal=0;meal<mealNumber;meal++)
	{
	int dishNo=( (MealInput*)(mealTabs->page(meal)) )->dishNo();
	dishNumbers<<dishNo;
	}

	// display the list
	DietViewDialog *dietDisplay=new DietViewDialog(0,*dietRList,dayNumber,mealNumber,dishNumbers);
	connect(dietDisplay,SIGNAL(signalOk()),this,SLOT(createShoppingList()));
	dietDisplay->show();
	}

}


void DietWizardDialog::populateIteratorList(RecipeList &rl, QValueList <RecipeList::Iterator> *il)
{
il->clear();
RecipeList::Iterator it;
for (it=rl.begin();it!=rl.end();it++) il->append(it);

}


class ConstraintsListItem:public QCheckListItem{
public:
	ConstraintsListItem(QListView* klv, IngredientProperty *pty ):QCheckListItem(klv,QString::null,QCheckListItem::CheckBox)
		{
		// Initialize the constraint data with the the property data
		ctStored=new Constraint();
		ctStored->id=pty->id;
		ctStored->name=pty->name;
		ctStored->perUnit=pty->perUnit;
		ctStored->units=pty->units;
		ctStored->max=0;
		ctStored->min=0;
		}

	~ConstraintsListItem(void)
	{
	delete ctStored;
	}

private:
	Constraint *ctStored;

public:
	double maxVal(){return ctStored->max;}
	double minVal(){return ctStored->min;}
	int propertyId(){return ctStored->id;}
	void setMax(double maxValue) {ctStored->max=maxValue; setText(3,QString::number(maxValue));}
	void setMin(double minValue) {ctStored->min=minValue; setText(2,QString::number(minValue));}
	virtual QString text(int column) const
		{
		if (column==1) return(ctStored->name);
		else if (column==2) return(QString::number(ctStored->min));
		else if (column==3) return(QString::number(ctStored->max));
		else return(QString::null);
		}
};

class CategoriesListItem:public QCheckListItem{
public:
	CategoriesListItem(QListView* klv, const Element &category ):QCheckListItem(klv,QString::null,QCheckListItem::CheckBox)
	{
	ctyStored.id=category.id;
	ctyStored.name=category.name;
	setOn(true); // Set checked by default
	}
	~CategoriesListItem(void){}
	virtual QString text(int column) const
		{
		if (column==1) return(ctyStored.name);
		else return(QString::null);
		}
	int categoryId(void){return ctyStored.id;}
	QString categoryName(void){return ctyStored.name;}
private:
	Element ctyStored;

};


MealInput::MealInput(QWidget *parent):QWidget(parent)
{

// Initialize data
categoriesListLocalCache.clear();
propertyListLocalCache.clear();

// Design the dialog
QVBoxLayout *layout=new QVBoxLayout(this);
layout->setSpacing(20);

	// Options box

mealOptions=new QHBox(this);
mealOptions->setSpacing(10);
layout->addWidget(mealOptions);

	// Number of dishes input
dishNumberBox=new QHBox(mealOptions); dishNumberBox->setSpacing(10);
dishNumberLabel=new QLabel(i18n("No. of dishes: "),dishNumberBox);
dishNumberInput=new QSpinBox(dishNumberBox); dishNumberInput->setMinValue(1); dishNumberInput->setMaxValue(10);
dishNumberBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	// Toolbar

toolBar=new QHGroupBox(mealOptions);
toolBar->setFrameStyle (QFrame::Panel | QFrame::Sunken);
toolBar->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum));

	// Next dish/ Previous dish buttons
KIconLoader il;
buttonPrev=new QToolButton(toolBar);
	buttonPrev->setUsesTextLabel(true);
	buttonPrev->setTextLabel(i18n("Previous Dish"));
	buttonPrev->setIconSet(il.loadIconSet("back", KIcon::Small));
	buttonPrev->setTextPosition(QToolButton::Under);
buttonNext=new QToolButton(toolBar);
	buttonNext->setUsesTextLabel(true);
	buttonNext->setTextLabel(i18n("Next Dish"));
	buttonNext->setIconSet(il.loadIconSet("forward", KIcon::Small));
	buttonNext->setTextPosition(QToolButton::Under);


	// Dish widgets
dishStack= new QWidgetStack(this);
layout->addWidget(dishStack);
dishStack->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	// Add default dishes
DishInput *newDish=new DishInput(this,i18n("1st Course")); dishStack->addWidget(newDish); dishInputList.append(newDish);
newDish=new DishInput(this,i18n("2nd Course")); dishStack->addWidget(newDish); dishInputList.append(newDish);
newDish=new DishInput(this,i18n("Dessert")); dishStack->addWidget(newDish); dishInputList.append(newDish);
dishNumber=3;dishNumberInput->setValue(dishNumber);

// Signals & Slots
connect(dishNumberInput,SIGNAL(valueChanged(int)),this,SLOT(changeDishNumber(int)));
connect(buttonPrev,SIGNAL(clicked()),this,SLOT(prevDish()));
connect(buttonNext,SIGNAL(clicked()),this,SLOT(nextDish()));

}

MealInput::~MealInput()
{
}

// reload from outside with new data

void MealInput::reload(ElementList &categoriesList,IngredientPropertyList &propertyList)
{
QValueList<DishInput*>::iterator it;

categoriesListLocalCache.clear();
propertyListLocalCache.clear();

// Cache the data into the internal lists so it can be reused when creating new dishes

	//Cache the possible constraints (properties) list
	for (IngredientProperty *pty=propertyList.getFirst(); pty; pty=propertyList.getNext())
	{
	propertyListLocalCache.add(*pty);
	}

	//Cache the categories list
	for (Element *el=categoriesList.getFirst(); el; el=categoriesList.getNext())
	{
	categoriesListLocalCache.add(*el);
	}

reload(); //load from the cache now

}

// reload internally with the cached data

void MealInput::reload()
{
QValueList<DishInput*>::iterator it;
for (it=dishInputList.begin(); it != dishInputList.end();it++)
{
	DishInput *di; di=(*it);
	di->reload(&categoriesListLocalCache,&propertyListLocalCache);
	}
}

void MealInput::changeDishNumber(int dn)
{
if (dn>dishNumber)
	{
	while (dishNumber!=dn)
	{
		DishInput *newDish=new DishInput(this,QString(i18n("Dish %1")).arg(dishNumber+1));
		newDish->reload(&categoriesListLocalCache,&propertyListLocalCache);
		dishStack->addWidget(newDish);
		dishInputList.append(newDish);
		dishStack->raiseWidget(newDish);
		dishNumber++;
	}
	}
else if (dn<dishNumber)
	{
	QValueList <DishInput*>::Iterator it;
	while (dishNumber!=dn)
	{
		it=dishInputList.fromLast();
		DishInput *lastDish=(*it);
		dishInputList.remove(it);

		if ((*it)==(DishInput*)dishStack->visibleWidget())
			{
			it--;
			dishStack->raiseWidget(*it);
			}
		delete lastDish;
		dishNumber--;
	}
	}
}


void MealInput::nextDish(void)
{
// First get the place of the current dish input in the list
QValueList <DishInput*>::iterator it=dishInputList.find((DishInput*)(dishStack->visibleWidget()));

//Show the next dish if it exists
it++;
if (it!=dishInputList.end())
{
dishStack->raiseWidget(*it);
}

}

void MealInput::prevDish(void)
{
// First get the place of the current dish input in the list
QValueList <DishInput*>::iterator it=dishInputList.find((DishInput*)(dishStack->visibleWidget()));

//Show the previous dish if it exists
it--;
if (it!=dishInputList.end())
{
dishStack->raiseWidget(*it);
}
}

DishInput::DishInput(QWidget* parent,const QString &title):QWidget(parent)
{

// Initialize internal variables
categoryFiltering=false;

// Design the widget

QVBoxLayout *layout=new QVBoxLayout(this);
layout->setSpacing(20);

	//Horizontal Box to hold the KListView's
listBox=new QHGroupBox(i18n("Dish Characteristics"),this);
layout->addWidget(listBox);

	// Dish id
dishTitle=new DishTitle(listBox,title);

	//Categories list
categoriesBox=new QVBox(listBox);
categoriesEnabledBox=new QCheckBox(categoriesBox);
categoriesEnabledBox->setText(i18n("Enable Category Filtering"));

categoriesView=new KListView(categoriesBox);
categoriesView->addColumn("*");
categoriesView->addColumn(i18n("Category"));
categoriesView->setEnabled(false); // Disable it by default
categoriesView->setAllColumnsShowFocus(true);

	//Constraints list
constraintsView=new KListView(listBox);
constraintsView->addColumn(i18n("Enabled"));
constraintsView->addColumn(i18n("Property"));
constraintsView->addColumn(i18n("Min. Value"));
constraintsView->addColumn(i18n("Max. Value"));
constraintsView->setAllColumnsShowFocus(true);

	// KDoubleInput based edit boxes
constraintsEditBox1=new EditBox(this);
constraintsEditBox1->hide();
constraintsEditBox2=new EditBox(this);
constraintsEditBox2->hide();


// Connect Signals & Slots
connect(constraintsView,SIGNAL(executed(QListViewItem*)),this,SLOT(insertConstraintsEditBoxes(QListViewItem*)));
connect(constraintsEditBox1,SIGNAL(valueChanged(double)),this,SLOT(setMinValue(double)));
connect(constraintsEditBox2,SIGNAL(valueChanged(double)),this,SLOT(setMaxValue(double)));
connect(categoriesEnabledBox,SIGNAL(toggled(bool)),this,SLOT(enableCategories(bool)));
}

DishInput::~DishInput()
{
}

void DishInput::enableCategories(bool enable)
{
categoriesView->setEnabled(enable);
categoryFiltering=enable;
}

bool DishInput::isCategoryFilteringEnabled(void)
{
return categoryFiltering;
}

void DishInput::reload(ElementList *categoryList, IngredientPropertyList *propertyList)
{
categoriesView->clear();
constraintsView->clear();

	//Load the possible constraints (properties) list
for (IngredientProperty *pty=propertyList->getFirst();pty; pty=propertyList->getNext())
{
ConstraintsListItem *it=new ConstraintsListItem(constraintsView,pty);
constraintsView->insertItem(it);
}

	//Load the categories list
for (Element *el=categoryList->getFirst(); el; el=categoryList->getNext())
{
CategoriesListItem *it=new CategoriesListItem(categoriesView,*el);
categoriesView->insertItem(it);
}

}

void DishInput::insertConstraintsEditBoxes(QListViewItem* it)
{

QRect r;

// Constraints Box1
r.setTopLeft(this->pos());r.setSize(QSize(30,30));
r.moveBy(listBox->pos().x()+constraintsView->pos().x()+constraintsView->header()->pos().x(),listBox->pos().y()+constraintsView->pos().y()+constraintsView->header()->pos().y()); // Place it on top of the header of the list view
r.moveBy(constraintsView->header()->sectionRect(2).x(),0); // Move it to column no 1
r.moveBy(0,constraintsView->header()->sectionRect(2).height()+constraintsView->itemRect(it).y()); //Move down to the item, note that its height is same as header's right now.

r.setHeight(it->height()); // Set the item's height
r.setWidth(constraintsView->header()->sectionRect(2).width()); // and width
constraintsEditBox1->setGeometry(r);


//Constraints Box2
r.setTopLeft(this->pos());r.setSize(QSize(30,30));
r.moveBy(listBox->pos().x()+constraintsView->pos().x()+constraintsView->header()->pos().x(),listBox->pos().y()+constraintsView->pos().y()+constraintsView->header()->pos().y()); // Place it on top of the header of the list view
r.moveBy(constraintsView->header()->sectionRect(3).x(),0); // Move it to column no 2
r.moveBy(0,constraintsView->header()->sectionRect(3).height()+constraintsView->itemRect(it).y()); //Move down to the item

r.setHeight(it->height()); // Set the item's height
r.setWidth(constraintsView->header()->sectionRect(3).width()); // and width
constraintsEditBox2->setGeometry(r);

// Set the values from the item
constraintsEditBox1->setValue(((ConstraintsListItem*)it)->minVal());
constraintsEditBox2->setValue(((ConstraintsListItem*)it)->maxVal());


// Show Boxes
constraintsEditBox1->show();
constraintsEditBox2->show();
}

void DishInput::loadConstraints(ConstraintList *constraints)
{
constraints->clear();
Constraint constraint;
	for (ConstraintsListItem *it=(ConstraintsListItem*)(constraintsView->firstChild());it;it=(ConstraintsListItem*)(it->nextSibling()))
	{
	constraint.id=it->propertyId();
	constraint.min=it->minVal();
	constraint.max=it->maxVal();
	constraint.enabled=it->isOn();
	constraints->add(constraint);
	}
}

void DishInput::loadEnabledCategories(ElementList* categories)
{
categories->clear();
Element category;
	for (CategoriesListItem *it=(CategoriesListItem*)(categoriesView->firstChild());it;it=(CategoriesListItem*)(it->nextSibling()))
	{
	if (it->isOn()) // Only load those that are checked
		{
		category.id=it->categoryId();
		category.name=it->categoryName();
		categories->add(category);
		}
	}
}

void DishInput::setMinValue(double minValue)
{
this->constraintsEditBox1->hide();

ConstraintsListItem *it=(ConstraintsListItem*)(constraintsView->selectedItem()); // Find selected property

if (it) it->setMin(minValue);
}

void DishInput::setMaxValue(double maxValue)
{
this->constraintsEditBox2->hide();


ConstraintsListItem *it=(ConstraintsListItem*)(constraintsView->selectedItem()); // Find selected property

if (it) it->setMax(maxValue);
}

DishTitle::DishTitle(QWidget *parent,const QString &title):QWidget(parent)
{
titleText=title;
}


DishTitle::~DishTitle()
{
}

void DishTitle::paintEvent(QPaintEvent * )
{



    // Now draw the text

    if (QT_VERSION>=0x030200)
    {
    // Case 1: Qt 3.2+

    QPainter painter(this);

    // First draw the decoration
    painter.setPen(KGlobalSettings::activeTitleColor());
    painter.setBrush(QBrush(KGlobalSettings::activeTitleColor()));
    painter.drawRoundRect(0,20,30,height()-40,50,(int)(50.0/(height()-40)*35.0));

    // Now draw the text

    QFont titleFont=KGlobalSettings::windowTitleFont ();
    titleFont.setPointSize(15);
    painter.setFont(titleFont);
    painter.rotate(-90);
    painter.setPen(QColor(0x00,0x00,0x00));
    painter.drawText(0,0,-height(),30,AlignCenter,titleText);
    painter.setPen(QColor(0xFF,0xFF,0xFF));
    painter.drawText(-1,-1,-height()-1,29,AlignCenter,titleText);
    painter.end();
    }
    else
    {
    // Case 2: Qt 3.1

    // Use a pixmap

    QSize pmSize(height(),width()); //inverted size so we can rotate later
    QPixmap pm(pmSize);
    pm.fill(QColor(0xFF,0xFF,0xFF));
    QPainter painter(&pm);

    // First draw the decoration
    painter.setPen(KGlobalSettings::activeTitleColor());
    painter.setBrush(QBrush(KGlobalSettings::activeTitleColor()));
    painter.drawRoundRect(20,0,height()-40,30,(int)(50.0/(height()-40)*35.0),50);

    // Now draw the text
    QFont titleFont=KGlobalSettings::windowTitleFont ();
    titleFont.setPointSize(15);
    painter.setFont(titleFont);
    painter.setPen(QColor(0x00,0x00,0x00));
    painter.drawText(0,0,height(),30,AlignCenter,titleText);
    painter.setPen(QColor(0xFF,0xFF,0xFF));
    painter.drawText(-1,-1,height()-1,29,AlignCenter,titleText);
    painter.end();

    //Set the border transparent using a mask
    QBitmap mask(pm.size());
    mask.fill(Qt::color0);
    painter.begin(&mask);
    painter.setPen(Qt::color1);
    painter.setBrush(Qt::color1);
    painter.drawRoundRect(20,0,height()-40,30,(int)(50.0/(height()-40)*35.0),50);

    painter.end();
    pm.setMask(mask);

    //And Rotate
    QWMatrix m ; m.rotate(-90);
    pm=pm.xForm(m);

    bitBlt(this, 0, 0, &pm);
    }

}
QSize DishTitle::sizeHint () const
{
return(QSize(40,200));
}

QSize DishTitle::minimumSizeHint() const
{
return(QSize(40,200));
}

bool DietWizardDialog::checkCategories(Recipe &rec,int meal,int dish)
{

// Check if the recipe is among the categories chosen
ElementList categoryList; loadEnabledCategories(meal,dish,&categoryList);


for (Element *category=rec.categoryList.getFirst();category; category=rec.categoryList.getNext())
	{
	if (categoryList.containsId(category->id)) return true;
	}

return false;
}

/*
** Calculate the recipe Properties and check if they're within the constraints
*/

bool DietWizardDialog::checkConstraints(Recipe &rec,int meal,int dish)
{
// FIXME: Before 0.5: why check constraints if none of the constraints is enabled?

// Calculate properties of the recipe
IngredientPropertyList properties;

calculateProperties(rec,cachedIngredientProperties,cachedUnitRatios,&properties);

// Check if the properties are within the constraints
ConstraintList constraints; loadConstraints(meal,dish,&constraints); //load the constraints
bool withinLimits=checkLimits(properties,constraints);

return (withinLimits);
}

void DietWizardDialog::loadConstraints(int meal,int dish,ConstraintList *constraints)
{
MealInput* mealTab=(MealInput*)(mealTabs->page(meal)); // Get the meal
DishInput* dishInput=mealTab->dishInputList[dish]; // Get the dish input
dishInput->loadConstraints(constraints); //Load the constraints form the KListView
}

void DietWizardDialog::loadEnabledCategories(int meal,int dish,ElementList *categories)
{
MealInput* mealTab=(MealInput*)(mealTabs->page(meal)); // Get the meal
DishInput* dishInput=mealTab->dishInputList[dish]; // Get the dish input
dishInput->loadEnabledCategories(categories); //Load the categories that have been checked in the KListView
}

bool DietWizardDialog::categoryFiltering(int meal,int dish)
{
MealInput* mealTab=(MealInput*)(mealTabs->page(meal)); // Get the meal
DishInput* dishInput=mealTab->dishInputList[dish]; // Get the dish input
return(dishInput->isCategoryFilteringEnabled()); //Load the categories that have been checked in the KListView
}

bool DietWizardDialog::checkLimits(IngredientPropertyList &properties,ConstraintList &constraints)
{
for (Constraint* ct=constraints.getFirst();ct; ct=constraints.getNext())
	{
	if (ct->enabled)
		{
		IngredientProperty* ip=properties.at(properties.find(ct->id));
		if (ip)
			{
			if ((ip->amount>ct->max)||(ip->amount<ct->min)) return false;
			}
		else return false;
		}
	}
return true;
}

void DietWizardDialog::createShoppingList(void)
{
emit dietReady();
}

RecipeList& DietWizardDialog::dietList(void)
{
return *dietRList;
}
