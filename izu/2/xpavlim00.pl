% Zadani c. 8:
% Napiste program resici ukol dany predikatem u8(LIN,VIN), kde LIN je vstupni 
% ciselny seznam s nejmene dvema prvky a VIN je vstupni promenna, jejiz 
% hodnotou musi byt kladne cislo. Predikat je pravdivy (ma hodnotu true), pokud 
% absolutni hodnoty rozdilu mezi vsemi sousednimi cisly v seznamu LIN jsou mensi
% nez cislo v promenne VIN, jinak je predikat nepravdivy (ma hodnotu false). 

% Testovaci predikaty: 	                               	  
u8_1:- u8([5,6.2,4,2.1,-1,1],4).                         %  true
u8_2:- u8([5,-6.2,4,2,-1,1],3).                          %  false
u8_3:- u8([5.1,6,4,2,-1,1.2],2).                         %  false
u8_r:- write('Zadej LIN: '),read(LIN),
       write('Zadej VIN: '),read(VIN),
       u8(LIN,VIN).

% Reseni:
u8(LIN,VIN) :-
    recursive_check(LIN, VIN).

recursive_check([_], _).                                % Base case - one element ?array? 
recursive_check([H1, H2|T], VIN) :-                     % Take first and second element
    (H1 - H2 < 0 -> Diff is H2 - H1 ; Diff is H1 - H2), % Calculate absolute difference, without the abs() function
    Diff < VIN,                                         % if difference is smaller than VIN, continue, else false
    recursive_check([H2|T], VIN).
