import sys
import os
import numpy as np
import json

class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return json.JSONEncoder.default(self, obj)


if __name__ == "__main__":
    # hard code files names in here ( i only need to run this once)
    flx0_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx0.json'
    flx1_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx1.json'
    flx2_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx2.json'
    flx3_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx3.json'
    flx4_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx4.json'
    flx5_file = '/home/tmengel/mvtx-decoder/deadpix/pixmasks/hot_pixel_mask_flx5.json'

    
