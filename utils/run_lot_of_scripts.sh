cd /work/johan/PND-Loads/

./johan.sh build_llvm 8

rm /work/johan/spec2017/benchspec/CPU/*/run/list
python3 utils/compile_spec_with_pgo.py 0
sudo python3 utils/parse_all_labelled.py with_pgo_shift4_0_E

python3 utils/run_models.py --run-type=main-tage-properly-scaled --addr-types="with_pgo_shift4_0_E" --cpu-models="a14,m4"


# rm /work/johan/spec2017/benchspec/CPU/625.x264_s/run/list
# python3 utils/compile_spec_with_pgo.py 1
# sudo python3 utils/parse_all_labelled.py with_pgo_1

# python3 utils/run_models.py --run-type=main-tage-properly-scaled --addr-types="with_pgo_1" --cpu-models="a14,m4,m4-0"


# rm /work/johan/spec2017/benchspec/CPU/625.x264_s/run/list
# python3 utils/compile_spec_with_pgo.py 2
# sudo python3 utils/parse_all_labelled.py with_pgo_2

# python3 utils/run_models.py --run-type=main-tage-properly-scaled --addr-types="with_pgo_2" --cpu-models="a14,m4,m4-0"

