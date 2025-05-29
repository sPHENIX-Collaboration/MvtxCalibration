UTILS.SH – MVTX CALIBRATION UTILITIES

This script provides a collection of Bash functions useful for MVTX calibration workflows
in the sPHENIX experiment.

================================================================================
USAGE
================================================================================

To use the functions in your shell session:

    source utils.sh

================================================================================
FUNCTIONS
================================================================================

1. get_feeids

Extract and display FEEIDs from PRDF files using the mvtx-decoder.

Syntax:
    get_feeids -f <file> [-p <packet_ids>] [-o <output_format>] [-v]

Options:
    -f <file>        Required. Path to the PRDF file.
    -p <ids>         Optional. Packet IDs to query. Defaults to all packets in the file.
    -o <format>      Optional. Output format. Default is "dec".
                     Valid options: dec, hex, bin, jo, stave
    -v               Optional. Enable verbose output.
    -h               Show help message.

Output Formats:
    dec   - Decimal (default)
    hex   - Hexadecimal (e.g., 0x1A2B)
    bin   - Binary (16-bit)
    jo    - Flat index [1–12] + offset by 12 × packet index
    stave - Stave ID with ALPIDE chip range

Example:
    get_feeids -f run.prdf -p 20001 -o stave -v


2. get_stave_id

Convert a FEEID to stave ID.

Syntax:
    get_stave_id <feeid>

Example:
    get_stave_id 412 // this is made up, replace with a real FEEID
    Output: L3_00


3. get_feeids_from_stave_id

Get the three FEEIDs corresponding to a given stave ID.

Syntax:
    get_feeids_from_stave_id <stave_id>

Example:
    get_feeids_from_stave_id L3_00



4. get_packet_ids

List all packet IDs found in a PRDF file.

Syntax:
    get_packet_ids <file>

Example:
    get_packet_ids run.prdf
    Output: 20001 20002 20003 ...


================================================================================
REQUIREMENTS
================================================================================

- mvtx-decoder binary (path hardcoded):
    `/sphenix/user/tmengel/MVTX/felix-mvtx/software/cpp/decoder/mvtx-decoder` is mine but you need a local path

- External tools required:
    ddump, dlist, awk, grep, printf

- A valid PRDF input file


================================================================================
NOTES
================================================================================

- This script is intended for use in the sPHENIX MVTX calibration pipeline.
- It must be sourced, not executed directly.
- Ensure the decoding environment and file paths are valid before use.
