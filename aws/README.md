# Using AWS to get the data

As described in the paper,
our results were produced using Amazon EC2 instances.
We used dedicated instances.
`c5.2xlarge` was used for PIKOS<k> with k from 1 to 4 and IKOS,
`c5.4xlarge` for PIKOS<k> with k from 5 to 8,
and `c5.9xlarge` for PIKOS<k> with k larger than equal to 9,

# How we used AWS

We used 200 instances running together to get the numbers in a reasonable time.
We created an image (AMI) for this purpose.
Instances were spawned from this image, and user-data were passed to make them
run the assigned batches.
This requires setting up S3 to collect the results and setting up an IAM role
for the permissions.

## AMI

Use the image `pikos-aws`, whose AMI-id is `ami-01f8031a36fcb210d`.
To create this image, follow the steps in [Install Dependencies](../README.md#Installation).
The scripts assume that `pikos` is in PATH.
[`../tools/`](../tools) in this repository should be in `/home/ubuntu`.
Also, a dummy benchmark file should be in `/home/ubuntu/`.
It should be either `dummy.bc` or `dummy.c`, and it doesn't have to take longer than 1 seconds.
The dummy benchmark is just to warm up the EC2 instances.
SVC and OSS benchmarks should be in `/home/ubuntu/`.
(The image `pikos-popl2020` mentioned in `../README.md` **cannot** be used
for this scripts!)


## S3 Buckets

One has to set up **two** S3 buckets. BUCKET1 is used to store XML files and benchmark lists
for benchexec, and BUCKET2 is used to store the results of the benchexec.
Their names have to be **updated** in the scripts.

## IAM role

Create an IAM role that gives access rights on the above S3 buckets to EC2 instances.
EC2 instances will use this role to read from the first bucket and write to the second bucket.

Go to IAM, policies. Create a policy using JSON. Edit the policy boilerplate below, substituting for POLICYNAMEHERE and BUCKETNAMEHERE. This policy will allow the role to read/write files in the S3 bucket.
Do it **two** times for BUCKET1 and BUCKET2 created above.
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

## Key and Security group

Finally, create a key pair and a security group from the EC2.

[Creating key pair](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-key-pairs.html#having-ec2-create-your-key-pair).

# Prerequisite 

The scripts uses awscli (>=1.16.142) and boto3 (>=1.9.132).
```
$ pip install --user awscli boto3
```
They also use benchexec mentioned in the dependencies.

# Scripts to run experiments on AWS

The scripts that we used are uploaded in [`./pre/`](./pre) and [`./post/`](./post).
```
.
├── pre
│   ├── button.py
│   ├── sync.py
│   ├── template.txt
│   └── xml
│       ├── exp1.xml
│       ├── list.txt
│       └── partition.py
├── post
│   ├── download_from_s3.sh
│   └── generate_csv.py
└── README.md
```

- `pre/` contains necessary files to run benchmarks on AWS.
- `button.py` is the script that runs things. It takes instance type, ami Id, security group,
  key name, and iam role name as argument.
  - `sync.py` is the script that updates `the s3://ikos`.
  - `template.txt` is a template for user-data that will be run on boot.
  - `xml/` contains files needed by `benchexec`.
  - `list.txt` is a list of files to run.
  - `partition.py` partitions the `list.txt` into `set{?}.txt`.
  - `exp{?}.xml` is the input to `benchexec` and describes the runs.
- `post/` contains scripts to merge the results.
  - `download_from_s3.sh` downloads the results in S3 to current directory.
  - `generate_csv.py` merges the downloaded results into `unprocessed.csv`.

# Running the experiments on AWS instances (working directory: `pre/`)
To run the benchmark instance, please **switch to the `aws/pre`** and follow the steps below:
1. Specify benchmark tasks in `xml/list.txt` like the default one:
```
bitcode-files/abduco-0.6/abduco.bc
...
sv-benchmarks/ssh-simplified/s3_srvr_8.cil.c
```
Note: Benchmarks that require context sensitivity adjustments, however, must be inlined into a separate xml file like in [../xml/all.xml](../xml/all.xml).

2. Specify PIKOS configuration for `{?}`-th experiment in `xml/exp{?}.xml`. `{?}` is just a placeholder for some number.
As shown below, the example file `xml/exp1.xml` is set to run baseline IKOS and PIKOS with `-nt=4`.
```
  ...

  <rundefinition name="interval-base">
  </rundefinition>
  <rundefinition name="interval-cc4">
    <option name="--cc"/>
    <option name="-nt">4</option>
  </rundefinition>
  
  ...
```

3. Run `xml/partition.py` to partition the `xml/list.txt` in to batches.
```
$ ./xml/partition.py
```
At the beginning of `./xml/partition.py`, one may want to adjust the number of benchmarks in a batch by changing `n` in the script.
Currently it is set to 21, so there will be 200 batches with 21 benchmarks in each (excluding the dummy).
```
dummy = './dummy.bc\n'
n = 21 # Number of benchmark in one batch.
inputlist = 'list.txt'
```

4. Run `sync.py` to upload the `xml/` to the BUCKET1.
```
$ ./sync.py
```

5. Run `button.py` to run the instances.
One can choose which `xml/exp{?}.xml` to
execute by changing `XML` in the script, and can set the range of batches to run
with `MIN` and `MAX`.
```
# Run the experiment `xml/exp{XML}.xml` on batch `set{i}.txt` for i in [MIN, MAX).
MIN=0   # Minimal batch number (xml/set{MIN}.txt).                               
MAX=200 # Maximal batch number (xml/set{MAX}.txt). Not included.                 
XML=1   # The experiment number (xml/exp{XML}.xml) to run.  
```

```
$ ./button.py <instance type> <ami ID> <security group ID> <key name> <iamrole name>
```

An ami ID has the form `ami-...`, and it should be the one created in [AMI](#AMI).
A security group ID has the form `sg-...`.
For example,
```
$ ./button.py c5.2xlarge ami-01f8031a36fcb210d sg-053bf5e3678ccd07b mykey myrole
```


# Downloading and merging the results (working directory: `post/`)
After all instances are finished, please **switch to the `aws/post` directory** and follow the steps below to merge the results.
1. Run `download_from_s3.sh` to download the results from `s3://results-ikos` to
`./results-ikos`.
```
$ ./download_from_s3.sh
```

2. Run `generate_csv.py` to generate the merged table (`unprocessed.csv`).
```
$ ./generate_csv.py
```

3. Manually process the `unprocessed.csv` to obtain the csv file as the one in
[`../results-paper/all.csv`](../results-paper/all.csv).
In particular, you might want to rename bitcode-files to OSS and sv-benchmarks to SVC.
