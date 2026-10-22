package main

type Array struct{ items []string }

func (a *Array) push(value string) { a.items = append(a.items, value) }

func (a *Array) insert(index int, value string) bool {
	if index < 0 || index > len(a.items) {
		return false
	}
	a.items = append(a.items, "")
	copy(a.items[index+1:], a.items[index:])
	a.items[index] = value
	return true
}

func (a *Array) get(index int) (string, bool) {
	if index < 0 || index >= len(a.items) {
		return "", false
	}
	return a.items[index], true
}

func (a *Array) set(index int, value string) bool {
	if index < 0 || index >= len(a.items) {
		return false
	}
	a.items[index] = value
	return true
}

func (a *Array) remove(index int) bool {
	if index < 0 || index >= len(a.items) {
		return false
	}
	a.items = append(a.items[:index], a.items[index+1:]...)
	return true
}

func (a *Array) values() []string { return append([]string{}, a.items...) }
