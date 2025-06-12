#!/bin/bash

# Usage check
if [ $# -ne 1 ]; then
  echo "Usage: $0 <benchmark> (e.g., x264.0)"
  exit 1
fi

benchmark=$1

#train_dir="/work/johan/results/main-tage-properly-scaled/with_pgo_shift4_0_E/a14/$benchmark/raw"
train_dir="/work/johan/results/final/pgo_full/m4/$benchmark/raw"
#train_dir="/work/johan/results/store_distance/sd_train_30_full/m4/$benchmark/raw"
#full_dir="/work/johan/results/store_distance/sd_train_30_full/m4/$benchmark/raw"
full_dir="/work/johan/results/final/base/m4/$benchmark/raw"

# ANSI Colors
GREEN=$'\033[1;32m'
RED=$'\033[1;31m'
GRAY=$'\033[0;90m'
RESET=$'\033[0m'

for full_path in "$full_dir"/*; do
  [ -d "$full_path" ] || continue
  folder=$(basename "$full_path")
  train_path="$train_dir/$folder"

  full_stats="$full_path/stats.txt"
  train_stats="$train_path/stats.txt"

  # Skip if stats.txt missing
  if [[ ! -f "$full_stats" || ! -f "$train_stats" ]]; then
    echo "$folder (missing stats.txt)"
    continue
  fi

  # Check both files have at least 2 matching lines
  if [[ $(grep -c '^system\.switch_cpus\.commitStats0\.ipc' "$full_stats") -lt 2 ]] || \
     [[ $(grep -c '^system\.switch_cpus\.commitStats0\.ipc' "$train_stats") -lt 2 ]]; then
    echo "$folder (less than 2 ipc values)"
    continue
  fi

  # Extract the second ipc values
  full_val=$(awk '/^system\.switch_cpus\.commitStats0\.ipc/ {c++; if (c==2) print $2}' "$full_stats")
  train_val=$(awk '/^system\.switch_cpus\.commitStats0\.ipc/ {c++; if (c==2) print $2}' "$train_stats")

  # Validate extraction
  if [[ -z "$full_val" || -z "$train_val" ]]; then
    echo "$folder (unable to extract ipc values)"
    continue
  fi

  # Compute percentage change
  pct=$(awk -v t="$train_val" -v f="$full_val" 'BEGIN { printf "%.2f", (t - f) * 100 / f }')

  # Print result with colored percentage
  if (( $(echo "$pct > 0" | bc -l) )); then
    printf "%s %.4f → %.4f (%s+%.2f%%%s)\n" "$folder" "$train_val" "$full_val" "$GREEN" "$pct" "$RESET"
  elif (( $(echo "$pct < 0" | bc -l) )); then
    printf "%s %.4f → %.4f (%s%.2f%%%s)\n" "$folder" "$train_val" "$full_val" "$RED" "$pct" "$RESET"
  else
    printf "%s %.4f = %.4f (%s0.00%%%s)\n" "$folder" "$train_val" "$full_val" "$GRAY" "$RESET"
  fi
done
