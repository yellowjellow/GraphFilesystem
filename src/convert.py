import sys

def parse_pairs(f) :
    vertex_map = dict()
    for line in f :
        line = line.strip()
        if len(line) == 0 :
            break
        if line[0] == '#' :
            continue
        pair = line.split(',')
        vertex_map[pair[0]] = pair[1]
    return vertex_map

def get_label_frequency(vdict) :
    s = dict()
    for (v,l) in vdict.items() :
        if not s.has_key(l) :
            s[l] = []
        s[l].append(v)
    return s.items()

def print_vertices(vdict) :
    label_freq = get_label_frequency(vdict)
    ordered_labels = sorted(label_freq, key=lambda pair:int(pair[0]))
    print ordered_labels
    f = open("vertex_file", "w")
    f.write(str(len(vdict.items())) + '\n')
    f.write(str(len(label_freq)) + '\n')
    for (l,vertices) in ordered_labels :
        f.write(str(l) + '\n')
    index = 0
    for (l,vertices) in ordered_labels :
        index += len(vertices)
        f.write(str(index) + '\n')
    for (l,vertices) in ordered_labels :
        for v in vertices :
            f.write(str(v) + '\n')
    f.close()


file_path = sys.argv[1]
f = open(file_path)

# VertexId => Label
vertices = parse_pairs(f)

# VertexId => VertexId
edges = parse_pairs(f)

print_vertices(vertices)
'''
print_edges(edges)
print_reverse_edges(edges)
'''
