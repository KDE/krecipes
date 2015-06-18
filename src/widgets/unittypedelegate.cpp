/****************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "unittypedelegate.h"

#include <KComboBox>

#include "datablocks/unit.h"

//#include <kdebug.h>


UnitTypeDelegate::UnitTypeDelegate(QObject *parent): QItemDelegate(parent)
{
}

QWidget * UnitTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	KComboBox *editor = new KComboBox( parent );
	editor->setAutoFillBackground( true );
	editor->setEditable( false );
	editor->insertItem( Unit::Other, Unit::typeToString(Unit::Other) );
	editor->insertItem( Unit::Mass, Unit::typeToString(Unit::Mass) );
	editor->insertItem( Unit::Volume, Unit::typeToString(Unit::Volume) );
	return editor;
}

void UnitTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QVariant value = index.model()->data( index, Qt::UserRole ).toString();
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	comboBox->setCurrentIndex( value.toInt() );
}

void UnitTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	QString value = comboBox->currentText();
	model->setData( index, comboBox->currentIndex(), Qt::UserRole );
	model->setData( index, value, Qt::EditRole );
}

void UnitTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

