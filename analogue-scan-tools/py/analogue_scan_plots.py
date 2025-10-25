#!/usr/bin/env python3
import argparse
import os
import sys

import matplotlib.pyplot as plt
import numpy as np

stave_map = {
    "0": ["L0_03", "L0_04", "L2_01", "L2_02", "L0_05", "L2_03", "L2_04", "L2_15"],
    "1": ["L1_00", "L1_01", "L2_06", "L2_07", "L1_02", "L1_03", "L2_08", "L2_09"],
    "2": ["L0_00", "L0_01", "L1_04", "L2_10", "L0_02", "L1_05", "L1_06", "L1_07"],
    "3": ["L0_09", "L0_10", "L2_05", "L2_11", "L0_11", "L2_12", "L2_13", "L2_14"],
    "4": ["L0_06", "L0_07", "L1_12", "L2_00", "L0_08", "L1_13", "L1_14", "L1_15"],
    "5": ["L1_08", "L1_09", "L2_16", "L2_17", "L1_10", "L1_11", "L2_18", "L2_19"],
}

N_TRIGGERS = 9


def read_stave_data(staveid, data_directory):
    feeids = sorted(
        [(int(staveid[1]) << 12) | int(staveid[3:]) | (gbt << 8) for gbt in range(3)]
    )
    data_scan = np.zeros((3 * len(feeids), 512, 1024), dtype=np.float32)
    for feeid in feeids:
        gbt_channel = feeids.index(feeid)
        file_scan = "ls " + data_directory + f"/*ycm_hitmap_{feeid}.dat"
        # get filename
        map_file = os.popen(file_scan).read().strip().split("\n")[0]
        if not map_file:
            print(
                (
                    "Warning: either threshold or rms file could be found for stave "
                    + f"{staveid} (feeid {feeid}). Skipping."
                )
            )
            continue
        # read the data from the threshold file
        fin = open(map_file, "r")
        scandata = np.fromfile(fin, dtype=np.int32)
        for j in range(512):
            for k in range(1024 * 3):
                i = k // 1024
                data_scan[gbt_channel * 3 + i][j][k - i * 1024] = (
                    scandata[j * 3 * 1024 + k]
                ) / N_TRIGGERS

        fin.close()
    return data_scan


