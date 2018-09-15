/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientseditor.h"
#include "ui_ingredientseditor.h"

#include "dialogs/recipeinput/ingredientnamedelegate.h"
#include "dialogs/recipeinput/amountdelegate.h"
#include "dialogs/recipeinput/unitdelegate.h"
#include "dialogs/recipeinput/prepmethoddelegate.h"
#include "dialogs/recipeinput/ingredientparserdialog.h"
#include "dialogs/recipeinput/nutrientinfodetailsdialog.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/mixednumberrange.h"
#include "backends/recipedb.h"

#include <QStandardItemModel>
#include <QPointer>

//FIXME: Remove this when this class is finished
#include "kdebug.h"

IngredientsEditor::IngredientsEditor( QWidget * parent)
		: QWidget( parent ),
		m_ingredientList( 0 ),
		m_database( 0 ),
		m_nutrientIncompleteCount( 0 ),
		m_nutrientIntermediateCount( 0 )
{
	ui = new Ui::IngredientsEditor;
	ui->setupUi( this );

	m_sourceModel = new QStandardItemModel;
	ui->m_treeView->setModel( m_sourceModel );

	m_nutrientInfoDetailsDialog = new NutrientInfoDetailsDialog(this);

	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Ingredient" )
		<< i18nc( "@title:column", "Amount" )
		<< i18nc( "@title:column", "Units" )
		<< i18nc( "@title:column", "Preparation Method" )
		<< i18nc( "@title:column", "Id" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );


	//Connect signals/slots
	connect( ui->m_addIngredientButton, SIGNAL(clicked()),
		this, SLOT(addIngredientSlot()) );
	connect( ui->m_addAltButton, SIGNAL(clicked()),
		this, SLOT(addAltIngredientSlot()) );
	connect( ui->m_addHeaderButton, SIGNAL(clicked()),
		this, SLOT(addHeaderSlot()) );

	connect( ui->m_ingParserButton, SIGNAL(clicked()),
		this, SLOT(ingParserSlot()) );

	connect( ui->m_moveUpButton, SIGNAL(clicked()),
		this, SLOT(moveIngredientUpSlot()) );
	connect( ui->m_moveDownButton, SIGNAL(clicked()),
		this, SLOT(moveIngredientDownSlot()) );

	connect( ui->m_deleteButton, SIGNAL(clicked()),
		this, SLOT(removeIngredientSlot()) );

	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
	connect( ui->m_treeView, SIGNAL(doubleClicked(QModelIndex)),
		this, SIGNAL(changed()) );

	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SLOT(itemChangedSlot(QStandardItem*)) );

	connect( m_nutrientInfoDetailsDialog, SIGNAL(updateRequested()),
		this, SLOT(updateNutrientInfoDetailsSlot()) );

	connect( ui->m_nutrientInfoStatusWidget, SIGNAL(updateButtonClicked()),
		this, SLOT(updateNutrientInfoDetailsSlot()) );

	connect( ui->m_nutrientInfoStatusWidget, SIGNAL(detailsButtonClicked()),
		this, SLOT(nutrientInfoDetailsSlot()) );
}

int IngredientsEditor::ingredientColumn()
{
	return 0;
}

int IngredientsEditor::amountColumn()
{
	return 1;
}

int IngredientsEditor::unitColumn()
{
	return 2;
}

int IngredientsEditor::prepmethodsColumn()
{
	return 3;
}

int IngredientsEditor::ingredientIdColumn()
{
	return 4;
}

int IngredientsEditor::headerColumn()
{
	return 0;
}

int IngredientsEditor::headerIdColumn()
{
	return 4;
}

void IngredientsEditor::setDatabase( RecipeDB * database )
{
	//Disconnect all signals from the old database
	if ( m_database ) {
		disconnect( m_database, 0, this, 0 );
	}

	//Update the database objects
	m_database = database;
	m_nutrientInfoDetailsDialog->setDatabase( m_database );

	//Connect signals from new database

	//Ingredients
	connect( m_database, SIGNAL(ingredientCreated(Element)),
		this, SLOT(ingredientCreatedDBSlot(Element)) );
	connect( m_database, SIGNAL(ingredientModified(Ingredient)),
		this, SLOT(ingredientModifiedDBSlot(Ingredient)) );
	connect( m_database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedDBSlot(int)) );

	//Units
	connect( m_database, SIGNAL(unitCreated(Unit)),
		this, SLOT(unitCreatedDBSlot(Unit)) );
	connect( m_database, SIGNAL(unitModified(Unit)),
		this, SLOT(unitModifiedDBSlot(Unit)) );
	connect( m_database, SIGNAL(unitRemoved(int)),
		this, SLOT(unitRemovedDBSlot(int)) );

	//Preparation methods
	connect( m_database, SIGNAL(prepMethodCreated(Element)),
		this, SLOT(prepMethodCreatedDBSlot(Element)) );
	connect( m_database, SIGNAL(prepMethodModified(Element)),
		this, SLOT(prepMethodModifiedDBSlot(Element)) );
	connect( m_database, SIGNAL(prepMethodRemoved(int)),
		this, SLOT(prepMethodRemovedDBSlot(int)) );

	//Headers
	connect( m_database, SIGNAL(ingGroupCreated(Element)),
		this, SLOT(headerCreatedDBSlot(Element)) );
	connect( m_database, SIGNAL(ingGroupModified(Element)),
		this, SLOT(headerModifiedDBSlot(Element)) );
	connect( m_database, SIGNAL(ingGroupRemoved(int)),
		this, SLOT(headerRemovedDBSlot(int)) );
}

