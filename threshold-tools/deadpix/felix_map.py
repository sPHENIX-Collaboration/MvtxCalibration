#!/usr/bin/env python3
import sys
import os
import argparse
import numpy as np
import json
import matplotlib.pyplot as plt

class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return json.JSONEncoder.default(self, obj)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Plot bad pixel map",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-f", "--files", nargs='+', required=True, help="List of files to read")
    parser.add_argument("-o", "--output", type=str, required=True, help="Output file")
    args = parser.parse_args()
    
    # files are /path/stave.json
    dmp = {}
    for f in args.files:
        if not os.path.isfile(f):
            raise FileNotFoundError(f'File {f} not found.')
        stave = f.split('/')[-1].split('.')[0]
        data = {}
        data = json.load(fin)
        dmp[stave] = data
    with open(args.output, 'w') as fout:
        json.dump(dmp, fout, indent=4, cls=NumpyEncoder)
    sys.exit(0)
    
