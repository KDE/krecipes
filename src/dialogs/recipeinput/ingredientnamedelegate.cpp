/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "ingredientnamedelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "backends/recipedb.h"
#include "datablocks/unit.h"

#include <KComboBox>
#include <QSortFilterProxyModel>

//#include <kdebug.h>


IngredientNameDelegate::IngredientNameDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void IngredientNameDelegate::loadAllIngredientsList( RecipeDB * database )
{
	//FIXME: This doesn't respect the limits configured in the program
	database->loadIngredientMaps( &m_idToIngredientMap, &m_ingredientNameToIdMap );

	connect( database, SIGNAL(ingredientCreated(const Element&)),
		this, SLOT(ingredientCreatedSlot(const Element&)) );
	connect( database, SIGNAL(ingredientModified(const Ingredient&)),
		this, SLOT(ingredientModifiedSlot(const Ingredient&)) );
	connect( database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedSlot(int)) );
}

void IngredientNameDelegate::loadAllHeadersList( RecipeDB * database )
{
	//FIXME: This doesn't respect the limits configured in the program
	database->loadIngredientGroups( &m_headerList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	ElementList::const_iterator it = m_headerList.constBegin();
	while ( it != m_headerList.constEnd() ) {
		m_headerNameToIdMap[it->name] = it->id;
		++it;
	}
	connect( database, SIGNAL(ingGroupCreated(const Element&)),
		this, SLOT(headerCreatedSlot(const Element&)) );
	connect( database, SIGNAL(ingGroupRemoved(int)),
		this, SLOT(headerRemovedSlot(int)) );
}

#include "kdebug.h"
void IngredientNameDelegate::ingredientCreatedSlot( const Element & element )
{
	m_idToIngredientMap[element.id] = element;
	m_ingredientNameToIdMap.insert(element.name, element.id);
}

void IngredientNameDelegate::ingredientModifiedSlot( const Ingredient & ingredient )
{
	ingredientRemovedSlot( ingredient.ingredientID );
	ingredientCreatedSlot( Element( ingredient.name, ingredient.ingredientID ) );
}

void IngredientNameDelegate::ingredientRemovedSlot( int id )
{
	QString ingredientName = m_idToIngredientMap[id].name;
	m_idToIngredientMap.remove( id );
	m_ingredientNameToIdMap.remove( ingredientName, id );
}

void IngredientNameDelegate::headerCreatedSlot( const Element & element )
{
	m_headerNameToIdMap[element.name] = element.id;
	m_headerList << element;
}

void IngredientNameDelegate::headerRemovedSlot( int id )
{
	m_headerNameToIdMap.remove( m_headerNameToIdMap.keys(id).first() );
}


QWidget * IngredientNameDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
	const QModelIndex & index ) const
{
	//Set up the combo box
	KComboBox * editor = new KComboBox( parent );
	editor->setAutoFillBackground( true );
	editor->setEditable( true );
	editor->setCompletionMode( KGlobalSettings::CompletionPopup );

	//Order
	editor->completionObject()->setOrder( KCompletion::Sorted );
	QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel( editor );
	proxyModel->setSourceModel(editor->model());
	// editor's current model must be reparented,
	// otherwise QComboBox::setModel() will delete it
	editor->model()->setParent(proxyModel);
	editor->setModel( proxyModel );

	//Fill the items and the completion objects
	int i = 0;
	if ( index.data(IngredientsEditor::IsHeaderRole).toBool() ) {
		ElementList::const_iterator it;
		ElementList::const_iterator list_end;
		it = m_headerList.constBegin();
		list_end = m_headerList.constEnd();
		QFont font = editor->font();
		font.setBold( true );
		font.setUnderline( true );
		editor->setFont( font );
		while ( it != list_end ) {
			editor->insertItem( i, it->name );
			editor->completionObject()->addItem( it->name );
			++i;
			++it;
		}
	} else {
		QHash<RecipeDB::IdType,Element>::const_iterator it = m_idToIngredientMap.constBegin();
		while (it != m_idToIngredientMap.constEnd()) {
			editor->insertItem( i, it.value().name );
			editor->completionObject()->addItem( it.value().name );
			++i;
			++it;
		}
	}
	proxyModel->sort(0);

	return editor;
}

void IngredientNameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString text = index.data( Qt::EditRole ).toString();
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	comboBox->setEditText( text );
}

void IngredientNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	QString text = comboBox->currentText();
	model->setData( index, text, Qt::EditRole );

	if ( index.data(IngredientsEditor::IsHeaderRole).toBool() ) {
		//The edited item is a header
		if ( m_headerNameToIdMap.contains(text) ) {
			model->setData( index, m_headerNameToIdMap[text], IngredientsEditor::IdRole );
		} else {
			model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
		}
	} else {
		//The edited item is an ingredient
		if ( m_ingredientNameToIdMap.contains(text) ) {
			model->setData( index, m_ingredientNameToIdMap.values(text).first(), IngredientsEditor::IdRole );
		} else {
			model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
		}
	}
}

void IngredientNameDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

