# Compiler and Profiling-Assisted Memory Dependency Prediction

This repository contains the necessary tools and instructions to reproduce the research findings from the paper "Compiler Analysis and Profiling Assisted Memory Dependency Prediction". The research introduces a compiler and profiling-assisted approach to enhance memory dependency prediction by labeling specific load instructions as "Predict-No-Dependency" (PND), allowing them to bypass the hardware predictor and execute more speculatively.

The core idea is to use the distance between a load and its dependent store as the primary metric to determine if a load is safe for speculative execution. By identifying and labeling these PND loads, we can achieve significant IPC improvements, particularly on smaller, more resource-constrained CPU models without any architectural modifications.

## Key Contributions
- Integration of advanced static analysis (SVF) and profile-guided methods to improve the accuracy of PND labeling.
- A novel PND labeling scheme that uses a memory "store distance" profile.
- Demonstrated geometric mean IPC improvements of **5.03%**, **0.42%**, and **0.01%** on SPEC2017 C/C++ workloads across the S5, A14, and M4 CPU models, respectively.

---

## Reproducing the Store Distance Results

Follow these steps to reproduce the store distance evaluation results presented in the thesis.

### Prerequisites

1.  **Gem5 Simulator:** You must have a working build of the [Gem5 simulator](https://www.gem5.org/documentation/general_docs/building). The paths in the scripts assume the Gem5 directory is located at `*/PND-Loads/gem5/`.
2.  **SPEC CPU2017:** The experiments are run on the SPEC CPU2017 benchmark suite. You may use any binary you like, although the sript may need significant modification. However, the Gem5 run command can found in `spec_automation_train.py`. Run that locally on any binary, remove the checkpoint restore arguments if not using checkpoints.
3.  **Directory Structure:** The `run_models.py` script assumes the following directory structure. Please create these directories and place the relevant files within them.
    * `*/PND-Loads/addr_files/`: To store the labeled address files generated from profiling.
    * `*/PND-Loads/cpu_models/`: To store the different O3CPU model configurations.
    * `*/PND-Loads/utils/`: To store utility scripts like `run_all_chkpts.py`.

### Step 1: Generate the Store Distance Profile

The first step is to profile the benchmarks to understand the distance between dependent stores and loads.

1.  **Modify Gem5 Source:** In the Gem5 source code, navigate to `src/cpu/simple/atomic.cc`. Uncomment the functions related to store distance profiling. Navigate to `/src/base/stats/text.cc` and uncomment the functions to print out the print out store distance information.
2.  **Recompile Gem5:** Build Gem5 with the modifications.
3.  **Run Profiling:** Run the SPEC2017 benchmarks using Gem5 with the `NonCachingSimpleCPU` model. This will generate the raw store distance data for each benchmark. Use the train version of spec automation, `spec_automation_train.py` to make this easier.

### Step 2: Aggregate Profile Data and Label Addresses

Once you have the raw profiling data, you need to aggregate it and generate labeled address files.

-   Use the `store_distance_train.py` script to process the output from the previous step. Always ensure the paths are modified correctly to match your system. This script will aggregate the distance data and create the final labeled address files, which should be stored in the `*/PND-Loads/addr_files/` directory.

### Step 3: Run the O3CPU Model Evaluation

With the labeled address files ready, you can now run the full evaluation using the different O3CPU models. Note, to the run the CPU model with no-MDP (i.e. S5), use the Gem5 from gem5-noMDP.

1.  **The `run_models.py` Script:** This script automates the process of running the benchmarks across different CPU models and address file configurations. It copies the appropriate CPU model configuration into the Gem5 source, rebuilds Gem5, and then executes the benchmarks.

2.  **How to Run:**
    Navigate to the directory containing `run_models.py` and execute it with the required arguments.

    ```bash
    python3 run_models.py --run-type [type] --addr-types [types] --cpu-models [models] --benches [benches]
    ```

3.  **Argument Explanations:**
    * `--run-type`: Specifies the type of run. This can be any name you like. This is a required argument.
    * `--addr-types`: A comma-separated list of the address file types you want to test (corresponding to subdirectories in `addr_file_dir`). Use `all` to run all available types or `all-with-base` to include a baseline run. This is a required argument.
    * `--cpu-models`: A comma-separated list of the CPU models to use (corresponding to `.py` files in `cpu_model_dir`). Use `all` to run all available models. This is a required argument.
    * `--benches` (Optional): A comma-separated list of specific benchmarks to run. If omitted, all benchmarks will be run.

4.  **Example Command:**
    To run all benchmarks with the "final" run-type, using "aliasOracle" address type and "a14" CPU model:

    ```bash
    python3 run_models.py --run-type final --addr-types aliasOracle --cpu-models a14
    ```

The script will first run all CPU models on the `base` configuration to generate comparison results, then proceed to run them against each specified address file type.

---
## Citation

If you use or refer to this work, please cite the original thesis:

> Jino, Johan. (2025). *Compiler Analysis and Profiling Assisted Memory Dependency Prediction*. MEng Individual Project, Department of Computing, Imperial College London.
