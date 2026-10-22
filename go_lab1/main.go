package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {
	filename := flag.String("file", "file.json", "")
	query := flag.String("query", "", "")
	flag.Parse()

	db, err := loadDatabase(*filename)
	if err != nil {
		fmt.Println("Ошибка загрузки:", err)
		return
	}
	if _, err := os.Stat(*filename); os.IsNotExist(err) {
		if err := saveDatabase(*filename, db); err != nil {
			fmt.Println("Ошибка сохранения:", err)
			return
		}
	}
	if *query != "" {
		run(*query, db, *filename)
		return
	}

	scanner := bufio.NewScanner(os.Stdin)
	fmt.Println("HELP — команды, EXIT — выход")
	for {
		fmt.Print("> ")
		if !scanner.Scan() || !run(scanner.Text(), db, *filename) {
			break
		}
	}
}

func run(line string, db *Database, filename string) bool {
	words := strings.Fields(line)
	if len(words) == 0 {
		return true
	}
	command := strings.ToUpper(words[0])
	args := words[1:]

	switch command {
	case "HELP", "?":
		showHelp()
		return true
	case "EXIT", "QUIT":
		return false
	case "SAVE":
		if len(args) != 0 {
			fmt.Println("Использование: SAVE")
		} else {
			saveOK(filename, db)
		}
		return true
	case "LOAD":
		if len(args) != 0 {
			fmt.Println("Использование: LOAD")
			return true
		}
		loaded, err := loadDatabase(filename)
		if err != nil {
			fmt.Println("Ошибка загрузки:", err)
		} else {
			*db = *loaded
			fmt.Println("OK")
		}
		return true
	case "CREATE":
		if len(args) != 2 {
			fmt.Println("Использование: CREATE <M|F|L|S|Q|T> <имя>")
		} else if db.create(args[0], args[1]) {
			saveOK(filename, db)
		} else {
			fmt.Println("Не удалось создать контейнер")
		}
		return true
	case "MCREATE", "FCREATE", "LCREATE", "SCREATE", "QCREATE", "TCREATE":
		if len(args) != 1 {
			fmt.Println("Укажи имя контейнера")
		} else if db.create(command[:1], args[0]) {
			saveOK(filename, db)
		} else {
			fmt.Println("Не удалось создать контейнер")
		}
		return true
	case "DROP":
		if len(args) != 2 {
			fmt.Println("Использование: DROP <M|F|L|S|Q|T> <имя>")
		} else if db.drop(args[0], args[1]) {
			saveOK(filename, db)
		} else {
			fmt.Println("Контейнер не найден или тип указан неверно")
		}
		return true
	case "PRINT":
		if len(args) == 1 {
			printNamed(db, args[0])
		} else if len(args) == 2 {
			if db.typeOf(args[1]) != codeType(args[0]) || codeType(args[0]) == "" {
				fmt.Println("Контейнер не найден или тип указан неверно")
			} else {
				printNamed(db, args[1])
			}
		} else {
			fmt.Println("Использование: PRINT <имя> или PRINT <тип> <имя>")
		}
		return true
	}

	kind := commandType(command)
	if kind == "" {
		fmt.Println("Неизвестная команда")
		return true
	}
	if len(args) == 0 {
		fmt.Println("Укажи имя контейнера")
		return true
	}
	name := args[0]
	if db.typeOf(name) == "" {
		fmt.Println("Контейнер не найден. Сначала используй CREATE")
		return true
	}
	if db.typeOf(name) != kind {
		fmt.Println(name, "это не", kind)
		return true
	}

	changed := false
	values := args[1:]
	switch kind {
	case "array":
		changed = handleArray(command, db.arrays[name], values)
	case "flist":
		changed = handleSingly(command, db.singlyLists[name], values)
	case "dlist":
		changed = handleDoubly(command, db.doublyLists[name], values)
	case "stack":
		changed = handleStack(command, db.stacks[name], values)
	case "queue":
		changed = handleQueue(command, db.queues[name], values)
	case "tree":
		changed = handleTree(command, db.trees[name], values)
	}
	if changed {
		if err := saveDatabase(filename, db); err != nil {
			fmt.Println("Ошибка сохранения:", err)
		}
	}
	return true
}

