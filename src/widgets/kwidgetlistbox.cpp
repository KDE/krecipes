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
 *  You should have received a copy of the GNU General Public License
 *  along with SuperKaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#include "kwidgetlistbox.h"
#include <kdebug.h>
#include <kglobalsettings.h>

KWidgetListbox::KWidgetListbox(QWidget *parent, const char *name)
 : QTable(parent, name)
{
  setNumRows(0);
  setNumCols(1);
  setColumnStretchable(0, true);
  setLeftMargin(0);
  setTopMargin(0);
  horizontalHeader()->hide();
  verticalHeader()->hide();
  setSelectionMode(QTable::NoSelection);
  setFocusStyle(QTable::FollowStyle);
  connect(this, SIGNAL(currentChanged(int, int)),
          this, SLOT(selectionChanged(int, int)));
  setHScrollBarMode(QScrollView::AlwaysOff);
  setVScrollBarMode(QScrollView::Auto);
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
  int row;

  if(index == -1)
  {
    row = numRows();
    setNumRows(row + 1);
  }
  else
    return -1;

  setRowHeight(row, item->height());
  setCellWidget(row, 0, item);
  setItemColors(row, even(row));
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
  return item(selected());
}

QWidget* KWidgetListbox::item(int index) const
{
  return cellWidget(index, 0);
}

int KWidgetListbox::index(QWidget* itm) const
{
  for(int i = 0; i < numRows(); ++i)
    if(item(i) == itm)
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
    if(!isRowHidden(i))
      ++v;
  }
  return (v%2 == 0);
}

void KWidgetListbox::updateColors()
{
  int v = 0;
  for(int i = 0; i < numRows(); ++i)
  {
    if(!isRowHidden(i))
    {
      setItemColors(i, (v%2 == 0));
      ++v;
    }
  }
}

void KWidgetListbox::setItemColors(int index, bool even)
{
  QWidget* itm = item(index);
/*
  if(index == selected())
  {
    itm->setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    itm->setPaletteForegroundColor(KGlobalSettings::highlightedTextColor());
  }*/
  if(even)
  {
    itm->setPaletteBackgroundColor(KGlobalSettings::baseColor());
    itm->setPaletteForegroundColor(KGlobalSettings::textColor());
  }
  else
  {
    itm->setPaletteBackgroundColor(
        KGlobalSettings::alternateBackgroundColor());
    itm->setPaletteForegroundColor(KGlobalSettings::textColor());
  }
}

void KWidgetListbox::showItems(show_callback func, void* data)
{
  for(int i = 0; i < numRows(); ++i)
  {
    if(func == 0)
      showRow(i);
    else
    {
      if(func(i, item(i), data))
        showRow(i);
      else
        hideRow(i);
    }
  }
  updateColors();
}

void KWidgetListbox::showEvent(QShowEvent*)
{
  //kdDebug() << k_funcinfo << endl;
  repaintContents(false);
}

void KWidgetListbox::paintCell(QPainter*, int, int, const QRect&,
                               bool, const QColorGroup&)
{
  //kdDebug() << k_funcinfo << endl;
}

#include "kwidgetlistbox.moc"
