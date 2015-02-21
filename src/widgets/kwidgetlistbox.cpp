/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
 *
 * Note: This file is now part of Krecipes, which is a slightly modified version of the
 *       original used in SuperKaramba
 *
 * This file is part of SuperKaramba.
 *
 *  SuperKaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SuperKaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>
 ****************************************************************************/
#include "kwidgetlistbox.h"
#include <kdebug.h>
#include <kglobalsettings.h>
//Added by qt3to4:
#include <QShowEvent>

KWidgetListbox::KWidgetListbox(QWidget *parent, const char *name)
 : Q3Table(parent, name)
{
  setNumRows(0);
  setNumCols(1);
  setColumnStretchable(0, true);
  setLeftMargin(0);
  setTopMargin(0);
  horizontalHeader()->hide();
  verticalHeader()->hide();
  setSelectionMode(Q3Table::NoSelection);
  setFocusStyle(Q3Table::FollowStyle);
  connect(this, SIGNAL(currentChanged(int, int)),
          this, SLOT(selectionChanged(int, int)));
  setHScrollBarMode(Q3ScrollView::AlwaysOff);
  setVScrollBarMode(Q3ScrollView::Auto);
}

KWidgetListbox::~KWidgetListbox()
{
  clear();
}

void KWidgetListbox::clear()
{
  for(int i = 0; i < numRows(); ++i)
    clearCellWidget(i, 0);
  setNumRows(0);
}

int KWidgetListbox::insertItem(QWidget* item, int index)
{
  int row = index;

  if(index == -1)
  {
    row = numRows();
  }
  //else
  //  return -1;

  insertRows(row);
  setRowHeight(row, item->height());
  setCellWidget(row, 0, item);

  for ( int i = row; i < numRows(); ++i ) {
    setItemColors(i, even(i));
  }

  ensureCellVisible(row,0);

  return row;
}

void KWidgetListbox::setSelected(QWidget* item)
{
  setSelected(index(item));
}

void KWidgetListbox::selectionChanged(int row, int col)
{
  ensureCellVisible(row, col);
  updateColors();
  emit selected(row);
}

void KWidgetListbox::removeItem(QWidget* item)
{
  removeItem(index(item));
}

void KWidgetListbox::removeItem(int index)
{
  removeRow(index);
  updateColors();
}

void KWidgetListbox::setSelected(int index)
{
  setCurrentCell(index, 0);
}

int KWidgetListbox::selected() const
{
  return currentRow();
}

QWidget* KWidgetListbox::selectedItem() const
{
  return cellWidget(selected(),0);
}

int KWidgetListbox::index(QWidget* itm) const
{
  for(int i = 0; i < numRows(); ++i)
    if(cellWidget(i,0) == itm)
      return i;
  return -1;
}

bool KWidgetListbox::even(int index)
{
  int v = 0;
  for(int i = 0; i < numRows(); ++i)
  {
    if(index == i)
      break;
    //if(!isRowHidden(i))
    ++v;
  }
  return (v%2 == 0);
}

void KWidgetListbox::updateColors()
{
  int v = 0;
  for(int i = 0; i < numRows(); ++i)
  {
    //if(!isRowHidden(i))
    {
      setItemColors(i, (v%2 == 0));
      ++v;
    }
  }
}

void KWidgetListbox::setItemColors(int index, bool even)
{
  QWidget* itm = cellWidget(index,0);
  // KDE4 add
  QPalette p = palette();
if ( !itm){ kDebug()<<"no widget at index "<<index; return; }
/*
  if(index == selected())
  {
    itm->setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    itm->setPaletteForegroundColor(KGlobalSettings::highlightedTextColor());
  }*/
  if(even)
   p.setColor(backgroundRole(), p.base().color() );
  else
   p.setColor(backgroundRole(), p.alternateBase().color() );
  
  p.setColor(foregroundRole(), p.text().color() );
  itm->setPalette(p); 
}

void KWidgetListbox::showItems(show_callback func, void* data)
{
  for(int i = 0; i < numRows(); ++i)
  {
    if(func == 0)
      showRow(i);
    else
    {
      if(func(i, cellWidget(i,0), data))
        showRow(i);
      else
        hideRow(i);
    }
  }
  updateColors();
}

void KWidgetListbox::showEvent(QShowEvent*)
{
  //kDebug();
  repaintContents(false);
}

void KWidgetListbox::paintCell(QPainter*, int, int, const QRect&,
                               bool, const QColorGroup&)
{
  //kDebug();
}

#include "kwidgetlistbox.moc"
