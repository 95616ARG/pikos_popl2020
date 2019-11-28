#!/bin/bash

# Downloads BUCKET2 to 'results-ikos/'.

BUCKET2=s3://results-ikos
if [[ $# == 1 ]]; then
  BUCKET2=s3://$1
fi

aws s3 cp --recursive $BUCKET results-ikos