func commandType(command string) string {
	switch command {
	case "MPUSH", "MINSERT", "MGET", "MSET", "MDEL", "MLEN", "MREAD":
		return "array"
	case "FPUSH", "FPUSHFRONT", "FINSERTBEFORE", "FINSERTAFTER", "FDEL", "FDELHEAD", "FDELTAIL", "FDELBEFORE", "FDELAFTER", "FGET", "FREAD", "FREADREVERSE", "FSEARCH":
		return "flist"
	case "LPUSH", "LPUSHFRONT", "LINSERTBEFORE", "LINSERTAFTER", "LDEL", "LDELHEAD", "LDELTAIL", "LDELBEFORE", "LDELAFTER", "LGET", "LREAD", "LREADREVERSE", "LSEARCH":
		return "dlist"
	case "SPUSH", "SPOP", "SGET", "SREAD":
		return "stack"
	case "QPUSH", "QPOP", "QGET", "QREAD":
		return "queue"
	case "TINSERT", "TSEARCH", "ISMEMBER", "TDEL", "TGET", "TREAD":
		return "tree"
	}
	return ""
}

func need(args []string, count int, usage string) bool {
	if len(args) != count {
		fmt.Println("Использование:", usage)
		return false
	}
	return true
}

func readIndex(text string) (int, bool) {
	index, err := strconv.Atoi(text)
	if err != nil || index < 0 {
		fmt.Println("Индекс должен быть неотрицательным целым числом")
		return 0, false
	}
	return index, true
}

