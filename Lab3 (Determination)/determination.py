import sys
import csv
from collections import deque, defaultdict

def read_nfa(file_path):
    """Считывает НКА из CSV файла"""
    with open(file_path, 'r', encoding='utf-8') as f:
        reader = csv.reader(f, delimiter=';')
        data = [row for row in reader]

    outputs = data[0]
    states = data[1]
    transitions = data[2:]

    nfa = {}
    for i in range(1, len(states)):
        state_name = states[i]
        nfa[state_name] = {
            'output': outputs[i],
            'transitions': defaultdict(list)
        }

    alphabet = set()
    for row in transitions:
        symbol = row[0]
        if symbol != 'ε':
            alphabet.add(symbol)
        for i in range(1, len(row)):
            if row[i]:
                targets = row[i].split(',')
                nfa[states[i]]['transitions'][symbol].extend(targets)

    return nfa, sorted(alphabet)


def epsilon_closure(nfa, states):
    """Вычисляет ε-замыкание для множества состояний"""
    closure = set(states)
    queue = deque(states)

    while queue:
        state = queue.popleft()
        for target in nfa[state]['transitions'].get('ε', []):
            if target not in closure:
                closure.add(target)
                queue.append(target)
    return frozenset(closure)


def convert(nfa, alphabet):
    """Конвертирует НКА в ДКА"""
    start_state = next(iter(nfa))
    initial = epsilon_closure(nfa, [start_state])

    state_map = {initial: 'S0'}
    queue = deque([initial])
    dfa_states = []
    state_counter = 0

    while queue:
        current = queue.popleft()
        current_name = state_map[current]

        # Определение финального состояния
        is_final = any(nfa[s]['output'] == 'F' for s in current)

        new_state = {
            'name': current_name,
            'output': 'F' if is_final else '',
            'transitions': defaultdict(list)
        }

        # Обработка переходов для каждого символа
        for symbol in alphabet:
            if symbol == 'ε':
                continue

            # Собираем все прямые переходы
            targets = set()
            for state in current:
                targets.update(nfa[state]['transitions'].get(symbol, []))

            if not targets:
                continue

            # Вычисляем полное ε-замыкание
            closure = epsilon_closure(nfa, targets)

            # Регистрируем новое состояние
            if closure not in state_map:
                state_counter += 1
                state_map[closure] = f'S{state_counter}'
                queue.append(closure)

            new_state['transitions'][symbol] = [state_map[closure]]

        dfa_states.append(new_state)

    return dfa_states, state_map


def export_dfa(dfa_states, state_map, output_file):
    """Экспортирует ДКА в CSV и выводит маппинг"""
    # Вывод соответствия состояний
    print("States mapping:")
    for dfa_state in sorted(state_map.values(), key=lambda x: int(x[1:])):
        nfa_states = sorted([s for s in state_map if state_map[s] == dfa_state][0])
        print(f"{dfa_state} -> {','.join(nfa_states)}")

    # Подготовка данных для CSV
    symbols = sorted({sym for s in dfa_states for sym in s['transitions']})
    header = [''] + [s['name'] for s in dfa_states]
    output_row = [''] + [s['output'] for s in dfa_states]

    with open(output_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(output_row)
        writer.writerow(header)

        for symbol in symbols:
            row = [symbol]
            for state in dfa_states:
                targets = state['transitions'].get(symbol, [''])
                row.append(','.join(targets))
            writer.writerow(row)


def main():
    if len(sys.argv) != 3:
        print("Usage: ./lab4 input.csv output.csv")
        sys.exit(1)

    nfa, alphabet = read_nfa(sys.argv[1])
    dfa_states, state_map = convert(nfa, alphabet)
    export_dfa(dfa_states, state_map, sys.argv[2])


if __name__ == "__main__":
    main()