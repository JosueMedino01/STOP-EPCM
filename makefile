run: 
	g++ app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out
	time ./app.out -f ./instancias/experiments/instances/att48FSTCII_q2_g1_p1.pop -C 0.001 -K 2 -T 10 -maxNotImproviment 15000 -seed 13
	rm -f *.o app.out

bug: 
	g++ -g app.cpp iterated-local-search/IteratedLocalSearch.cpp greedy-algorithm/GreedyAlgorithm.cpp   evaluate-tour-probability/EvaluateTourProbability.cpp -o app.out