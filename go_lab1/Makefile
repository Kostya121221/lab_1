.PHONY: all clean test

all: dbms

dbms: main.go database.go array.go singly_list.go doubly_list.go stack.go queue.go avl_tree.go storage.go
	go build -o dbms .

clean:
	rm -f dbms

test:
	go test ./...
