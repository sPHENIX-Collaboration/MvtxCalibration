#!/bin/bash
# SETUP

start_time=$(date +%s)
running_directory=$(pwd)
FELIX_NUMBER=${HOSTNAME[0] : -1}

# source mvtx software
source /home/mvtx/software/setup.sh

# directory where code is located
exe_dir=/home/phnxrc/tmengel/software/cpp
decoder_executable=${exe_dir}/thrsana
merger_executable=${exe_dir}/file-merger

if [ ! -f $decoder_executable ]; then
    echo "Decoder $decoder_executable does not exist"
    cd $exe_dir
    make
    if [ ! -f $decoder_executable ]; then
        echo "Decoder $decoder_executable does not exist"
        exit 1
    fi
fi

if [ ! -f $merger_executable ]; then
    echo "File merger $merger_executable does not exist"
    cd $exe_dir
    make
    if [ ! -f $merger_executable ]; then
        echo "File merger $merger_executable does not exist"
        exit 1
    fi
fi


## Arguments
# make sure runnumber and log dir are provided
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo "Usage: $0 <run number> <data directory> <scan type> <prefix (optional)>"
    exit 1
fi

run_number=$1
# see if runnumber is 8 characters long if not pad with zeros
if [ ${#run_number} -ne 8 ]; then
    run_number=$(printf "%08d" $run_number)
fi

data_directory=$2
# make sure data directory is absolute and exists
if [ ${data_directory:0:1} != "/" ]; then
    echo "Data directory $data_directory is not absolute"
    exit 1
fi 
if [ ! -d $data_directory ]; then
    echo "Data directory $data_directory does not exist"
    exit 1
fi

scan_type=$3
VALID_SCAN_TYPES=("DeadPix" "Thrs" "ThrsTune" "FakeHit" "HotPix")
# check if scan type is valid
if [[ ! " ${VALID_SCAN_TYPES[@]} " =~ " ${scan_type} " ]]; then
    echo "Invalid scan type $scan_type"
    echo "Valid scan types: ${VALID_SCAN_TYPES[@]}"
    exit 1
fi

prefix=$4
if [ -z "$prefix" ]; then
    prefix=""
fi

# set output directory
output_directory=${running_directory}
if [ ! -z "$prefix" ]; then
    output_directory=${output_directory}/${prefix}_
fi
output_directory=${output_directory}${scan_type}

# clear output directory if it exists
if [ -d $output_directory ]; then
    rm -rf $output_directory
fi
if [ ! -d $output_directory ]; then
    mkdir -p $output_directory
fi

echo "---------------------------------"
echo "Decoder script"
echo "---------------------------------"
echo "Date: $(date)"
echo "Start time: $start_time"
echo "Running directory: $running_directory"
echo "Host: $HOSTNAME"
echo "Felix number: $FELIX_NUMBER"
echo "---------------------------------"
echo "Arguments"
echo "---------------------------------"
echo "Run number: $run_number"
echo "Data directory: $data_directory"
echo "Scan type: $scan_type"
if [ ! -z "$prefix" ]; then
    echo "Prefix: $prefix"
fi
echo "---------------------------------"
echo "Output directory: $output_directory"

##############################
# Fuctions
# define function to get feeids
get_feeids() {
    packet=$1
    executable=$2
    prdf_file=$3

    feeids=$(ddump -s -g -n 0 -p $packet $prdf_file | $executable -t 1 | sed -n '2p')
    feeids=${feeids:1:${#feeids}-2}
    feeids=(`echo $feeids | tr ',' ' '`)

    echo ${feeids[@]}
}

# define function to get stave id
get_stave_id() {
    feedid=$1
    layer=$(printf "%d" $((($feedid >> 12) & 0x1F)))
    id=$(printf "%d" $(($feedid & 0xFF)))
    stave_id=$(printf "L%01d_%02d" $layer $id)

    echo $stave_id
}

# define function to order feeids from stave id
get_feeids_from_stave_id() {
    stave_id=$1
    layer=$(echo $stave_id | awk -F'_' '{print $1}' | awk -F'L' '{print $2}')
    id=$(echo $stave_id | awk -F'_' '{print $2}')
    layer=$(printf "%d" $layer)
    id=$(printf "%d" $id)

    first_fee=$((($layer << 12) | $id))
    for i in {0..2}
    do
        # increment id by 256
        fee=$((($first_fee + $i * 256)))
        feeids+=($fee)
    done

    echo ${feeids[@]}
}

# define function to get packet ids
get_packet_ids() {
    file=$1

    packet_ids=$(dlist $file | grep "Packet" | awk '{print $2}')

    echo $packet_ids
}

##############################
# make file list
cd $data_directory
files=$(ls *${run_number}*.*)
if [ ${#files[@]} -eq 0 ]; then
    echo "No files in $data_directory with run number $run_number"
    exit 1
fi

file_list=${output_directory}/file_list_${run_number}.list
if [ -f $file_list ]; then
    rm $file_list
fi
for file in $files
do
    echo $file >> $file_list
done

echo "Wrote file list to $file_list"

# see if there are multiple files
PRDF_FILE=""
NEEDS_TO_BE_MERGED=0
if [ ${#files[@]} -gt 1 ]; then
    NEEDS_TO_BE_MERGED=1
else
    echo "Only one file in $data_directory with run number $run_number"
    # set first file as prdf file
    PRDF_FILE=${data_directory}/${files[0]}
fi

if [ $NEEDS_TO_BE_MERGED -eq 1 ]; then
    # set merger executable
    merged_prdf_file=/tmp/mvtx${FELIX_NUMBER}-${run_number}-all.prdf
    if [ ! -f $merged_prdf_file ]; then
        echo "Merging files into $prdf_file"
        $merger_executable $file_list $merged_prdf_file
        PRDF_FILE=$merged_prdf_file
        echo "Done"
    fi
fi

if [ ! -f $PRDF_FILE ]; then
    echo "PRDF file $PRDF_FILE does not exist or is empty"
    exit 1
fi

##############################
# decode data
packet_ids=$(get_packet_ids $PRDF_FILE)
echo "Packet ids: $packet_ids"

staves=()
for packet_id in $packet_ids
do
    fee_ids=$(get_feeids $packet_id $decoder_executable $PRDF_FILE)
    echo "Packet: $packet_id"
    echo "   Feeids: ${fee_ids[@]}"

    for fee_id in ${fee_ids[@]}
    do 
        stave_id=$(get_stave_id $fee_id)  
        stave_output_directory=${output_directory}
        if [ ! -d $stave_output_directory ]; then
            mkdir -p $stave_output_directory
            staves+=($stave_id)
        fi
        output_file=$(printf "$stave_output_directory/hmap" $fee_id)

        # call the decoder
        if [ $scan_type == "DeadPix" ]; then
            time ddump -s -g -n 0 -p $packet_id $PRDF_FILE  | $decoder_executable -p $output_file -t 0 -f $fee_id # dead pixel scan
        elif [ $scan_type == "Thrs" ]; then
            time ddump -s -g -n 0 -p $packet_id $PRDF_FILE  | $decoder_executable -p $output_file -t 1 -f $fee_id # threshold scan
        elif [ $scan_type == "ThrsTune" ]; then
            time ddump -s -g -n 0 -p $packet_id $PRDF_FILE  | $decoder_executable -p $output_file -t 2 -f $fee_id # threshold tune
        # elif [ $scan_type == "FakeHit" ]; then
        #     time ddump -s -g -n 0 -p $packet_id $PRDF_FILE  | $decoder_executable -p $output_file -t 3 -f $fee_id # threshold scan
        elif [ $scan_type == "HotPix" ]; then
            time ddump -s -g -n 0 -p $packet_id $PRDF_FILE  | $decoder_executable -p $output_file -t 0 -f $fee_id # threshold scan
        # fi
        fi
    done
done


if [ $scan_type == "DeadPix" ]; then

    # if deadpix scan then merge files
    bad_pix_exe=/home/phnxrc/tmengel/software/py/bad_pix_map.py
    felix_map_exe=/home/phnxrc/tmengel/software/py/felix_map.py
    if [ ! -f $bad_pix_exe ]; then
        echo "Bad pixel map executable $bad_pix_exe does not exist"
        exit 1
    fi
    if [ ! -f $felix_map_exe ]; then
        echo "Felix map executable $felix_map_exe does not exist"
        exit 1
    fi
    
    deadpix_maps=()
    for stave in ${staves[@]}
    do
        stave_output_directory=${output_directory}/${stave}
        if [ -d $stave_output_directory ]; then
            files=$(ls ${stave_output_directory}/*_deadpix.dat)
            fees=()
            if [ ${#files[@]} -gt 0 ]; then
                for file in ${files[@]}
                do
                    feeids=$(echo $file | awk -F'/' '{print $NF}' | awk -F'_' '{print $2}')
                    fees+=($feeids)
                done
                
                deadpix_map=${stave_output_directory}/${stave}.json
                python3 $bad_pix_exe --fee_ids ${fees[@]} --fee_id_files ${files[@]} --output $deadpix_map
                deadpix_maps+=($deadpix_map)
            fi
        fi
    done

    if [ ${#deadpix_maps[@]} -gt 0 ]; then
        output_file=${output_directory}/deadpix_map_${run_number}.json
        python3 $felix_map_exe --files ${deadpix_maps[@]} --output $output_file
    
        echo "Wrote dead pixel map to $output_file"
    fi

fi

echo "All done"
end_time=$(date +%s)
diff=$(( $end_time - $start_time ))
echo "It took $diff seconds"





