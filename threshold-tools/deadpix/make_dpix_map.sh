#!/bin/bash
export DECODER=$PWD/decoder/mvtx-decoder
export OUTPUTDIR=$PWD/DeadPixMaps

# check there is 1 argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <input file list>"
    exit 1
fi


# make directory for output
if [ ! -d $OUTPUTDIR ]; then
    mkdir -p $OUTPUTDIR
fi

# read in file list
function flist {
    cat $1 | while read line
    do
        echo $line
    done
}

FILES=($(flist $1))
printf "%s\n" ${FILES[@]}

for FILE in ${FILES[@]}
do 
    PACKET_IDS=$(dlist $FILE | grep "Packet" | awk '{print $2}')
    for PACKET_ID in $PACKET_IDS
    do
        FEEDIDSTR=$(ddump -s -g -n 0 -p $PACKET_ID $FILE | $DECODER -t 1 | sed -n '2p')
        FEEDIDSTR=${FEEDIDSTR:1:${#FEEDIDSTR}-2}
        FEEDIDS=(`echo $FEEDIDSTR | tr ',' ' '`)

        echo "Packet: $PACKET_ID"
        echo "   Feeids: ${FEEDIDS[@]}"

        for FEEDID in ${FEEDIDS[@]}
        do
            layer=$(printf "%d" $((($FEEDID >> 12) & 0x1F)))
            id=$(printf "%d" $(($FEEDID & 0xFF)))
            STAVEID=$(printf "L%01d_%02d" $layer $id)
            
            echo "   Stave: $STAVEID"

            # make directory for stave
            THIS_STAVE_OUTPUT=$OUTPUTDIR/$STAVEID
            mkdir -p $THIS_STAVE_OUTPUT

            # make directory for threshold scan
            THRESHOLD_SCAN_OUTPUT=$THIS_STAVE_OUTPUT/ThresholdScans
            mkdir -p $THRESHOLD_SCAN_OUTPUT

            FILEOUT=$(printf "$THRESHOLD_SCAN_OUTPUT/fee_%04d" $FEEDID)
            # call the decoder
            # time ddump -s -g -n 0 -p $PACKET_ID $FILE  | $DECODER -p $FILEOUT -t 3 -f $FEEDID
        done
    done
done

# # get all packet ids
# PACKET_IDS=$(dlist $1 | grep "Packet" | awk '{print $2}')

# STAVE_OUTPUT_DIRECTORIES=()

# # only do 1 stave (3 feeids)
# N_DONE=0
# for PACKET_ID in $PACKET_IDS
# do
#     FEEDIDSTR=$(ddump -s -g -n 0 -p $PACKET_ID $1 | $DECODER -t 1 | sed -n '2p')
#     FEEDIDSTR=${FEEDIDSTR:1:${#FEEDIDSTR}-2}
#     FEEDIDS=(`echo $FEEDIDSTR | tr ',' ' '`)

#     echo "Packet: $PACKET_ID"
#     echo "   Feeids: ${FEEDIDS[@]}"

#     for FEEDID in ${FEEDIDS[@]}
#     do
#         layer=$(printf "%d" $((($FEEDID >> 12) & 0x1F)))
#         id=$(printf "%d" $(($FEEDID & 0xFF)))
#         STAVEID=$(printf "L%01d_%02d" $layer $id)
        
#         echo "   Stave: $STAVEID"

#         # make directory for stave
#         THIS_STAVE_OUTPUT=$OUTPUTDIR/$STAVEID
#         mkdir -p $THIS_STAVE_OUTPUT

#         # check if stave is already in list
#         FOUND=0
#         for STAVE_DIR in ${STAVE_OUTPUT_DIRECTORIES[@]}
#         do
#             if [ $STAVE_DIR == $THIS_STAVE_OUTPUT ]; then
#                 FOUND=1
#                 break
#             fi
#         done
#         if [ $FOUND == 0 ]; then
#             STAVE_OUTPUT_DIRECTORIES+=($THIS_STAVE_OUTPUT)
#         fi

#         # make directory for threshold scan
#         THRESHOLD_SCAN_OUTPUT=$THIS_STAVE_OUTPUT/ThresholdScans
#         mkdir -p $THRESHOLD_SCAN_OUTPUT

#         FILEOUT=$(printf "$THRESHOLD_SCAN_OUTPUT/fee_%04d" $FEEDID)
#         # call the decoder
#         # time ddump -s -g -n 0 -p $PACKET_ID $1  | $DECODER -p $FILEOUT -t 3 -f $FEEDID
    
#         # N_DONE=$((N_DONE+1))
#         # if [ $N_DONE -eq 3 ]; then
#         #     break 2
#         # fi
#     done
# done


# # FINISHED_STAVES=()
# # for STAVE_DIRECTORY in ${STAVE_OUTPUT_DIRECTORIES[@]}
# # do

# #     # check if stave is already done
# #     THIS_STAVE_ID=${STAVE_DIRECTORY##*/}
# #     for FINISHED_STAVE in ${FINISHED_STAVES[@]}
# #     do
# #         if [ $THIS_STAVE_ID == $FINISHED_STAVE ]; then
# #             continue 2
# #             # don't redo stave
# #         fi
# #     done

# #     # get all feeids in stave
# #     FEEIDS_IN_STAVE=()
# #     for PACKET_ID in $PACKET_IDS
# #     do
# #         # get feedids that belong to this stave
# #         FEEDIDSTR=$(ddump -s -g -n 0 -p $PACKET_ID $1 | $DECODER -t 1 | sed -n '2p')
# #         FEEDIDSTR=${FEEDIDSTR:1:${#FEEDIDSTR}-2}
# #         FEEDIDS=(`echo $FEEDIDSTR | tr ',' ' '`)
# #         for FEEDID in ${FEEDIDS[@]}
# #         do
# #             layer=$(printf "%d" $((($FEEDID >> 12) & 0x1F)))
# #             id=$(printf "%d" $(($FEEDID & 0xFF)))
# #             STAVE_ID=$(printf "L%01d_%02d" $layer $id)
        
# #             if [ $THIS_STAVE_ID == $STAVE_ID ]; then
# #                 FEEIDS_IN_STAVE+=($FEEDID)
# #             fi
# #         done
# #     done

# #     FILES_FOR_FEEIDS=()
# #     for FEE_ID in ${FEEIDS_IN_STAVE[@]}
# #     do
# #         FEE_FILE=$(printf "$STAVE_DIRECTORY/ThresholdScans/fee_%04d.dat" $FEE_ID)
# #         FILES_FOR_FEEIDS+=($FEE_FILE)
# #     done

# #     # make bad pixel map output directory
# #     BAD_PIX_MAP_DIR=$STAVE_DIRECTORY/BadPixelMap
# #     mkdir -p $BAD_PIX_MAP_DIR

# #     BAD_PIX_MAP_FILE=$BAD_PIX_MAP_DIR/$THIS_STAVE_ID.json
# #     python3 bad_pix_map.py --fee_ids ${FEEIDS_IN_STAVE[@]} --fee_id_files ${FILES_FOR_FEEIDS[@]} --output $BAD_PIX_MAP_FILE
# # done