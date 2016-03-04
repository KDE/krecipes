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
#include "datablocks/ingredientlist.h"
#include "backends/recipedb.h"

#include <QStandardItemModel>

//FIXME: Remove this when this class is finished
#include "kdebug.h"

IngredientsEditor::IngredientsEditor( QWidget * parent)
		: QWidget( parent ),
		m_ingredientList( 0 ),
		m_database( 0 )
{
	ui = new Ui::IngredientsEditor;
	ui->setupUi( this );

	m_sourceModel = new QStandardItemModel;
	ui->m_treeView->setModel( m_sourceModel );


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
}

void IngredientsEditor::setDatabase( RecipeDB * database )
{
	m_database = database;
}

void IngredientsEditor::loadIngredientList( IngredientList * ingredientList )
{
	m_ingredientList = ingredientList;

	m_sourceModel->setRowCount( 0 );
	m_sourceModel->setRowCount( ingredientList->count() );

	IngredientNameDelegate * ingredientNameDelegate = new IngredientNameDelegate;
	if ( m_database ) {
		ingredientNameDelegate->loadAllIngredientsList( m_database );
	}
	ui->m_treeView->setItemDelegateForColumn(0, ingredientNameDelegate);

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
	index = m_sourceModel->index( row, 4 );
	m_sourceModel->setData( index, QVariant(ingredient.ingredientID), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.groupID), GroupIdRole );
	m_sourceModel->setData( index, QVariant(ingredient.group), GroupNameRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), UnitsIdRole );
	m_sourceModel->itemFromIndex( index )->setEditable( false );
	//The "Ingredient" item.
	index = m_sourceModel->index( row, 0 );
	m_sourceModel->setData( index, QVariant(ingredient.ingredientID), IdRole );
	m_sourceModel->setData( index, QVariant(ingredient.name), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.groupID), GroupIdRole );
	m_sourceModel->setData( index, QVariant(ingredient.group), GroupNameRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), UnitsIdRole );
	QStandardItem * ingItem = m_sourceModel->itemFromIndex( index );
	QPersistentModelIndex ingIndex = index;
	ingItem->setEditable( true );
	//The "Amount" item.
	index = m_sourceModel->index( row, 1 );
	m_sourceModel->setData( index, QVariant(ingredient.amountString()), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.groupID), GroupIdRole );
	m_sourceModel->setData( index, QVariant(ingredient.group), GroupNameRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), UnitsIdRole );
	m_sourceModel->itemFromIndex( index )->setEditable( true );
	//The "Units" item.
	index = m_sourceModel->index( row, 2 );
	m_sourceModel->setData( index, QVariant(ingredient.amountUnitString()), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.groupID), GroupIdRole );
	m_sourceModel->setData( index, QVariant(ingredient.group), GroupNameRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), UnitsIdRole );
	m_sourceModel->itemFromIndex( index )->setEditable( true );
	//The "PreparationMethod" item.
	index = m_sourceModel->index( row, 3 );
	ElementList::const_iterator prep_it = ingredient.prepMethodList.constBegin();
	while (prep_it != ingredient.prepMethodList.constEnd()) {
		m_prepMethodNameToId[prep_it->name] = prep_it->id;
		++prep_it;
	}
	QString prepMethodListString = ingredient.prepMethodList.join(", ");
	if ( !ingredient.prepMethodList.isEmpty() )
		m_sourceModel->setData( index, QVariant(prepMethodListString), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(false), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(ingredient.groupID), GroupIdRole );
	m_sourceModel->setData( index, QVariant(ingredient.group), GroupNameRole );
	m_sourceModel->setData( index, QVariant(ingredient.units.id()), UnitsIdRole );
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
	index = m_sourceModel->index( row, 4 );
	m_sourceModel->setData( index, QVariant(header.id), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(true), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(header.id), GroupIdRole );
	m_sourceModel->setData( index, QVariant(header.name), GroupNameRole );
	m_sourceModel->itemFromIndex( index )->setEditable( false );
	//The "Header" item.
	index = m_sourceModel->index( row, 0 );
	m_sourceModel->setData( index, QVariant(header.name), Qt::EditRole );
	m_sourceModel->setData( index, QVariant(true), IsHeaderRole );
	m_sourceModel->setData( index, QVariant(header.id), GroupIdRole );
	m_sourceModel->setData( index, QVariant(header.name), GroupNameRole );
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
	QStandardItem * subItemAmount = parentItem->child( row, 1 );
	subItemAmount->setEditable( false );
	//Ingredient units
	//(substitutes can't have an amount so adding a dummy item)
	QStandardItem * subItemUnits = parentItem->child( row, 2 );
	subItemUnits->setEditable( false );
	//Preparation method
	//(substitutes can't have a preparation method so adding a dummy item)
	QStandardItem * subItemPrep = parentItem->child( row, 3 );
	subItemPrep->setEditable( false );
	//Substitute ingredient ID
	QStandardItem * subItemId = parentItem->child( row, 4 );
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
	int columnWidth = ui->m_treeView->columnWidth( 0 );
	ui->m_treeView->setColumnWidth( 0, columnWidth + 15 );
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
	QModelIndex ingNameIndex = m_sourceModel->index( rowCount, 0 );
	if ( ingNameIndex.row() == 0 ) {
		ingredient.groupID = RecipeDB::InvalidId;
	} else {
		QModelIndex ingAboveIndex = m_sourceModel->index( ingNameIndex.row()-1, 0);
		ingredient.groupID = m_sourceModel->data( ingAboveIndex, GroupIdRole ).toInt();
		ingredient.group = m_sourceModel->data( ingAboveIndex, GroupNameRole ).toString();
	}

	//Add the data to the model
	setRowData( rowCount, ingredient );

	//Edit the ingredient name
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
	QModelIndex headerNameIndex = m_sourceModel->index( rowCount, 0 );
	ui->m_treeView->edit( headerNameIndex );
}

