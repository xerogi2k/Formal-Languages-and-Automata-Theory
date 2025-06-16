import sys
import re

def is_nonterminal(s):
    return re.match(r'^<[^>]+>$', s) is not None

def read_grammar(filename):
    productions = []
    with open(filename, 'r', encoding='utf-8') as f:
        lines = [line.strip() for line in f if line.strip()]
    for line in lines:
        if '->' in line:
            lhs, rhs = line.split('->', 1)
            productions.append({
                'lhs': lhs.strip(),
                'rhs': [alt.strip() for alt in rhs.split('|')]
            })
        else:
            productions[-1]['rhs'].extend(
                [alt.strip() for alt in line.split('|')]
            )
    return productions

def determine_grammar_type(productions):
    left = right = True
    for p in productions:
        for alt in p['rhs']:
            if alt == 'ε': continue
            tokens = alt.split()
            nts = [i for i, t in enumerate(tokens) if is_nonterminal(t)]

            if not nts:
                if len(tokens) != 1: left = right = False
                continue

            if any(pos != 0 for pos in nts): left = False
            if any(pos != len(tokens) - 1 for pos in nts): right = False

    return left, right


def build_nfa(productions, is_left):
    states = {}
    transitions = []
    symbols = set()
    state_id = 0

    # Create states for non-terminals
    states[productions[0]['lhs']] = f'q{state_id}'
    state_id += 1
    for p in productions[1:]:
        if p['lhs'] not in states:
            states[p['lhs']] = f'q{state_id}'
            state_id += 1

    # Create F state only if needed
    has_single_term = any(
        (len(alt.split()) == 1 and not is_nonterminal(alt)
         for p in productions
         for alt in p['rhs']
    ))
    if has_single_term:
        states['F'] = f'q{state_id}'

    # Build transitions
    start_state = states[productions[0]['lhs']]
    for p in productions:
        lhs = p['lhs']
        for alt in p['rhs']:
            if alt == 'ε':
                transitions.append((states[lhs], 'ε', start_state))
                symbols.add('ε')
                continue

            tokens = alt.split()
            if len(tokens) == 1:
                term = tokens[0]
                if is_nonterminal(term): continue
                if is_left:
                    transitions.append((states['F'], term, states[lhs]))
                else:
                    transitions.append((states[lhs], term, states['F']))
                symbols.add(term)
            else:
                if is_left:
                    nt, term = tokens[0], tokens[1]
                    transitions.append((states[nt], term, states[lhs]))
                else:
                    term, nt = tokens[0], tokens[1]
                    transitions.append((states[lhs], term, states[nt]))
                symbols.add(term)


    final_state = [start_state] if is_left else [states.get('F', '')]
    return states, transitions, sorted(symbols - {'ε'}), final_state

def write_csv(filename, states, transitions, symbols, finals):
    # Упорядочиваем состояния: сначала все кроме F, потом F (если есть)
    ordered_states = sorted(
        [s for s in states.values() if s != states.get('F', None)],
        key=lambda x: int(x[1:])
    )
    has_f = 'F' in states
    if has_f:
        ordered_states.append(states['F'])

    with open(filename, 'w', encoding='utf-8') as f:
        # Первая строка: заголовок с F
        f.write(';' * (len(ordered_states) - (1 if has_f else 0)))
        if has_f:
            f.write(';F')
        f.write('\n')

        # Вторая строка: метки состояний
        f.write(';' + ';'.join(ordered_states) + '\n')

        # Транзиции для символов
        for sym in symbols:
            line = [sym]
            for st in ordered_states:
                targets = [t[2] for t in transitions if t[0] == st and t[1] == sym]
                line.append(','.join(targets) if targets else '')
            f.write(';'.join(line) + '\n')

        # Эпсилон-переходы
        eps_lines = [t for t in transitions if t[1] == 'ε']
        if eps_lines:
            f.write('ε')
            for st in ordered_states:
                targets = [t[2] for t in eps_lines if t[0] == st]
                f.write(';' + ','.join(targets) if targets else ';')
            f.write('\n')

    # Вывод в консоль
    print("States mapping:")
    for nt, state in states.items():
        print(f"{nt} -> {state}")
    if has_f:
        print("Final state: F")

def main():
    if len(sys.argv) != 3:
        print("Usage: python nfa_converter.py input.txt output.csv")
        sys.exit(1)

    productions = read_grammar(sys.argv[1])
    left, right = determine_grammar_type(productions)

    if not left and not right:
        print("Error: Grammar is not regular")
        sys.exit(1)

    states, transitions, symbols, finals = build_nfa(productions, left)
    write_csv(sys.argv[2], states, transitions, symbols, finals)
    print("NFA saved to", sys.argv[2])


if __name__ == "__main__":
    main()