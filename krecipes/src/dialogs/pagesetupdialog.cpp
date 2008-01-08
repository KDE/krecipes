/***************************************************************************
*   Copyright (C) 2003-2005 by Jason Kivlighn                             *
*   (jkivlighn@gmail.com)                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pagesetupdialog.h"

#include <qdir.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qtooltip.h>
#include <qtabwidget.h>
#include <qlabel.h>

#include <khtmlview.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>

#include <widgets/thumbbar.h>
#include "setupdisplay.h"

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample, const QString &configEntry ) : KDialog( parent, 0, true ), m_configEntry(configEntry)
{
	KIconLoader *il = KGlobal::iconLoader();

	QVBoxLayout * layout = new QVBoxLayout( this );

	KToolBar *toolbar = new KToolBar( this );
	KActionCollection *actionCollection = new KActionCollection( this );

	KStdAction::open( this, SLOT(loadFile()), actionCollection )->plug( toolbar );
	KStdAction::save( this, SLOT( saveLayout() ), actionCollection ) ->plug( toolbar );
	KStdAction::saveAs( this, SLOT( saveAsLayout() ), actionCollection ) ->plug( toolbar );
	KStdAction::revert( this, SLOT( selectNoLayout() ), actionCollection ) ->plug( toolbar );

	KToolBarPopupAction *shown_items = new KToolBarPopupAction( i18n( "Items Shown" ), "frame_edit" );
	shown_items->setDelayed( false );
	shown_items_popup = shown_items->popupMenu();
	shown_items_popup->insertTitle( i18n( "Show Items" ) );
	shown_items->plug( toolbar );
	layout->addWidget( toolbar );

	QLabel *help = new QLabel(i18n("<i>Usage: Select a template along the left, and right-click any element to edit the look of that element.</i>"),this);
	layout->addWidget( help );

	QHBox *viewBox = new QHBox( this );
	ThumbBarView *thumbBar = new ThumbBarView(viewBox,Vertical);
	connect(thumbBar,SIGNAL(signalURLSelected(const QString&)), this, SLOT(loadTemplate(const QString&)));
	QDir included_templates( getIncludedLayoutDir(), "*.xsl", QDir::Name | QDir::IgnoreCase, QDir::Files );
	for ( uint i = 0; i < included_templates.count(); i++ ) {
		new ThumbBarItem(thumbBar,included_templates.path() + "/" +included_templates[ i ]);
	}
	m_htmlPart = new SetupDisplay(sample, this);
	m_htmlPart->view()->reparent(viewBox,QPoint());
	layout->addWidget( viewBox );

	QHBox *buttonsBox = new QHBox( this );
	QPushButton *okButton = new QPushButton( il->loadIconSet( "ok", KIcon::Small ), i18n( "Save and Close" ), buttonsBox );
	QPushButton *cancelButton = new QPushButton( il->loadIconSet( "cancel", KIcon::Small ), i18n( "&Cancel" ), buttonsBox );
	layout->addWidget( buttonsBox );

	connect( m_htmlPart, SIGNAL(itemVisibilityChanged(KreDisplayItem*,bool)), this, SLOT(updateItemVisibility(KreDisplayItem*,bool)) );
	connect( okButton, SIGNAL( clicked() ), SLOT( accept() ) );
	connect( cancelButton, SIGNAL( clicked() ), SLOT( reject() ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Page Setup" );
	QSize defaultSize(800,600);
	resize(config->readSizeEntry( "WindowSize", &defaultSize ));

	//let's do everything we can to be sure at least some layout is loaded
	QString layoutFile = config->readEntry( m_configEntry+"Layout", locate( "appdata", "layouts/None.klo" ) );
	if ( layoutFile.isEmpty() || !QFile::exists( layoutFile ) )
		layoutFile = locate( "appdata", "layouts/None.klo" );

	QString tmpl = config->readEntry( m_configEntry+"Template", locate( "appdata", "layouts/Default.xsl" ) );
	if ( tmpl.isEmpty() || !QFile::exists( tmpl ) )
		tmpl = locate( "appdata", "layouts/Default.xsl" );
	kdDebug()<<"tmpl: "<<tmpl<<endl;
	active_template = tmpl;
	loadLayout( layoutFile );

	thumbBar->setSelected(thumbBar->findItemByURL(active_template));

	initShownItems();
}

void PageSetupDialog::accept()
{
	if ( m_htmlPart->hasChanges() )
		saveLayout();

	KConfig * config = kapp->config();
	config->setGroup( "Page Setup" );
	config->writeEntry( m_configEntry+"Layout", active_filename );

	if ( !active_template.isEmpty() ) {
		config->writeEntry( m_configEntry+"Template", active_template );
	}

	config->writeEntry( "WindowSize", size() );

	QDialog::accept();
}

void PageSetupDialog::reject()
{
	if ( m_htmlPart->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "The recipe view layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout();
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	QDialog::reject();
}

void PageSetupDialog::updateItemVisibility( KreDisplayItem *item, bool visible )
{
	shown_items_popup->setItemChecked( widget_popup_map[ item ], visible );
}

void PageSetupDialog::initShownItems()
{
	shown_items_popup->clear();

	PropertiesMap properties = m_htmlPart->properties();
	
	QValueList<QString> nameList;
	QMap<QString,KreDisplayItem*> nameMap;

	for ( PropertiesMap::const_iterator it = properties.begin(); it != properties.end(); ++it ) {
		nameList << it.key()->name;
		nameMap.insert( it.key()->name, it.key() );
	}
	qHeapSort( nameList );

	for ( QValueList<QString>::const_iterator it = nameList.begin(); it != nameList.end(); ++it ) {
		KreDisplayItem *item = nameMap[*it];
		if ( properties[item] & SetupDisplay::Visibility ) {
			int new_id = shown_items_popup->insertItem ( *it );
			shown_items_popup->setItemChecked( new_id, item->show );
			shown_items_popup->connectItem( new_id, this, SLOT( setItemShown( int ) ) );

			popup_widget_map.insert( new_id, item );
			widget_popup_map.insert( item, new_id );
		}
	}
}

void PageSetupDialog::setItemShown( int id )
{
	shown_items_popup->setItemChecked( id, !shown_items_popup->isItemChecked( id ) );
	m_htmlPart->setItemShown( popup_widget_map[ id ], shown_items_popup->isItemChecked( id ) );
}

void PageSetupDialog::loadFile()
{
	QString file = KFileDialog::getOpenFileName( locateLocal( "appdata", "layouts/" ), QString("*.klo *.xsl|%1").arg(i18n("Krecipes style or template file")), this, i18n( "Select Layout" ) );

	if ( file.endsWith(".klo") )
		loadLayout( file );
	else {
		active_template = file;
		m_htmlPart->loadTemplate( file );
	}
}

void PageSetupDialog::loadLayout( int popup_param )
{
	loadLayout( getIncludedLayoutDir() + "/" + included_layouts_map[ popup_param ] );
}

void PageSetupDialog::loadTemplate( const QString& filename )
{
	active_template = filename;
	m_htmlPart->loadTemplate( active_template );
}

void PageSetupDialog::loadLayout( const QString &filename )
{
	if ( m_htmlPart->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout();
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	m_htmlPart->loadLayout( filename );
	setActiveFile( filename );
}

void PageSetupDialog::reloadLayout()
{
	m_htmlPart->reload();
}

void PageSetupDialog::saveLayout()
{
	if ( m_htmlPart->hasChanges() ) {
		if ( have_write_perm )
			m_htmlPart->saveLayout( active_filename );
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "Unable to save the layout because you do not have sufficient permissions to modify this file.\nWould you like to instead save the current layout to a new file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout();
				break;
			default:
				break;
			}
		}
	}
}

void PageSetupDialog::saveAsLayout()
{
	QString filename = KFileDialog::getSaveFileName( KGlobal::instance() ->dirs() ->saveLocation( "appdata", "layouts/" ), "*.klo|Krecipes Layout (*.klo)", this, QString::null );

	if ( !filename.isEmpty() ) {
		if ( haveWritePerm( filename ) ) {
			m_htmlPart->saveLayout( filename );
			setActiveFile(filename);
		}
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "You have selected a file that you do not have the permissions to write to.\nWould you like to select another file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout();
				break;
			default:
				break;
			}
		}
	}
}

QString PageSetupDialog::getIncludedLayoutDir() const
{
	QFileInfo file_info( locate( "appdata", "layouts/None.klo" ) );
	return file_info.dirPath( true );
}

void PageSetupDialog::setActiveFile( const QString &filename )
{
	active_filename = filename;
	have_write_perm = haveWritePerm( filename );
}

bool PageSetupDialog::haveWritePerm( const QString &filename )
{
	QFileInfo info( filename );

	if ( info.exists() )  //check that we can write to this particular file
	{
		QFileInfo info( filename );
		return info.isWritable();
	}
	else //check that we can write to the directory since the file doesn't exist
	{
		QFileInfo dir_info( info.dirPath( true ) );
		return dir_info.isWritable();
	}
}

void PageSetupDialog::selectNoLayout()
{
	m_htmlPart->loadLayout( QString::null );
	setActiveFile( QString::null );
}

#include "pagesetupdialog.moc"
