% Generate the pole expansion through a contour integral representation
%
% This is similar strategy as currently used in PEXSI
%
% Revision: 02/18/2017

NpoleList = [10:10:120];
RangeList = 10.^(1:6);
fout = fopen('method1.dat', 'w');
for Npole = NpoleList
  for Range = RangeList
    fprintf('Npole = %d, Range = %25.15e\n', Npole, Range);
    [zshift, zweight] = GenPoleContour( Npole, Range );

    % Contour integral method for pole expansion
    fprintf(fout, '%5d %5d %25.15e\n', 1, Npole, Range);
    for l = 1 : Npole
      fprintf(fout, '%25.15e %25.15e %25.15e %25.15e\n', ...
        real(zshift(l)), imag(zshift(l)), real(zweight(l)), ...
        imag(zweight(l)));
    end
  end
end
