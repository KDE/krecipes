#!/bin/sh
mysqldump --no-create-db --no-data --databases $1> $2