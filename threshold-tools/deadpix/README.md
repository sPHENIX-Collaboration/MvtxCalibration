# Source MVTX env
    - source /home/mvtx/software/setup.sh

# Decoder
    - same as cpp decoder in `/home/mvtx/felix-mvtx/software/cpp/decoder` but will less output for threshold scan and allows for specifying output file name. Added test called 'ThresholdScan Sum' which just produces a hitmap for a threshold scan prdf file. Bad pixels are those with a value of 0.

# make_dpix_map.sh
    - shell script which sperated prdf into stave ids, and produces a hitmap for each stave. It then feeds the hitmap to `bad_pix_map.py` which produces a json file with 0 hit pixels with the coordinates [chip,row,col].
    
# Scans on each felix sever
    - mvtx0: /data/runs/20230603_220613_ThresholdScan/test_00000224-0000.prdf
    - mvtx1: ...