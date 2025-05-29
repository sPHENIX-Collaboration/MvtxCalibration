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

class DeadPixelMap:
    def __init__(self, verbose=True):
        self.verbose = verbose
        self.feeids = []
        self.feeid_files = []
        
    def add_fee(self, feeid, feeid_file):
        if not os.path.isfile(feeid_file):
            raise FileNotFoundError(f'File {feeid_file} not found.')
        self.feeids.append(feeid)
        self.feeid_files.append(feeid_file)
        if self.verbose:
            print(f'Added feeid {feeid} file {feeid_file}')

    @staticmethod
    def read_stave_data(feeids, feeid_files):

        data = np.zeros((3*len(feeids), 512, 1024), dtype=np.float32)

        for feeid in feeids:
            gbt_channel = feeids.index(feeid)
            hit_file = feeid_files[gbt_channel]
            fin = open(hit_file, 'r')
            filedata = np.fromfile(fin, dtype=np.float32)
            for j in range(512):
                for k in range(1024*3):
                    i = k // 1024
                    data[gbt_channel*3 + i][j][k - i*1024] = filedata[j*3*1024 + k]
        fin.close()

        # find dead pixels
        dead_pixels = np.where(data <= 0)
        alive_pixels = np.where(data > 0)
        print(f'Dead pixels: {len(dead_pixels[0])}')
        
        dead_pixel_map = np.zeros((3*len(feeids), 512, 1024), dtype=np.int32)
        dead_pixel_map[dead_pixels] = 1
        dead_pixel_map[alive_pixels] = 0
        return dead_pixel_map

    def create_dead_pixel_map(self, outputfile):
        if len(self.feeids) == 0:
            raise ValueError('No feeids added.')
        dead_pixel_map = self.read_stave_data(self.feeids, self.feeid_files)

        # create json file with coordinates of dead pixels (chip, row, column)
        dead_pixels = np.where(dead_pixel_map == 1)
        dead_pixs = []
        ndead = len(dead_pixels[0])
        if(self.verbose):
            print(f'Found {ndead} dead pixels')

        bad_pixs = []
        for i in range(len(dead_pixels[0])):
            chip = dead_pixels[0][i] // 3
            row = dead_pixels[1][i]
            col = dead_pixels[2][i]
            bad_pix= [chip, row, col]
            bad_pixs.append(bad_pix)
        dead_pixs.append(bad_pixs)
        dead_pixs = np.array(dead_pixs)
        with open(outputfile, 'w') as f:
            json.dump(dead_pixs, f, indent=4, cls=NumpyEncoder)
        if self.verbose:
            print(f'Dead pixel map saved to {outputfile}')

        # plot dead pixel map
        # fig, ax = plt.subplots(1,3*len(self.feeids), figsize=(6*len(self.feeids), 6))
        # for i in range(3*len(self.feeids)):
            # chip_dead_pix = dead_pixel_map[i]
            # ax[i].scatter(chip_dead_pix[0], chip_dead_pix[1], s=1, c='r')
            # ax[i].set_title(f'Chip {i//3}')
            # ax[i].set_xlabel('Column')
            # ax[i].set_ylabel('Row')
            # ax[i].set_xlim(0, 1024)
            # ax[i].set_ylim(0, 512)
        # plt.tight_layout()

        # fig.savefig(outputfile.replace('.json', '.png'))
        # plt.close(fig)

        


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Plot bad pixel map",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-fs", "--fee_ids", nargs='+', required=True, help="List of feeids")
    parser.add_argument("-ff", "--fee_id_files", nargs='+', required=True, help="List of feeid files")
    parser.add_argument("-o", "--output", type=str, required=True, help="Output file")
    args = parser.parse_args()
    
    # remove quotes from feeids
    fee_ids = [int(feeid) for feeid in args.fee_ids]
    
    # create dead pixel map object
    dpm = DeadPixelMap(verbose=True)

    for i in range(len(fee_ids)):
        dpm.add_fee(fee_ids[i], args.fee_id_files[i])
    dpm.create_dead_pixel_map(args.output)

    sys.exit(0)