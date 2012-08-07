/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dependanciesdialog.h"
#include "datablocks/elementlist.h"

#include <kvbox.h>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QVBoxLayout>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>


DependanciesDialog::DependanciesDialog( QWidget *parent, const QList<ListInfo> &lists, bool deps_are_deleted )
	: KDialog( parent ), m_depsAreDeleted(deps_are_deleted)
{
	init( lists );
}

DependanciesDialog::DependanciesDialog( QWidget *parent, const ListInfo &list, bool deps_are_deleted ) : KDialog( parent ),
	m_depsAreDeleted(deps_are_deleted)
{
	QList<ListInfo> lists;
	lists << list;
	init( lists );
}

DependanciesDialog::~DependanciesDialog()
{}

void DependanciesDialog::init( const QList<ListInfo> &lists )
{
	setModal(true);
	setDefaultButton(KDialog::Cancel);
	setButtons(KDialog::Ok | KDialog::Cancel);

	setCaption( i18nc( "@title:window", "Element with Dependencies" ) );

	KVBox *page = new KVBox( this );
	setMainWidget( page );
	// Design the dialog

	instructionsLabel = new QLabel( page );
	instructionsLabel->setMinimumSize( QSize( 100, 30 ) );
	instructionsLabel->setMaximumSize( QSize( 10000, 10000 ) );
	instructionsLabel->setAlignment( Qt::AlignVCenter );
	instructionsLabel->setWordWrap(true);
	if ( m_depsAreDeleted ) {
		instructionsLabel->setText( i18nc( "@info", "<warning>The following will have to be removed also, since currently they use the element you have chosen to be removed.</warning>" ) );
	}
	else {
		instructionsLabel->setText( i18nc( "@info", "<warning>The following currently use the element you have chosen to be removed.</warning>" ) );
	}

	for ( QList<ListInfo>::const_iterator list_it = lists.begin(); list_it != lists.end(); ++list_it ) {
		if ( !((*list_it).list).isEmpty() ) {
			QGroupBox * groupBox = new QGroupBox( list_it->name, page );
			QListWidget *listBox = new QListWidget;
			QVBoxLayout *vbox = new QVBoxLayout;
			vbox->addWidget( listBox );
			groupBox->setLayout( vbox );
			loadList( listBox, (*list_it).list );
		}
	}

	setSizeGripEnabled( true );
	resize( QSize(500, 350) );
}

void DependanciesDialog::loadList( QListWidget* listBox, const ElementList &list )
{
	KConfigGroup config( KGlobal::config(), "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );

	for ( ElementList::const_iterator el_it = list.begin(); el_it != list.end(); ++el_it ) {
		QString name = ( *el_it ).name;
		if ( show_id )
			name += " (" + QString::number(( *el_it ).id) + ')';
		QListWidgetItem * item = new QListWidgetItem( name );
		listBox->addItem( item );
	}
}

void DependanciesDialog::accept()
{
	if ( !m_msg.isEmpty() ) {
		switch ( KMessageBox::warningYesNo(this,
			QString("<b>%1</b><br><br>%2").arg(m_msg).arg(i18nc("@info", "Are you sure you wish to proceed?")),
			QString(),KStandardGuiItem::yes(),KStandardGuiItem::no(),"doubleCheckDelete") )
		{
		case KMessageBox::Yes: KDialog::accept(); break;
		case KMessageBox::No: KDialog::reject(); break;
		}
	}
	else
		KDialog::accept();
}
