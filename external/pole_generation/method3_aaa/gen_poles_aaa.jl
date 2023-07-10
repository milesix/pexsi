using Printf
using LinearAlgebra
using Random
using SparseArrays 
using GenericLinearAlgebra: qr, eigvals

#############################################################################
#
#   This file contains the machinery to generate poles and residues 
#   of rational approximations to the Fermi-Dirac function, free energy
#   function, and scaled Fermi-Dirac function using the AAA method. The
#   poles for all 3 rational approximations are the same. This file
#   contains 2 user-callable functions: gen_poles_T, gen_poles_ran.
#   The following is a description of these functions:
#
#       gen_poles_T - given the desired number of pole, number of
#       discretization points that should be used in the computation, and
#       the temperature, this function produces a file PoleData.dat that
#       contains all poles and residues. PoleData.dat can be converted to a
#       cpp file for PEXSI using the pypole.py script.
#
#       gen_poles_ran - same as gen_poles_T except that the user inputs a
#       parameter ran such that all three functions are approximated on the
#       interval [-ran,infinity) instead of the temperature.
#
#   PLEASE NOTE THAT BOTH OF THESE FUNCTIONS MAY PRODUCE SPURIOUS POLES.
#   IT IS UP TO THE USER TO VERIFY THAT THESE POLES ARE NOT PRESENT.
#
#############################################################################

function gen_poles_T(npol=40,N=1e2,T=50)
#
#   This function generates the file PoleData.dat containing residues and a
#   common set of poles for rational approximations to the 3 desired
#   functions. 
#
#       Input parameters:
#
#   npol - the number of poles desired; note that the number of poles
#          produced by this function may be fewer than npol due to supurious
#          poles.
#
#   N - this parametrization controls the discretization points that should
#       be used. After the infinite interval is mapped to [-1,1] the
#       rational approximations are computed on a mesh exponentially graded
#       at 0 with 2*N+1 discretization points. If spurious poles are
#       removed, they might be removed by increasing this parameter. 
#
#   T - the temperature. this changes the interval on which the functions
#       are approximated 
#
#       Output:
#
#   The file PoleData.dat containing all poles and residues. This can be
#   converted to a cpp data file using the pypole.py script. 
#
  Random.seed!(1)
  eigval = BigFloat.(rand(10000))
  eigval = sort(eigval)

  K2au = BigFloat(3.166815e-6)
  beta = 1/(T*K2au)
  numElecExact = 2000
  occ, muexact = getocc(eigval,numElecExact,beta)

  ran = beta * (muexact-minimum(eigval).+1)

  pol, res = main_aaa(npol,N,ran)
  writedata(ran,pol,res,T,N)

end

#############################################################################

function gen_poles_ran(npol=40,N=1e2,ran=1000)
#
#   This function generates the file PoleData.dat containing residues and a
#   common set of poles for rational approximations to the 3 desired
#   functions. 
#
#       Input parameters:
#
#   npol - the number of poles desired; note that the number of poles
#          produced by this function may be fewer than npol due to supurious
#          poles.
#
#   N - this parametrization controls the discretization points that should
#       be used. After the infinite interval is mapped to [-1,1] the
#       rational approximations are computed on a mesh exponentially graded
#       at 0 with 2*N+1 discretization points. If spurious poles are
#       removed, they might be removed by increasing this parameter. 
#
#   ran - the approximations will be computed on the interval [-ran,infinity) 
#
#       Output:
#
#   The file PoleData.dat containing all poles and residues. This can be
#   converted to a cpp data file using the pypole.py script. 
#
    pol, res = main_aaa(npol,N,ran)
    writedata_noT(ran,pol,res,N)

end

#############################################################################

 function main_aaa(npol,N,ran)
