#!/usr/bin/env python3
import subprocess
import boto3
import time
import os
import glob
import pandas as pd
import datetime
import sys

ec2 = boto3.client('ec2', region_name='us-west-2')

MIN=0
MAX=200
XML=1

def run_instance(instType, amiId, sg, key, iam, udName):
    '''
    Run the EC2 instance with given
    AmiImageId, LaunchTemplateName and UserDataName
    '''
    instanceType = instType
    with open(udName, 'r') as f:
        ud = f.read()
    return ec2.run_instances(
            ImageId=amiId,
            InstanceType=instanceType,
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
    instances = []
    for i in range(MIN, MAX):
        # Create user data for instance i
        subprocess.call(['cp', 'template.txt', 'ud%d.txt' % i])
        subprocess.call(['sed', '-i', 's/@/%d/g' % i, 'ud%d.txt' % i])
        subprocess.call(['sed', '-i', 's/&/%d/g' % XML, 'ud%d.txt' % i])

        # Lauch an instance
        instType = sys.argv[1]
        amiImageId = sys.argv[2]
        securityGroup = sys.argv[3]
        keyName = sys.argv[4]
        iamRole = sys.argv[5]
        instance = run_instance(instType, amiImageId, securityGroup, keyName, iamRole, 'ud%d.txt' % i)
        print(instance['Instances'][0]['InstanceId'])
        instances.append(instance['Instances'][0]['InstanceId'])
        subprocess.call(['rm', 'ud%d.txt' % i])
        time.sleep(5)
