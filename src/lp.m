% --- Get problem parameters
[N, K, nvar] = parameters;
cr = crossings;

% --- Set up inequalities. (Initially empty)
Aineq = zeros(0, nvar);
bineq = zeros(0, 1);
wineq = struct('matrix', {}, 'weight', {}, 'dual', {}, 'constraint', {}, 'rhs', {}, 'id', {});
lb    = zeros(nvar, 1);
ub    = ones(nvar, 1);
alpha = 0;


% --- Load known inequalities
if ( (N==3) && (K==4))
    fprintf('Loading inequalities from inequalities_3x4.mat\n');
    load inequalities_3x4_905.mat
    
    for i = length(wineq):-1:1
        if (wineq(i).matrix < 1)
            wineq(i) = [];
            Aineq(i,:) = [];
            bineq(i) = [];
        end            
    end

    % --- Add Sergey's additional inequalities
    a = zeros(1, 6393);
    a([4857 4858 4859]) = -2;
    a([4860 4861]) = 6;
    a([4882 4883 4903]) = -1;
    Aineq = [Aineq; a];
    bineq = [bineq; 0];
    t = length(wineq);
    wineq(t+1).id = 1; 
    wineq(t+1).constraint = a;
    wineq(t+1).rhs = 0;
  
%     load('sergey_ineq2.mat');
%     Aineq = [Aineq; sergey_ineq2];
%     bineq = [bineq; 0];
%     t = length(wineq);
%     wineq(t+1).matrix = 0; wineq(t+1).weight = [];
      
%    ineqSq = textread('ineqSquares.txt');
%    cr = zeros(1, 6393);
%    cr(ineqSq(:, 1)) = ineqSq(:,2);

    ineqSq = textread('ineqSquares.txt');

    alpha = 0.90503;
    
    a = zeros(1, 6393);
    a(ineqSq(:, 1)) = ineqSq(:, 2);
    b = (9/8)*alpha^2;
    Aineq = [Aineq; a];
    bineq = [bineq; b];
    t = length(wineq);
    wineq(t+1).id = 2; 
    wineq(t+1).constraint = a;
    wineq(t+1).rhs = b;
end

% --- Set up equalities. (The sum of all variables should be one.)
Aeq   = ones(1, nvar);
beq   = 1;

% --- Iteration counter
iter  = 1;

CS_MATRIX_COUNT = 4;
EIGCOUNT = ones(CS_MATRIX_COUNT) * 20;

fprintf('Solving problem for N=%d, K=%d\n', N, K);

stopAfterNextCplex = 1;

x = zeros(nvar,1);

zgraph = [];

while (true)
    % -- Solve current LP
    [sol, obj, exitflag, output, lambda] = cplexlp(cr', Aineq, bineq, Aeq, beq, lb, ub);
    if (exitflag ~= 1)
        error('Cplex terminated with the message: %s', output.message);
    end
    
    for i=1:length(wineq)
        wineq(i).dual = lambda.ineqlin(i);
    end
    
    % -- Remove redundant inequalities
    if (size(Aineq,1) > 1)    
        Aineq = Aineq(lambda.ineqlin ~= 0, :);
        bineq = bineq(lambda.ineqlin ~= 0, :);
        wineq = wineq(lambda.ineqlin ~= 0);
        lambda.ineqlin = lambda.ineqlin(lambda.ineqlin ~= 0);
    end
    
    z = 16*obj/(N*(N-1)*K*(K-1));
    zgraph = [zgraph; z];
    plot(zgraph(max(1,length(zgraph)-20):length(zgraph)));
    yt=get(gca,'YTick');
    ylab=num2str(yt(:), '%11.8f');
    set(gca,'YTickLabel',ylab);
    drawnow;
    
    %z = sqrt((8/9) * obj);
    fprintf('%3d %6d %10.7f %10.7f (alpha=%10.7f) (|diff|_inf=%10.7f)| ', iter, size(Aineq, 1), obj, z, alpha, max(abs(x-sol)));
    
    
    % -- Write solutions to file
    f = fopen('solution.txt', 'w');
    for i = 1:nvar
        if (sol(i) > 1e-8)
            fprintf(f, 'x(%d) = %0.7f\n', i, sol(i));
        end
    end
    fclose(f);
    
    % -- Only store solution if it is feasible
    x = sol;
    
    if (stopAfterNextCplex == 1)
        fprintf('Sufficiently close to optimal solution\n');
        break;
    end
    
    for i = 1:length(wineq)
        if (wineq(i).matrix == -1)
            q = 1.000001 *16*obj/(N*(N-1)*K*(K-1)); 
            bineq(i) = (18/16)*q^2;
            %fprintf('\nSetting right hand side of Sergey''s inequality to correspond to %f\n', q);
            alpha=q;
        end
    end

    Dstar = zeros(1, CS_MATRIX_COUNT);
    
    % -- Check if SDP constraints are satisfied
    for m = 1:CS_MATRIX_COUNT
        Z         = eval(['CSmatrix' num2str(m) '(x)']);
        
        % -- Calculate eigenvalues; at most EIGCOUNT(m),
        %    but if no negative eigenvalues are found, EIGCOUNT(m)
        %    is gradually increased.
        if (EIGCOUNT(m) < size(Z, 1))
            D = 0;
            while (true)
                if (EIGCOUNT(m) >= size(Z, 1))
                    [V, D] = eig(Z);
                    break;
                end
                [V, D] = eigs(sparse(Z), EIGCOUNT(m));
                if min(diag(D)) < 0 
                    break
                end
                EIGCOUNT(m) = EIGCOUNT(m) * 2;
            end
        else
            [V, D] = eig(Z);
        end
        Dstar(m)  = min(diag(D));
    
        % -- Add new Cauchy-Schwarz inequalities
        Mset      = (1:size(D, 1))';
        q         = diag(D);
        addSet    = sort([q(q<0) Mset(q<0)]);
        ineqCnt   = 0;
        for i = 1:size(addSet, 1)
            w = V(:, addSet(i, 2));
            new_ineq = eval(['CSineq' num2str(m) '(w)']);
            ineqCurValue = new_ineq * x;
            if (ineqCurValue < 1e-6)
                %fprintf('Adding CS constraint corresponding to eigenvalue %f; current value = %f\n', addSet(i, 1), ineqCurValue);
                Aineq = [Aineq; -new_ineq];
                bineq = [bineq; 0];
                t = length(wineq)+1;
                wineq(t).matrix = m;
                wineq(t).weight = w;
            end
        end
    end
    
    for m = 1:CS_MATRIX_COUNT
        fprintf('%10.7f ', Dstar(m));
    end
    fprintf('\n');

    if (min(Dstar) > -1e-6)
        stopAfterNextCplex = 1;
    end
    
    iter = iter + 1;
end