#
#   This function uses set AAA to find a good collection of poles to
#   approximate all 3 functions and then good residues for each function
#   using least squares fitting. 
#
#       Input parameters:
#
#   npol - the number of poles desired; note that the number of poles
#          produced by this function may be fewer than npol due to supurious
#          poles.
#
#   N - this parametrization controls the discretization points that should
#       be used. After the infinite interval is mapped to [-1,1] the
#       rational approximations are computed on a mesh exponentially graded
#       at 0 with 2*N+1 discretization points. If spurious poles are
#       removed, they might be removed by increasing this parameter. 
#
#   ran - the approximations will be computed on the interval [-ran,infinity) 
#
#       Output:
#
#   pole - the array of poles. Note that the length of this array might be
#          smaller than npol. 
#
#   res - a 3 x # of poles array containing the residue data for each
#         function. res[1,:], res[2,:], and res[3,:] contain residues for the
#         Fermi-Dirac function, free energy, and scaled Fermi-Dirac function,
#         respectively.
#
#
   # functions to approximate
   farr = [s->fermidirac(s,0,1), 
        s->freeegy(s), 
        s-> s.*fermidirac(s,0,1)]
 
   # Mobius transform the interval [-ran,infinity] to [-1,1]
   a = ran
   ff = [s->farr[1].(-a*(s.-1)./(s.+1).-ran),
         s->farr[2].(-a*(s.-1)./(s.+1).-ran),
         s->farr[3].(-a*(s.-1)./(s.+1).-ran)]

   # create mesh on [-1,1] that is exponentially graded at the origin
   Z = exp10.(range(BigFloat.(-20),stop=-1e-10,length=Int(N/2)))
   Z = sort([Z;0;-Z]);

   # run AAA
   z,f,w = setaaa(ff,Z,npol+1)
   println("Completed AAA")

   # get the poles from the barycentric form by solving an eigenvalue
   # problem
   pol = comppoles(z,f,w)
   println("Computed poles")

   # throw away poles on/near [-1,1] 
   polind = (abs.(pol) .<= 1) .& (abs.(imag.(pol)) .<= 1e-10)
   pol = pol[.!polind]
 
   # take the remaining poles and solve a least squares problem to get
   # the residues

   c = Complex{BigFloat}.(zeros(3))
   for k = 1:3
     c[k] = sum(w.*f[k,:])/sum(w)
   end

   C = [1/(Z[l]-pol[k]) for l = 1:length(Z), k = 1:length(pol)]

   D = Z[2:end]-Z[1:end-1]
   D = [D[1]; D] 
   D = spdiagm(0 => sqrt.(abs.(D)))

   Q,R = qr(D*C)
   Q = Array(Q)
 
   res = Complex{BigFloat}.(zeros(3,length(pol)))
   for k = 1:3
     res[k,:] = R\(Q'*(D*(ff[k](Z).-c[k])))
   end
   println("Computed residues")
 
   # undo the Mobius transform 
   for k = 1:length(pol)
     opol = pol[k]
     pol[k] = (a-ran-a*pol[k]-pol[k]*ran)/(pol[k]+1)
     for j = 1:3
       res[j,k] = -res[j,k]*(a+pol[k]+ran)/(opol+1)
     end
   end
   println("Mapped back to original domain")
 
   # return computed poles and residues
   return pol, res

 end

#############################################################################
#
#   This function runs set AAA on a collection of functions. 
#
#       Input parameters:
#
#   farr - array of function handles to run set AAA on  
#
#   Z - array of sample points on which the functions should be approximated
#       on 
#
#   nsteps - the number of steps of AAA to run 
#
#       Output:
#
#   z - the array of common interpolation points on Z (|z|=nsteps)
#
#   f - an |farray| x nsteps array containing each function in farray
#       evaluated on each point in z 
#
#   w - the array of barycentric weights (|w|=nsteps)

function setaaa(farr, Z, nsteps)
  Z = Complex{BigFloat}.(Z)
  n = length(farr)
  M = length(Z)
  F = Complex{BigFloat}.(zeros(n,M))

  SF = Array{SparseMatrixCSC{Complex{BigFloat},Int64},1}(undef,n)
  for k = 1:n
    F[k,:] = farr[k].(Z)
    SF[k] = spdiagm(0 => F[k,:])
  end

  z = Complex{BigFloat}.(zeros(nsteps))
  f = Complex{BigFloat}.(zeros(n,nsteps)) 

  R = ones(n,M) .* F[:,1]

  J = trues(M)
  w = []

  C = Complex{BigFloat}.(zeros(M,nsteps))

  for m = 1:nsteps
    maxx = 0
    j = 1

    # determine max error across functions
    for k = 1:n 
      val,ind = findmax(abs.(F[k,:]-R[k,:]))
      if val > maxx
        maxx = val
        j = ind
      end
    end

    # add support point
    z[m] = Z[j]

    # add data values
    for k = 1:n
        f[k,m] = F[k,j] 
    end 

    # cancel out index
    J[j] = false 

    # append column to Cauchy matrix
    C[:,m]=[1/(Z[l]-z[m]) for l=1:M]

    # compute coefficient matrix for least squares problem 
    Aall = Complex{BigFloat}.(zeros(sum(J)*n,m))
    for k = 1:n
      Sf = diagm(0 => f[k,1:m])
      Aall[sum(J)*(k-1)+1:sum(J)*k,:] = SF[k][J,J]*C[J,1:m] - C[J,1:m]*Sf
    end

    # determine barycentric weights by solving least squares problem
    _,RR = qr(Aall)
    _,_,V = svd(RR)
    w = V[:,end]
    N = Complex{BigFloat}.(zeros(n,M))
    D = Complex{BigFloat}.(zeros(n,M))
    for k = 1:n
      N[k,:] = C[:,1:m]*(w.*f[k,1:m])
      D[k,:] = C[:,1:m]*w
      R[k,:] = F[k,:]; R[k,J] = N[k,J]./D[k,J];
    end

  end

  return z,f,w 

end

#############################################################################

function rhandle(z,f,w)
  r = function(zz)
    ind = zz .== z
    if any(ind); return f[ind][1] end
    return sum(w.*f./(zz .- z))/sum(w./(zz .- z))
  end
  return r
end

#############################################################################

function comppoles(z,f,w)
  m = length(w)
  e = ones(m)
  E,_ = qr(e)
  W,_ = qr(w)
  E = E[:,2:end]
  W = W[:,2:end]
  pol = eigvals((E'*W)\(E'*diagm(0=>z)*W))

  D(zz) = sum(w./(zz .- z))
  Dp(zz) = -sum(w./(zz .- z).^2)
  pol = pol - D.(pol)./Dp.(pol)

  return pol
end

#############################################################################

function getocc(ev,nocc,Tbeta)
  nev = length(ev)
  tol = 1e-15
  maxiter = 100
  
  if nev > nocc
    ilb = nocc - 1
    iub = nocc + 1
    lb = ev[ilb]
    ub = ev[iub]

    flb = sum(fermidirac.(ev,lb,Tbeta))
    fub = sum(fermidirac.(ev,ub,Tbeta))

    while (nocc-flb)*(fub-nocc) < 0
      if flb > nocc
        if ilb > 1
          ilb = ilb - 2
          lb = ev[ilb]
          flb = sum(fermidirac.(ev,lb,Tbeta))
        else
          println("getocc: cannot find lower bound for efermi")
          break
        end
      end
      if fub < nocc
        if iub < nev
          iub = iub + 1
          ub = ev[iub]
          fub = sum(fermidirac.(ev,ub,Tbeta))
        else
          println("getocc: cannot find upper bound for efermi")
          break
        end
      end
    end
    efermi = (lb+ub)/2
    occ = fermidirac.(ev,efermi,Tbeta)
    occsum = sum(occ)
    iter = 1
    while (abs(occsum-nocc) > tol) & (iter < maxiter)
      if occsum < nocc
        lb = efermi
      else
        ub = efermi
      end
      efermi = (lb+ub)/2 
      occ = fermidirac.(ev,efermi,Tbeta)
      occsum = sum(occ)
      iter = iter + 1
    end
  elseif nev == nocc
    occ = ones(nocc)
    efermi = ev[nocc]
  else
    occ = []
    efermi = 0
    println("getocc: the number of eigenvalues in ev larger than nocc")
  end
  return occ, efermi
end

#############################################################################

function fermidirac(ev,efermi,Tbeta)
  ev = real(ev)
  dif = ev - efermi
  if dif > 0
    return exp(-Tbeta*dif)/(1+exp(-Tbeta*dif))
  end
  return 1/(1+exp(Tbeta*dif))
end

#############################################################################

function freeegy(z)
  if real(z) >= 0; return -log(1.0+exp(-z)) end
  return z-log(1.0+exp(z))
end

#############################################################################

function writedata(ran,pol,res,T,N)
  N = Int64(N)
  T = Int64(T)

###    open("JL_PoleAAA_allZ$(length(pol))N$(N)T$(T).dat","w") do io
    open("PoleData.dat","w") do io
    wpol = pol[1:2:end]
    wres = 2im*res[:,1:2:end]
    s = @sprintf("%5d %5d %25.15e\n", 3, length(wpol), ran);
    write(io,s)
    for k = 1:length(wpol)
        s = @sprintf("%25.15e %25.15e %25.15e %25.15e %25.15e %25.15e %25.15e %25.15e\n",
                  real(wpol[k]), imag(wpol[k]), 
                  real(wres[1,k]), imag(wres[1,k]),
                  real(wres[2,k]), imag(wres[2,k]), 
                  real(wres[3,k]), imag(wres[3,k]));
        write(io,s)
    end
  end
end

#############################################################################

function writedata_noT(ran,pol,res,N)
    N = Int64(N)
    
###    open("JL_PoleAAA_allZ$(length(pol))N$(N)Ran$(Int64(ran)).dat","w") do io
    open("PoleData.dat","w") do io
        wpol = pol[1:2:end]
        wres = 2im*res[:,1:2:end]
        s = @sprintf("%5d %5d %25.15e\n", 3, length(wpol), ran);
        write(io,s)
        for k = 1:length(wpol)
            s = @sprintf("%25.15e %25.15e %25.15e %25.15e %25.15e %25.15e %25.15e %25.15e\n",
                      real(wpol[k]), imag(wpol[k]), 
                      real(wres[1,k]), imag(wres[1,k]),
                      real(wres[2,k]), imag(wres[2,k]), 
                      real(wres[3,k]), imag(wres[3,k]));
            write(io,s)
        end
    end
end

#############################################################################
