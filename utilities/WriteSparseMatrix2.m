function nzval = WriteSparseMatrix2(A, filename, colptr, rowind)
% Write a sparse symmetric matrix A in compressed column storage, for
% given colptr and rowind

disp('Computing the compressed column storage...');
tic
N = length(A);	
nnzAlower = length(rowind);
nzval = zeros(nnzAlower, 1);
cnt = 1;
for i = 1 : nnzAlower
	if( i == colptr( cnt+ 1 ) )
		cnt = cnt + 1;
	end
	nzval(i) = A(rowind(i), cnt);
end
toc

if(0)
	disp('Writing the matrix to file (text format)...');
	tic
		fid = fopen(filename,'w');
		fprintf(fid, '%d %d ', [N, nnzAlower]);
		fprintf(fid, '\n');
		fprintf(fid, '%d ', colptr);
		fprintf(fid, '\n');
		fprintf(fid, '%d ', rowind);
		fprintf(fid, '\n');
		fprintf(fid, '%g ', nzval);
		fclose(fid);
	toc
else
	disp('Writing the matrix to file (binary format)...');
	tic
		fid = fopen(filename,'wb');
		serialize(fid, N, {'int'} );
		serialize(fid, nnzAlower, {'int'});
		serialize(fid, colptr, {'IntNumVec'});
		serialize(fid, rowind, {'IntNumVec'});
		serialize(fid, nzval,  {'DblNumVec'});
		fclose(fid);
	toc
end
