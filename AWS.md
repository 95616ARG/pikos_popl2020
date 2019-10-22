# Using AWS to get the data

As described in the paper,
our results are produced using Amazon EC2 instances.
We used dedicated instances.
`c5.2xlarge` was used for PIKOS<k> with k from 1 to 4 and IKOS,
`c5.4xlarge` for PIKOS<k> with k from 5 to 8,
and `c5.9xlarge` for PIKOS<k> with k larger than equal to 9,

## AMI

One can run this artifact by using the Amazon Machine Images (AMIs).
The image has all the depenencies installed.
`pikos-popl2020`.
