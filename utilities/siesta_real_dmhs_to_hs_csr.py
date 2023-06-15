""" Convert SIESTA DMHS file into binary HS file

According to SIESTA manual,

https://siesta-project.org/SIESTA_MATERIAL/Docs/Manuals/siesta-MaX-1.3.0.pdf

after setting

Write.DMHS.NetCDF True

the density matrix (DM), Hamiltonian (H) and overlap (S) matrices will
be stored in the CSR format (according to the sparsity pattern of H as
XX_DMHS.nc using NetCDF. 

This subroutine reads the H and S matrices from NetCDF, and output it
into .csr binary files that can be directly tested using PEXSI.

NOTE: H and S are real symmetric matrices. Therefore CSR and CSC are
equivalent.

Example:

     python siesta_real_dmhs_to_hs_csr.py  -i C148_DMHS.nc

Lin Lin
Last revision: 06/14/2023
"""
import numpy as np
from netCDF4 import Dataset
import struct

def convert_siesta_dmhs(inp_fname, out_H_fname, out_S_fname):
    print(f'Reading file {inp_fname}...')
    data = Dataset(inp_fname, "r", format="NETCDF4")
    norbs = data.dimensions['norbs'].size
    nnzs  = data.dimensions['nnzs'].size
    rowptr = np.array(data['row_pointer']) #0-based
    rowptr += 1 # change to 1-based
    rowptr = np.append(rowptr, nnzs+1)
    
    colind = np.array(data['column']) # 1-based
    H_val = np.array(data['h'])[0,0,:]
    S_val = np.array(data['overlap'])

    print('Writing the matrix to file (binary csc format)...')


    with open(out_H_fname, 'wb') as f:
        f.write(struct.pack('i', norbs))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('i', norbs+1))
        f.write(struct.pack('i' * len(rowptr), *rowptr))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('i' * len(colind), *colind))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('d' * len(H_val),  *H_val))
    
    with open(out_S_fname, 'wb') as f:
        f.write(struct.pack('i', norbs))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('i', norbs+1))
        f.write(struct.pack('i' * len(rowptr), *rowptr))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('i' * len(colind), *colind))
        f.write(struct.pack('i', nnzs))
        f.write(struct.pack('d' * len(S_val),  *S_val))

    return

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--inp', default="C148_DMHS.nc",
            help='Input XX_DMHS.nc file from SIESTA')

    args = parser.parse_args()
    inp_fname = args.inp


    out_H_fname = 'H.csr'
    out_S_fname = 'S.csr'
    
    convert_siesta_dmhs(inp_fname, out_H_fname, out_S_fname)
