#include "storage.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

void handleArray(Command command, Json& db, const Arguments& args);
void handleSingly(Command command, Json& db, const Arguments& args);
void handleDoubly(Command command, Json& db, const Arguments& args);
void handleStack(Command command, Json& db, const Arguments& args);
void handleQueue(Command command, Json& db, const Arguments& args);
void handleTree(Command command, Json& db, const Arguments& args);

Command parseCommand(const std::string& text) {
    struct CommandEntry {
        const char* name;
        Command command;
    };

    const CommandEntry commands[] = {
        {"HELP", Command::Help}, {"?", Command::Help},
        {"EXIT", Command::Exit}, {"QUIT", Command::Quit},
        {"SAVE", Command::Save}, {"CREATE", Command::Create},
        {"DROP", Command::Drop}, {"PRINT", Command::Print},
        {"MCREATE", Command::MCreate}, {"MPUSH", Command::MPush},
        {"MINSERT", Command::MInsert}, {"MGET", Command::MGet},
        {"MSET", Command::MSet}, {"MDEL", Command::MDel},
        {"MLEN", Command::MLen}, {"MREAD", Command::MRead},
        {"FCREATE", Command::FCreate}, {"FPUSH", Command::FPush},
        {"FPUSHFRONT", Command::FPushFront}, {"FINSERTBEFORE", Command::FInsertBefore},
        {"FINSERTAFTER", Command::FInsertAfter}, {"FDEL", Command::FDel},
        {"FDELHEAD", Command::FDelHead}, {"FDELTAIL", Command::FDelTail},
        {"FDELBEFORE", Command::FDelBefore}, {"FDELAFTER", Command::FDelAfter},
        {"FGET", Command::FGet}, {"FREAD", Command::FRead},
        {"FSEARCH", Command::FSearch},
        {"LCREATE", Command::LCreate}, {"LPUSH", Command::LPush},
        {"LPUSHFRONT", Command::LPushFront}, {"LINSERTBEFORE", Command::LInsertBefore},
        {"LINSERTAFTER", Command::LInsertAfter}, {"LDEL", Command::LDel},
        {"LDELHEAD", Command::LDelHead}, {"LDELTAIL", Command::LDelTail},
        {"LDELBEFORE", Command::LDelBefore}, {"LDELAFTER", Command::LDelAfter},
        {"LGET", Command::LGet}, {"LREAD", Command::LRead},
        {"LSEARCH", Command::LSearch},
        {"SCREATE", Command::SCreate}, {"SPUSH", Command::SPush},
        {"SPOP", Command::SPop}, {"SGET", Command::SGet},
        {"SREAD", Command::SRead},
        {"QCREATE", Command::QCreate}, {"QPUSH", Command::QPush},
        {"QPOP", Command::QPop}, {"QGET", Command::QGet},
        {"QREAD", Command::QRead},
        {"TCREATE", Command::TCreate}, {"TINSERT", Command::TInsert},
        {"TGET", Command::TGet}, {"TREAD", Command::TRead},
        {"TSEARCH", Command::TSearch}, {"TDEL", Command::TDel},
        {"ISMEMBER", Command::IsMember}
    };

    const std::string key = upper(text);
    for (const CommandEntry& entry : commands) {
        if (key == entry.name) return entry.command;
    }
    return Command::Unknown;
}

Arguments splitArguments(const std::string& line) {
    Arguments result;
    std::string word;
    char quote = 0;
    bool started = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (quote != 0) {
            if (ch == quote) quote = 0;
            else if (ch == '\\' && i + 1 < line.size()) word += line[++i];
            else word += ch;
            started = true;
        } else if (ch == '"' || ch == '\'') {
            quote = ch;
            started = true;
        } else if (std::isspace(static_cast<unsigned char>(ch))) {
            if (started) {
                if (!result.pushBack(word)) {
                    throw std::runtime_error("Слишком много аргументов команды.");
                }
                word.clear();
                started = false;
            }
        } else {
            word += ch;
            started = true;
        }
    }

    if (quote != 0) throw std::runtime_error("Не закрыта кавычка.");
    if (started && !result.pushBack(word)) {
        throw std::runtime_error("Слишком много аргументов команды.");
    }
    return result;
}