void IngredientsEditor::setRecipeTitle( const QString & title )
{
	m_recipeTitle = title;
}

void IngredientsEditor::loadIngredientList( IngredientList * ingredientList )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	m_ingredientList = ingredientList;

	m_sourceModel->setRowCount( 0 );
	m_sourceModel->setRowCount( ingredientList->count() );

	//Set ingredient name delegate
	IngredientNameDelegate * ingredientNameDelegate = new IngredientNameDelegate( m_database );
	if ( m_database ) {
		ingredientNameDelegate->loadAllIngredientsList( m_database );
		ingredientNameDelegate->loadAllHeadersList( m_database );
	}
	ui->m_treeView->setItemDelegateForColumn(ingredientColumn(), ingredientNameDelegate);

	//Set amount delegate
	AmountDelegate * amountDelegate = new AmountDelegate;
	ui->m_treeView->setItemDelegateForColumn(amountColumn(), amountDelegate);

	//Set unit delegate
	UnitDelegate * unitDelegate = new UnitDelegate;
	if ( m_database ) {
		unitDelegate->loadAllUnitsList( m_database );
	}
	ui->m_treeView->setItemDelegateForColumn(unitColumn(), unitDelegate);

	//Set preparation method delegate
	PrepMethodDelegate * prepMethodDelegate = new PrepMethodDelegate;
	if ( m_database ) {
		prepMethodDelegate->loadAllPrepMethodsList( m_database );
	}
	ui->m_treeView->setItemDelegateForColumn(prepmethodsColumn(), prepMethodDelegate);

	//Populate the ingredient list
	populateModel( ingredientList );

	//Update the nutrient information status
	updateNutrientInfoDetailsSlot();

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::populateModel( IngredientList * ingredientList )
{
	IngredientList::const_iterator it;
	int current_row = 0;
	QModelIndex index;
	RecipeDB::IdType lastGroupId = RecipeDB::InvalidId;
	for ( it = ingredientList->begin(); it != ingredientList->end(); ++it ) {
		//Check if we have to add a header
		if ( it->groupID != lastGroupId ) {
			//Prepare the header data
			Element header;
			header.id = it->groupID;
			header.name = it->group;
			//Add data to the model
			m_sourceModel->insertRow( current_row );
			setRowData( current_row, header );
			//Increase row count
			++current_row;
		}
		setRowData( current_row, *it );
		lastGroupId = it->groupID;
		++current_row;
	}
	ui->m_treeView->expandAll();
	resizeColumnsToContents();
}

