# Scripts to run experiments on AWS
- `pre/` contains necessary files to run benchmarks on AWS.
  - `button.py` is the script that runs things. It takes ami Id, security group,
  key name, and iam role name as argument.
  - `sync.py` is the script that updates the s3://ikos.
  - `template.txt` is a template for user-data that will be run on boot.
  - `xml/` contains files needed by `benchexec`.
    - `list.txt` is a list of files to run.
    - `partition.py` partitions the `list.txt`.
    - `exp?.xml` is the input to `benchexec` and describes the runs.
  - Requires binary of IKOS in S3, benchmarks in S3, an AMI image with dependencies installed, and a launch template.
- `post/` contains scripts to merge the results.
  - `download_from_s3.sh` downloads the results in S3 to current directory.
  - `generate_csv.py` merges the downloaded results into `unprocessed.csv`.
