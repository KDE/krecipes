#!/bin/sh
mysqldump --no-create-db --no-create-info --databases $1> $2