void IngredientsEditor::setRowData( int row, const Ingredient & ingredient )
{
	QModelIndex index;
	//The "Id" item.
	index = m_sourceModel->index( row, ingredientIdColumn() );
	m_sourceModel->setData( index, QVariant(ingredient.ingredientID), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->itemFromIndex( index )->setEditable( false );
	//The "Ingredient" item.
	index = m_sourceModel->index( row, ingredientColumn() );
	m_sourceModel->setData( index, QVariant(ingredient.ingredientID), IdRole );
	m_sourceModel->setData( index, QVariant(ingredient.name), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	QStandardItem * ingItem = m_sourceModel->itemFromIndex( index );
	QPersistentModelIndex ingIndex = index;
	ingItem->setEditable( true );
	//The "Amount" item.
	index = m_sourceModel->index( row, amountColumn() );
	m_sourceModel->setData( index,
		QVariant( ingredient.amountRange().toString(true) ), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->itemFromIndex( index )->setEditable( true );
	bool isAmountPlural = ingredient.amountRange().isPlural();
	//The "Units" item.
	index = m_sourceModel->index( row, unitColumn() );
	m_sourceModel->setData( index, QVariant(ingredient.amountUnitString()), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), IdRole );
	m_sourceModel->setData( index, QVariant(isAmountPlural), IsPluralRole );
	m_sourceModel->itemFromIndex( index )->setEditable( true );
	//The "PreparationMethod" item.
	index = m_sourceModel->index( row, prepmethodsColumn() );
		//Set the prep methods id list
		QList<QVariant> prepMethodsIds;
		ElementList::const_iterator prep_it = ingredient.prepMethodList.constBegin();
		while ( prep_it != ingredient.prepMethodList.constEnd() ) {
			prepMethodsIds << prep_it->id;
			++prep_it;
		}
		m_sourceModel->setData( index, prepMethodsIds, IdRole );
		//Set the prep methods string
		QString prepMethodListString = ingredient.prepMethodList.join(", ");
		if ( !ingredient.prepMethodList.isEmpty() ) {
			m_sourceModel->setData( index, QVariant(prepMethodListString), Qt::EditRole );
		}
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->itemFromIndex( index )->setEditable( true );

	//Add the susbtitutes, if any
	int substitute_row = 0;
	Ingredient::SubstitutesList::const_iterator sub_it = ingredient.substitutes.constBegin();
	while ( sub_it != ingredient.substitutes.constEnd() ) {
		//Append a new blank row. NOTE: this
		//ingItem->setRowCount( ingredient.substitutes.count() );
		//doesn't work so we have to do it this way.
		QList<QStandardItem*> itemList;
		QStandardItem * childItem;
		int columnCount = m_sourceModel->columnCount();
		for ( int i = 0; i < columnCount; ++i ) {
			childItem = new QStandardItem;
			itemList << childItem;
		}
		ingItem->appendRow( itemList );
		//Set data for the row we have just created
		setRowDataAlternative( substitute_row, *sub_it, ingIndex );
		//Increase iterator and number of row
		++substitute_row;
		++sub_it;
	}
}

void IngredientsEditor::setRowData( int row, const Element & header )
{
	QModelIndex index;
	//The "Id" item.
	index = m_sourceModel->index( row, headerIdColumn() );
	m_sourceModel->setData( index, QVariant(header.id), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(header.id), IdRole );
	m_sourceModel->setData( index, QVariant(true), IsHeaderRole );
	m_sourceModel->itemFromIndex( index )->setEditable( false );
	//The "Header" item.
	index = m_sourceModel->index( row, headerColumn() );
	m_sourceModel->setData( index, QVariant(header.name), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(header.id), IdRole );
	m_sourceModel->setData( index, QVariant(true), IsHeaderRole );
	QStandardItem * headerItem = m_sourceModel->itemFromIndex( index );
	headerItem->setEditable( true );
	QFont font = headerItem->font();
	font.setBold( true );
	font.setUnderline( true );
	headerItem->setFont( font );
	//Set the rest of the empty columns (headers have no amount, units
	// or preparation method) as read only
	QStandardItem * item;
	for ( int i = 1; i <= 3; ++i ) {
		index = m_sourceModel->index( row, i );
		item = m_sourceModel->itemFromIndex( index );
		item->setEditable( false );
		item->setData( QVariant(true), IsHeaderRole );
	}
}

void IngredientsEditor::setRowDataAlternative( int row, const IngredientData & ingredient,
	const QModelIndex & parent )
{
	QStandardItem * parentItem = m_sourceModel->itemFromIndex( parent );
	//Ingredient name
	QStandardItem * subItem = parentItem->child( row, 0 );
	subItem->setData( QVariant(ingredient.name), Qt::EditRole );
	subItem->setData( QVariant( /*i18n("OR") + " " +*/ ingredient.name), Qt::DisplayRole );
	subItem->setData( QVariant(ingredient.ingredientID), IdRole );
	//Ingredient amount
	//(substitutes can't have an amount so adding a dummy item)
	QStandardItem * subItemAmount = parentItem->child( row, amountColumn() );
	subItemAmount->setEditable( false );
	//Ingredient units
	//(substitutes can't have an amount so adding a dummy item)
	QStandardItem * subItemUnits = parentItem->child( row, unitColumn() );
	subItemUnits->setEditable( false );
	//Preparation method
	//(substitutes can't have a preparation method so adding a dummy item)
	QStandardItem * subItemPrep = parentItem->child( row, prepmethodsColumn() );
	subItemPrep->setEditable( false );
	//Substitute ingredient ID
	QStandardItem * subItemId = parentItem->child( row, ingredientIdColumn() );
	subItemId->setEditable( false );
	subItemId->setData( QVariant(ingredient.ingredientID), Qt::DisplayRole );
}

void IngredientsEditor::resizeColumnsToContents()
{
	int columnCount = m_sourceModel->columnCount();
	for ( int i = 0; i < columnCount; ++i ) {
		ui->m_treeView->resizeColumnToContents( i );
	}
	//Put some extra space for the ingredient name column,
	//this way it won't look ugly when edited
	int columnWidth = ui->m_treeView->columnWidth( ingredientColumn() );
	ui->m_treeView->setColumnWidth( ingredientColumn(), columnWidth + 15 );
	//Put some extra space for the unit column,
	//this way it won't look ugly when edited
	columnWidth = ui->m_treeView->columnWidth( unitColumn() );
	ui->m_treeView->setColumnWidth( unitColumn(), columnWidth + 15 );
}

void IngredientsEditor::addIngredientSlot()
{
	//Add a new row
	int rowCount = m_sourceModel->rowCount();
	m_sourceModel->insertRow( rowCount );

	//Prepare the new ingredient data
	Ingredient ingredient;
	ingredient.ingredientID = RecipeDB::InvalidId;
	ingredient.units.setId( RecipeDB::InvalidId );
	QModelIndex ingNameIndex = m_sourceModel->index( rowCount, ingredientColumn() );

	//Add the data to the model
	setRowData( rowCount, ingredient );

	//Edit the ingredient name
	ui->m_treeView->setCurrentIndex( ingNameIndex );
	ui->m_treeView->scrollTo( ingNameIndex );
	ui->m_treeView->edit( ingNameIndex );
}

void IngredientsEditor::addAltIngredientSlot()
{
	//Get selected index and abort if nothing is selected
	QModelIndex currentIndex = ui->m_treeView->currentIndex();
	if ( currentIndex == QModelIndex() ) {
		return;
	}

	//Find out the index of the parent ingredient
	int selectedRow;
	if ( currentIndex.parent() == QModelIndex() ) {
		selectedRow = currentIndex.row();
	} else {
		selectedRow = currentIndex.parent().row();
	}
	QModelIndex parentIngIndex = m_sourceModel->index( selectedRow, 0 );

	//Add a child row to the parent ingredient
	QStandardItem * parentIngItem = m_sourceModel->itemFromIndex( parentIngIndex );
	int rowCount = parentIngItem->rowCount();
	kDebug() << rowCount << parentIngIndex;
	QList<QStandardItem*> itemList;
	QStandardItem * childItem;
	int columnCount = m_sourceModel->columnCount();
	for ( int i = 0; i < columnCount; ++i ) {
		childItem = new QStandardItem;
		itemList << childItem;
	}
	parentIngItem->appendRow( itemList );

	//Prepare data
	IngredientData ingredient;
	ingredient.ingredientID = RecipeDB::InvalidId;

	//Set data in the model
	parentIngIndex = m_sourceModel->index( selectedRow, 0 );
	setRowDataAlternative( rowCount, ingredient, parentIngIndex );

	//Edit the ingredient name
	QModelIndex ingNameIndex = m_sourceModel->indexFromItem( itemList.first() );
	ui->m_treeView->setCurrentIndex( ingNameIndex );
	ui->m_treeView->scrollTo( ingNameIndex );
	ui->m_treeView->edit( ingNameIndex );
}

void IngredientsEditor::addHeaderSlot()
{
	//Prepare header data
	Element header;
	header.id = RecipeDB::InvalidId;
	//Create a new row
	int rowCount = m_sourceModel->rowCount();
	m_sourceModel->insertRow( rowCount );
	//Put data in the row
	setRowData( rowCount, header );
	//Edit the header name
	QModelIndex headerNameIndex = m_sourceModel->index( rowCount, headerColumn() );
	ui->m_treeView->setCurrentIndex( headerNameIndex );
	ui->m_treeView->scrollTo( headerNameIndex );
	ui->m_treeView->edit( headerNameIndex );
}

void IngredientsEditor::ingParserSlot()
{
	UnitList units;
	m_database->loadUnits(&units);
	QPointer<IngredientParserDialog> dlg = new IngredientParserDialog(units,this);
	if ( dlg->exec() == QDialog::Accepted ) {
		IngredientList ings = dlg->ingredients();
		//TODO: Insert ingredients in the model
	}
	delete dlg;

}

void IngredientsEditor::moveRow( int offset )
{
	QModelIndex index = ui->m_treeView->currentIndex();
	int row = index.row();
	int col = index.column();
	if ( index.parent() != QModelIndex() ) {
		QStandardItem * parentItem = m_sourceModel->itemFromIndex( index.parent() );
		int rowCount = parentItem->rowCount();
		//Abort if we are moving the elements beyond the limit of the list
		//e.g. moving up the first element or moving down the last one
		if ( (row+offset < 0) || (row+offset > rowCount-1) ) {
			return;
		}
		//Move the item
		QList<QStandardItem*> itemList;
		itemList = parentItem->takeRow(row);
		parentItem->insertRow( row += offset, itemList );
		//Re-select it in the QTreeView
		index = m_sourceModel->indexFromItem( parentItem->child( row, col ) );
		ui->m_treeView->setCurrentIndex( index );
	} else {
		int rowCount = m_sourceModel->rowCount();
		//Abort if we are moving the elements beyond the limit of the list
		//e.g. moving up the first element or moving down the last one
		if ( (row+offset < 0) || (row+offset > rowCount-1) ) {
			return;
		}
		//Check if we have children and save it to re-add them after the moving
		bool gotChildren = false;
		QList< QList<QStandardItem*> > childRowItems;
		if ( m_sourceModel->hasChildren( index ) ) {
			QStandardItem * item = m_sourceModel->itemFromIndex( index );
			for ( int i = 0; i < item->rowCount(); ++i ) {
				childRowItems << m_sourceModel->itemFromIndex(index)->takeRow( 0 );
			}
			gotChildren = true;
		}
		//Move the item
		QList<QStandardItem*> itemList;
		itemList = m_sourceModel->takeRow(row);
		m_sourceModel->insertRow( row += offset, itemList );
		//Restore the children
		QStandardItem * item = m_sourceModel->item( row, 0 );
		if ( gotChildren ) {
			int childRow = 0;
			QList< QList<QStandardItem*> >::const_iterator it;
			for ( it = childRowItems.constBegin(); it != childRowItems.constEnd(); ++it ) {
				item->insertRow( childRow, *it );
				++childRow;
			}
		}
		//Re-select the moved ingredient in the QTreeView
		ui->m_treeView->expandAll();
		ui->m_treeView->setCurrentIndex( m_sourceModel->index( row, col ) );
	}
}

void IngredientsEditor::moveIngredientUpSlot()
{
	QModelIndex index = ui->m_treeView->currentIndex();
	if ( index != QModelIndex() ) {
		moveRow( -1 );
		emit changed();
	}
}

void IngredientsEditor::moveIngredientDownSlot()
{
	QModelIndex index = ui->m_treeView->currentIndex();
	if ( index != QModelIndex() ) {
		moveRow( +1 );
		emit changed();
	}
}

void IngredientsEditor::removeIngredientSlot()
{
	QModelIndex index = ui->m_treeView->currentIndex();
	if ( index != QModelIndex() ) {
		m_sourceModel->removeRow( index.row(), index.parent() );
		emit changed();
	}
}

void IngredientsEditor::updateNutrientInfoDetailsSlot()
{
	if ( !m_database ) {
		return;
	}

	Ingredient ingredient;
	QModelIndex index;
	NutrientInfo::Status nutrientInfoStatus;
	QString incompleteErrMsg;
	QString intermediateErrMsg;
	m_nutrientIncompleteCount = 0;
	m_nutrientIntermediateCount = 0;
	m_nutrientInfoDetailsDialog->clear();
	int rowCount = m_sourceModel->rowCount();
	for ( int i = 0; i < rowCount; ++i ) {
		//Read the ingredients we have so far in the editor
		ingredient = readIngredientFromRow(i);
		//Check the nutrient info status and error message
		nutrientInfoStatus = NutrientInfoDetailsDialog::checkIngredientStatus(
			ingredient, m_database,
			&incompleteErrMsg, &intermediateErrMsg );
		//Increase bad status counters
		if ( nutrientInfoStatus == NutrientInfo::Incomplete ) {
			++m_nutrientIncompleteCount;
			//Add the error message for displaying in the details dialog
			m_nutrientInfoDetailsDialog->addIncompleteText( incompleteErrMsg );
		}
		if ( nutrientInfoStatus == NutrientInfo::Intermediate ) {
			++m_nutrientIntermediateCount;
			//Add the error message for displaying in the details dialog
			m_nutrientInfoDetailsDialog->addIntermediateText( intermediateErrMsg );
		}
	}
	if ( m_nutrientIncompleteCount > 0 ) {
		ui->m_nutrientInfoStatusWidget->setStatus( NutrientInfo::Incomplete );
	} else if ( m_nutrientIntermediateCount > 0 ) {
		ui->m_nutrientInfoStatusWidget->setStatus( NutrientInfo::Intermediate );
	} else {
		ui->m_nutrientInfoStatusWidget->setStatus( NutrientInfo::Complete );
	}
	m_nutrientInfoDetailsDialog->setWindowTitle(
		i18nc( "@title", "Nutritive information status" ) +
		(m_recipeTitle.isEmpty()?QString():" - "+m_recipeTitle) );
	m_nutrientInfoDetailsDialog->displayText();

}

void IngredientsEditor::nutrientInfoDetailsSlot()
{
	updateNutrientInfoDetailsSlot();
	m_nutrientInfoDetailsDialog->show();
}

void IngredientsEditor::itemChangedSlot( QStandardItem * item )
{
	Q_UNUSED(item)
	ui->m_nutrientInfoStatusWidget->setStatus( NutrientInfo::Unknown );
}

void IngredientsEditor::ingredientCreatedDBSlot( const Element & newIngredient )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	bool isHeader;
	QStandardItem * item;
	QStandardItem * child;
	int rowCount = m_sourceModel->rowCount();
	int childRowCount;
	RecipeDB::IdType modelIngredientId;
	QString modelIngredientName;
	//Check in the model if there is an ingredient with the same name and the Id set
	//as RecipeDB::InvalidId (that means the ingredient is going to be created when
	//saving the recipe). If there is any, update its Id to the Id of the ingredient
	//which was just created in the database so we won't have nonsense duplicates.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, ingredientColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( isHeader ) {
			continue;
		}
		item = m_sourceModel->itemFromIndex( index );
		modelIngredientId = m_sourceModel->data( index, IdRole ).toInt();
		modelIngredientName = m_sourceModel->data( index, Qt::DisplayRole ).toString();
		if ( (modelIngredientId == RecipeDB::InvalidId)
		&& (newIngredient.name == modelIngredientName) ) {
			m_sourceModel->setData( index, newIngredient.id, IdRole );
			index = m_sourceModel->index( i, ingredientIdColumn() );
			m_sourceModel->setData( index, newIngredient.id, Qt::DisplayRole );
		}
		childRowCount = item->rowCount();
		for ( int j = 0; j < childRowCount; ++j ) {
			child = item->child( j, ingredientColumn() );
			modelIngredientId = child->data( IdRole ).toInt();
			modelIngredientName = child->data( Qt::DisplayRole ).toString();
			if ( (modelIngredientId == RecipeDB::InvalidId)
			&& (newIngredient.name == modelIngredientName) ) {
				child->setData( newIngredient.id, IdRole );
				child = item->child( j, ingredientIdColumn() );
				child->setData( newIngredient.id, Qt::DisplayRole );
			}
		}

	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::ingredientModifiedDBSlot( const Ingredient & newIngredient )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	bool isHeader;
	QStandardItem * item;
	QStandardItem * child;
	int rowCount = m_sourceModel->rowCount();
	int childRowCount;
	RecipeDB::IdType modelIngredientId;
	//Find the modified ingredient in the model and update its name
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, ingredientColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( isHeader ) {
			continue;
		}
		item = m_sourceModel->itemFromIndex( index );
		modelIngredientId = m_sourceModel->data( index, IdRole ).toInt();
		if ( newIngredient.ingredientID == modelIngredientId ) {
			m_sourceModel->setData( index, newIngredient.name, Qt::DisplayRole );
		}
		childRowCount = item->rowCount();
		for ( int j = 0; j < childRowCount; ++j ) {
			child = item->child( j, ingredientColumn() );
			modelIngredientId = child->data( IdRole ).toInt();
			if ( newIngredient.ingredientID == modelIngredientId ) {
				child->setData( newIngredient.name, Qt::DisplayRole );
			}
		}

	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::ingredientRemovedDBSlot( RecipeDB::IdType ingredientRemovedId )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	bool isHeader;
	QStandardItem * item;
	QStandardItem * child;
	RecipeDB::IdType modelIngredientId;
	int rowCount = m_sourceModel->rowCount();
	int childRowCount;
	//If the ingredient was removed in the database set the ID to RecipeDB::InvalidId
	//in the model, this means the ingredient will be created again when saving the
	//recipe.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, ingredientColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( isHeader ) {
			continue;
		}
		item = m_sourceModel->itemFromIndex( index );
		modelIngredientId = m_sourceModel->data( index, IdRole ).toInt();
		if ( ingredientRemovedId == modelIngredientId ) {
			m_sourceModel->setData( index, RecipeDB::InvalidId, IdRole );
			index = m_sourceModel->index( i, ingredientIdColumn() );
			m_sourceModel->setData( index, RecipeDB::InvalidId, Qt::DisplayRole );
		}
		childRowCount = item->rowCount();
		for ( int j = 0; j < childRowCount; ++j ) {
			child = item->child( j, ingredientColumn() );
			modelIngredientId = child->data( IdRole ).toInt();
			if ( ingredientRemovedId == modelIngredientId ) {
				child->setData( RecipeDB::InvalidId, IdRole );
				child = item->child( j, ingredientIdColumn() );
				child->setData( RecipeDB::InvalidId, Qt::DisplayRole );
			}
		}
	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::unitCreatedDBSlot( const Unit & newUnit )
{
	Q_UNUSED(newUnit)
	//We don't need to add code here for now because right now new units
	//are created in the database when the user types a new unit
}

