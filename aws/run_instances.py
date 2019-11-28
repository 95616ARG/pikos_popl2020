#!/usr/bin/env python3
import subprocess
import boto3
import time
import os
import glob
import pandas as pd
import datetime
import sys

# Runs the instances.

ec2 = boto3.client('ec2', region_name='us-west-2')

def run_batch(instType, amiId, sg, key, iam, ud):
    '''
    Run the EC2 instance with given
    AmiImageId, LaunchTemplateName and UserDataName
    '''
    return ec2.run_instances(
            ImageId=amiId,
            InstanceType=instType,
            InstanceInitiatedShutdownBehavior='terminate',
            Placement={'Tenancy': 'dedicated'},
            KeyName=key,
            MaxCount=1,
            MinCount=1,
            EbsOptimized=True,
            UserData=ud,
            SecurityGroupIds=[sg],
            IamInstanceProfile={'Name': iam})

if __name__ == '__main__':
    config = sys.argv[1]
    instType = 'c5.2xlarge'
    nbatches=200 # Number of batches.
    if config == 'b24':
        pass
    elif config == '68':
        instType = 'c5.4xlarge'
    elif config == '1216':
        instType = 'c5.9xlarge'
        nbatches = 5
    else:
        print('Wrong configuration.')
        sys.exit(-1)
    amiImageId = sys.argv[2]
    securityGroup = sys.argv[3]
    keyName = sys.argv[4]
    iamRole = sys.argv[5]
    bucket1 = sys.argv[6]
    bucket2 = sys.argv[7]

    with open('user-data-template.txt', 'r') as udt:
        udt = udt.read()
        udt = udt.replace('{config}', config)
        udt = udt.replace('{bucket1}', bucket1)
        udt = udt.replace('{bucket2}', bucket2)
        for i in range(nbatches):
            ud = udt.replace('{batch}', str(i))
            # Lauch an instance
            instance = run_batch(instType, amiImageId, securityGroup, keyName, iamRole, ud)
            print(instance['Instances'][0]['InstanceId'])
            time.sleep(5)
