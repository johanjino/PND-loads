#!/bin/bash

BASE_DIR="addr_files"

# Output header
printf "%-20s %-10s %-10s %-12s %-10s\n" "File" "NoAlias%" "MayOnly%" "PartialOnly%" "MustOnly%"

# Initialize arrays to store fractions
no_vals=()
may_vals=()
# partial_vals=()
must_vals=()
count=0

for total_file in "$BASE_DIR/must_alias_count"/*; do
    fname=$(basename "$total_file")
    must=$(wc -l < "$total_file")

    # Skip if must is 0
    if [ "$must" -eq 0 ]; then
        continue
    fi

    # Read counts
    no=$(wc -l < "$BASE_DIR/pgo_no_alias_count/$fname" 2>/dev/null || echo 0)
    may=$(wc -l < "$BASE_DIR/may_alias_count/$fname" 2>/dev/null || echo 0)
    # partial=$(wc -l < "$BASE_DIR/partial_alias_count/$fname" 2>/dev/null || echo 0)

    # Compute exclusive counts
    may_only=$((may - no))
    #partial_only=$((partial - may))
    #must_only=$((must - partial))
    must_only=$((must - may))

    # Convert to percentages
    no_pct=$(awk -v v="$no" -v t="$must" 'BEGIN { printf "%.2f", (v/t)*100 }')
    may_pct=$(awk -v v="$may_only" -v t="$must" 'BEGIN { printf "%.2f", (v/t)*100 }')
    # partial_pct=$(awk -v v="$partial_only" -v t="$must" 'BEGIN { printf "%.2f", (v/t)*100 }')
    must_pct=$(awk -v v="$must_only" -v t="$must" 'BEGIN { printf "%.2f", (v/t)*100 }')

    # Print row
    printf "%-20s %-10s %-10s %-12s %-10s\n" "$fname" "$no_pct" "$may_pct" "$partial_pct" "$must_pct"

    # Store fractions
    no_vals+=("$(awk -v v="$no" -v t="$must" 'BEGIN { print v/t }')")
    may_vals+=("$(awk -v v="$may_only" -v t="$must" 'BEGIN { print v/t }')")
    # partial_vals+=("$(awk -v v="$partial_only" -v t="$must" 'BEGIN { print v/t }')")
    must_vals+=("$(awk -v v="$must_only" -v t="$must" 'BEGIN { print v/t }')")
    
    count=$((count + 1))
done

# Function to compute geometric mean from array of fractions
geomean() {
    arr=("$@")
    prod=1
    for val in "${arr[@]}"; do
        prod=$(awk -v p="$prod" -v v="$val" 'BEGIN { print p * v }')
    done
    awk -v p="$prod" -v n="${#arr[@]}" 'BEGIN { printf "%.2f", 100 * (p ** (1/n)) }'
}

# Compute and print geometric means
no_gm=$(geomean "${no_vals[@]}")
may_gm=$(geomean "${may_vals[@]}")
# partial_gm=$(geomean "${partial_vals[@]}")
must_gm=$(geomean "${must_vals[@]}")

echo
# printf "%-20s %-10s %-10s %-12s %-10s\n" "GeoMean(%)" "$no_gm" "$may_gm" "$partial_gm" "$must_gm"
printf "%-20s %-10s %-10s %-12s %-10s\n" "GeoMean(%)" "$no_gm" "$may_gm" "0" "$must_gm"