void IngredientsEditor::unitModifiedDBSlot( const Unit & newUnit )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	RecipeDB::IdType modelUnitId;
	QString newUnitName;
	Ingredient modelIngredient;

	//Find the modified ingredient in the model and update its name
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, unitColumn() );
		modelUnitId = m_sourceModel->data( index, IdRole ).toInt();
		if ( newUnit.id() == modelUnitId ) {
			modelIngredient = readIngredientFromRow( i );
			newUnitName = modelIngredient.amountUnitString();
			m_sourceModel->setData( index, newUnitName, Qt::DisplayRole );
		}

	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

}

void IngredientsEditor::unitRemovedDBSlot( int unitRemovedId )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	RecipeDB::IdType modelUnitId;
	int rowCount = m_sourceModel->rowCount();
	//If the ingredient was removed in the database set the ID to RecipeDB::InvalidId
	//in the model, this means the ingredient will be created again when saving the
	//recipe.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, unitColumn() );
		modelUnitId = m_sourceModel->data( index, IdRole ).toInt();
		if ( unitRemovedId == modelUnitId ) {
			m_sourceModel->setData( index, RecipeDB::InvalidId, IdRole );
			m_sourceModel->setData( index, QString(""), Qt::DisplayRole );
		}
	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

}

void IngredientsEditor::prepMethodCreatedDBSlot( const Element & newPrepMethod )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	QString modelIngredientName;
	bool prepMethodFound;
	//Check in the model if there is a preparation method with the same name and the Id
	//set as RecipeDB::InvalidId (that means the preparation method is going to be created
	//when saving the recipe). If there is any, update its Id to the Id of the preparation
	//method which was just created in the database so we won't have nonsense duplicates.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, prepmethodsColumn() );

		QList<QVariant> prepMethodsIds = m_sourceModel->data( index, IdRole ).toList();
		QString prepMethodsString = m_sourceModel->data( index, Qt::EditRole ).toString();
		QStringList prepStringList = prepMethodsString.split(", ", QString::SkipEmptyParts);
		QList<QVariant>::iterator prep_ids_it = prepMethodsIds.begin();
		QStringList::const_iterator prep_str_it = prepStringList.constBegin();
		Element element;
		prepMethodFound = false;
		while ( prep_str_it != prepStringList.constEnd() ) {
			if ( (prep_ids_it->toInt() == RecipeDB::InvalidId)
			&& (*prep_str_it == newPrepMethod.name) ) {
				*prep_ids_it = newPrepMethod.id;
				prepMethodFound = true;
			}
			++prep_ids_it;
			++prep_str_it;
		}

		if ( prepMethodFound ) {
			m_sourceModel->setData( index, prepMethodsIds, IdRole );
		}
	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::prepMethodModifiedDBSlot( const Element & newPrepMethod )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	QString modelIngredientName;
	bool prepMethodFound;
	//Find the modified prep method in the model and update its name
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, prepmethodsColumn() );

		QList<QVariant> prepMethodsIds = m_sourceModel->data( index, IdRole ).toList();
		QString prepMethodsString = m_sourceModel->data( index, Qt::EditRole ).toString();
		QStringList prepStringList = prepMethodsString.split(", ", QString::SkipEmptyParts);
		QList<QVariant>::const_iterator prep_ids_it = prepMethodsIds.constBegin();
		QStringList::iterator prep_str_it = prepStringList.begin();
		Element element;
		prepMethodFound = false;
		while ( prep_str_it != prepStringList.end() ) {
			if ( prep_ids_it->toInt() == newPrepMethod.id ) {
				*prep_str_it = newPrepMethod.name;
				prepMethodFound = true;
			}
			++prep_ids_it;
			++prep_str_it;
		}

		if ( prepMethodFound ) {
			prepMethodsString = prepStringList.join(", ");
			m_sourceModel->setData( index, prepMethodsString, Qt::DisplayRole );
		}
	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::prepMethodRemovedDBSlot( int prepMethodRemovedId )
{

	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	QString modelIngredientName;
	bool prepMethodFound;
	//Find the preparation method in the model and, if found, set its Id as RecipeDB::InvalidId
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, prepmethodsColumn() );

		QList<QVariant> prepMethodsIds = m_sourceModel->data( index, IdRole ).toList();
		QString prepMethodsString = m_sourceModel->data( index, Qt::EditRole ).toString();
		QStringList prepStringList = prepMethodsString.split(", ", QString::SkipEmptyParts);
		QList<QVariant>::iterator prep_ids_it = prepMethodsIds.begin();
		QStringList::const_iterator prep_str_it = prepStringList.constBegin();
		Element element;
		prepMethodFound = false;
		while ( prep_str_it != prepStringList.constEnd() ) {
			if ( prep_ids_it->toInt() == prepMethodRemovedId ) {
				*prep_ids_it = RecipeDB::InvalidId;
				kDebug() << "found";
				prepMethodFound = true;
			}
			++prep_ids_it;
			++prep_str_it;
		}

		if ( prepMethodFound ) {
			m_sourceModel->setData( index, prepMethodsIds, IdRole );
		}
	}


	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::headerCreatedDBSlot( const Element & newHeader )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	RecipeDB::IdType modelHeaderId;
	QString modelHeaderName;
	bool isHeader;
	//Check in the model if there is a header with the same name and the Id set
	//as RecipeDB::InvalidId (that means the header is going to be created when
	//saving the recipe). If there is any, update its Id to the Id of the header
	//which was just created in the database so we won't have nonsense duplicates.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, headerColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( !isHeader ) {
			continue;
		}
		modelHeaderId = m_sourceModel->data( index, IdRole ).toInt();
		modelHeaderName = m_sourceModel->data( index, Qt::DisplayRole ).toString();
		if ( (modelHeaderId == RecipeDB::InvalidId)
		&& (newHeader.name == modelHeaderName) ) {
			m_sourceModel->setData( index, newHeader.id, IdRole );
			index = m_sourceModel->index( i, headerIdColumn() );
			m_sourceModel->setData( index, newHeader.id, Qt::DisplayRole );
		}

	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

