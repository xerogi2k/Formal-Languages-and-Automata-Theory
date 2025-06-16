import sys
import csv
from collections import defaultdict


def read_nfa(file_path):
    """Чтение НКА с точным соответствием JS-структуре"""
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = [line.strip().split(';') for line in f]

    nfa = {
        'alphabet': set(),
        'states': 0,
        'start': 0,
        'edges': defaultdict(lambda: defaultdict(set)),
        'accepting': set(),
        'outgoing': defaultdict(int),
        'incoming': defaultdict(int)
    }

    # Первая строка - финальные состояния
    outputs = lines[0]
    for i in range(1, len(outputs)):
        if outputs[i] == 'F':
            nfa['accepting'].add(i - 1)

    # Вторая строка - имена состояний
    state_names = lines[1]
    nfa['states'] = len(state_names) - 1  # X0..Xn

    # Чтение переходов
    for row in lines[2:]:
        symbol = row[0]
        if symbol != 'ε':
            nfa['alphabet'].add(symbol)

        for col_idx in range(1, len(row)):
            from_state = col_idx - 1  # X0=0, X1=1,...
            targets = row[col_idx].split(',') if row[col_idx] else []

            for target in targets:
                to_state = int(target[1:])  # X1 -> 1
                nfa['edges'][from_state][symbol].add(to_state)
                nfa['outgoing'][from_state] += 1
                nfa['incoming'][to_state] += 1

    return nfa


def epsilon_closure(nfa, states):
    """Вычисление ε-замыкания как в JS"""
    closure = set(states)
    stack = list(closure)

    while stack:
        s = stack.pop()
        for t in nfa['edges'][s].get('ε', set()):
            if t not in closure:
                closure.add(t)
                stack.append(t)
    return frozenset(closure)


def subset_construction(nfa):
    """Точный порт JS subsetConstruction"""
    dfa = {
        'alphabet': sorted(nfa['alphabet']),
        'states': 0,
        'edges': defaultdict(lambda: defaultdict(set)),
        'accepting': set(),
        'statescor': {}
    }

    # Начальное состояние
    initial = epsilon_closure(nfa, {0})
    dfa['statescor'][0] = initial
    dfa['states'] = 1
    queue = [0]

    while queue:
        current = queue.pop(0)
        current_states = dfa['statescor'][current]

        # Проверка на финальное состояние
        if any(s in nfa['accepting'] for s in current_states):
            dfa['accepting'].add(current)

        # Обработка символов
        for symbol in dfa['alphabet']:
            reachable = set()
            for s in current_states:
                reachable.update(nfa['edges'][s].get(symbol, set()))

            if not reachable:
                continue

            closure = epsilon_closure(nfa, reachable)

            # Поиск существующего состояния
            found = None
            for state in dfa['statescor']:
                if dfa['statescor'][state] == closure:
                    found = state
                    break

            if found is None:
                new_state = dfa['states']
                dfa['statescor'][new_state] = closure
                dfa['edges'][current][symbol].add(new_state)
                dfa['states'] += 1
                queue.append(new_state)
            else:
                dfa['edges'][current][symbol].add(found)

    return dfa


def export_dfa(dfa, output_file):
    """Экспорт с точным соответствием JS"""
    # Соответствие состояний
    print("States mapping:")
    for state in sorted(dfa['statescor']):
        orig_states = sorted(f'X{s}' for s in dfa['statescor'][state])
        print(f"S{state} -> {','.join(orig_states)}")

    # Подготовка CSV
    symbols = sorted(dfa['alphabet'])
    states = sorted(dfa['statescor'].keys())

    with open(output_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f, delimiter=';')

        # Заголовок выходов
        writer.writerow([''] + ['F' if s in dfa['accepting'] else '' for s in states])

        # Имена состояний
        writer.writerow([''] + [f'S{s}' for s in states])

        # Переходы
        for symbol in symbols:
            row = [symbol]
            for state in states:
                targets = dfa['edges'][state].get(symbol, set())
                row.append(','.join(f'S{s}' for s in sorted(targets)) if targets else '')
            writer.writerow(row)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: ./lab4 input.csv output.csv")
        sys.exit(1)

    nfa = read_nfa(sys.argv[1])
    dfa = subset_construction(nfa)
    export_dfa(dfa, sys.argv[2])