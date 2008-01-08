/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dependanciesdialog.h"
#include "datablocks/elementlist.h"

#include <qvbox.h>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>

DependanciesDialog::DependanciesDialog( QWidget *parent, const QValueList<ListInfo> &lists, bool deps_are_deleted ) : KDialogBase( parent, "DependanciesDialog", true, QString::null,
	  KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Cancel ),
	  m_depsAreDeleted(deps_are_deleted)
{
	init( lists );
}

DependanciesDialog::DependanciesDialog( QWidget *parent, const ListInfo &list, bool deps_are_deleted ) : KDialogBase( parent, "DependanciesDialog", true, QString::null,
	  KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Cancel ),
	  m_depsAreDeleted(deps_are_deleted)
{
	QValueList<ListInfo> lists;
	lists << list;
	init( lists );
}

DependanciesDialog::~DependanciesDialog()
{}	

void DependanciesDialog::init( const QValueList<ListInfo> &lists )
{
	QVBox *page = makeVBoxMainWidget();

	// Design the dialog

	instructionsLabel = new QLabel( page );
	instructionsLabel->setMinimumSize( QSize( 100, 30 ) );
	instructionsLabel->setMaximumSize( QSize( 10000, 10000 ) );
	instructionsLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
	
	if ( m_depsAreDeleted ) {
		instructionsLabel->setText( i18n( "<b>WARNING:</b> The following will have to be removed also, since currently they use the element you have chosen to be removed." ) );
	}
	else {
		instructionsLabel->setText( i18n( "<b>WARNING:</b> The following currently use the element you have chosen to be removed." ) );
	}

	for ( QValueList<ListInfo>::const_iterator list_it = lists.begin(); list_it != lists.end(); ++list_it ) {
		if ( !((*list_it).list).isEmpty() ) {
			QGroupBox *groupBox = new QGroupBox( 1, Qt::Vertical, (*list_it).name, page );
			KListBox *listBox = new KListBox( groupBox );
			loadList( listBox, (*list_it).list );
		}
	}

	setSizeGripEnabled( true );
}

void DependanciesDialog::loadList( KListBox* listBox, const ElementList &list )
{
	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );

	for ( ElementList::const_iterator el_it = list.begin(); el_it != list.end(); ++el_it ) {
		QString name = ( *el_it ).name;
		if ( show_id )
			name += " (" + QString::number(( *el_it ).id) + ")";
		listBox->insertItem( name );
	}
}

void DependanciesDialog::accept()
{
	if ( !m_msg.isEmpty() ) {
		switch ( KMessageBox::warningYesNo(this,
			QString("<b>%1</b><br><br>%2").arg(m_msg).arg(i18n("Are you sure you wish to proceed?")),
			QString::null,KStdGuiItem::yes(),KStdGuiItem::no(),"doubleCheckDelete") )
		{
		case KMessageBox::Yes: QDialog::accept(); break;
		case KMessageBox::No: QDialog::reject(); break;
		}
	}
	else
		QDialog::accept();
}
