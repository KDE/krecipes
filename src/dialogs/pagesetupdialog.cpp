/***************************************************************************
*   Copyright (C) 2003 by Jason Kivlighn                                  *
*   mizunoami44@users.sourceforge.net                                     *
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

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : KDialog( parent, 0, true )
{
	KIconLoader il;

	QVBoxLayout * layout = new QVBoxLayout( this );

	KToolBar *toolbar = new KToolBar( this );
	active_display = setup_display = new SetupDisplay( sample, this );
	print_setup_display = new PrintSetupDisplay( sample, this );

	KActionCollection *actionCollection = new KActionCollection( this );

	KAction *std_open = KStdAction::open( 0, 0, 0 ); //use this to create a custom open action
	KToolBarPopupAction *custom_open = new KToolBarPopupAction( std_open->text(), std_open->icon(), std_open->shortcut(), this, SLOT( loadLayout() ), actionCollection, "open_popup" );
	KPopupMenu *open_popup = custom_open->popupMenu();
	open_popup->insertTitle( i18n( "Included Layouts" ) );
	QDir included_layouts( getIncludedLayoutDir(), "*.klo", QDir::Name | QDir::IgnoreCase, QDir::Files );

	for ( unsigned int i = 0; i < included_layouts.count(); i++ ) {
		int id = open_popup->insertItem( included_layouts[ i ], this, SLOT( loadLayout( int ) ) );
		included_layouts_map.insert( id, included_layouts[ i ] );
	}

	custom_open->plug( toolbar );

	KStdAction::save( this, SLOT( saveLayout() ), actionCollection ) ->plug( toolbar );
	KStdAction::saveAs( this, SLOT( saveAsLayout() ), actionCollection ) ->plug( toolbar );
	KStdAction::redisplay( this, SLOT( reloadLayout() ), actionCollection ) ->plug( toolbar );

	KToolBarPopupAction *shown_items = new KToolBarPopupAction( i18n( "Items Shown" ), "frame_edit" );
	shown_items->setDelayed( false );
	shown_items_popup = shown_items->popupMenu();
	shown_items_popup->insertTitle( i18n( "Show Items" ) );
	shown_items->plug( toolbar );

	layout->addWidget( toolbar );

	KConfig *config = kapp->config();
	config->setGroup( "Page Setup" );

	//let's do everything we can to be sure at least some layout is loaded
	QString filename = config->readEntry( "Layout", locate( "appdata", "layouts/default.klo" ) );
	if ( filename.isEmpty() || !QFile::exists( filename ) )
		filename = locate( "appdata", "layouts/default.klo" );
	loadLayout( filename, setup_display );

	filename = config->readEntry( "PrintLayout", locate( "appdata", "layouts/default_print.klo" ) );
	if ( filename.isEmpty() || !QFile::exists( filename ) )
		filename = locate( "appdata", "layouts/default_print.klo" );
	loadLayout( filename, print_setup_display->display() );

	initShownItems();

	QTabWidget *tabWidget = new QTabWidget(this);
	tabWidget->insertTab( setup_display, i18n("Recipe View") );
	tabWidget->insertTab( print_setup_display, il.loadIconSet( "fileprint", KIcon::Small ), i18n("Print View") );
	layout->addWidget( tabWidget );

	QHBox *buttonsBox = new QHBox( this );
	QPushButton *okButton = new QPushButton( il.loadIconSet( "ok", KIcon::Small ), i18n( "Save and Close" ), buttonsBox );
	QPushButton *cancelButton = new QPushButton( il.loadIconSet( "cancel", KIcon::Small ), i18n( "&Cancel" ), buttonsBox );
	layout->addWidget( buttonsBox );

	connect( okButton, SIGNAL( clicked() ), SLOT( accept() ) );
	connect( cancelButton, SIGNAL( clicked() ), SLOT( reject() ) );
	connect( setup_display, SIGNAL( itemVisibilityChanged( QWidget*, bool ) ), SLOT( updateItemVisibility( QWidget*, bool ) ) );
	connect( print_setup_display->display(), SIGNAL( itemVisibilityChanged( QWidget*, bool ) ), SLOT( updateItemVisibility( QWidget*, bool ) ) );
	connect( tabWidget, SIGNAL( currentChanged( QWidget* ) ), SLOT( setActiveDisplay( QWidget* ) ) );
}

void PageSetupDialog::accept()
{
	if ( setup_display->hasChanges() )
		saveLayout(setup_display);
	if ( print_setup_display->display()->hasChanges() )
		saveLayout(print_setup_display->display());

	if ( !active_filename_map[setup_display].isEmpty() ) {
		KConfig * config = kapp->config();
		config->setGroup( "Page Setup" );
		config->writeEntry( "Layout", active_filename_map[setup_display] );
	}

	if ( !active_filename_map[print_setup_display->display()].isEmpty() ) {
		KConfig * config = kapp->config();
		config->setGroup( "Page Setup" );
		config->writeEntry( "PrintLayout", active_filename_map[print_setup_display->display()] );
	}

	QDialog::accept();
}

void PageSetupDialog::reject()
{
	if ( setup_display->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout(setup_display);
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	if ( print_setup_display->display()->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout(print_setup_display->display());
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	QDialog::reject();
}

QSize PageSetupDialog::minimumSize() const
{
	return ( QSize( 300, 400 ) );
}

QSize PageSetupDialog::sizeHint( void ) const
{
	return ( minimumSize() );
}

void PageSetupDialog::updateItemVisibility( QWidget *item, bool visible )
{
	shown_items_popup->setItemChecked( widget_popup_map[ item ], visible );
}

//TODO: Sort these by alphabetical order
void PageSetupDialog::initShownItems()
{
	for ( PropertiesMap::const_iterator it = active_display->properties().begin(); it != active_display->properties().end(); ++it ) {
		if ( it.data() & SetupDisplay::Visibility ) {
			int new_id = shown_items_popup->insertItem( QToolTip::textFor( it.key() ->widget ) );
			shown_items_popup->setItemChecked( new_id, it.key() ->widget->isShown() );
			shown_items_popup->connectItem( new_id, this, SLOT( setItemShown( int ) ) );

			popup_widget_map.insert( new_id, it.key() ->widget );
			widget_popup_map.insert( it.key() ->widget, new_id );
		}
	}
}

void PageSetupDialog::setItemShown( int id )
{
	shown_items_popup->setItemChecked( id, !shown_items_popup->isItemChecked( id ) );
	active_display->setItemShown( popup_widget_map[ id ], shown_items_popup->isItemChecked( id ) );
}

void PageSetupDialog::loadLayout()
{
	loadLayout( KFileDialog::getOpenFileName( locateLocal( "appdata", "layouts/" ), "*.klo|Krecipes Layout (*.klo)", this, i18n( "Select Layout" ) ) );
}

void PageSetupDialog::loadLayout( int popup_param )
{
	loadLayout( getIncludedLayoutDir() + "/" + included_layouts_map[ popup_param ] );
}

void PageSetupDialog::loadLayout( const QString &filename, SetupDisplay *display )
{
	if ( display == 0 ) display = active_display;

	if ( display->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout(display);
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	if ( !filename.isEmpty() ) {
		display->loadLayout( filename );
		setActiveFile( filename, display );
	}
}

void PageSetupDialog::reloadLayout()
{
	loadLayout( active_filename_map[active_display] );
}

void PageSetupDialog::saveLayout( SetupDisplay *display )
{
	if ( display == 0 ) display = active_display;

	if ( display->hasChanges() ) {
		if ( have_write_perm_map[display] )
			display->saveLayout( active_filename_map[display] );
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "Unable to save the layout because you do not have sufficient permissions to modify this file.\nWould you like to instead save the current layout to a new file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout(display);
				break;
			default:
				break;
			}
		}
	}
}

void PageSetupDialog::saveAsLayout( SetupDisplay *display )
{
	if ( display == 0 ) display = active_display;

	QString filename = KFileDialog::getSaveFileName( KGlobal::instance() ->dirs() ->saveLocation( "appdata", "layouts/" ), "*.klo|Krecipes Layout (*.klo)", this, QString::null );

	if ( !filename.isEmpty() ) {
		if ( haveWritePerm( filename ) ) {
			display->saveLayout( filename );
			active_filename_map.insert(display,filename);
		}
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "You have selected a file that you do not have the permissions to write to.\nWould you like to select another file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout(display);
				break;
			default:
				break;
			}
		}
	}
}

QString PageSetupDialog::getIncludedLayoutDir() const
{
	QFileInfo file_info( locate( "appdata", "layouts/default.klo" ) );
	return file_info.dirPath( true );
}

void PageSetupDialog::setActiveFile( const QString &filename, SetupDisplay *display )
{
	if ( display == 0 ) display = active_display;

	active_filename_map.insert(display,filename);
	have_write_perm_map.insert(display,haveWritePerm( filename ));
}

void PageSetupDialog::setActiveDisplay( QWidget *widget )
{
	if ( widget == setup_display )
		active_display = setup_display;
	else
		active_display = print_setup_display->display();

	for ( QMap<QWidget*,int>::const_iterator it = widget_popup_map.begin(); it != widget_popup_map.end(); ++it ) {
		shown_items_popup->setItemChecked( it.data(), it.key()->isShown() );
	}
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


#include "pagesetupdialog.moc"
