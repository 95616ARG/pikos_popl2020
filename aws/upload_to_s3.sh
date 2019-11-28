#!/bin/bash

# Uploads the 'experiments/' to BUCKET1.

BUCKET1=s3://results-ikos
if [[ $# == 1 ]]; then
  BUCKET1=s3://$1
fi

mkdir tmp
cp -r experiments tmp
aws s3 cp --recursive tmp $BUCKET1
rm -r tmp
