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

One needs to create an image with all the dependencies installed.
It should have `pikos` installed also.
One can do these by following the steps in [Install Dependencies](../README.md#Install-Dependencies).
The scripts assume that `pikos` is in PATH.
[`../tools/`](../tools) in this repository should be in `/home/ubuntu`.
Also, a dummy benchmark file should be in `benchmarks/` directory.
It should be either `dummy.bc` or `dummy.c`, and it doesn't have to take longer than 1 seconds.
The dummy benchmark is just to warm up the EC2 instances.

## S3 Buckets

One also needs to set up two S3 buckets. One bucket is used to store XML files and benchmark lists
for benchexec. It is named `S3://ikos` in the scripts, and its name has to be updated.
Another bucket is used to store the results of the benchexec. It is named `S3://results-ikos`
in the scripts, and its name also has to be updated.

## IAM role

Then, one needs to create IAM role that can access to the S3 buckets above.
EC2 instances will use this role to read `S3://ikos` and write to `S3://results-ikos`.

## Key and Security group

Finally, one needs to have key and a security group that the EC2 instance will use.

# Scripts to run experiments on AWS

The scripts that we used are uploaded in [`./pre/`](./pre) and [`./post/`](./post).

- `pre/` contains necessary files to run benchmarks on AWS.
  - `button.py` is the script that runs things. It takes instance type, ami Id, security group,
  key name, and iam role name as argument.
  - `sync.py` is the script that updates `the s3://ikos`.
  - `template.txt` is a template for user-data that will be run on boot.
  - `xml/` contains files needed by `benchexec`.
    - `list.txt` is a list of files to run.
    - `partition.py` partitions the `list.txt` into `set?.txt`.
    - `exp?.xml` is the input to `benchexec` and describes the runs.
- `post/` contains scripts to merge the results.
  - `download_from_s3.sh` downloads the results in S3 to current directory.
  - `generate_csv.py` merges the downloaded results into `unprocessed.csv`.

# Steps to run the instances (working directory: `pre/`)
1. List the benchmarks you want to run in `xml/list.txt`. Benchmarks that require
context sensitivity adjustments, however, must be inlined into a separate xml file
like in [`../xml/all.xml`](../xml/all.xml).
2. Define PIKOS configuration in `xml/exp?.xml`. ? is just a placeholder for some number.
An example file `xml/exp1.xml` is set to run baseline IKOS and PIKOS with `-nt=4`.
3. Run `xml/partition.py` to partition the `xml/list.txt` in to batches. One may want to
adjust the number of benchmarks in a batch by changing `n` in the script. Currently it is set to 21, so there will be 200 batches with 21 benchmarks in each (excluding the dummy).
4. Run `sync.py` to upload the `xml/` to `s3://ikos`.
5. Run `button.py` to run the instances. One can choose which `xml/exp?.xml` to
execute by changing `XML` in the script, and can set the range of batches to run
with `MIN` and `MAX`. Currently it is set to range over batch 0 to batch 199.
```
$ ./button.py <instance type> <ami ID> <security group ID> <key name> <iamrole name>
```

An ami ID has the form `ami-...`, and it should be the one created in [AMI](#AMI).
A security group ID has the form `sg-...`.

# Steps to merge the results (working directory: `post/`)
After all instances are finished, follow the steps below to merge the results.
1. Run `download_from_s3.sh` to download the results from `s3://results-ikos` to
`./results-ikos`.
2. Run `generate_csv.py` to generate the merged table, `unprocessed.csv`.
3. Manually process the `unprocessed.csv` to obtain the csv file as the one in
[`../results-paper/all.csv`](../results-paper/all.csv).
