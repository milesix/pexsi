#ifndef _POLE_HPP_
#define _POLE_HPP_

#include "environment_impl.hpp"

namespace PEXSI{


// Wrapper function

// Shift and weights of poles for calculating the density matrix, the
// total energy, and the Hellman-Feynman force. This is the most
// commonly used subroutine.
int GetPoleDensity(Complex* zshift, Complex* zweight, 
	     int Npole, double temp, double gap, double deltaE,
	     double mu);

// Shift and weights of poles for calculating the Helmholtz free energy
// density matrix, and the Helmholtz free energy using the formula by
// Alavi et al.
int GetPoleHelmholtz(Complex* zshift, Complex* zweight, 
	     int Npole, double temp, double gap, double deltaE,
	     double mu);


// Shift and weights of poles for calculating the energy energy density
// matrix, and the Pulay force
int GetPoleForce(Complex* zshift, Complex* zweight, 
	     int Npole, double temp, double gap, double deltaE,
	     double mu);

}

#endif