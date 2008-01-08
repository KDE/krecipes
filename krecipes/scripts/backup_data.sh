#!/bin/sh
mysqldump -p --no-create-db --no-create-info --databases $1> $2