void IngredientsEditor::headerModifiedDBSlot( const Element & newHeader )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	RecipeDB::IdType modelHeaderId;
	QString modelHeaderName;
	bool isHeader;
	//Check in the model if there is a header with the same name and the Id set
	//as RecipeDB::InvalidId (that means the header is going to be created when
	//saving the recipe). If there is any, update its Id to the Id of the header
	//which was just created in the database so we won't have nonsense duplicates.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, headerColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( !isHeader ) {
			continue;
		}
		modelHeaderId = m_sourceModel->data( index, IdRole ).toInt();
		modelHeaderName = m_sourceModel->data( index, Qt::DisplayRole ).toString();
		if ( modelHeaderId == newHeader.id ) {
			m_sourceModel->setData( index, newHeader.name, Qt::DisplayRole );
		}

	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

}

void IngredientsEditor::headerRemovedDBSlot( int headerRemovedId )
{
	//Disconnect the changed signal temporarily
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	QModelIndex index;
	int rowCount = m_sourceModel->rowCount();
	RecipeDB::IdType modelHeaderId;
	QString modelHeaderName;
	bool isHeader;
	//Check in the model if there is a header with the same name and the Id set
	//as RecipeDB::InvalidId (that means the header is going to be created when
	//saving the recipe). If there is any, update its Id to the Id of the header
	//which was just created in the database so we won't have nonsense duplicates.
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, headerColumn() );
		isHeader = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( !isHeader ) {
			continue;
		}
		modelHeaderId = m_sourceModel->data( index, IdRole ).toInt();
		modelHeaderName = m_sourceModel->data( index, Qt::DisplayRole ).toString();
		if ( modelHeaderId == headerRemovedId ) {
			m_sourceModel->setData( index, RecipeDB::InvalidId, IdRole );
			index = m_sourceModel->index( i, headerIdColumn() );
			m_sourceModel->setData( index, RecipeDB::InvalidId, Qt::DisplayRole );
		}
	}

	//Re-connect the changed signal
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
}

