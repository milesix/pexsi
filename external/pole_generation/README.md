# Generate poles for PEXSI

## Method 1: Contour discretization

Directory: `method1_contour`

Language: MATLAB

Main: ProcContour_all.m to generate method1_all.dat (then need to
convert)

Reference: 

Lin, L., Lu, J., Ying, L. and E, W., 2009. Pole-based approximation of the Fermi-Dirac function. Chinese Annals of Mathematics, Series B, 30(6), pp.729-742.

Hale, N., Higham, N.J. and Trefethen, L.N., 2008. Computing A^α,\log(A), and related matrix functions by contour integrals. SIAM Journal on Numerical Analysis, 46(5), pp.2505-2523.

## Method 2: Minimax optimization

Directory: `method2_minimax`

Language: C

Main routine: main.sh (need to modify to include the table)

Then run ProcMiniMax.m to combine the output into a single file (maybe
better to write a python script to do this)

Reference:

Moussa, J.E., 2016. Minimax rational approximation of the Fermi-Dirac distribution. The Journal of chemical physics, 145(16).

## Method 3: adaptive Antoulas–Anderson (AAA) 

Directory: `method3_aaa`

Language: Julia

Main routine: run_gen_poles_aaa.jl and then run pypole.py to convert the
output 

Reference:

Nakatsukasa, Y., Sète, O. and Trefethen, L.N., 2018. The AAA algorithm for rational approximation. SIAM Journal on Scientific Computing, 40(3), pp.A1494-A1522.
