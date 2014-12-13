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

def dict_add(d, k, k2, v) :
    if not d.has_key(k) :
        d[k] = dict()
    if not d[k].has_key(k2) :
        d[k][k2] = list()
    d[k][k2].append(v)

def buildEdgeMapping(edgelist, vdict) :
    d = dict()
    for (src, dst) in edgelist :
        dst_label = vdict[dst]
        dict_add(d, src, dst_label, dst)
    return d

def generate_end_offsets(vertex_dict_list) :
    d = []
    index = 0
    for (v, dictionary) in vertex_dict_list :
        index += 2 * len(dictionary.keys())
        d.append((v,index))
    return dict(d, key=lambda p:int(p[0]))

def generate_label_offsets(ordered_mapping) :
    d = list()
    vertex_block = []
    index = 0
    for (src_id, label_dict) in ordered_mapping :
        # Order labels by interger score
        ordered_labels = sorted(label_dict.items(), key=lambda x:int(x[0]))
        for (label, vertex_list) in ordered_labels :
            if type(vertex_list) == int :
                index += 1
                vertex_block.append(vertex_list)
            else :
                index += len(vertex_list)
                vertex_block.extend(vertex_list)
            d.append((label, index))
    vertex_block = sorted(vertex_block, key=lambda x:int(x))
    return (d, vertex_block)

def print_edges(edge_list, vdict, path="edge_file") :
    # Dict[VertexId][Label] => [VertexId]
    f = open(path, "w")
    f.write(str(len(vdict.items())) + '\n')

    mapping = buildEdgeMapping(edge_list, vdict)
    ordered_by_vertexid = sorted(mapping.items(), key=lambda d:int(d[0]))
    end_offsets = generate_end_offsets(ordered_by_vertexid)
    vertex_list = sorted(vdict.keys(), key=lambda k:int(k))
    recent = 0
    for v in vertex_list :
        if end_offsets.has_key(v) :
            f.write(str(end_offsets[v]) + '\n')
            recent = end_offsets[v]
        else :
            f.write(str(recent) + '\n')

    (label_offsets, vertex_block) = generate_label_offsets(ordered_by_vertexid)
    for (label,index) in label_offsets : 
        f.write(str(label) + '\n' + str(index) + '\n')
    for v in vertex_block :
        f.write(str(v) + '\n')
    f.close()

def print_reverse_edges(edges,vdict) :
    reverse_edgelist = []
    for (src,dst) in edges.items() :
        reverse_edgelist.append((dst,src))
    print_edges(reverse_edgelist, vdict, "reverse_edge_file")


file_path = sys.argv[1]
f = open(file_path)

# VertexId => Label
vertices = parse_pairs(f)

# VertexId => VertexId
edges = parse_pairs(f)

print_vertices(vertices)
print_edges(edges.items(), vertices)
print_reverse_edges(edges, vertices)
