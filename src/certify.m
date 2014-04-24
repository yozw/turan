k = length(wineq);

%k= 17;

Ad = zeros(k, nvar);
An = zeros(k, nvar);
y = zeros(1, k);

f = fopen('certify_mathematica.m', 'w');

% ---- write dual solution
fprintf(f, 'y = {');
for i = 1:k
    % -- rational approximation
    [wn, wd] = rat(wineq(i).dual);
    %wd = 10000;

    % -- write to file 
    if i > 1
        fprintf(f, ',');
    end
    
    % -- note that we are taking -|y| to make sure the dual
    %    variables are negative; some versions of cplex seem to give
    %    positive dual variables??
    fprintf(f, '%d/%d', -abs(wn), abs(wd));
end
fprintf(f, '};\n');

% ---- write objective coefficients
fprintf(f, 'cr = {');
for i = 1:nvar
    if i > 1
        fprintf(f, ',');
    end
    fprintf(f, '{%d}', cr(i));
end
fprintf(f, '};\n');


fprintf(f, 'toZar[z_] := 16 * z / (%d * %d * %d * %d);\n', N, N-1, K, K-1);

% ---- store Mathematica representation of the active constraints
fprintf(f, 'z = 100;\n');
for i = 1:nvar
    
    % -- construct rational expression for the constraint
    fprintf(f, 'z = Min[z, cr[[%d]]', i);
    for j = 1:k
        % -- use rational approximation of eigenvector
        [wn, wd] = rat(wineq(j).weight);
        
        expr = eval(['CSineq' num2str(wineq(j).matrix) '(wn, wd, i)']);
        fprintf(f, '+y[[%d]]*(%s)', j, expr);
    end
    fprintf(f, ']; SetPrecision[N[toZar[z]],20]\n');
end




% ---- write code to print certified bound
fprintf(f, 'N[Floor[10^6 * 16 * z / (%d * %d * %d * %d)]/10^6]\n', N, N-1, K, K-1);
fclose(f);


fprintf('Certifying Mathematica calculations have been written to certify_mathematica.m.\n');
fprintf('Run: "MathKernel < certify_mathematica.m" to check calculations and determine proved bound.\n');
