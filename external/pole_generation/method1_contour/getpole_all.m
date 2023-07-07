function [zshift, zweight] = getpole_all( Npole, T, Gap, DeltaE, mu, nspin )
% getpole2.m generates the poles positions and weights. 
%
% The weights include:
%
% zweight{1}: weights for Fermi-Dirac
% zweight{2}: weights for free energy
% zweight{3}: weights for energy (for the (x-mu) part)
%
% Input:
%		Npole: the number of poles to be used.
%		T: temperature, unit(K)
%		Gap: 	Energy gap defined to be min(abs(EV-mu)). EV is the eigenvalue set
%			of Hamiltonian, unit(hartree)
%		DeltaE: Spectrum width defined to be max(EV)-min(EV). EV is the eigenvalue
%			set of Hamiltonian, unit(hartree)
%		mu: Chemical potential, unit(hartree)
%		nspin: Number of spin component
%
% Output:
%		
%		zshift:  Shift of poles (polepos).
%		zweight: Weight of poles (poleweight).
% Usage of pole information to calculate eletronic density: 
%
%   Rho = zeros(N, 1);  
%   % VERY IMPORTANT!  This is different from previous version
%
%   for i = 1 : npoles
%     Calculate Result = diag( inv( (H - polepos(i))) )  !
%     % VERY IMPORTANT!  Sign of polepos is different from previous version
%     Rho = Rho + imag( poleweight(i) * Result );
%   end 
% 
% Lin Lin
% Revision: 7/21/2018

K2au = 3.166815d-6;
au2K = 315774.67;
beta = au2K/(T);

Npolehalf = Npole/2;
M = DeltaE;
mshift = (pi/beta)^2;
m2 = mshift+(Gap)^2;
M2 = M^2;
k = (sqrt(M2/m2)-1)/(sqrt(M2/m2)+1);
L = -log(k)/pi;
[K,Kp] = ellipkkp(L);

t = .5i*Kp - K + (.5:Npolehalf)*2*K/Npolehalf;  %t = .9999999999i*Kp - K + (.5:Npolehalf)*2*K/Npolehalf;
[u cn dn] = ellipjc(t,L);
z = sqrt(m2*M2)*((1/k+u)./(1/k-u));
dzdt = cn.*dn./(1/k-u).^2;

zsqrt = sqrt(z-mshift);


zweight = cell(3,1);
zweight{1} = zeros(Npole, 1);
zweight{2} = zeros(Npole, 1);
zweight{3} = zeros(Npole, 1);
zshift = zeros(Npole, 1);

f = cell(3,1);
f{1} = @(z) nspin./(1+exp(beta*z));  
f{2} = @(z) nspin * f2(z);
f{3} = @(z) nspin*z./(1+exp(beta*z));

% From Eq. (2.10) in 
%   Lin, Lu, Ying and E, " Pole-Based Approximation of the Fermi-Dirac
%   Function",  Chin. Ann. Math. 30B, 729, 2009
%
for j = 1 : Npolehalf
  zshift(j) = mu + zsqrt(j);
  zshift(j+Npolehalf) = mu - zsqrt(j);
  % Old version which does not include the identity in the formulation.
  %
  % zweight(j) = ...
    % 2*K*sqrt(m2*M2)/(k*pi*Npolehalf) / zsqrt(j) * ...
    % dzdt(j) * (-tanh(beta*zsqrt(j)/2));
  % zweight(j+Npolehalf) = ...
    % 2*K*sqrt(m2*M2)/(k*pi*Npolehalf) / (-zsqrt(j)) * ...
    % dzdt(j) * (-tanh(beta*(-zsqrt(j))/2));
  %
  % New version which takes into account the identity.
  for ll = 1 : 3
    zweight{ll}(j) = ...
      2*K*sqrt(m2*M2)/(k*pi*Npolehalf) / zsqrt(j) * ...
      dzdt(j) * f{ll}(zsqrt(j));
    zweight{ll}(j+Npolehalf) = ...
      2*K*sqrt(m2*M2)/(k*pi*Npolehalf) / (-zsqrt(j)) * ...
      dzdt(j) * f{ll}(-zsqrt(j));
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function y = f2(z)
  % -log(1+exp(-z)) implemented in a stable way
  if( real(z) >= 0 )
    ez = exp(-z);
    y = -log(1.0 + ez);
  else
    ez = exp(z);
    y = z - log(1.0+ez);
  end
