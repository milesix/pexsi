function [zshift, zweight] = GenPoleContour_all( numPole, Range )
% Generate the pole expansion through a contour integral representation
% by discarding poles with small weights. This takes into account the
% weights from all three functions
%
% This is a simpler strategy than that used in PEXSI
%
% Revision: 7/21/2018 

numX = 10000;
xGrid  = linspace(-Range, Range, numX)';
fdGrid = 1./(1.0+exp(xGrid));

% Iteration. Heuristics is to only look at the density matrix part
% (zweight{1})
poleIter = 0;
numPoleInput = numPole;
while(1)
  au2K = 315774.67;
  % So beta is set to 1.
  [zshift, zweight] = getpole_all(...
    numPoleInput, au2K, 0.0, Range, 0.0, 1);
  ind = find(abs(zweight{1}) > 1e-12);
  if( length(ind) >= numPole )
    break;
  end
  numPoleInput = numPoleInput+2;
end
if( length(ind) > numPole )
  [~,ind] = sort(abs(zweight{1}),'descend');
  ind = ind(1:numPole);
end
zshift = zshift(ind);
zweight{1} = zweight{1}(ind);
zweight{2} = zweight{2}(ind);
zweight{3} = zweight{3}(ind);
