package main

type StackNode struct {
	value string
	next  *StackNode
}

type Stack struct{ top *StackNode }

func (s *Stack) push(value string) { s.top = &StackNode{value: value, next: s.top} }

func (s *Stack) pop() (string, bool) {
	if s.top == nil {
		return "", false
	}
	value := s.top.value
	s.top = s.top.next
	return value, true
}

func (s *Stack) peek() (string, bool) {
	if s.top == nil {
		return "", false
	}
	return s.top.value, true
}

func (s *Stack) values() []string {
	values := []string{}
	for p := s.top; p != nil; p = p.next {
		values = append(values, p.value)
	}
	return values
}
