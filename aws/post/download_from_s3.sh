#!/bin/bash

BUCKETNAME2=s3://results-ikos
aws s3 cp --recursive $BUCKETNAME2 results-ikos
