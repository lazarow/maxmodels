liczba(1..377).
indeks_bloku(1..3).

blok(L, I) :- liczba(L), indeks_bloku(I), not nie_w_bloku(L, I).
nie_w_bloku(L, I) :- liczba(L), indeks_bloku(I), not blok(L, I).

jest_w_bloku(L) :- indeks_bloku(I), blok(L, I).
:- liczba(L), not jest_w_bloku(L).
:- liczba(L), indeks_bloku(I1), indeks_bloku(I2), I1 != I2, blok(L, I1), blok(L, I2).
blok_ma_jakas_liczbe(I) :- liczba(L), blok(L, I).
:- indeks_bloku(I), not blok_ma_jakas_liczbe(I).
:- liczba(X), liczba(Y), liczba(Z), Z = X + Y, indeks_bloku(I), blok(X, I), blok(Y, I), blok(Z, I). 
