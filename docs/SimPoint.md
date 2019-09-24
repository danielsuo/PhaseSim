# Generating SPEC CPU2017 SimPoints

## Assumptions
- Compile SPEC using `ref/spec/gcc-mixed_asm_C.cfg`
- Examine first run command for each benchmark (see `./scripts/spec/invoke.sh`)
- Default `valgrind --tool=exp-bbv`
  - Interval size: 100,000,000
- SimPoint
  - maxK 30

## Instructions
- Set up SPEC CPU2017 (note, requires `git` and `git-lfs`)
  ```bash
  # Fetch
  git clone https://github.com/danielsuo/spec
  pushd spec
  git lfs pull

  # Build and install
  ./install.sh
  ln -s $(pwd)/../ref/spec/gcc-mixed_asm_C.cfg config/default.cfg
  ./bin/runcpu -a build all

  # Return to phasesim home
  popd
  ```

- Generate run commands (run each line and cancel once run directories are set up)
  ```bash
  # Generate run meta data
  ./spec/bin/runcpu -a run all --fake
  mkdir -p tmp/spec

  # Convert to bash scripts
  ./scripts/spec/invoke.sh
  ```

-
