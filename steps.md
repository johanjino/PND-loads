How to run test:

- Setup spec config is setup for Instrumentation 
- Modify AliasHints pass to print instructions to file (Uncomment code)
- Run `dump_instructions_pgo.py` to get instructions to instrument
- Remove all run/build diretories and `list` files. This is to ensure directory is numbered `.0000`
- Comment Alias Hints pass to not print our instructions anymore.
- Run `gen_profiledata.py` to start compiling and start profiling
- Reset config to now use aarch64 and ready for gem5 sim
- First set config to withPGObinary and generate raw binary
- Run `compile_spec_with_pgo.py`
- Now set config to withPGO and generate labelled binary
- Run `compile_spec_with_pgo.py` again
- Run `sudo parse_all_labelled.py with_pgo`
- Run `python3 run_models.py --run-type=main-tage-properly-scaled --addr-types=with_pgo --cpu-models=m4`

