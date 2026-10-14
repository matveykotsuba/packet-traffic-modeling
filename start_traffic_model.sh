#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: $0 <input_file> [output_dir]"
  echo "Example: $0 cpp/examples/equal.txt"
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT_FILE="$1"
OUTPUT_DIR="${2:-${SCRIPT_DIR}/output}"

if [[ "$INPUT_FILE" != /* ]]; then
  INPUT_FILE="${PWD}/${INPUT_FILE}"
fi

if [[ "$OUTPUT_DIR" != /* ]]; then
  OUTPUT_DIR="${PWD}/${OUTPUT_DIR}"
fi

CSV_FILE="${OUTPUT_DIR}/traffic.csv"
METADATA_FILE="${OUTPUT_DIR}/metadata.txt"

mkdir -p "$OUTPUT_DIR"

"${SCRIPT_DIR}/cpp/bin/traffic_model" "$INPUT_FILE" "$CSV_FILE" "$METADATA_FILE"
python3 "${SCRIPT_DIR}/python/analyze.py" --csv "$CSV_FILE" --metadata "$METADATA_FILE" --output-dir "$OUTPUT_DIR"