Ingredient IngredientsEditor::readIngredientFromRow( int row )
{
	Ingredient ingredient;

	//Ingredient Id
	QModelIndex index = m_sourceModel->index( row, ingredientColumn() );
	ingredient.ingredientID = m_sourceModel->data( index, IdRole ).toInt();
	//Ingredient name
	ingredient.name = m_sourceModel->data( index, Qt::EditRole ).toString();
	//Ingredient amount
	index = m_sourceModel->index( row, amountColumn() );
	QString amountString = m_sourceModel->data( index, Qt::EditRole ).toString();
	MixedNumberRange range;
	MixedNumberRange::fromString( amountString, range );
	ingredient.setAmountRange( range );
	//Ingredient units
	index = m_sourceModel->index( row, unitColumn() );
	RecipeDB::IdType unitsId = m_sourceModel->data( index, IdRole ).toInt();
	ingredient.units.setId( unitsId );
	if ( m_database ) {
		//This is needed to get the unit type which must be known
		//to check the nutrient info status
		ingredient.units = m_database->unitName( unitsId );
	}
	//Ingredient preparation methods
	index = m_sourceModel->index( row, prepmethodsColumn() );
	QList<QVariant> prepMethodsIds = m_sourceModel->data( index, IdRole ).toList();
	QString prepMethodsString = m_sourceModel->data( index, Qt::EditRole ).toString();
	QStringList prepStringList = prepMethodsString.split(", ", QString::SkipEmptyParts);
	QList<QVariant>::const_iterator prep_ids_it = prepMethodsIds.constBegin();
	QStringList::const_iterator prep_str_it = prepStringList.constBegin();
	Element element;
	while ( prep_str_it != prepStringList.constEnd() ) {
		element.id = prep_ids_it->toInt();
		element.name = *prep_str_it;
		ingredient.prepMethodList << element;
		++prep_ids_it;
		++prep_str_it;
	}

	return ingredient;
}

