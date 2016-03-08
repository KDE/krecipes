/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef DEPRECATED_H
#define DEPRECATED_H

#ifdef KRECIPES_DEPRECATED_WARNINGS
#define KRECIPES_DEPRECATED __attribute__((__deprecated__))
#else
#define KRECIPES_DEPRECATED
#endif

#endif //DEPRECATED_H
