v(1..3).
e(1,2;2,3;3,1;1,3;3,2;2,1).
chosen(U,V) :- e(U,V), not rejected(U,V).
rejected(U,V) :- e(U,V), not chosen(U,V).
reachable(V) :- chosen(1,V), e(1,V).
reachable(V) :- reachable(U), chosen(U,V), e(U,V).
:- chosen(U,V), chosen(W,V), U!=W.
:- chosen(U,V), chosen(U,W), V!=W.
:- not reachable(V), v(V).