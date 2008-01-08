#!/bin/sh
mysqldump -p --no-create-db --no-data --databases $1> $2