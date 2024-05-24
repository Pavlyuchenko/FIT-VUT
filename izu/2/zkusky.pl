% u1 : len_no0

u1_1:- u1([5,-20,0,16,0,23,-4],VOUT),write(VOUT).       % 5
u1_2:- u1([0,1,2],VOUT),write(VOUT).                            % 2
u1_3:- u1([0,0,0],VOUT),write(VOUT).                            % 0
u1_r:- write('Zadej LIN: '),read(LIN),u1(LIN,VOUT),write(VOUT).

u1([], 0).
u1([0|T], N) :- u1(T, N), !.
u1([_|T], N) :- u1(T, NN), N is NN + 1, !.



% Zadání č. 13:
% Napište program řešící úkol daný predikátem u13(LIN), kde LIN je vstupní
% číselný seznam s nejméně jedním prvkem. Predikát je pravdivý (má hodnotu true),
% pokud je hodnota největšího prvku seznamu LIN větší než absolutní hodnota
% nejmenšího prvku seznamu LIN, jinak je predikát nepravdivý má hodnotu false.

% Testovací predikáty:
u13_1:- u13([22.15, 1, -23, 12, 6, -8, 2]).        % false
u13_2:- u13([22, 1.8, -23, 42.5, 6, -8, 2]).               % true
u13_3:- u13([7]).                                       % false
u13_r:- write('Zadej LIN: '),read(LIN),u13(LIN).

mymax([H], H) :- !.
mymax([H|T], B) :- mymax(T, C), C >= H, B is C, !.
mymax([H|T], B) :- B is H.

mymin([H], H) :- !.
mymin([H|T], B) :- mymin(T, C), C =< H, B is C, !.
mymin([H|T], B) :- B is H.

u13(L) :- mymax(L, M), mymin(L, N), M > abs(N), !.


% Zadání č. 22:
% Napište program řešící úkol daný predikátem u22(LIN1,LIN2,LOUT), kde LIN1
% a LIN2 jsou vstupní seznamy a LOUT je výstupní seznam vytvořený sloučením
% seznamů LIN1 a LIN2 systémem zip.

% Testovací predikáty:                              % LOUT        
u22_1:- u22([a,b,c,d,e],[1,2,3],LOUT),write(LOUT).  % [a,1,b,2,c,3,d,e]
u22_2:- u22([a,b,c],[1,2,3,4,5],LOUT),write(LOUT).  % [a,1,b,2,c,3,4,5]
u22_3:- u22([a,b,c],[1,2,3],LOUT),write(LOUT).      % [a,1,b,2,c,3]
u22_r:- write('Zadej LIN1: '),read(LIN1),
    write('Zadej LIN2: '),read(LIN2),
    u22(LIN1,LIN2,LOUT),write(LOUT).

u22([], [], []) :- !.
u22(L, [], L) :- !.
u22([], L, L) :- !.
u22([H|T], [HH|TT], [H, HH|L]) :- u22(T, TT, L), !.


% Zadání c. 31:
% Napiste program resici úkol daný predikátem u31(LIN,VOUT), kde LIN je vstupní 
% císelný seznam s nejméne jedním prvkem a VOUT je promenná, ve které se vrací 
% index prvního výskytu maximálního císla v seznamu LIN (indexování zacíná 
% jednickou). 

% Testovací predikáty:                                      % LOUT 
u31_1:- u31([5,3,-18,2,-9,-13,17,4],VOUT),write(VOUT).        % 7
u31_2:- u31([5,3.1,17,2,-9.4,-13,17,4], VOUT),write(VOUT).    % 3
u31_3:- u31([5,3.3],VOUT),write(VOUT).                % 1
u31_r:- write('Zadej LIN: '),read(LIN),
    u31(LIN,VOUT),write(VOUT).


u31([], 0) :- !.

% Zadání č. 33:
% Napište program řešící úkol daný predikátem u33(LIN,LOUT), kde LIN je vstupní 
% celočíselný seznam a LOUT je výstupní seznam obsahující všechny prvky seznamu 
% LIN, ve kterém jsou všechna sudá čísla převedena na lichá odečtením jedničky. 

% Testovací predikáty:                                     % LOUT 
u33_1:- u33([5,-3,4,8,3,1,0,-2], LOUT),write(LOUT).    % [5,-3,3,7,3,1,-1,-3]
u33_2:- u33([10,3,1,7],LOUT),write(LOUT).        % [9,3,1,7]
u33_3:- u33([],LOUT),write(LOUT).            % []
u33_r:- write('Zadej LIN: '),read(LIN),
    u33(LIN,LOUT),write(LOUT).

u33([], []) :- !.
u33([H|T], [H|L]) :- mod(H, 2) =:= 1, u33(T, L), !.
u33([H|T], [HA|L]) :- HA is H-1, u33(T, L), !.


% Zadání č. 7:
% Napište program řešící úkol daný predikátem u7(LIN,VOUT), kde LIN je vstupní 
% číselný seznam s nejméně jedním prvkem a VOUT je výstupní proměnná, ve které  
% se vrací hodnota rozdílu počtu kladných a záporných čísel v seznamu LIN (nula  
% není ani kladné, ani záporné číslo). 

% Testovací predikáty:                                                  % VOUT 
u7_1:-u7([5,27.5,-1.4,0,16,-4], VOUT),write(VOUT).                  % 1
u7_2:-u7([0],VOUT),write(VOUT).                                          % 0
u7_3:-u7([-1,-34,98.5,-2.78,-4],VOUT),write(VOUT).                 % -3
u7_r:- write('Zadej LIN: '),read(LIN),u7(LIN,VOUT),write(VOUT).

u7([], 0) :- !.
u7([H|T], R) :- H > 0, u7(T, S), R is S + 1, !.
u7([H|T], R) :- H < 0, u7(T, S), R is S - 1, !.
u7([H|T], R) :- u7(T, R), !.