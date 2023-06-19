#!/bin/bash

mpirun -n 1 ~/Projects/pexsi/build/examples/run_ksdft \
  -H lap2dr.matrix -T 1 -ne 12 -npole 40 -npoint 1 \
  -mumin 0.0 -mumax 0.5 -tolint 0.01

echo ""
echo "Output:"
cat logwrapper0