bool countIs(const Arguments& args, std::size_t count, const std::string& usage) {
    if (args.size() == count) return true;
    std::cout << "Использование: " << usage << '\n';
    return false;
}

bool readIndex(const Arguments& args, std::size_t position, std::size_t& index) {
    if (position >= args.size() || args[position].empty()) {
        std::cout << "Ошибка: индекс должен быть неотрицательным целым числом.\n";
        return false;
    }

    std::size_t result = 0;
    for (unsigned char ch : args[position]) {
        if (!std::isdigit(ch)) {
            std::cout << "Ошибка: индекс должен быть неотрицательным целым числом.\n";
            return false;
        }

        const std::size_t digit = static_cast<std::size_t>(ch - '0');
        if (result > (std::numeric_limits<std::size_t>::max() - digit) / 10) {
            std::cout << "Ошибка: индекс слишком большой.\n";
            return false;
        }
        result = result * 10 + digit;
    }

    index = result;
    return true;
}

void showHelp() {
    std::cout
        << "CREATE <M|F|L|S|Q|T> <имя> | DROP <тип> <имя> | PRINT <тип> <имя>\n"
        << "M: MPUSH MINSERT MGET MSET MDEL MLEN MREAD\n"
        << "F: FPUSH FPUSHFRONT FINSERTBEFORE FINSERTAFTER FDEL FDELHEAD FDELTAIL FDELBEFORE FDELAFTER FGET FREAD FSEARCH\n"
        << "L: LPUSH LPUSHFRONT LINSERTBEFORE LINSERTAFTER LDEL LDELHEAD LDELTAIL LDELBEFORE LDELAFTER LGET LREAD LSEARCH\n"
        << "S: SPUSH SPOP SGET SREAD\n"
        << "Q: QPUSH QPOP QGET QREAD\n"
        << "T (AVL): TINSERT TSEARCH ISMEMBER TDEL TGET TREAD\n"
        << "Изменения сохраняются в JSON. Индексы начинаются с нуля.\n";
}

char typeFromCommand(Command command) {
    switch (command) {
        case Command::MCreate: case Command::MPush: case Command::MInsert:
        case Command::MGet: case Command::MSet: case Command::MDel:
        case Command::MLen: case Command::MRead:
            return 'M';
        case Command::FCreate: case Command::FPush: case Command::FPushFront:
        case Command::FInsertBefore: case Command::FInsertAfter: case Command::FDel:
        case Command::FDelHead: case Command::FDelTail: case Command::FDelBefore:
        case Command::FDelAfter: case Command::FGet: case Command::FRead:
        case Command::FSearch:
            return 'F';
        case Command::LCreate: case Command::LPush: case Command::LPushFront:
        case Command::LInsertBefore: case Command::LInsertAfter: case Command::LDel:
        case Command::LDelHead: case Command::LDelTail: case Command::LDelBefore:
        case Command::LDelAfter: case Command::LGet: case Command::LRead:
        case Command::LSearch:
            return 'L';
        case Command::SCreate: case Command::SPush: case Command::SPop:
        case Command::SGet: case Command::SRead:
            return 'S';
        case Command::QCreate: case Command::QPush: case Command::QPop:
        case Command::QGet: case Command::QRead:
            return 'Q';
        case Command::TCreate: case Command::TInsert: case Command::TGet:
        case Command::TRead: case Command::TSearch: case Command::TDel:
        case Command::IsMember:
            return 'T';
        default:
            return '\0';
    }
}

