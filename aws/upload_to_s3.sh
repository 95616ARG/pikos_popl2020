#!/bin/bash

# Uploads the 'batches/' to BUCKET1.

BUCKET1=s3://ikos
if [[ $# == 1 ]]; then
  BUCKET1=s3://$1
fi

mkdir tmp
cp -r batches tmp
aws s3 cp --recursive tmp $BUCKET1
rm -r tmp
