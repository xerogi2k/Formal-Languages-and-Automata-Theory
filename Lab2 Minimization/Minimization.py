import csv
import sys

def parse_list(file):
    elements = []
    line = next(file).strip()[1:]
    while ';' in line:
        elements.append(line[:line.find(';')])
        line = line[line.find(';') + 1:]
    elements.append(line)
    return elements

def write_state_headers(output_file, state_mapping):
    index = 0
    for _, value in state_mapping.items():
        if int(value) >= index:
            output_file.write(f";q{value}")
            index += 1
    output_file.write("\n")

def update_state_mapping(previous_mapping, state_list, transition_table):
    new_mapping = {}
    index = 0
    for i in range(len(transition_table)):
        found = False
        for j in range(i):
            if transition_table[i] == transition_table[j] and previous_mapping[state_list[i]] == previous_mapping[state_list[j]]:
                found = True
                new_mapping[state_list[i]] = new_mapping[state_list[j]]
                break
        if not found:
            new_mapping[state_list[i]] = str(index)
            index += 1
    return new_mapping

def process_mealy(input_filename, output_filename):
    with open(input_filename, newline='') as input_file:
        reader = csv.reader(input_file, delimiter=';')
        state_list = parse_list(input_file)
        num_states = len(state_list)
        original_table = [[] for _ in range(num_states)]
        temp_table = [[] for _ in range(num_states)]
        input_symbols = []

        for row in reader:
            input_symbols.append(row[0])
            for col_idx, move in enumerate(row[1:]):
                if '/' in move:
                    next_state, output_symbol = move.split('/')
                else:
                    next_state, output_symbol = move, ''
                original_table[col_idx].append((next_state, output_symbol))
                temp_table[col_idx].append(next_state)

        state_mapping = initialize_state_mapping(state_list, original_table, input_symbols)
        state_mapping = minimize_states(state_mapping, state_list, original_table, temp_table)

        write_mealy_output(output_filename, input_symbols, state_list, original_table, temp_table, state_mapping)

def initialize_state_mapping(state_list, original_table, input_symbols):
    state_mapping = {}
    index = 0
    for i in range(len(state_list)):
        found = False
        for j in range(i):
            if all(original_table[i][k][1] == original_table[j][k][1] for k in range(len(input_symbols))):
                found = True
                state_mapping[state_list[i]] = state_mapping[state_list[j]]
                break
        if not found:
            state_mapping[state_list[i]] = str(index)
            index += 1
    return state_mapping

def minimize_states(state_mapping, state_list, original_table, temp_table):
    changes = True
    while changes:
        changes = False
        for i in range(len(original_table)):
            for j in range(len(original_table[i])):
                if original_table[i][j][0] in state_mapping:
                    temp_table[i][j] = state_mapping[original_table[i][j][0]]
                else:
                    temp_table[i][j] = original_table[i][j][0]

        new_mapping = update_state_mapping(state_mapping, state_list, temp_table)
        if state_mapping != new_mapping:
            changes = True
            state_mapping = new_mapping.copy()
    return state_mapping

def write_mealy_output(output_filename, input_symbols, state_list, original_table, temp_table, state_mapping):
    with open(output_filename, 'w', newline='') as output_file:
        write_state_headers(output_file, state_mapping)
        for i in range(len(input_symbols)):
            output_file.write(input_symbols[i])
            index = 0
            for key, value in state_mapping.items():
                if int(value) >= index:
                    next_state = temp_table[state_list.index(key)][i]
                    output_symbol = original_table[state_list.index(key)][i][1]
                    output_file.write(f";q{next_state}/{output_symbol}")
                    index += 1
            output_file.write("\n")

def process_moore(input_filename, output_filename):
    with open(input_filename, newline='') as input_file:
        reader = csv.reader(input_file, delimiter=';')
        output_symbols = parse_list(input_file)
        state_list = parse_list(input_file)
        num_states = len(state_list)
        original_table = [[] for _ in range(num_states)]
        input_symbols = []

        for row in reader:
            input_symbols.append(row[0])
            for col_idx, value in enumerate(row[1:]):
                original_table[col_idx].append(value)

        temp_table = [row[:] for row in original_table]
        state_mapping = initialize_state_mapping_moore(state_list, output_symbols)
        state_mapping = minimize_states(state_mapping, state_list, original_table, temp_table)
        state_mapping = minimize_states_moore(state_mapping, state_list, original_table, temp_table)

        write_moore_output(output_filename, input_symbols, state_list, output_symbols, temp_table, state_mapping)

def initialize_state_mapping_moore(state_list, output_symbols):
    state_mapping = {}
    index = 0
    for i in range(len(state_list)):
        found = False
        for j in range(i):
            if output_symbols[i] == output_symbols[j]:
                found = True
                state_mapping[state_list[i]] = state_mapping[state_list[j]]
                break
        if not found:
            state_mapping[state_list[i]] = str(index)
            index += 1
    return state_mapping

def minimize_states_moore(state_mapping, state_list, original_table, temp_table):
    changes = True
    while changes:
        changes = False
        for i in range(len(original_table)):
            for j in range(len(original_table[i])):
                if original_table[i][j] in state_mapping:
                    temp_table[i][j] = state_mapping[original_table[i][j]]
                else:
                    temp_table[i][j] = original_table[i][j]

        new_mapping = update_state_mapping(state_mapping, state_list, temp_table)
        if state_mapping != new_mapping:
            changes = True
            state_mapping = new_mapping.copy()
    return state_mapping

def write_moore_output(output_filename, input_symbols, state_list, output_symbols, temp_table, state_mapping):
    with open(output_filename, 'w', newline='') as output_file:
        index = 0
        for key, value in state_mapping.items():
            if int(value) >= index:
                output_file.write(f";{output_symbols[state_list.index(key)]}")
                index += 1
        output_file.write("\n")

        write_state_headers(output_file, state_mapping)
        for i in range(len(input_symbols)):
            output_file.write(input_symbols[i])
            index = 0
            for key, value in state_mapping.items():
                if int(value) >= index:
                    output_file.write(f";q{temp_table[state_list.index(key)][i]}")
                    index += 1
            output_file.write("\n")

def main():
    if len(sys.argv) < 4:
        print("Usage: <program> <mode(mealy/moore)> <input.csv> <output.csv>")
        return 1

    mode = sys.argv[2]
    input_filename = sys.argv[3]
    output_filename = sys.argv[4]

    if mode == "mealy":
        process_mealy(input_filename, output_filename)
    elif mode == "moore":
        process_moore(input_filename, output_filename)
    else:
        print("Wrong mode")
        return 1

if __name__ == "__main__":
    main()
