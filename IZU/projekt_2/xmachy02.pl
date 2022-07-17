% Zadání è. 31:
% Napište program øešící úkol daný predikátem u31(LIN,VOUT), kde LIN je vstupní 
% èíselný seznam s nejménì jedním prvkem a VOUT je promìnná, ve které se vrací 
% index prvního výskytu maximálního èísla v seznamu LIN (indexování zaèíná 
% jednièkou). 

% Testovací predikáty:       
u31_1:- u31([5,3,-18,2,-9,-13,17,4],VOUT),write(VOUT).		% 7
u31_2:- u31([5,3.1,17,2,-9.4,-13,17,4], VOUT),write(VOUT).	% 3
u31_3:- u31([5,3.3],VOUT),write(VOUT).				        % 1
u31_r:- write('Zadej LIN: '),read(LIN),
	u31(LIN,VOUT),write(VOUT).

%u31_4:- u31([1,2,3,5,4],VOUT),write(VOUT).				    % 4
%u31_5:- u31([5,4,3,2,1],VOUT),write(VOUT).				    % 1
%u31_6:- u31([1,2,3,5,4,7],VOUT),write(VOUT).				% 6

u31(LIN, VOUT) :- 
    getMax(LIN, Num),
    getFirst(LIN, Num, VOUT).

getFirst([H|_], Num, Index) :-
    H is Num,
    Index is 1.
getFirst([_|T], Num, Index) :-
    getFirst(T, Num, I),
    Index is I + 1.

getMax([H], H).
getMax([], false). % Empty list!

getMax([H1,H2|T], Max) :- 
    H1 >= H2,
    getMax([H1|T], Max).

getMax([H1,H2|T], Max) :- 
    H1 < H2,
    getMax([H2|T], Max).
