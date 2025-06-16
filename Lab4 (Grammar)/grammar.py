import sys
import re


def main():
    if len(sys.argv) < 3:
        print("Usage: python script.py <grammar.txt> <output.csv>")
        sys.exit(1)

    inp, out = sys.argv[1], sys.argv[2]
    lines, prods = [], []

    with open(inp, 'r', encoding='utf-8') as f:
        curr = f.readline()
        while curr:
            block = curr
            curr = f.readline()
            while curr and "->" not in curr:
                block += curr
                curr = f.readline()
            lhs_rhs = block.split("->", 1)
            if len(lhs_rhs) == 2:
                lhs = lhs_rhs[0].strip()
                rhs = lhs_rhs[1].strip()
                alts = [x.strip() for x in rhs.split("|")]
                prods.append({"lhs": lhs, "rhs": alts})

    pattern = re.compile(r"<(?:\d|\w)+>\s+\S")
    left_linear = any(pattern.match(alt) for p in prods for alt in p["rhs"])

    st_map = {}
    if left_linear:
        st_map["F"] = "q0"
        idx = 1
        for i in range(1, len(prods)):
            st_map[prods[i]["lhs"]] = f"q{idx}"
            idx += 1
        st_map[prods[0]["lhs"]] = f"q{len(st_map)}"
    else:
        for i, p in enumerate(prods):
            st_map[p["lhs"]] = f"q{i}"
        st_map["F"] = f"q{len(st_map)}"

    transitions, symbols = [], []
    for p in prods:
        left_side = p["lhs"]
        for alt in p["rhs"]:
            tokens = [x for x in alt.split() if x]
            if not tokens:
                continue
            if len(tokens) == 1:
                arg = tokens[0]
                if left_linear:
                    fr, to = st_map["F"], st_map[left_side]
                else:
                    fr, to = st_map[left_side], st_map["F"]
                transitions.append({"from": fr, "to": to, "arg": arg})
                symbols.append(arg)
            else:
                if left_linear:
                    fr, to, arg = st_map.get(tokens[0], ""), st_map[left_side], tokens[1]
                else:
                    fr, to, arg = st_map[left_side], st_map.get(tokens[1], ""), tokens[0]
                if fr and to:
                    transitions.append({"from": fr, "to": to, "arg": arg})
                    symbols.append(arg)

    symbols = sorted(set(symbols))
    states = sorted(st_map.values(), key=lambda x: int(x[1:]) if x.startswith('q') else -1)

    with open(out, 'w', encoding='utf-8') as out:
        for _ in states:
            out.write(";")
        out.write("F\n;")
        for st in states:
            out.write(st + ";")
        out.write("\n")
        for sym in symbols:
            out.write(sym + ";")
            for st in states:
                tgts = [t["to"] for t in transitions if t["from"] == st and t["arg"] == sym]
                if tgts:
                    out.write(",".join(tgts))
                out.write(";")
            out.write("\n")

    for k, v in st_map.items():
        print(k, "->", v)

if __name__ == "__main__":
    main()