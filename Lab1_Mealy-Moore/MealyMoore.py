import csv
import sys

CSV_DELIMITER = ';'
DEFAULT_STATE_PREFIX = 'S'
MEALY_TO_MOORE_OPERATION = 'mealy-to-moore'
MOORE_TO_MEALY_OPERATION = 'moore-to-mealy'
ERROR_WRONG_OPERATION = "Неверное определение операции, используйте ""mealy-to-moore"" или ""moore-to-mealy"""
ERROR_USAGE = "Используйте параметры: program.py <mode> <input_file> <output_file>"
STATE_NOT_FOUND = "NULL"
STATE_INDEX_OFFSET = 1

def are_moves_equal(move1, move2):
    return move1[1] == move2[1] and move1[0] == move2[0]

def find_move_index(moves_list, move):
    for index in range(len(moves_list)):
        if are_moves_equal(moves_list[index], move):
            return index
    return -1

def convert_mealy_to_moore(input_file, output_file):
    with open(input_file, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=CSV_DELIMITER)
        header_states = next(reader)[STATE_INDEX_OFFSET:]
        state_names = list(header_states)

        transition_table = []
        input_signals = []
        state_transitions = []

        for row in reader:
            input_signals.append(row[0])
            current_row_transitions = []

            for entry in row[STATE_INDEX_OFFSET:]:
                state, output_signal = entry.split('/')
                transition = (output_signal, state)
                current_row_transitions.append(transition)

                if find_move_index(state_transitions, transition) == -1:
                    state_transitions.append(transition)

            transition_table.append(current_row_transitions)

    state_transitions.sort(key=lambda move: move[1])

    if state_transitions[0][1] != state_names[0]:
        state_transitions.insert(0, (STATE_NOT_FOUND, state_names[0]))

    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=CSV_DELIMITER)
        writer.writerow([""] + [move[0] for move in state_transitions])
        writer.writerow([""] + [f"{DEFAULT_STATE_PREFIX}{i}" for i in range(len(state_transitions))])

        for i, input_signal in enumerate(input_signals):
            row = [input_signal]
            for move in state_transitions:
                transition = transition_table[i][state_names.index(move[1])]
                row.append(f"{DEFAULT_STATE_PREFIX}{find_move_index(state_transitions, transition)}")
            writer.writerow(row)

def convert_moore_to_mealy(input_file, output_file):
    with open(input_file, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=CSV_DELIMITER)
        output_signals = next(reader)[STATE_INDEX_OFFSET:]
        state_names = next(reader)[STATE_INDEX_OFFSET:]

        state_output_mapping = {state: output for state, output in zip(state_names, output_signals)}

        data_to_write = []
        for row in reader:
            output_row = [row[0]]
            for state in row[STATE_INDEX_OFFSET:]:
                output_row.append(f"{state}/{state_output_mapping[state]}")
            data_to_write.append(output_row)

    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=CSV_DELIMITER)
        writer.writerow([""] + state_names)
        writer.writerows(data_to_write)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(ERROR_USAGE)
        sys.exit(1)

    operation_mode = sys.argv[1]
    input_file_path = sys.argv[2]
    output_file_path = sys.argv[3]

    if operation_mode == MEALY_TO_MOORE_OPERATION:
        convert_mealy_to_moore(input_file_path, output_file_path)
    elif operation_mode == MOORE_TO_MEALY_OPERATION:
        convert_moore_to_mealy(input_file_path, output_file_path)
    else:
        print(ERROR_WRONG_OPERATION)
        sys.exit(1)