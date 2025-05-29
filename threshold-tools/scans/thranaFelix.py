#!/usr/bin/env python3

import os
import argparse
import json
import sys
import numpy as np
from matplotlib import pyplot as plt


class ThresholdScan:
    def __init__(self, run_parameters, data_dir="thrana/", nsteps=50, verbose=True):
        self.filename = run_parameters
        self.data_directory = data_dir
        self.verbose = verbose
        self.nsteps = nsteps
        with open(run_parameters) as f:
            self.params = json.load(f)
        self.ladders = self.params['_cable_resistance']
        self.staveids = [f'L{((int(lstr[1]) << 12) | int(lstr[3:]) | (0 << 8)) >> 12:01d}_{((int(lstr[1]) << 12) | int(lstr[3:]) | (0 << 8)) & 0x1F:02d}' for lstr in self.ladders]
        if self.verbose:
            for staveid in self.staveids:
                print(f'Stave {staveid} found.')

    @staticmethod
    def read_stave_data(staveid, data_directory):
        feeids = sorted([(int(staveid[1]) << 12) | int(staveid[3:]) | (gbt << 8) for gbt in range(3)])
        data = np.zeros((3*len(feeids), 512, 1024), dtype=np.float32)
        for feeid in feeids:
            gbt_channel = feeids.index(feeid)
            print(f'Processing stave {staveid} feeid {feeid} gbt_channel {gbt_channel}')
            thrmap_file = f'{data_directory}thr_map_{feeid}.dat'
            fin = open(thrmap_file, 'r')
            thrdata = np.fromfile(fin, dtype=np.float32)
            for j in range(512):
                for k in range(1024*3):
                    i = k // 1024
                    data[gbt_channel*3 + i][j][k - i*1024] = thrdata[j*3*1024 + k]
        fin.close()
        return data

    @staticmethod
    def count_dead_pixels(stave_data, nsteps):
        for chip in range(0,stave_data.shape[0]):
            dead_pixs = 0
            nan_count = 0
            n_hits = np.sum(stave_data[chip] > 0)
            if n_hits == 0:  # no data for this chip
                # print(f'gbt_channel {chip // 3} chip {chip % 3}. No hits.')
                # dead_pixels[chip][:][:] = nsteps-1
                dead_pixs = stave_data.shape[1]*stave_data.shape[2]
                print(f'gbt_channel {chip // 3} chip {chip}. {dead_pixs} dead, no hits.')
                continue

            for r in range(0, stave_data.shape[1]):
                for c in range(0, stave_data.shape[2]):
                    m = stave_data[chip][r][c]
                    if m == 0:
                        dead_pixs += 1
                    if (m < 0) or (m > float(nsteps)):
                        dead_pixs += 1
                    if m is np.nan:
                        nan_count += 1
            print(f'gbt_channel {chip // 3} chip {chip}. {dead_pixs} dead, {nan_count} nan.')

    def plot_threshold_map(self, outputfile, dead=False):
        thrmap_fig, thrmap_ax = plt.subplots(len(self.staveids),9, figsize=(20,1.5*len(self.staveids)), sharex=True, sharey='row', gridspec_kw={'hspace': 0, 'wspace': 0})
        staveid_idx = 0
        staveid_idx_max = len(self.staveids)
        if staveid_idx_max == 1:
            ax_for_cb = thrmap_ax[0]
        else:
            ax_for_cb = thrmap_ax[0][0]

        for stave in self.staveids:
            stave_data = self.read_stave_data(stave, self.data_directory)
            if dead:
                self.count_dead_pixels(stave_data, self.nsteps)

            for chip in range(0,stave_data.shape[0]):
                if staveid_idx_max == 1:
                    ax_thrmap = thrmap_ax[chip]
                else:
                    ax_thrmap = thrmap_ax[staveid_idx][chip]

                if chip == 0:
                    ax_thrmap.set_ylabel(f'{stave}', rotation=90, labelpad=20, fontsize=14)
                else:
                    ax_thrmap.yaxis.set_visible(False)

                if staveid_idx == staveid_idx_max-1:
                    ax_thrmap.set_xlabel(f'Chip {chip}', rotation=0, labelpad=20, fontsize=14)
                    ax_thrmap.xaxis.set_ticks([500, 1000])
                else:
                    ax_thrmap.xaxis.set_visible(False)

                chip_thrmap = stave_data[chip]
                ax_thrmap.invert_yaxis()
                if dead:
                    deadmask = chip_thrmap == 0
                    chip_thrmap[deadmask] = 100.0
                    cmap = plt.colormaps["viridis"].copy()
                    cmap.set_under(color='white')
                    ax_thrmap.imshow(chip_thrmap, cmap=cmap, aspect='auto', origin='lower', vmin=float(self.nsteps), vmax=101)

                else:
                    chip_thrmap[chip_thrmap[:, :] == 0] = np.nan
                    cmap = plt.colormaps["viridis"].copy()
                    cmap.set_under(color='white')
                    ax_thrmap.imshow(chip_thrmap, cmap=cmap, aspect='auto', origin='lower', vmin=2, vmax=25)

            staveid_idx += 1

        thrmap_fig.colorbar(ax_for_cb.images[0],format='%.0e',ax=thrmap_ax, label='Threshold (e$^-$)', orientation='vertical',
                            pad=0.01, fraction=0.05,aspect=5)
        thrmap_fig.savefig(outputfile, dpi=300)
        plt.show()
        if not self.verbose:
            plt.close(thrmap_fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Threshold analysis.",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    # parser.add_argument("-thr", help="Threshold file", required=True)
    parser.add_argument("-i", help="Run parameters json file", required=True)
    parser.add_argument("-d", help="Data directory", required=False, default="./thrana/")
    parser.add_argument("-p", help="Output file prefix", required=False, default='./')
    parser.add_argument("-nsteps", help="Threshold step", type=int, default=50, required=False)
    parser.add_argument('-dead', '--dead', action='store_true', help="Plot dead pixels.")
    parser.add_argument('-q', '--quiet', action='store_true', help="Do not display plots.")

    args = parser.parse_args()

    # thrfile = args.thr
    run_param_file = args.i
    data_dir = args.d
    prefix = args.p
    nsteps = args.nsteps
    verbose = not args.quiet
    dead = args.dead

    #check if run_params is a json file and exists
    if not run_param_file.endswith(".json") or not os.path.exists(run_param_file):
        print("Please provide a valid run parameters file.")
        sys.exit(1)
    #check if data directory exists
    if not os.path.exists(data_dir):
        print("Please provide a valid data directory.")
        sys.exit(1)

    if verbose:
        print("Run parameters file:", run_param_file)
        print("Data directory:", data_dir)
        print("Output prefix:", prefix)
        print("Threshold step:", nsteps)

    ThrScan = ThresholdScan(run_param_file, data_dir, nsteps, verbose)
    if prefix.endswith("/"):
        thrfilename_png = f'{prefix}threshold_map.png'
        deadfilename_png = f'{prefix}dead_pixels.png'
    else:
        thrfilename_png = f'{prefix}_threshold_map.png'
        deadfilename_png = f'{prefix}_dead_pixels.png'

    if dead:
        print("Plotting dead pixels.")
        ThrScan.plot_threshold_map(deadfilename_png, dead=True)

    print("Plotting threshold map.")
    ThrScan.plot_threshold_map(thrfilename_png, dead=False)
