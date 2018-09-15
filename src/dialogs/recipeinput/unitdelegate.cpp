/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "unitdelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "dialogs/createunitdialog.h"
#include "backends/recipedb.h"
#include "datablocks/unit.h"

#include <KComboBox>

//#include <kdebug.h>


UnitDelegate::UnitDelegate(QObject *parent): QStyledItemDelegate(parent)
{
	m_database = 0;
}

void UnitDelegate::loadAllUnitsList( RecipeDB * database )
{
	m_database = database;
	m_database->loadUnits( &m_unitList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	UnitList::const_iterator it = m_unitList.constBegin();
	while ( it != m_unitList.constEnd() ) {
		m_singularNameToIdMap[it->name()] = it->id();
		m_pluralNameToIdMap[it->plural()] = it->id();
		++it;
	}
	//TODO: connect database signals
	/*connect( database, SIGNAL(ingredientCreated(Element)),
		this, SLOT(ingredientCreatedSlot(Element)) );
	connect( database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedSlot(int)) );*/

}

/*void UnitDelegate::ingredientCreatedSlot( const Element & element )
{
	m_ingredientNameToIdMap[element.name] = element.id;
	m_ingredientList << element;
}*/

/*void UnitDelegate::ingredientRemovedSlot( int id )
{
	m_ingredientNameToIdMap.remove( m_ingredientNameToIdMap.keys(id).first() );
}*/

QWidget * UnitDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
	const QModelIndex & index ) const
{
	//Set up the combo box
	KComboBox * editor = new KComboBox( parent );
	editor->setAutoFillBackground( true );
	editor->setEditable( true );
	editor->setCompletionMode( KGlobalSettings::CompletionPopup );

	//Fill the items and the completion objects
	UnitList::const_iterator unit_it = m_unitList.constBegin();
	if ( !index.data(IngredientsEditor::IsPluralRole).toBool() ) {
		int i = 0;
		while ( unit_it != m_unitList.constEnd() ) {
			editor->insertItem( i, unit_it->name() );
			editor->completionObject()->addItem( unit_it->name() );
			++i; ++unit_it;
		}
	} else {
		int i = 0;
		while ( unit_it != m_unitList.constEnd() ) {
			editor->insertItem( i, unit_it->plural() );
			editor->completionObject()->addItem( unit_it->plural() );
			++i; ++unit_it;
		}
	}
	return editor;
}

void UnitDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString text = index.data( Qt::EditRole ).toString();
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	comboBox->setEditText( text );
}

void UnitDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	QString text = comboBox->currentText();
	model->setData( index, text, Qt::EditRole );

	if ( text.trimmed().isEmpty() ) {
		model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
		model->setData( index, QString(""), Qt::EditRole );
		return;
	}

	bool singular = !index.data(IngredientsEditor::IsPluralRole).toBool();
	if ( singular && m_singularNameToIdMap.contains(text) ) {
		//The edited item is singular and exists already
		model->setData( index, m_singularNameToIdMap[text], IngredientsEditor::IdRole );
	} else if ( !singular && m_pluralNameToIdMap.contains(text) ) {
		//The edited item is plural and exists already
		model->setData( index, m_pluralNameToIdMap[text], IngredientsEditor::IdRole );
	} else {
		//The edited item contains a new unit, ask user for data
		QPointer<CreateUnitDialog> getUnit = new CreateUnitDialog( 0, text, text, text, text );
		if ( getUnit->exec() == QDialog::Accepted ) {
			Unit unit = getUnit->newUnit();
			RecipeDB::IdType unitId = m_database->createNewUnit( unit );
			model->setData( index, unitId, IngredientsEditor::IdRole );
		} else {
			model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
			model->setData( index, QString(""), Qt::EditRole );
		}
		delete getUnit;
	}
}

void UnitDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

