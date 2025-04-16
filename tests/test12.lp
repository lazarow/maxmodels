v(0..4).
in(X) :- v(X), not not_in(X).
not_in(X) :- v(X), not in(X).
:- in(2).
:~ in(X). [1, X]
