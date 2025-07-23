matrix:
	g++ -c src/matrix.cpp -o obj/matrix.o
lp:
	g++ -c src/lp.cpp -o obj/lp.o
ip:
	g++ -c src/ip.cpp -o obj/ip.o
model_reader:
	g++ -c src/model_reader.cpp -o obj/model_reader.o
constraint:
	g++ -c src/constraint.cpp -o obj/constraint.o
main:
	g++ -c src/main.cpp -o obj/main.o
build: matrix lp constraint model_reader ip main
	g++ -o ./bin/main obj/main.o obj/matrix.o obj/lp.o obj/constraint.o obj/model_reader.o obj/ip.o
run: build
	clear
	./bin/main
clean:
	rm -f obj/* ./bin/main
	mkdir -p obj