void IngredientsEditor::updateIngredientList()
{
	//Clear the IngredientList.
	m_ingredientList->clear();

	//Close open editor (if any) and submit data to the model
	QModelIndex currentIndex = ui->m_treeView->currentIndex();
	disconnect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );
	ui->m_treeView->setCurrentIndex( QModelIndex() );
	ui->m_treeView->setCurrentIndex( currentIndex );
	connect( m_sourceModel, SIGNAL(itemChanged(QStandardItem*)),
		this, SIGNAL(changed()) );

	//Dump the contents of the GUI to the IngredientList
	int rowCount = m_sourceModel->rowCount();
	QModelIndex index;
	Element lastHeaderKnown;
	lastHeaderKnown.id = RecipeDB::InvalidId;
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, headerColumn() );
		//Skip the row if it's a header;
		bool is_header = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( is_header ) {
			lastHeaderKnown.id = index.data( IdRole ).toInt();
			lastHeaderKnown.name = index.data( Qt::EditRole ).toString();
			continue;
		}
		//Dump the contents of the current row to an ingredient Object
		Ingredient ingredient = readIngredientFromRow( i );
		//Ingredient group id
		ingredient.groupID = lastHeaderKnown.id;
		//Ingredient group name
		ingredient.group = lastHeaderKnown.name;
		//Dump the contents of the row childrens as ingredient substitutes
		index = m_sourceModel->index( i, ingredientColumn() );
		if ( m_sourceModel->hasChildren( index ) ) {
			QStandardItem * item = m_sourceModel->itemFromIndex( index );
			int childCount = item->rowCount();
			for ( int j = 0; j < childCount; ++j ) {
				IngredientData substitute;
				substitute.ingredientID = item->child( j )->data( IdRole ).toInt();
				substitute.name = item->child( j )->data( Qt::EditRole ).toString();
				ingredient.substitutes << substitute;
			}
		}
		//Append the Ingredient Object to the list
		*m_ingredientList << ingredient;
	}
	kDebug() << *m_ingredientList;
}
