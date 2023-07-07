function [zshift, zweight] = GenPoleContour( numPole, Range )
% Generate the pole expansion through a contour integral representation
% by discarding poles with small weights
%
% This is a simpler strategy than that used in PEXSI
%
% Revision: 02/18/2017

numX = 10000;
xGrid  = linspace(-Range, Range, numX)';
fdGrid = 1./(1.0+exp(xGrid));

% Iteration
poleIter = 0;
numPoleInput = numPole;
while(1)
  au2K = 315774.67;
  % So beta is set to 1.
  [zshift, zweight] = getpole(...
    numPoleInput, au2K, 0.0, Range, 0.0, 1);
  ind = find(abs(zweight) > 1e-12);
  if( length(ind) >= numPole )
    break;
  end
  numPoleInput = numPoleInput+2;
end
if( length(ind) > numPole )
  [~,ind] = sort(abs(zweight),'descend');
  ind = ind(1:numPole);
end
zshift = zshift(ind);
zweight = zweight(ind);