def get_stave_ids(felix_number):
    """Get stave IDs for a given Felix number."""
    if felix_number < 0 or felix_number > 5:
        raise ValueError("Felix number must be between 0 and 5.")
    return stave_map[str(felix_number)]


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Analogue scan analysis.")
    parser.add_argument(
        "-f",
        "--felix",
        help="Felix number (0-5)",
        type=int,
        choices=range(6),
        required=True,
    )
    parser.add_argument("-d", help="Data directory", required=True)
    parser.add_argument("-o", help="Output file prefix", required=False, default="./")
    args = parser.parse_args()

    if not os.path.exists(args.o):
        os.makedirs(args.o)

    staveids = get_stave_ids(args.felix)

    # The scan data map
    map_fig, map_ax = plt.subplots(
        len(staveids),
        9,
        figsize=(20, 1.5 * len(staveids)),
        sharex=True,
        sharey="row",
        gridspec_kw={"hspace": 0, "wspace": 0},
    )

    staveid_idx = 0
    staveid_idx_max = len(staveids)
    print(f"Processing {staveid_idx_max} staves: {staveids}")
    if staveid_idx_max == 1:
        ax_for_cb = map_ax[0]
    else:
        ax_for_cb = map_ax[0][0]

    all_avg = {}  # Dictionary to store average thresholds for each stave
    all_std = {}  # Dictionary to store standard deviation of thresholds for each stave
    for stave in staveids:
        stave_data = read_stave_data(stave, args.d)
        print(f"Processing stave {stave} with data shape {stave_data.shape}.")
        if (
            stave_data.ndim != 3
            or stave_data.shape[0] != 9
            or stave_data.shape[1] != 512
            or stave_data.shape[2] != 1024
        ):
            print(
                (
                    f"Error: data for stave {stave} has an unexpected shape {stave_data.shape}."
                    + " Expected (9, 512, 1024)."
                )
            )
            exit(1)

        # initialize arrays for average and standard deviation
        avg_chip = np.zeros(
            (9,), dtype=float
        )  # Array to store average thresholds for each chip
        std_chip = np.zeros(
            (9,), dtype=float
        )  # Array to store standard deviation of thresholds for each chip
        nparr_avg_stave = np.zeros(
            (1,), dtype=float
        )  # Array to store all non-nan entries for the stave
        nparr_rms_stave = np.zeros(
            (1,), dtype=float
        )  # Array to store all non-nan rms entries for the stave

        # loop over chips and calculate average and standard deviation of non-zero
        # and non-nan entries. each chip has 512 rows and 1024 columns so a total
        # of 512*1024 entries
        for chip in range(0, stave_data.shape[0]):
            # get non-zero and non-nan entries for each chip
            non_zero_entries = stave_data[chip][stave_data[chip] != 0]
            non_nan_entries = non_zero_entries[~np.isnan(non_zero_entries)]
            print(
                f"Chip {chip} has {non_nan_entries.size} non-zero and non-nan entries."
            )
            avg_chip[chip] = np.mean(non_nan_entries)
            std_chip[chip] = np.std(non_nan_entries)
            print(
                f"Chip {chip} average: {avg_chip[chip]:.2f}, std: {std_chip[chip]:.2f}"
            )
            # append the non_nan_entries to avg_stave
            nparr_avg_stave = np.append(nparr_avg_stave, non_nan_entries)

        avg_stave = np.mean(nparr_avg_stave)
        std_stave = np.std(nparr_avg_stave)

        for chip in range(0, stave_data.shape[0]):
            if staveid_idx_max == 1:
                ax_map = map_ax[chip]
            else:
                ax_map = map_ax[staveid_idx][chip]

            if chip == 0:
                ax_map.set_ylabel(f"{stave}", rotation=90, labelpad=20, fontsize=14)
            else:
                ax_map.yaxis.set_visible(False)

            if staveid_idx == staveid_idx_max - 1:
                ax_map.set_xlabel(f"Chip {chip}", rotation=0, labelpad=20, fontsize=14)
                ax_map.xaxis.set_ticks([500, 1000])
            else:
                ax_map.xaxis.set_visible(False)

            chip_map = stave_data[chip]
            ax_map.invert_yaxis()
            cmap = plt.colormaps["Blues"].copy()
            ax_map.imshow(
                chip_map,
                interpolation=None,
                cmap=cmap,
                aspect="equal",
            )
            ax_map.text(
                0.5,
                0.5,
                rf"{avg_chip[chip]:.2f} $\pm$ {std_chip[chip]:.2f}",
                horizontalalignment="center",
                transform=ax_map.transAxes,
                fontsize=10,
            )

        all_avg[staveid_idx] = avg_stave
        all_std[staveid_idx] = std_stave
        print(f"Stave {stave} average: {avg_stave:.2f}, std: {std_stave:.2f}")
        staveid_idx += 1

    map_fig.colorbar(
        ax_for_cb.images[0],
        format="%.1f",
        ax=map_ax,
        label="efficiency",
        orientation="vertical",
    )
    map_fig.savefig(f"{args.o}/mvtx{args.felix}.png", dpi=300, bbox_inches="tight")
    map_fig.savefig(f"{args.o}/mvtx{args.felix}.pdf", bbox_inches="tight")
    plt.close(map_fig)

    # print the map
    print("Efficiency map (avg):", all_avg)
    print("Efficiency map (std):", all_std)

    eff_file = f"{args.o}/mvtx{args.felix}_eff.txt"
    with open(eff_file, "w") as f:
        for stave in staveids:
            stave_idx = staveids.index(stave)
            mean_thr = all_avg[stave_idx]
            std_thr = all_std[stave_idx]
            f.write(f"{stave}: {mean_thr:.2f} e$^-$ $\\pm$ {std_thr:.2f} e$^-$\n")

    print(f"Efficincy plots saved to {eff_file}")

    sys.exit(0)
# End of script
