CONVERTER:
	g++ -o converter converter.cpp
clean:
	rm converter out.txt
test:
	./converter sample_input.txt out.txt
