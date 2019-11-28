# Using AWS to get the data

As described in the paper,
our results were produced using Amazon EC2 instances.
We used dedicated instances.
`c5.2xlarge` was used for PIKOS<k> with k from 1 to 4 and IKOS,
`c5.4xlarge` for PIKOS<k> with k from 5 to 8,
and `c5.9xlarge` for PIKOS<k> with k larger than equal to 9,

We used **200** instances running together to get the numbers in a reasonable time.
We created an image (AMI) for this purpose.
EC2 Instances were spawned from this image, and user-data were passed to run the assigned batches.
The instances access two S3 buckets to obtain the batch and save the results.
Thus, they should have access permissions to those buckets through IAM role.

# Prerequisite 

The scripts uses awscli (>=1.16.142) and boto3 (>=1.9.132).
```
$ pip install --user awscli boto3
```
BenchExec mentioned in the dependencies is also required to generate tables.

## AWS set up

### AMI

Use the image `pikos-aws` in `us-west-2` region, whose AMI-id is `ami-01f8031a36fcb210d`.
(The image `pikos-popl2020` mentioned in `../README.md` **cannot** be used
for this scripts!)

To create this image, follow the steps in [Install Dependencies](../README.md#Installation).
The scripts assume that `pikos` is in PATH.
[`../tools/`](../tools) in this repository should be in `/home/ubuntu`.
A dummy benchmark file (`dummy.bc`) in `/home/ubuntu/` is used to warm up the EC2 instances.
SVC and OSS benchmarks should be in `/home/ubuntu/`.

### S3 Buckets

**Create** **two** S3 buckets.
BUCKET1 stores the batch configurations, and BUCKET2 stores the experiment results.

[Creating S3 bucket](https://docs.aws.amazon.com/AmazonS3/latest/gsg/CreatingABucket.html)

### IAM role

**Create** an IAM role that gives access rights on the buckets to instances.
The instances uses this role to read from BUCKET1 and write to BUCKET2.

[Creating IAM role](https://docs.aws.amazon.com/IAM/latest/UserGuide/id_roles_create.html)

Go to IAM, policies. Create a policy using JSON. Edit the policy boilerplate below, substituting for POLICYNAMEHERE and BUCKETNAMEHERE. This policy will allow the role to read/write files in the S3 bucket.
Do this **two** times for BUCKET1 and BUCKET2 created above.
```
{
  "Version": "2012-10-17",
    "Statement": [
    {
      "Sid": "POLICYNAMEHERE",
      "Effect": "Allow",
      "Action": "s3:*",
      "Resource": "arn:aws:s3:::BUCKETNAMEHERE/*"
    },
    {
      "Sid": "VisualEditor1",
      "Effect": "Allow",
      "Action": "s3:ListAllMyBuckets",
      "Resource": "*"
    },
    {
      "Sid": "AllowRootListing",
      "Action": [
        "s3:ListBucket"
      ],
      "Effect": "Allow",
      "Resource": [
        "arn:aws:s3:::BUCKETNAMEHERE"
      ]
    }
  ]
}
```

The trust policy for your job role needs to extend to ec2 and ECS. Go to edit trust relationships and add,

```
{
  "Version": "2012-10-17",
  "Statement": [
  {
    "Effect": "Allow",
    "Principal": {
      "Service": "ec2.amazonaws.com"
    },
    "Action": "sts:AssumeRole"
  }
  ]
}
```

### Security group

**Create** a security group.

[Creating a security group](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-security-groups.html#creating-security-group)

### Key and Security group

**Create** a key pair.

[Creating key pair](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-key-pairs.html#having-ec2-create-your-key-pair).

# Scripts to run experiments on AWS

```
.
├── upload_to_s3.sh
├── run_instances.py
├── user-data-template.txt
├── download_from_s3.sh
├── merge_csv.py
├── partition.py
└── batches/
    ├── batch-b24-0.xml
    ├── batch-b24-1.xml
    └── ...
```

- `upload_to_s3.sh` uploads batch definitions in `batches/` to BUCKET1.
- `run_instances.py` runs the instances. It takes as arguments an instance type,
   an ami Id, a security group, a key name, and an iam role name.
- `user-data-template.txt` is a template of script that will be run in instances on boot.
- `download_from_s3.sh` downloads the results in BUCKET2 to current directory.
- `merge_csv.py` merges the downloaded results into `unprocessed.csv`.
- `partition.py` partitions the list of benchmarks into batches in `batches/`.
- `experiments/` contains the batch configurations.
  - `experiments/batch-{config}-{batch number}.xml` is fed into `benchexec` in an instance.

# Reproducing data on AWS instances
## Steps to reproduce data [`../results-paper/all.csv`](../results-paper/all.csv) and table 3.

0. Follow the steps in prerequisite.

1. Run `upload_to_s3.sh` to upload the `batches/` to BUCKET1.
```
$ ./upload_to_s3.sh your-BUCKET1-name
```

2. Run `run_instances.py` to run the instances.
The script assumes that you are in region **us-west-2 (Oregon)**.

First, run baseline IKOS, Pikos<2>, and Pikos<4> in c5.2xlarge.
You need to be able to run **200** c5.2xlarge instances in your account.
```
$ ./run_instances.py b24 ami-01f8031a36fcb210d your-security-group-id your-key-name your-iam-role-name your-BUCKET1-name your-BUCKET2-name
```

After they are finished, run Pikos<6> and Pikos<8> in c5.4xlarge.
You need to be able to run **200** c5.4xlarge instances in your account.
```
$ ./run_instances.py 68 ami-01f8031a36fcb210d your-security-group-id your-key-name your-iam-role-name your-BUCKET1-name your-BUCKET2-name
```

Finally, after they are finished, run Pikos<12> and Pikos<16> in c5.9xlarge.
You need to be able to run **5** c5.9xlarge instances in your account.
```
$ ./run_instances.py 1216 ami-01f8031a36fcb210d your-security-group-id your-key-name your-iam-role-name your-BUCKET1-name your-BUCKET2-name
```

4. Run `download_from_s3.sh` to download the results from BUCKET2 to `./results-ikos`.
```
$ ./download_from_s3.sh your-BUCKET2-name
```

5. Run `merge_csv.py` to generate the merged table (`b24.csv`, `68.csv`, `1216.csv`).
```
$ ./merge_csv.py
```

5. Manually process the `b24.csv`, `68.csv`, and `1216.csv` to obtain the csv file as the one in
[`../results-paper/all.csv`](../results-paper/all.csv).
