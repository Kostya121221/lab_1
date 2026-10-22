package main

import "strings"

type Database struct {
	arrays      map[string]*Array
	singlyLists map[string]*SinglyList
	doublyLists map[string]*DoublyList
	stacks      map[string]*Stack
	queues      map[string]*Queue
	trees       map[string]*AVLTree
}

func newDatabase() *Database {
	return &Database{
		arrays: map[string]*Array{}, singlyLists: map[string]*SinglyList{},
		doublyLists: map[string]*DoublyList{}, stacks: map[string]*Stack{},
		queues: map[string]*Queue{}, trees: map[string]*AVLTree{},
	}
}

func (db *Database) typeOf(name string) string {
	switch {
	case db.arrays[name] != nil:
		return "array"
	case db.singlyLists[name] != nil:
		return "flist"
	case db.doublyLists[name] != nil:
		return "dlist"
	case db.stacks[name] != nil:
		return "stack"
	case db.queues[name] != nil:
		return "queue"
	case db.trees[name] != nil:
		return "tree"
	}
	return ""
}

func codeType(code string) string {
	switch strings.ToUpper(code) {
	case "M":
		return "array"
	case "F":
		return "flist"
	case "L":
		return "dlist"
	case "S":
		return "stack"
	case "Q":
		return "queue"
	case "T":
		return "tree"
	}
	return ""
}

func typeCode(kind string) string {
	switch kind {
	case "array":
		return "M"
	case "flist":
		return "F"
	case "dlist":
		return "L"
	case "stack":
		return "S"
	case "queue":
		return "Q"
	case "tree":
		return "T"
	}
	return ""
}

func (db *Database) create(code, name string) bool {
	kind := codeType(code)
	if kind == "" || name == "" || db.typeOf(name) != "" {
		return false
	}
	switch kind {
	case "array":
		db.arrays[name] = &Array{}
	case "flist":
		db.singlyLists[name] = &SinglyList{}
	case "dlist":
		db.doublyLists[name] = &DoublyList{}
	case "stack":
		db.stacks[name] = &Stack{}
	case "queue":
		db.queues[name] = &Queue{}
	case "tree":
		db.trees[name] = &AVLTree{}
	}
	return true
}

func (db *Database) drop(code, name string) bool {
	if db.typeOf(name) != codeType(code) || db.typeOf(name) == "" {
		return false
	}
	delete(db.arrays, name)
	delete(db.singlyLists, name)
	delete(db.doublyLists, name)
	delete(db.stacks, name)
	delete(db.queues, name)
	delete(db.trees, name)
	return true
}
