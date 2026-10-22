package main

import (
	"encoding/json"
	"fmt"
	"os"
)

type savedContainer struct {
	Type string   `json:"type"`
	Data []string `json:"data"`
}

func saveDatabase(filename string, db *Database) error {
	data := map[string]savedContainer{}
	for name, a := range db.arrays {
		data[name] = savedContainer{"array", a.values()}
	}
	for name, list := range db.singlyLists {
		data[name] = savedContainer{"flist", list.values(false)}
	}
	for name, list := range db.doublyLists {
		data[name] = savedContainer{"dlist", list.values(false)}
	}
	for name, stack := range db.stacks {
		data[name] = savedContainer{"stack", stack.values()}
	}
	for name, queue := range db.queues {
		data[name] = savedContainer{"queue", queue.values()}
	}
	for name, tree := range db.trees {
		data[name] = savedContainer{"tree", tree.values()}
	}
	text, err := json.MarshalIndent(data, "", "  ")
	if err != nil {
		return err
	}
	return os.WriteFile(filename, append(text, '\n'), 0644)
}

func loadDatabase(filename string) (*Database, error) {
	text, err := os.ReadFile(filename)
	if os.IsNotExist(err) {
		return newDatabase(), nil
	}
	if err != nil {
		return nil, err
	}
	data := map[string]savedContainer{}
	if len(text) > 0 {
		if err := json.Unmarshal(text, &data); err != nil {
			return nil, err
		}
	}
	db := newDatabase()
	for name, saved := range data {
		if !db.create(typeCode(saved.Type), name) {
			return nil, fmt.Errorf("не удалось загрузить контейнер %s", name)
		}
		switch saved.Type {
		case "array":
			for _, value := range saved.Data {
				db.arrays[name].push(value)
			}
		case "flist":
			for _, value := range saved.Data {
				db.singlyLists[name].pushBack(value)
			}
		case "dlist":
			for _, value := range saved.Data {
				db.doublyLists[name].pushBack(value)
			}
		case "stack":
			for i := len(saved.Data) - 1; i >= 0; i-- {
				db.stacks[name].push(saved.Data[i])
			}
		case "queue":
			for _, value := range saved.Data {
				db.queues[name].push(value)
			}
		case "tree":
			for _, value := range saved.Data {
				db.trees[name].insert(value)
			}
		default:
			return nil, fmt.Errorf("неизвестный тип контейнера %s", saved.Type)
		}
	}
	return db, nil
}

func saveOK(filename string, db *Database) {
	if err := saveDatabase(filename, db); err != nil {
		fmt.Println("Ошибка сохранения:", err)
	} else {
		fmt.Println("OK")
	}
}
