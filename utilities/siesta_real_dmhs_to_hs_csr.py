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

NOTE: The unit of H is Rydberg from dmhs.

Lin Lin
Last revision: 06/14/2023
"""
import numpy as np
from netCDF4 import Dataset
import struct
from scipy.sparse import csr_matrix
import scipy.linalg

def convert_siesta_dmhs(inp_fname, out_H_fname, out_S_fname, testing = False):
    print(f'Reading file {inp_fname}...')
    data = Dataset(inp_fname, "r", format="NETCDF4")
    # matrix size
    norbs = data.dimensions['norbs'].size
    # number of nonzeros
    nnzs  = data.dimensions['nnzs'].size
    # row pointer, of size norbs+1
    rowptr = np.array(data['row_pointer']) #0-based
    rowptr = np.append(rowptr, nnzs)
    # column indices in each row, size nnz
    colind = np.array(data['column'])-1 # 0-based
    # nonzero values in each row, size nnz
    H_val = np.array(data['h'])[0,0,:]
    S_val = np.array(data['overlap'])
    
    if testing:
        print('Convert to scipy sparse matrix')
        H = csr_matrix((H_val, colind, rowptr), shape=(norbs, norbs)).toarray() 
        S = csr_matrix((S_val, colind, rowptr), shape=(norbs, norbs)).toarray() 
        dm_val = np.array(data['dm_out'])[0,0,:]
        dm = csr_matrix((dm_val, colind, rowptr), shape=(norbs, norbs)).toarray() 
        print('Tr[dm*S] = ', np.trace(dm @ S))
        ev = scipy.linalg.eigh(H,S,lower=True)[0] 
        print('ev[0:10] = ', ev[0:10])


    print('Writing the matrix to file (binary csc format)...')

    rowptr += 1 # change to 1-based
    colind += 1 # change to 1-based

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
    
    convert_siesta_dmhs(inp_fname, out_H_fname, out_S_fname,
            testing=False)
