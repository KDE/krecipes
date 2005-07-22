#!/bin/sh
#
# This script will convert an SQLite 2.x database file to SQLite 3.
#
# Calling it without any parameters, it will convert the file specified in the
# Krecipes configuration
#
# If Krecipes can't find the database file, you need to manually give
# it on the command-line, e.g.:
#
#   $./convert_sqlite.sh /path/to/database.file
#

file=$1
if test -z $1; then
  file=`kreadconfig --file krecipesrc --group Server --key DBFile`
  if test "$?" -ne "0"; then
    echo "Unable to find database file using 'kreadconfig'"
    echo "You need to manually specify where the SQLite 2 database file is, e.g.:"
    echo "  $./convert_sqlite3.sh /path/to/database.file"
    exit 1
  fi
fi

if test -e $file; then

sqlite $file .dump > /dev/null

if test "$?" -ne "0"; then
  echo "Conversion failed"
  exit 1
fi

sqlite $file .dump | sqlite3 $file.new

if test "$?" -ne "0"; then
  echo "Conversion failed"
  exit 1
fi

mv $file $file.sqlite2
if test "$?" -ne "0"; then
  echo "Unable to backup old SQLite 2 file... aborting"
  exit 1
fi

mv $file.new $file
if test "$?" -ne "0"; then
  echo "Created SQLite 3 database, '$file.new', but unable to copy it to $file"
else
  echo "Conversion successful!"
fi

else
  echo "Database file '$file' doesn't exist"
  echo "You need to manually specify where the SQLite 2 database file is, e.g.:"
  echo "  $./convert_sqlite3.sh /path/to/database.file"
fi
