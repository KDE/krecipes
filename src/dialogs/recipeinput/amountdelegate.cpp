/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "amountdelegate.h"

#include <KLineEdit>


AmountDelegate::AmountDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}


QWidget * AmountDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
	const QModelIndex & index ) const
{
	KLineEdit * editor = new KLineEdit( parent );
	//TODO
	return editor;
}

void AmountDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	//TODO
}

void AmountDelegate::setModelData(QWidget *editor, 
	QAbstractItemModel *model, const QModelIndex &index) const
{
	//TODO
}

void AmountDelegate::updateEditorGeometry(QWidget *editor, 
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

