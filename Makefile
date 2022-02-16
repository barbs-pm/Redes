build:
	gcc router.c -o router -lpthread

run: build
	./router
