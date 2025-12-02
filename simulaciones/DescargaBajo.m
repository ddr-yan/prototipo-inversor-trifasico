clear; clc; close all;
syms t tau ie(t) Rgloff_eq Vplt Ciss Vdrv Vbe2 Vbe3 B1 B2 Rc Rb ie2 Qgd t_off tsw Vb

% ---- Definir ecuación de descarga del capacitor

eqn = (Vdrv-Vbe2)-(1/Ciss)*int(ie(tau), tau, 0, t) == + ie(t)*(Rgloff_eq) + Vbe3+Vb;

% ---- Pasar a ecuación diferencial: derivar LHS y RHS por separado ----

odeEq = diff(lhs(eqn), t) == diff(rhs(eqn), t);
odeEq = simplify(odeEq);

% ---- Condición inicial i_e(0) ----

eqn0 = subs(eqn, t, 0);  

% Crear un símbolo para ie(0) y sustituirlo

syms ie0;
eqn0_ie0 = subs(eqn0, ie(0), ie0);

% Resolver simbólicamente para ie0

ie0_sol = solve(eqn0_ie0, ie0);
ie0_sol = simplify(ie0_sol);

% ---- Condición inicial para dsolve ----

icEq = ie(0) == ie0_sol;

% ---- Resolver la ODE ----

ie_sol = dsolve(odeEq, icEq);
ie_sol = simplify(ie_sol);

% ---- Definir el voltaje en el capacitor ----

ie_tau = subs(ie_sol, t, tau);
Vcharge = 1/Ciss * int(ie_tau, tau, 0, t);

% ---- Despejar tpo ----

tpo = solve(Vcharge == Vplt, t);

% ---- Definir ecuación de carga de plateau

eqn = (Vplt-Vbe3-Vb)*t_off == Qgd*(Rgloff_eq);

% ---- Despejar ton ----

ton = solve(eqn, t_off);

% ---- Determinar Rgloff_eq ----

eqn = tsw==ton+tpo;

valorRgloff_eq = solve(eqn, Rgloff_eq);

% ---- Evaluar las ecuaciones ----

% Valores utilizados
B2_val    = 230;
tsw_val  = 200e-9;
Ciss_val  = 1625e-12;
Rc_val    = 470;
Vplt_val  = 3.6;
Vbe2_val  = 0.7;
Vbe3_val  = 0.7;
Vdrv_val  = 12;
Qgd_val=23e-9;
Vb_val=1.2;


sol_num2 = subs(valorRgloff_eq, ...
    [B2,      tsw,      Ciss,     Rc,     Vplt,     Vbe3, Vbe2,     Vdrv,     Qgd     , Vb], ...
    [B2_val,  tsw_val,  Ciss_val, Rc_val, Vplt_val, Vbe3_val, Vbe2_val, Vdrv_val  Qgd_val,  Vb_val]);



% ---- Valor calculado para la resistencia ----
Rgloff_eq_val = double(sol_num2);

Rglon_val = 47;

Rgloff=-Rgloff_eq_val*Rglon_val/(Rgloff_eq_val-Rglon_val);


% ---- Determinar el tiempo total ----

Rgloff_eq_val = 20*47/(20+47);

sol_num1 = subs(ton, ...
    [B2,       Ciss,     Rc,     Vplt,     Vbe3, Vbe2,    Vdrv,     Qgd,     Rgloff_eq,     Vb], ...
    [B2_val,    Ciss_val, Rc_val, Vplt_val, Vbe3_val,Vbe2_val, Vdrv_val  Qgd_val, Rgloff_eq_val ,Vb_val]);

ton_val=double (sol_num1);

sol_num1 = subs(tpo, ...
    [B2,         Ciss,     Rc,     Vplt,     Vbe3, Vbe2,    Vdrv,       Rgloff_eq,     Vb], ...
    [B2_val,  Ciss_val, Rc_val, Vplt_val, Vbe3_val,Vbe2_val, Vdrv_val, Rgloff_eq_val ,Vb_val]);

tpo_val=double (sol_num1);

ton_val+tpo_val;



