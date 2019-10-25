#!/bin/bash
curl https://bootstrap.pypa.io/get-pip.py | sudo -H python3.6
python3 -m pip install pandas matplotlib scipy --user
