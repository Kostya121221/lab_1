package main

type SinglyNode struct {
	value string
	next  *SinglyNode
}

type SinglyList struct{ head *SinglyNode }

func (list *SinglyList) pushFront(value string) {
	list.head = &SinglyNode{value: value, next: list.head}
}

func (list *SinglyList) pushBack(value string) {
	node := &SinglyNode{value: value}
	if list.head == nil {
		list.head = node
		return
	}
	p := list.head
	for p.next != nil {
		p = p.next
	}
	p.next = node
}

func (list *SinglyList) find(value string) *SinglyNode {
	for p := list.head; p != nil; p = p.next {
		if p.value == value {
			return p
		}
	}
	return nil
}

func (list *SinglyList) insertBefore(target, value string) bool {
	if list.head == nil {
		return false
	}
	if list.head.value == target {
		list.pushFront(value)
		return true
	}
	for p := list.head; p.next != nil; p = p.next {
		if p.next.value == target {
			p.next = &SinglyNode{value: value, next: p.next}
			return true
		}
	}
	return false
}

func (list *SinglyList) insertAfter(target, value string) bool {
	node := list.find(target)
	if node == nil {
		return false
	}
	node.next = &SinglyNode{value: value, next: node.next}
	return true
}

func (list *SinglyList) popFront() (string, bool) {
	if list.head == nil {
		return "", false
	}
	value := list.head.value
	list.head = list.head.next
	return value, true
}

func (list *SinglyList) popBack() (string, bool) {
	if list.head == nil {
		return "", false
	}
	if list.head.next == nil {
		return list.popFront()
	}
	p := list.head
	for p.next.next != nil {
		p = p.next
	}
	value := p.next.value
	p.next = nil
	return value, true
}

func (list *SinglyList) erase(value string) bool {
	if list.head == nil {
		return false
	}
	if list.head.value == value {
		list.head = list.head.next
		return true
	}
	for p := list.head; p.next != nil; p = p.next {
		if p.next.value == value {
			p.next = p.next.next
			return true
		}
	}
	return false
}

func (list *SinglyList) eraseBefore(target string) bool {
	if list.head == nil || list.head.value == target {
		return false
	}
	if list.head.next != nil && list.head.next.value == target {
		list.head = list.head.next
		return true
	}
	for p := list.head; p.next != nil && p.next.next != nil; p = p.next {
		if p.next.next.value == target {
			p.next = p.next.next
			return true
		}
	}
	return false
}

func (list *SinglyList) eraseAfter(target string) bool {
	node := list.find(target)
	if node == nil || node.next == nil {
		return false
	}
	node.next = node.next.next
	return true
}

func (list *SinglyList) get(index int) (string, bool) {
	for i, p := 0, list.head; p != nil; i, p = i+1, p.next {
		if i == index && index >= 0 {
			return p.value, true
		}
	}
	return "", false
}

func (list *SinglyList) index(value string) int {
	for i, p := 0, list.head; p != nil; i, p = i+1, p.next {
		if p.value == value {
			return i
		}
	}
	return -1
}

func (list *SinglyList) values(reverse bool) []string {
	values := []string{}
	for p := list.head; p != nil; p = p.next {
		values = append(values, p.value)
	}
	if reverse {
		for i, j := 0, len(values)-1; i < j; i, j = i+1, j-1 {
			values[i], values[j] = values[j], values[i]
		}
	}
	return values
}
