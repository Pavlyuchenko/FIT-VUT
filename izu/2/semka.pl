even_odd([], []) :- !.
even_odd([H|T], L) :- even(T, L).

even([], []) :- !.
even([H|T], [H|L]) :- odd(T, L).

odd([], []) :- !.
odd([H|T], L) :- even(T, L).
