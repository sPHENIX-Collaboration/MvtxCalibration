#!/bin/bash

# useful functions for MVTX calibration
# usage: source utils.sh

get_feeids() {

    local file=""
    local out="dec"
    local packets=()
    local show_help=false
    local valid_outputs=("dec" "jo" "hex" "bin" "stave")
    local verbose=false
    local exe=/sphenix/user/tmengel/MVTX/felix-mvtx/software/cpp/decoder/mvtx-decoder
    local OUTPUT=""

    OPTIND=1
   
    display_help() {
      echo "Usage: get_feeids -f <file> -p <packet numbers...(optional: defaults to all packets)> -o <output format (optional: default dec)> -v (optional: verbose output)"
      echo "Options:"
      echo "  -h          Display this help message"
      echo "  -f <file>   Specify the prdf file to read"
      echo "  -o <output> OPTIONAL: Specify the output format (dec, hex, bin, val)"
      echo "  -p <params> OPTIONAL: Specify the packet ids to read"
      echo "  -v          OPTIONAL: Verbose output (default: false)"
    }

    while getopts "hf:o:p:v" opt; do
        case $opt in
            h) show_help=true
               ;;
            f) file="$OPTARG"
               ;;
            o) out="$OPTARG"
               ;;
            p) packets+=("$OPTARG")
               ;;
            v) verbose=true
               ;;
            \?) echo "ERROR: Invalid option -$OPTARG" >&2
                display_help
                return 1
                ;;
            :) echo "ERROR: Option -$OPTARG requires an argument." >&2
               display_help
               return 1
               ;;
        esac
    done

    # Shift off the options and optional --
    shift $((OPTIND - 1))

    # Collect remaining arguments after the -p flag
    while (( "$#" )); do
        if [[ "$1" == -* ]]; then
            break
        fi
        packets+=("$1")
        shift
    done

    # help message
    if $show_help; then
         display_help
         return 0
    fi

    # Validate the -o flag value
    if [[ ! " ${valid_outputs[@]} " =~ " ${out} " ]]; then
        echo "ERROR: Invalid value for -o: $out. Valid options are: dec, val, hex, bin" >&2
        display_help
        return 1
    fi
   
    # Check if file exists if specified
    if [ -n "$file" ]; then
        if [ ! -f "$file" ]; then
            echo "File does not exist: $file" >&2
            display_help
            return 1
        fi
    fi

    # If no packets are provided, use default values
    if [ ${#packets[@]} -eq 0 ]; then
        if [ -n "$file" ]; then
            packets=($(dlist "$file" | grep "Packet" | awk '{print $2}'))
        else
            echo "ERROR: No file provided and no packets provided. Cannot determine packets." >&2
            display_help
            return 1
        fi
    fi

    if $verbose; then
        echo "PRDF File: $file"
        echo "Output format: $out"
        echo "Packets to scan: ${packets[@]}"
    fi

    
    if [ ! -f $exe ]; then
        echo "ERROR: Decoder executable not found: $exe" >&2
        return 1
    fi

    for packet in "${packets[@]}"; do

        local feeids=()
        
        # Get feeids from the executable
        feeids_raw=$(ddump -s -g -n 0 -p "$packet" "$file" | "$exe" -t 1 | grep -oP '\[\K[^\]]+')

        # Convert feeids_raw into an array
        IFS=', ' read -r -a feeids <<< "$feeids_raw"

        # Format feeids based on the output type
        local pack=""
        local feeids_formatted=()
        case $out in
               hex) 
                  pack=$(printf "0x%04X" $packet)
                  # Convert feeids to hexadecimal format
                  for feeid in "${feeids[@]}"; do
                      feeids_formatted+=("0x$(printf "%04X" $feeid)")
                  done
                  ;;
               bin)
                  pack=$(printf "%016b" $packet)
                  # Convert feeids to binary format
                  for feeid in "${feeids[@]}"; do
                      feeids_formatted+=("$(printf "%016b" $feeid)")
                  done
                  ;;
               jo)
                  # get the index of the packet in the array
                  pack_num=$(echo ${packets[@]} | tr ' ' '\n' | grep -n $packet | cut -d: -f1)
                  pack=$(($pack_num - 1))
                  # Convert feeids to 1-12 + 12*pack_num
                  feeidx=1
                  for feeid in "${feeids[@]}"; do
                        feeids_formatted+=("$(($feeidx + 12*$pack))")
                        feeidx=$((feeidx + 1))
                  done
                  pack=$(($pack + 1))
                  ;;
               dec)
                  pack=$packet
                  feeids_formatted=("${feeids[@]}")
                  ;;
               stave)
                  pack=$packet
                  for feeid in "${feeids[@]}"; do
                     stave_id=$(get_stave_id $feeid)
                     layer=$(echo $stave_id | awk -F'_' '{print $1}' | awk -F'L' '{print $2}')
                     id=$(echo $stave_id | awk -F'_' '{print $2}')
                     layer=$(printf "%d" $layer)
                     id=$(printf "%d" $id)
                     first_fee=$(((($layer << 12) | $id)))
                     for feeidx in {0..2}
                     do
                        # increment id by 256
                        fee=$((($first_fee + $feeidx * 256)))
                        if [ $fee -eq $feeid ]; then
                           start_alpide=$((3*$feeidx))
                           end_alpide=$((3*$feeidx+2))
                           feeids_formatted+=("$stave_id (Chips: $start_alpide-$end_alpide)")
                        fi
                     done
                  done
                  ;;
         esac


        OUTPUT+="Packet: $pack | Feeids: ${feeids_formatted[@]}\n"
    done

    echo -e $OUTPUT

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

    # first_fee=$((($layer << 12) | $id))
    first_fee=$(((($layer << 12) | $id)))
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