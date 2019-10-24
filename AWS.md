# Using AWS to get the data

As described in the paper,
our results were produced using Amazon EC2 instances.
We used dedicated instances.
`c5.2xlarge` was used for PIKOS<k> with k from 1 to 4 and IKOS,
`c5.4xlarge` for PIKOS<k> with k from 5 to 8,
and `c5.9xlarge` for PIKOS<k> with k larger than equal to 9,

## AMI for this artifact

One can run this artifact by using the Amazon Machine Images (AMIs).
The image has all the depenencies installed.
`pikos-popl2020`.

## How we used AWS

We used 200 instances running together to get the numbers in a reasonable time.
We created an image different from the above for this purpose.
Instances were spawned from the image, and user-data were passed to make them
run the assigned batch.
This requires setting up S3 to collect the results and setting up an IAM role
for the permissions.

The scripts that we used are uploaded on `aws_scripts/`.
One needs to configure the AWS in order to use the scripts.
Specifically, buckets `ikos` and `results-ikos` should be in the S3.
The Iam role should have access rights to these buckets.