func handleArray(command string, array *Array, args []string) bool {
	switch command {
	case "MPUSH":
		if !need(args, 1, "MPUSH <имя> <значение>") {
			return false
		}
		array.push(args[0])
		fmt.Println("->", args[0])
		return true
	case "MINSERT":
		if !need(args, 2, "MINSERT <имя> <индекс> <значение>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		if !array.insert(index, args[1]) {
			fmt.Println("Неверный индекс")
			return false
		}
		fmt.Println("->", args[1])
		return true
	case "MGET":
		if !need(args, 1, "MGET <имя> <индекс>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		value, found := array.get(index)
		if found {
			fmt.Println("->", value)
		} else {
			fmt.Println("Неверный индекс")
		}
	case "MSET":
		if !need(args, 2, "MSET <имя> <индекс> <значение>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		if !array.set(index, args[1]) {
			fmt.Println("Неверный индекс")
			return false
		}
		fmt.Println("OK")
		return true
	case "MDEL":
		if !need(args, 1, "MDEL <имя> <индекс>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		if !array.remove(index) {
			fmt.Println("Неверный индекс")
			return false
		}
		fmt.Println("OK")
		return true
	case "MLEN":
		if !need(args, 0, "MLEN <имя>") {
			return false
		}
		fmt.Println("->", len(array.items))
	case "MREAD":
		if !need(args, 0, "MREAD <имя>") {
			return false
		}
		fmt.Println(formatValues(array.values()))
	}
	return false
}

func handleSingly(command string, list *SinglyList, args []string) bool {
	switch command {
	case "FPUSH":
		if !need(args, 1, "FPUSH <имя> <значение>") {
			return false
		}
		list.pushBack(args[0])
		fmt.Println("->", args[0])
	case "FPUSHFRONT":
		if !need(args, 1, "FPUSHFRONT <имя> <значение>") {
			return false
		}
		list.pushFront(args[0])
		fmt.Println("->", args[0])
	case "FINSERTBEFORE", "FINSERTAFTER":
		if !need(args, 2, command+" <имя> <целевой элемент> <значение>") {
			return false
		}
		var ok bool
		if command == "FINSERTBEFORE" {
			ok = list.insertBefore(args[0], args[1])
		} else {
			ok = list.insertAfter(args[0], args[1])
		}
		if !ok {
			fmt.Println("Целевой элемент не найден")
			return false
		}
		fmt.Println("->", args[1])
	case "FDEL":
		if !need(args, 1, "FDEL <имя> <значение>") {
			return false
		}
		if !list.erase(args[0]) {
			fmt.Println("Элемент не найден")
			return false
		}
	case "FDELHEAD", "FDELTAIL":
		if !need(args, 0, command+" <имя>") {
			return false
		}
		var value string
		var ok bool
		if command == "FDELHEAD" {
			value, ok = list.popFront()
		} else {
			value, ok = list.popBack()
		}
		if !ok {
			fmt.Println("Список пуст")
			return false
		}
		fmt.Println("->", value)
	case "FDELBEFORE", "FDELAFTER":
		if !need(args, 1, command+" <имя> <целевой элемент>") {
			return false
		}
		var ok bool
		if command == "FDELBEFORE" {
			ok = list.eraseBefore(args[0])
		} else {
			ok = list.eraseAfter(args[0])
		}
		if !ok {
			fmt.Println("Операция невозможна")
			return false
		}
	case "FGET":
		if !need(args, 1, "FGET <имя> <индекс>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		value, found := list.get(index)
		if found {
			fmt.Println("->", value)
		} else {
			fmt.Println("Неверный индекс")
		}
	case "FSEARCH":
		if !need(args, 1, "FSEARCH <имя> <значение>") {
			return false
		}
		fmt.Println("->", list.index(args[0]))
	case "FREAD", "FREADREVERSE":
		if len(args) > 1 {
			fmt.Println("Использование: FREAD <имя> [REVERSE]")
			return false
		}
		reverse := command == "FREADREVERSE" || (len(args) == 1 && strings.EqualFold(args[0], "REVERSE"))
		fmt.Println(formatValues(list.values(reverse)))
	}
	return strings.HasPrefix(command, "FPUSH") || strings.HasPrefix(command, "FINSERT") || strings.HasPrefix(command, "FDEL")
}

func handleDoubly(command string, list *DoublyList, args []string) bool {
	switch command {
	case "LPUSH":
		if !need(args, 1, "LPUSH <имя> <значение>") {
			return false
		}
		list.pushBack(args[0])
		fmt.Println("->", args[0])
	case "LPUSHFRONT":
		if !need(args, 1, "LPUSHFRONT <имя> <значение>") {
			return false
		}
		list.pushFront(args[0])
		fmt.Println("->", args[0])
	case "LINSERTBEFORE", "LINSERTAFTER":
		if !need(args, 2, command+" <имя> <целевой элемент> <значение>") {
			return false
		}
		node := list.find(args[0])
		if node == nil {
			fmt.Println("Целевой элемент не найден")
			return false
		}
		if command == "LINSERTBEFORE" {
			list.insertBefore(node, args[1])
		} else {
			list.insertAfter(node, args[1])
		}
		fmt.Println("->", args[1])
	case "LDEL":
		if !need(args, 1, "LDEL <имя> <значение>") {
			return false
		}
		if !list.deleteValue(args[0]) {
			fmt.Println("Элемент не найден")
			return false
		}
	case "LDELHEAD", "LDELTAIL":
		if !need(args, 0, command+" <имя>") {
			return false
		}
		var value string
		var ok bool
		if command == "LDELHEAD" {
			value, ok = list.popFront()
		} else {
			value, ok = list.popBack()
		}
		if !ok {
			fmt.Println("Список пуст")
			return false
		}
		fmt.Println("->", value)
	case "LDELBEFORE", "LDELAFTER":
		if !need(args, 1, command+" <имя> <целевой элемент>") {
			return false
		}
		var ok bool
		if command == "LDELBEFORE" {
			ok = list.eraseBefore(args[0])
		} else {
			ok = list.eraseAfter(args[0])
		}
		if !ok {
			fmt.Println("Операция невозможна")
			return false
		}
	case "LGET":
		if !need(args, 1, "LGET <имя> <индекс>") {
			return false
		}
		index, ok := readIndex(args[0])
		if !ok {
			return false
		}
		value, found := list.get(index)
		if found {
			fmt.Println("->", value)
		} else {
			fmt.Println("Неверный индекс")
		}
	case "LSEARCH":
		if !need(args, 1, "LSEARCH <имя> <значение>") {
			return false
		}
		fmt.Println("->", list.index(args[0]))
	case "LREAD", "LREADREVERSE":
		if len(args) > 1 {
			fmt.Println("Использование: LREAD <имя> [REVERSE]")
			return false
		}
		reverse := command == "LREADREVERSE" || (len(args) == 1 && strings.EqualFold(args[0], "REVERSE"))
		fmt.Println(formatValues(list.values(reverse)))
	}
	return strings.HasPrefix(command, "LPUSH") || strings.HasPrefix(command, "LINSERT") || strings.HasPrefix(command, "LDEL")
}

func handleStack(command string, stack *Stack, args []string) bool {
	switch command {
	case "SPUSH":
		if !need(args, 1, "SPUSH <имя> <значение>") {
			return false
		}
		stack.push(args[0])
		fmt.Println("->", args[0])
		return true
	case "SPOP":
		if !need(args, 0, "SPOP <имя>") {
			return false
		}
		value, ok := stack.pop()
		if !ok {
			fmt.Println("Стек пуст")
			return false
		}
		fmt.Println("->", value)
		return true
	case "SGET":
		if !need(args, 0, "SGET <имя>") {
			return false
		}
		value, ok := stack.peek()
		if !ok {
			fmt.Println("Стек пуст")
		} else {
			fmt.Println("->", value)
		}
	case "SREAD":
		if !need(args, 0, "SREAD <имя>") {
			return false
		}
		fmt.Println(formatValues(stack.values()))
	}
	return false
}

func handleQueue(command string, queue *Queue, args []string) bool {
	switch command {
	case "QPUSH":
		if !need(args, 1, "QPUSH <имя> <значение>") {
			return false
		}
		queue.push(args[0])
		fmt.Println("->", args[0])
		return true
	case "QPOP":
		if !need(args, 0, "QPOP <имя>") {
			return false
		}
		value, ok := queue.pop()
		if !ok {
			fmt.Println("Очередь пуста")
			return false
		}
		fmt.Println("->", value)
		return true
	case "QGET":
		if !need(args, 0, "QGET <имя>") {
			return false
		}
		value, ok := queue.peek()
		if !ok {
			fmt.Println("Очередь пуста")
		} else {
			fmt.Println("->", value)
		}
	case "QREAD":
		if !need(args, 0, "QREAD <имя>") {
			return false
		}
		fmt.Println(formatValues(queue.values()))
	}
	return false
}

func handleTree(command string, tree *AVLTree, args []string) bool {
	switch command {
	case "TINSERT":
		if !need(args, 1, "TINSERT <имя> <значение>") {
			return false
		}
		if !tree.insert(args[0]) {
			fmt.Println("Значение уже существует")
			return false
		}
		fmt.Println("->", args[0])
		return true
	case "TSEARCH", "ISMEMBER":
		if !need(args, 1, command+" <имя> <значение>") {
			return false
		}
		if tree.contains(args[0]) {
			fmt.Println("TRUE")
		} else {
			fmt.Println("FALSE")
		}
	case "TDEL":
		if !need(args, 1, "TDEL <имя> <значение>") {
			return false
		}
		if !tree.delete(args[0]) {
			fmt.Println("Элемент не найден")
			return false
		}
		fmt.Println("OK")
		return true
	case "TGET", "TREAD":
		if !need(args, 0, command+" <имя>") {
			return false
		}
		fmt.Println(formatValues(tree.values()))
	}
	return command == "TINSERT" || command == "TDEL"
}

func printNamed(db *Database, name string) {
	switch db.typeOf(name) {
	case "array":
		fmt.Println(formatValues(db.arrays[name].values()))
	case "flist":
		fmt.Println(formatValues(db.singlyLists[name].values(false)))
	case "dlist":
		fmt.Println(formatValues(db.doublyLists[name].values(false)))
	case "stack":
		fmt.Println(formatValues(db.stacks[name].values()))
	case "queue":
		fmt.Println(formatValues(db.queues[name].values()))
	case "tree":
		fmt.Println(formatValues(db.trees[name].values()))
	default:
		fmt.Println("Контейнер не найден")
	}
}

func formatValues(values []string) string {
	return "[" + strings.Join(values, ", ") + "]"
}

func showHelp() {
	fmt.Println("CREATE <M|F|L|S|Q|T> <имя>, DROP <тип> <имя>, PRINT <имя>")
	fmt.Println("M: MPUSH MINSERT MGET MSET MDEL MLEN MREAD")
	fmt.Println("F: FPUSH FPUSHFRONT FINSERTBEFORE FINSERTAFTER FDEL FDELHEAD FDELTAIL FDELBEFORE FDELAFTER FGET FREAD FSEARCH")
	fmt.Println("L: LPUSH LPUSHFRONT LINSERTBEFORE LINSERTAFTER LDEL LDELHEAD LDELTAIL LDELBEFORE LDELAFTER LGET LREAD LSEARCH")
	fmt.Println("S: SPUSH SPOP SGET SREAD; Q: QPUSH QPOP QGET QREAD")
	fmt.Println("T (AVL): TINSERT TSEARCH ISMEMBER TDEL TGET TREAD; SAVE LOAD EXIT")
}
