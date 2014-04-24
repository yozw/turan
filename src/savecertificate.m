function savecertificate(wineq)

f = fopen('certificate.m', 'w');

fprintf('Saving certificate ....     ');

% ---- write matrix number of CS-constraints
writeVector(f, 'M', [wineq.matrix]);

b = [wineq.rhs];
bd = b * 0;
bn = b * 0;
for i = 1:length(b)
        % -- approximate right hand side at extreme high precision and
        %    making sure that we are rounding up (weakening the inequality).
        rhs = b(i);
        [wn, wd] = rat(rhs, 1e-10);
        while (wn/wd < b(i))
            rhs = rhs + (rhs - wn/wd);
            [wn, wd] = rat(rhs, 1e-10);
        end
        bd(i) = wd;
        bn(i) = wn;
end

writeVector(f, 'b', bn, bd);

% ---- write Cauchy-Schwarz weight vectors
fprintf(f, 'W = {');
nrows = 0;
lambdan = [];
lambdad = [];
for i = 1:length(wineq)
     if (~isempty(wineq(i).matrix))
         % -- use rational approximation of eigenvector
         if nrows > 0
             fprintf(f, ',\n');
         end
         [wn, wd] = rat(wineq(i).weight, 1e-6);
         writeVector(f, '', wn, wd);

         [wn, wd] = rat(abs(wineq(i).dual), 1e-6);
         lambdan = [lambdan; wn];
         lambdad = [lambdad; wd];

         nrows = nrows + 1;
     end    
end
fprintf(f, '}\n');

writeVector(f, 'lambda', lambdan, lambdad);

% ---- write manually inserted inequalities
fprintf(f, 'B = {');
nrows = 0;
nud = [];
nun = [];
for i = 1:length(wineq)
     if (isempty(wineq(i).matrix))
         for j = 1:length(wineq(i).constraint)
            % -- check that the coefficients are integers so
            %    that there is no round-off error
            if (rem(wineq(i).constraint(j), 1) ~= 0)
                error('Non-Cauchy-Schwarz constraint encountered with non-integral coefficients');
            end
         end
        
         if nrows > 0
             fprintf(f, ',\n');
         end
         
         [wn, wd] = rat(abs(wineq(i).dual), 1e-6);
         nun = [nun; wn];
         nud = [nud; wd];
         
         writeVector(f, '', wineq(i).constraint);
         nrows = nrows + 1;
     end    
end
fprintf(f, '};\n');

writeVector(f, 'nu', nun, nud);

fclose(f);


fprintf('Certificate has been written to certificate.m.\n');
fprintf('Run: "math < check_certificate.m" to check calculations and determine proved bound.\n');

end




function writeVector(f, name, vector_wn, vector_wd)

if (length(name) > 0)
    fprintf(f, '%s = ', name);
end

fprintf(f, '{');
for i = 1:length(vector_wn) 
    if i ~= 1
        fprintf(f, ',');
    end
    if (nargin == 3)
        fprintf(f, '%d', vector_wn(i));
    else
        fprintf_rational(f, vector_wn(i), vector_wd(i));
    end
end
fprintf(f, '}');

if (length(name) > 0)
    fprintf(f, ';\n');
end

end




function fprintf_rational(f, num, denom)

    if denom == 1
        fprintf(f, '%d', num);
    else
        fprintf(f, '%d/%d', num, denom);
    end
end