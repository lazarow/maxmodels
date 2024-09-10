v(0..2).

e(0, 1, 3000).
e(0, 2, 2000).
e(1, 2, 1000).

1 { v_in(X) : v(X) }.
e_out(X, Y, W) :- e(X, Y, W), v_in(X), v_in(Y).
e_out(X, Y, W) :- e(X, Y, W), not v_in(Y), not v_in(X).
:~ e_out(X, Y, W). [W, X, Y]
