layers(7).
width(0,8).
in_layer(0,n1).
in_layer(0,n2).
in_layer(0,n3).
in_layer(0,n4).
in_layer(0,n5).
in_layer(0,n6).
in_layer(0,n7).
in_layer(0,n8).
width(1,8).
in_layer(1,n9).
in_layer(1,n10).
in_layer(1,n11).
in_layer(1,n12).
in_layer(1,n13).
in_layer(1,n14).
in_layer(1,n15).
in_layer(1,n16).
width(2,8).
in_layer(2,n17).
in_layer(2,n18).
in_layer(2,n19).
in_layer(2,n20).
in_layer(2,n21).
in_layer(2,n22).
in_layer(2,n23).
in_layer(2,n24).
width(3,8).
in_layer(3,n25).
in_layer(3,n26).
in_layer(3,n27).
in_layer(3,n28).
in_layer(3,n29).
in_layer(3,n30).
in_layer(3,n31).
in_layer(3,n32).
width(4,8).
in_layer(4,n33).
in_layer(4,n34).
in_layer(4,n35).
in_layer(4,n36).
in_layer(4,n37).
in_layer(4,n38).
in_layer(4,n39).
in_layer(4,n40).
width(5,8).
in_layer(5,n41).
in_layer(5,n42).
in_layer(5,n43).
in_layer(5,n44).
in_layer(5,n45).
in_layer(5,n46).
in_layer(5,n47).
in_layer(5,n48).
width(6,8).
in_layer(6,n49).
in_layer(6,n50).
in_layer(6,n51).
in_layer(6,n52).
in_layer(6,n53).
in_layer(6,n54).
in_layer(6,n55).
in_layer(6,n56).
edge(n12,n22).
edge(n7,n11).
edge(n5,n16).
edge(n47,n49).
edge(n44,n51).
edge(n47,n54).
edge(n11,n22).
edge(n41,n53).
edge(n7,n14).
edge(n25,n33).
edge(n43,n55).
edge(n17,n31).
edge(n40,n41).
edge(n43,n54).
edge(n41,n54).
edge(n46,n54).
edge(n23,n28).
edge(n4,n13).
edge(n15,n24).
edge(n16,n19).
edge(n11,n24).
edge(n20,n29).
edge(n28,n33).
edge(n39,n42).
edge(n15,n19).
edge(n30,n33).
edge(n5,n11).
edge(n15,n17).
edge(n36,n41).
edge(n5,n13).
edge(n41,n52).
edge(n20,n26).
edge(n48,n56).
edge(n3,n11).
edge(n24,n30).
edge(n42,n55).
edge(n27,n37).
edge(n6,n14).
edge(n9,n21).
edge(n23,n31).
edge(n20,n32).
edge(n28,n36).
edge(n3,n16).
edge(n7,n15).
edge(n27,n38).
edge(n44,n55).
edge(n1,n14).
edge(n46,n50).
edge(n7,n10).
edge(n48,n51).
edge(n18,n25).
edge(n24,n31).
edge(n8,n13).
edge(n40,n46).
edge(n24,n26).
edge(n24,n28).
edge(n2,n10).
edge(n17,n26).
edge(n35,n42).
edge(n4,n14).
edge(n18,n27).
edge(n44,n53).
edge(n41,n56).
edge(n33,n42).
edge(n18,n29).
edge(n28,n37).
edge(n43,n53).
edge(n41,n55).
edge(n7,n12).
edge(n29,n37).
edge(n29,n39).
edge(n11,n23).
edge(n29,n33).
edge(n12,n20).
edge(n2,n14).
edge(n43,n52).

pvalue(L,1)   :- width(L,T), 0 < T.
pvalue(L,X+1) :- pvalue(L,X), width(L,T), X < T.

{ gt(L,N,P) } :- in_layer(L,N), pvalue(L,P), not width(L,P).
:- pvalue(L,P), gt(L,N,P+1), not gt(L,N,P).

position(N,1) :- in_layer(L,N), width(L,T), 0 < T, not gt(L,N,1).
position(N,Q) :- gt(L,N,P), Q = P+1, not gt(L,N,Q).
:- pvalue(L,P), 2 <= #count{ N : in_layer(L,N), position(N,P) }.

edge_layer(L1,L2,N1,N2) :- edge(N1,N2), in_layer(L1,N1), in_layer(L2,N2), L1 < L2.
edge_layer(L1,L2,N1,N2) :- edge(N2,N1), in_layer(L1,N1), in_layer(L2,N2), L1 < L2.

edge_pair(N1,N2,N3,N4) :- edge_layer(L1,L2,N1,N2), edge_layer(L1,L2,N3,N4), N1 < N3, N2 != N4.
edge_paired(N2,N4)     :- edge_pair(N1,N2,N3,N4).

compare(N1,N3) :- edge_pair(N1,N2,N3,N4).
compare(N2,N4) :- edge_paired(N2,N4), N2 < N4.
compare(N4,N2) :- edge_paired(N2,N4), N4 < N2.

inorder(N1,N2) :- compare(N1,N2), gt(L,N2,P), not gt(L,N1,P).

crossing(N1,N2,N3,N4) :- edge_pair(N1,N2,N3,N4), not edge_pair(N1,N4,N3,N2), compare(N2,N4), inorder(N1,N3), not inorder(N2,N4).
crossing(N1,N2,N3,N4) :- edge_pair(N1,N2,N3,N4), not edge_pair(N1,N4,N3,N2), compare(N2,N4), inorder(N2,N4), not inorder(N1,N3).
crossing(N1,N4,N3,N2) :- edge_pair(N1,N4,N3,N2), not edge_pair(N1,N2,N3,N4), compare(N2,N4), inorder(N1,N3), inorder(N2,N4).
crossing(N1,N4,N3,N2) :- edge_pair(N1,N4,N3,N2), not edge_pair(N1,N2,N3,N4), compare(N2,N4), not inorder(N1,N3), not inorder(N2,N4).

%:~ crossing(N1,N2,N3,N4). [1,N1,N2,N3,N4]
penalty :- crossing(N1,N2,N3,N4).
