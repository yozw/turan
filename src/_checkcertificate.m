(* Turn off storing intermediate results in Out[n] to avoid memory problems *)
$HistoryLength = 0;

(* Read Cauchy-Schwarz matrices from F.m *)
Print[];
Print["Reading problem data..."];
Get["problemdata.m"];

(* Read certificate from certificate.m *)
Print["Reading certificate..."];
Get["certificate.m"];

(* Check dual feasibility *)

(* 1. Nonnegativity of lambda *)
Print["Checking nonnegativity..."];
Do[
   If[lambda[[i]] >= 0, True,
      Print["lambda[", i, "] >= 0 is violated: value = ", lambda[[i]], "."];
      Quit[]
   ],
   {i, 1, Length[lambda]}
];

(* 2. Nonnegativity of nu *)
Do[
   If[nu[[i]] >= 0, True,
      Print["nu[", i, "] >= 0 is violated: value = ", nu[[i]], "."];
      Quit[]
   ],
   {i, 1, Length[nu]}
];

(* 3. Nonnegativity of slack variables *)
Print["Checking slack values..."];


mu = Infinity;

Do[
   mu = Min[mu, cr[[j]] + nu . B[[All, j]] - Sum[ lambda[[i]] W[[i]] . Subscript[F, M[[i]]][[j]] . W[[i]], {i, 1, Length[W]}] ],
   {j, 1, nvar}
];

alpha = mu - nu.b;
z = alpha * 16 / (k*(k-1)*n*(n-1));

(* The dual solution is feasible *)   
Print[]; Print["Dual feasible solution with mu = ", N[mu], " has objective ", alpha, "( >= ", N[Floor[alpha*10^5]/10^5], ")"];
Print["This correponds to ", N[Floor[z*10^5]/10^5], " times the Zarankiewicz bound"];
