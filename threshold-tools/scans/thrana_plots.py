#!/usr/bin/env python3
import os
import argparse
import sys
sys.path.insert(0, './packages')

import numpy as np
import matplotlib.pyplot as plt

# global factor to translate threshold values from ADC to e- (electrons)
ADC_TO_ELECTRONS = 10.0  # This is a placeholder value, adjust as needed

stave_map = {
    "0": [
        "L0_03",
        "L0_04",
        "L2_01",
        "L2_02",
        "L0_05",
        "L2_03",
        "L2_04",
        "L2_15"
    ],
    "1": [
        "L1_00",
        "L1_01",
        "L2_06",
        "L2_07",
        "L1_02",
        "L1_03",
        "L2_08",
        "L2_09"
    ],
    "2": [
        "L0_00",
        "L0_01",
        "L1_04",
        "L2_10",
        "L0_02",
        "L1_05",
        "L1_06",
        "L1_07"
    ],
    "3": [
        "L0_09",
        "L0_10",
        "L2_05",
        "L2_11",
        "L0_11",
        "L2_12",
        "L2_13",
        "L2_14"
    ],
    "4": [
        "L0_06",
        "L0_07",
        "L1_12",
        "L2_00",
        "L0_08",
        "L1_13",
        "L1_14",
        "L1_15"
    ],
    "5": [
        "L1_08",
        "L1_09",
        "L2_16",
        "L2_17",
        "L1_10",
        "L1_11",
        "L2_18",
        "L2_19"
    ]
}

def read_stave_data(staveid, data_directory):
    feeids = sorted([(int(staveid[1]) << 12) | int(staveid[3:]) | (gbt << 8) for gbt in range(3)])
    data = np.zeros((3*len(feeids), 512, 1024), dtype=np.float32)
    for feeid in feeids:
        gbt_channel = feeids.index(feeid)
        # cmd_for_file="ls " + data_directory + f"/*thr_map*{feeid}.dat"
        cmd_for_file="ls " + data_directory + f"/*FEEID{feeid}_thrs.dat"
        # get filename 
        thrmap_file = os.popen(cmd_for_file).read().strip().split('\n')[0]
        if not thrmap_file:
            print(f"Warning: No threshold map file found for stave {staveid} (feeid {feeid}). Skipping.")
            continue
        fin = open(thrmap_file, 'r')
        thrdata = np.fromfile(fin, dtype=np.float32)
        for j in range(512):
            for k in range(1024*3):
                i = k // 1024
                data[gbt_channel*3 + i][j][k - i*1024] = thrdata[j*3*1024 + k]
    fin.close()
    return data

def get_stave_ids(felix_number):
    """Get stave IDs for a given Felix number."""
    if felix_number < 0 or felix_number > 5:
        raise ValueError("Felix number must be between 0 and 5.")
    return stave_map[str(felix_number)]

if __name__ == "__main__":

    parser = argparse.ArgumentParser("Threshold analysis.")
    parser.add_argument("-f", "--felix", help="Felix number (0-5)", type=int, choices=range(6), required=True)
    parser.add_argument("-d", help="Data directory", required=True)
    parser.add_argument("-o", help="Output file prefix", required=False, default='./')
    parser.add_argument("-nsteps", help="Threshold step", type=int, default=50, required=False)
    args = parser.parse_args()

    if not os.path.exists(args.o):
        os.makedirs(args.o)
        
    staveids = get_stave_ids(args.felix)

    thrmap_fig, thrmap_ax = plt.subplots( len(staveids), 9, 
                                         figsize=(20,1.5*len(staveids)), 
                                         sharex=True, sharey='row', 
                                         gridspec_kw={'hspace': 0, 'wspace': 0})
    staveid_idx = 0
    staveid_idx_max = len(staveids)
    print (f'Processing {staveid_idx_max} staves: {staveids}')
    if staveid_idx_max == 1:
        ax_for_cb = thrmap_ax[0]
    else:
        ax_for_cb = thrmap_ax[0][0]

    all_avg = np.zeros((len(staveids),9, 512))
    all_std = np.zeros((len(staveids),9, 512))
    for stave in staveids:
        # avg = np.zeros((9, 512))
        stave_data = read_stave_data(stave, args.d)
        print(f'Processing stave {stave} with data shape {stave_data.shape}')
        if stave_data.ndim != 3 or stave_data.shape[0] != 9 or stave_data.shape[1] != 512 or stave_data.shape[2] != 1024:
            print(f'Error: Stave data for stave {stave} has unexpected shape {stave_data.shape}. Expected (9, 512, 1024).')
            exit(1)

        # initialize arrays for average and standard deviation
        avg_chip = np.zeros((9,), dtype=float)
        std_chip = np.zeros((9,), dtype=float)
        nparr_avg_stave = np.zeros((1,), dtype=float)

        # loop over chips and calculate average and standard deviation of non-zero and non-nan entries. each chip has 512 rows and 1024 columns so a total of 512*1024 entries
        for chip in range(0, stave_data.shape[0]):
            # get non-zero and non-nan entries for each chip
            non_zero_entries = stave_data[chip][stave_data[chip] != 0]
            non_nan_entries = non_zero_entries[~np.isnan(non_zero_entries)]
            print(f'Chip {chip} has {non_nan_entries.size} non-zero and non-nan entries.')
            avg_chip[chip] = np.mean(non_nan_entries) * ADC_TO_ELECTRONS
            std_chip[chip] = np.std(non_nan_entries) * ADC_TO_ELECTRONS
            print(f'Chip {chip} average: {avg_chip[chip]:.2f}, std: {std_chip[chip]:.2f}')
            # append the non_nan_entries to avg_stave
            nparr_avg_stave = np.append(nparr_avg_stave, non_nan_entries)

        avg_stave = np.mean(nparr_avg_stave) * ADC_TO_ELECTRONS
        std_stave = np.std(nparr_avg_stave) * ADC_TO_ELECTRONS

        for chip in range(0, stave_data.shape[0]):
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

            
            chip_thrmap = stave_data[chip] * ADC_TO_ELECTRONS
            ax_thrmap.invert_yaxis()
            cmap = plt.colormaps["Reds"].copy()
            ax_thrmap.imshow(chip_thrmap, cmap=cmap, aspect='auto', origin='lower')
            ax_thrmap.text(0.5, .5, r' {avg_chip[chip]:.2f} $/pm$ {std_chip[chip]:.2f}', horizontalalignment='center', transform=ax_thrmap.transAxes, fontsize=10)

        all_avg[staveid_idx] = avg_stave
        all_std[staveid_idx] = std_stave
        print(f'Stave {stave} average: {avg_stave:.2f}, std: {std_stave:.2f}')
        staveid_idx += 1

    thrmap_fig.colorbar(ax_for_cb.images[0],format='%.1f',ax=thrmap_ax, label='Threshold (e$^-$)', orientation='vertical')
    thrmap_fig.savefig(f'{args.o}/mvtx{args.felix}.png', dpi=300)

    thrs_file= f'{args.o}/mvtx{args.felix}_thresholds.txt'
    with open(thrs_file, 'w') as f:
        for stave in staveids:
            stave_idx = staveids.index(stave)
            mean_thr = np.mean(all_avg[stave_idx])
            std_thr = np.std(all_avg[stave_idx])
            f.write(f"{stave}: {mean_thr:.2f} e$^-$ $pm$ {std_thr:.2f} e$^-$\n")

    print(f"Thresholds saved to {thrs_file}")
    print("Threshold map saved to", f'{args.o}/mvtx{args.felix}.png')

    plt.close(thrmap_fig)
    sys.exit(0)
# End of script
