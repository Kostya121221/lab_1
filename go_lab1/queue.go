package main

type QueueNode struct {
	value string
	next  *QueueNode
}

type Queue struct {
	head *QueueNode
	tail *QueueNode
}

func (q *Queue) push(value string) {
	node := &QueueNode{value: value}
	if q.head == nil {
		q.head, q.tail = node, node
	} else {
		q.tail.next = node
		q.tail = node
	}
}

func (q *Queue) pop() (string, bool) {
	if q.head == nil {
		return "", false
	}
	value := q.head.value
	q.head = q.head.next
	if q.head == nil {
		q.tail = nil
	}
	return value, true
}

func (q *Queue) peek() (string, bool) {
	if q.head == nil {
		return "", false
	}
	return q.head.value, true
}

func (q *Queue) values() []string {
	values := []string{}
	for p := q.head; p != nil; p = p.next {
		values = append(values, p.value)
	}
	return values
}
