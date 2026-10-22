package main

type AVLNode struct {
	value  string
	left   *AVLNode
	right  *AVLNode
	height int
}

type AVLTree struct{ root *AVLNode }

func treeHeight(node *AVLNode) int {
	if node == nil {
		return 0
	}
	return node.height
}

func updateTreeHeight(node *AVLNode) {
	left, right := treeHeight(node.left), treeHeight(node.right)
	if left > right {
		node.height = left + 1
	} else {
		node.height = right + 1
	}
}

func balanceFactor(node *AVLNode) int { return treeHeight(node.left) - treeHeight(node.right) }

func rotateRight(root *AVLNode) *AVLNode {
	left := root.left
	root.left = left.right
	left.right = root
	updateTreeHeight(root)
	updateTreeHeight(left)
	return left
}

func rotateLeft(root *AVLNode) *AVLNode {
	right := root.right
	root.right = right.left
	right.left = root
	updateTreeHeight(root)
	updateTreeHeight(right)
	return right
}

func balanceTree(node *AVLNode) *AVLNode {
	if node == nil {
		return nil
	}
	updateTreeHeight(node)
	b := balanceFactor(node)
	if b > 1 {
		if balanceFactor(node.left) < 0 {
			node.left = rotateLeft(node.left)
		}
		return rotateRight(node)
	}
	if b < -1 {
		if balanceFactor(node.right) > 0 {
			node.right = rotateRight(node.right)
		}
		return rotateLeft(node)
	}
	return node
}

func insertTreeNode(root *AVLNode, value string) (*AVLNode, bool) {
	if root == nil {
		return &AVLNode{value: value, height: 1}, true
	}
	var added bool
	if value < root.value {
		root.left, added = insertTreeNode(root.left, value)
	} else if value > root.value {
		root.right, added = insertTreeNode(root.right, value)
	} else {
		return root, false
	}
	return balanceTree(root), added
}

func minTreeNode(node *AVLNode) *AVLNode {
	for node.left != nil {
		node = node.left
	}
	return node
}

func deleteTreeNode(root *AVLNode, value string) (*AVLNode, bool) {
	if root == nil {
		return nil, false
	}
	var removed bool
	if value < root.value {
		root.left, removed = deleteTreeNode(root.left, value)
	} else if value > root.value {
		root.right, removed = deleteTreeNode(root.right, value)
	} else {
		removed = true
		if root.left == nil {
			return root.right, true
		}
		if root.right == nil {
			return root.left, true
		}
		next := minTreeNode(root.right)
		root.value = next.value
		root.right, _ = deleteTreeNode(root.right, next.value)
	}
	return balanceTree(root), removed
}

func (tree *AVLTree) insert(value string) bool {
	var added bool
	tree.root, added = insertTreeNode(tree.root, value)
	return added
}

func (tree *AVLTree) contains(value string) bool {
	for node := tree.root; node != nil; {
		if value == node.value {
			return true
		} else if value < node.value {
			node = node.left
		} else {
			node = node.right
		}
	}
	return false
}

func (tree *AVLTree) delete(value string) bool {
	var removed bool
	tree.root, removed = deleteTreeNode(tree.root, value)
	return removed
}

func (tree *AVLTree) values() []string {
	values := []string{}
	var walk func(*AVLNode)
	walk = func(node *AVLNode) {
		if node == nil {
			return
		}
		walk(node.left)
		values = append(values, node.value)
		walk(node.right)
	}
	walk(tree.root)
	return values
}