void IngredientsEditor::ingParserSlot()
{
	kDebug() << "here";
	//TODO
	//Don't forget emit changed();
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

void IngredientsEditor::updateIngredientList()
{
	//Clear the IngredientList.
	m_ingredientList->clear();

	//Dump the contents of the GUI to the IngredientList
	int rowCount = m_sourceModel->rowCount();
	QModelIndex index;
	for ( int i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, 0 );
		//Skip the row if it's a header;
		bool is_header = m_sourceModel->data( index, IsHeaderRole ).toBool();
		if ( is_header ) {
			continue;
		}
		//Dump the contents of the current row to a ingredient Object
		Ingredient ingredient;
		//Ingredient Id
		ingredient.ingredientID = m_sourceModel->data( index, IdRole ).toInt();
		//Ingredient name
		ingredient.name = m_sourceModel->data( index, Qt::EditRole ).toString();
		//Ingredient group id
		ingredient.groupID = m_sourceModel->data( index, GroupIdRole ).toInt();
		//Ingredient group name
		ingredient.group = m_sourceModel->data( index, GroupNameRole ).toString();
		//Ingredient amount
		index = m_sourceModel->index( i, 1 );
		QString amountString = m_sourceModel->data( index, Qt::EditRole ).toString();
		ingredient.setAmount( amountString );
		//Ingredient units
		index = m_sourceModel->index( i, 2 );
		ingredient.units.setId( m_sourceModel->data( index, UnitsIdRole ).toInt() );
		//Ingredient preparation methods
		index = m_sourceModel->index( i, 3 );
		QString prepMethodsString = m_sourceModel->data( index, Qt::EditRole ).toString();
		QStringList prepStringList = prepMethodsString.split(",", QString::SkipEmptyParts);
		QStringList::const_iterator prep_str_it = prepStringList.constBegin();
		Element element;
		while ( prep_str_it != prepStringList.constEnd() ) {
			element.id = m_prepMethodNameToId[prep_str_it->trimmed()];
			element.name = prep_str_it->trimmed();
			ingredient.prepMethodList << element;
			++prep_str_it;
		}
		//Dump the contents of the row childrens as ingredient substitutes
		index = m_sourceModel->index( i, 0 );
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