bool execute(const Arguments& args, Json& db) {
    if (args.empty()) return true;

    const Command command = parseCommand(args[0]);
    if (command == Command::Help) {
        showHelp();
        return true;
    }
    if (command == Command::Exit || command == Command::Quit) return false;
    if (command == Command::Unknown) {
        std::cout << "Неизвестная команда. Введите HELP.\n";
        return true;
    }
    if (command == Command::Save) {
        if (countIs(args, 1, "SAVE")) std::cout << "OK\n";
        return true;
    }

    if (command == Command::Create || command == Command::Drop || command == Command::Print) {
        const std::string usage = command == Command::Create ? "CREATE <M|F|L|S|Q|T> <имя>" :
                                  command == Command::Drop ? "DROP <M|F|L|S|Q|T> <имя>" :
                                                             "PRINT <M|F|L|S|Q|T> <имя>";
        if (!countIs(args, 3, usage)) return true;
        if (args[1].size() != 1) {
            std::cout << "Тип должен быть M, F, L, S, Q или T.\n";
            return true;
        }

        const char type = static_cast<char>(std::toupper(static_cast<unsigned char>(args[1][0])));
        if (command == Command::Create) {
            if (createContainer(db, type, args[2])) std::cout << "OK\n";
            else std::cout << "Не удалось создать контейнер. Проверьте тип и имя.\n";
        } else if (command == Command::Drop) {
            if (dropContainer(db, type, args[2])) std::cout << "OK\n";
            else std::cout << "Контейнер не найден или тип указан неверно.\n";
        } else {
            printContainer(db, type, args[2]);
        }
        return true;
    }

    const char type = typeFromCommand(command);
    if (type == '\0') return true;

    const bool isCreate = command == Command::MCreate || command == Command::FCreate ||
                          command == Command::LCreate || command == Command::SCreate ||
                          command == Command::QCreate || command == Command::TCreate;
    if (isCreate) {
        if (!countIs(args, 2, args[0] + " <имя>")) return true;
        if (createContainer(db, type, args[1])) std::cout << "OK\n";
        else std::cout << "Не удалось создать контейнер. Проверьте тип и имя.\n";
        return true;
    }

    if (args.size() < 2) {
        std::cout << "Укажите имя структуры.\n";
        return true;
    }

    const char* expected = type == 'M' ? "array" : type == 'F' ? "flist" :
                           type == 'L' ? "dlist" : type == 'S' ? "stack" :
                           type == 'Q' ? "queue" : "tree";
    if (!hasContainer(db, args[1], expected)) {
        std::cout << "Контейнер не найден или имеет другой тип.\n";
        return true;
    }

    switch (type) {
        case 'M': handleArray(command, db, args); break;
        case 'F': handleSingly(command, db, args); break;
        case 'L': handleDoubly(command, db, args); break;
        case 'S': handleStack(command, db, args); break;
        case 'Q': handleQueue(command, db, args); break;
        case 'T': handleTree(command, db, args); break;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::string filename = "file.json";
    std::string query;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--file" && i + 1 < argc) filename = argv[++i];
        else if (argument == "--query" && i + 1 < argc) query = argv[++i];
        else {
            std::cout << "Использование: ./dbms [--file file.json] [--query 'COMMAND']\n";
            return 1;
        }
    }

    Json db = Json::object();
    try {
        std::ifstream input(filename);
        if (input.is_open() && input.peek() != EOF) {
            db = Json::parse(input);
            if (!db.is_object()) {
                std::cout << "Корень JSON должен быть объектом.\n";
                return 1;
            }
        }
    } catch (const std::exception& error) {
        std::cout << "Ошибка чтения JSON: " << error.what() << '\n';
        return 1;
    }

    auto save = [&]() {
        std::ofstream output(filename);
        output << db.dump(4) << '\n';
        if (!output) std::cout << "Ошибка записи JSON.\n";
    };

    if (!query.empty()) {
        try {
            execute(splitArguments(query), db);
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << '\n';
        }
        save();
        return 0;
    }

    std::cout << "Лабораторная работа: структуры данных. HELP — команды, EXIT — выход.\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        bool keepGoing = true;
        try {
            keepGoing = execute(splitArguments(line), db);
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << '\n';
        }
        save();
        if (!keepGoing) break;
    }
    return 0;
}
