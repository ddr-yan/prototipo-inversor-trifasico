clear; clc; close all;
syms t tau ie(t) Rgloff Rglon Vplt Ciss Vf Vdrv Vbe2 B1 B2 Rc Rb ie2 Qgd t_on tsw Vb Vbe3

% ---- Definir ecuación de carga del capacitor

eqn = -(Vbe3+Vb)+ (1/Ciss)*int(ie(tau), tau, 0, t)== - ie(t)*(Rc/(B2+1)+Rglon) + Vdrv-Vf-Vbe2;

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

eqn = (Vdrv-Vplt-Vbe2)*t_on== Qgd*(Rc/(B2+1)+Rglon);

% ---- Despejar ton ----

ton = solve(eqn, t_on);

% ---- Determinar Rglon ----

eqn = tsw==ton+tpo;

valorRglon = solve(eqn, Rglon);

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
Vf_val=0.3;
Vb_val=1.2;


sol_num2 = subs(valorRglon, ...
    [B2,      tsw,      Ciss,     Rc,     Vplt,     Vbe2,     Vdrv,     Qgd    , Vb, Vbe3, Vf], ...
    [B2_val,  tsw_val,  Ciss_val, Rc_val, Vplt_val, Vbe2_val, Vdrv_val  Qgd_val, Vb_val,Vbe3_val, Vf_val]);


% ---- Valor calculado para la resistencia ----
Rglon_val = double(sol_num2);


% ---- Determinar el tiempo total ----

Rglon_val = 47;
sol_num1 = subs(ton, ...
    [B2,      Ciss,     Rc,     Vplt,     Vbe2,     Vdrv,     Qgd,     Rglon,   Vb,     Vbe3, Vf ], ...
    [B2_val,  Ciss_val, Rc_val, Vplt_val, Vbe2_val, Vdrv_val  Qgd_val, Rglon_val, Vb_val, Vbe3_val, Vf_val]);

ton_val=double (sol_num1);

sol_num1 = subs(tpo, ...
    [B2,      Ciss,     Rc,     Vplt,     Vbe2,     Vdrv,        Rglon,   Vb,     Vbe3, Vf ], ...
    [B2_val,  Ciss_val, Rc_val, Vplt_val, Vbe2_val, Vdrv_val, Rglon_val, Vb_val, Vbe3_val, Vf_val]);

tpo_val=double (sol_num1);

ton_val+tpo_val;