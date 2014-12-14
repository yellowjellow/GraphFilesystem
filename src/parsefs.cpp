#ifndef PARSEFS_CPP_
#define PARSEFS_CPP_

#include "parsefs.h"

void EdgeFs::FlipEdges() {
    UINT* tmp = data;
    data = reverse_data;
    reverse_data = tmp;

    UINT tmp2 = size;
    size = reverse_size;
    reverse_size = tmp2;
}

void EdgeFs::PrintGraphEdges(const char* file_path) {
    int adj_labels_offset;
    int vertex_count;
    int right_offset;
    int adj_labels_rbound;
    UINT* vertex_iterator;
    LABEL label;
    ofstream f;
    f.open(file_path);

    // Header
    f << size << '\n' << num_vertices << '\n';

    // We always know the first vertex to get printed for each label since
    // labels and vertexID are both sorted.
    vertex_count = 0;
    for(int vertex = 0; vertex < num_vertices; vertex += 1) {
        if (vertex == 0)
            adj_labels_offset = 0;
        else
            adj_labels_offset = data[vertex];

        adj_labels_rbound = data[vertex + 1];
        for(; adj_labels_offset < adj_labels_rbound; adj_labels_offset += 2) {
            label = adjacent_vertex_labels[adj_labels_offset];
            right_offset = adjacent_vertex_labels[adj_labels_offset + 1];
            f << vertex << '\t' << label;
            for(int offset = 0; offset < right_offset - vertex_count; offset += 1) {
                f << '\t' << adjacent_vertex_ids[vertex_count + offset];
            }
            f << endl;
            vertex_count = right_offset;
        }
    }
}

void EdgeFs::WriteToFile(const char* file_path) {
    ofstream f;
    f.open(file_path);

    // Write number of UINTS needed to be allocated
    f << to_string(size) << '\n';
    for (UINT* it = data; it < data + size; it += 1) {
        f << to_string(*(it)) << '\n';
    }

    f.close();
}

void VertexFs::WriteToFile(const char* file_path) {
    ofstream f;
    f.open(file_path);

    // Write number of UINTS needed to be allocated
    f << to_string(size) << '\n';

    f << to_string(data[0]) << '\n';
    f << to_string(data[1]) << '\n';

    for (int label_index = 0; label_index < data[1]; label_index += 1) {
        f << to_string(label_names_offset[label_index]) << '\n';
    }

    for (int label_index = 0; label_index < data[1]; label_index += 1) {
        f << to_string(label_end_offset[label_index]) << '\n';
    }

    for (int vertex_index = 0; vertex_index < data[0]; vertex_index += 1) {
        f << to_string(vertex_offset[vertex_index]) << '\n';
    }

    f.close();
}

UINT* VertexFs::GetVertexIterator(const UINT label) {
    if (label == 0) {
        return vertex_offset;
    }
    return vertex_offset + label_end_offset[label-1];
}

UINT* VertexFs::GetVertexIterator() {
    return vertex_offset;
}

UINT VertexFs::NumVertices(const UINT label) {
    if (label == 0) {
        return this->label_end_offset[label];
    }
    return this->label_end_offset[label] - this->label_end_offset[label-1];
}

UINT VertexFs::NumVertices() {
    return data[0];
}

UINT VertexFs::NumLabels() {
    return data[1];
}

VertexFs::VertexFs(const char* file_path) {
    ULONG file_len;
    vector<string> str_buf;
    string line;
    UINT* data_it;

    ifstream file(file_path);
    getline(file, line);
    file_len = stoul(line);
    this->data = new UINT[file_len];
    this->size = file_len;
    data_it = data;
    while(getline(file,line)) {
        *(data_it) = stoul(line);
        data_it ++;
    }

    this->label_names_offset = this->data + 2;
    this->label_end_offset = this->label_names_offset + this->data[1];
    this->vertex_offset = this->label_end_offset + this->data[1];
}

/*
EdgeFs::EdgeFs(const char* file_path, const UINT num_vertices) {
    FILE *file_stream;
    ULONG file_len;

    file_stream = fopen(file_path, "rb");
    fseek(file_stream, 0, SEEK_END);
    file_len = ftell(file_stream);
    rewind(file_stream);

    this->data = new UINT[file_len];
    this->num_vertices = num_vertices;
    fread(this->data, sizeof(UINT), file_len, file_stream);
    fclose(file_stream);

    for (int i = 0; i < file_len; i += 1) {
        cout << data[i] << endl;
    }

    this->adjacent_vertex_labels = this->data + 1 + num_vertices;
    this->adjacent_vertex_ids = this->adjacent_vertex_labels + this->data[num_vertices];
    this->size = num_vertices + 1 + data[num_vertices] + adjacent_vertex_ids[-1];
}
EdgeFs::EdgeFs(const char* file_path, VertexFs* vfs) {
    ULONG file_len;
    vector<string> str_buf;
    string line;
    UINT* data_it;

    ifstream file(file_path);
    getline(file, line);
    file_len = stoul(line);
    this->data = new UINT[file_len];
    this->size = file_len;
    this->vfs = vfs;
    data_it = data;
    while(getline(file,line)) {
        *(data_it) = stoul(line);
        data_it ++;
    }

    this->num_vertices = data[0];
    this->adjacent_vertex_labels = this->data + 1 + num_vertices;
    this->adjacent_vertex_ids = this->adjacent_vertex_labels + this->data[num_vertices];
    ConstructReverseEdges();
    this->rev_adjacent_vertex_labels = this->reverse_data + 1 + num_vertices;
    this->rev_adjacent_vertex_ids = this->rev_adjacent_vertex_labels + this->reverse_data[num_vertices];
}
*/

EdgeFs::EdgeFs(const char* file_path, const char* rev_file_path, VertexFs* vfs) {
    ULONG file_len;
    vector<string> str_buf;
    string line;
    UINT* data_it;

    ifstream file(file_path);
    getline(file, line);
    file_len = stoul(line);
    this->data = new UINT[file_len];
    this->size = file_len;
    this->vfs = vfs;
    data_it = data;
    while(getline(file,line)) {
        *(data_it) = stoul(line);
        data_it ++;
    }
    file.close();

    ifstream rev_file(rev_file_path);
    getline(rev_file, line);
    file_len = stoul(line);
    this->reverse_data = new UINT[file_len];
    this->reverse_size = file_len;
    data_it = reverse_data;
    while(getline(rev_file,line)) {
        *(data_it) = stoul(line);
        data_it ++;
    }
    rev_file.close();

    this->num_vertices = data[0];
    this->adjacent_vertex_labels = this->data + 1 + num_vertices;
    this->adjacent_vertex_ids = this->adjacent_vertex_labels + this->data[num_vertices];
    this->rev_adjacent_vertex_labels = this->reverse_data + 1 + num_vertices;
    this->rev_adjacent_vertex_ids = this->rev_adjacent_vertex_labels + this->reverse_data[num_vertices];
}

/*
void EdgeFs::ConstructReverseEdges() {
    unordered_map<VERTEX,unordered_map<LABEL, vector<VERTEX>*>*>* map;
    unordered_map<LABEL, vector<VERTEX>*>* label_map;
    unordered_map<LABEL, vector<VERTEX>*>::iterator it;
    vector<vector<VERTEX>*> vertex_queue;
    vector<VERTEX>* print_vertex_queue;
    priority_queue<pair<LABEL, vector<VERTEX>*>*,
                   vector<pair<LABEL, vector<VERTEX>*>*>,
                   CompareLabels> pq;
    pair<LABEL, vector<VERTEX>*>* p;
    reverse_size = 1 + num_vertices;
    UINT adjacent_labels_offset;
    UINT vertex_offset;
    UINT* end_offset_writer;
    UINT* adjacent_labels_writer;

    map = GenerateReverseIndex(reverse_size);
    reverse_data = new UINT[reverse_size];
    reverse_data[0] = num_vertices;
*/
    /*
    cout << "start reverse index enumeration" << endl;
    for (int src_id = 0; src_id < num_vertices; src_id += 1) {
        if(map->count(src_id) == 0) {
            continue;
        }
        unordered_map<LABEL, vector<VERTEX>*>* m2 = map->at(src_id);
        for (int label_id = 0; label_id < 3; label_id += 1) {
            if(m2->count(label_id) == 0) {
                continue;
            }
            vector<VERTEX>* v = m2->at(label_id);
            cout << src_id << '\t' << label_id ;
            for (int index = 0; index < v->size(); index += 1) {
                cout << '\t' << v->at(index);
            }
            cout << endl;
        }

    }*/
/*
    end_offset_writer = reverse_data + 1;
    adjacent_labels_writer = end_offset_writer + num_vertices;
    adjacent_labels_offset = 0;
    vertex_offset = 0;
    for (int v_index = 0; v_index < num_vertices; v_index += 1) {
        label_map = map->find(v_index)->second;
        it = label_map->begin();

        // Arrange byte order by label (ascending)
        for(; it != label_map->end(); ++it) {
            p = new pair<LABEL, vector<VERTEX>*>;
            p->first = it->first;
            p->second = it->second;
            pq.push(p);
        }

        for (int i = pq.size(); i > 0; i -= 1) {
            *(adjacent_labels_writer++) = pq.top()->first;
            vertex_offset += pq.top()->second->size();
            *(adjacent_labels_writer++) = vertex_offset;
            vertex_queue.push_back(pq.top()->second);
            adjacent_labels_offset += 2;
            delete pq.top();
            pq.pop();
        }
        *(end_offset_writer++) = adjacent_labels_offset;
        delete label_map;
    }

    for (int print_idx = 0; print_idx < vertex_queue.size(); print_idx += 1) {
        print_vertex_queue = vertex_queue[print_idx];
        for (int v_idx = 0; v_idx < print_vertex_queue->size(); v_idx += 1) {
            *(adjacent_labels_writer++) = print_vertex_queue->at(v_idx);
        }
        delete print_vertex_queue;
    }
}*/

UINT* EdgeFs::IncomingEdgeIterator(const UINT vertex_id, const LABEL label) {
    UINT count = GetInDegree(vertex_id);
    UINT* label_it = IncomingLabelIterator(vertex_id);
    UINT* edge_it = NULL;

    while (count > 0) {
        if (label == *(label_it)) {
            if ((vertex_id == 0 && label == 0) ||
                label_it == rev_adjacent_vertex_labels)
            {
                edge_it = rev_adjacent_vertex_ids;
            }
            else
                edge_it = rev_adjacent_vertex_ids + *(label_it - 1);
            break;
        }
        if ((vertex_id == 0 && label == 0) ||
            label_it == rev_adjacent_vertex_labels)
            count -= *(label_it + 1);
        else
            count -= *(label_it + 1) - *(label_it - 1);
    }
    return edge_it;
}

UINT* EdgeFs::OutgoingEdgeIteratorEnd(const UINT vertex_id, const LABEL label) {
    int label_offset;
    int label_bound;

    if (vertex_id == 0)
        label_offset = 0;
    else
        label_offset = data[vertex_id];
    label_bound = data[vertex_id + 1];
    while (adjacent_vertex_labels[label_offset] < label &&
           label_offset < label_bound)
    {
        label_offset += 2;
    }

    if (label_offset >= label_bound) {
        return adjacent_vertex_ids + adjacent_vertex_labels[label_offset - 1];
    }

    return adjacent_vertex_ids + adjacent_vertex_labels[label_offset + 1];
}

UINT* EdgeFs::OutgoingEdgeIterator(const UINT vertex_id, const LABEL label) {
    int label_offset;
    int label_bound;

    if (vertex_id == 0)
        label_offset = 0;
    else
        label_offset = data[vertex_id];
    label_bound = data[vertex_id + 1];
    while (adjacent_vertex_labels[label_offset] < label &&
           label_offset < label_bound)
    {
        label_offset += 2;
    }

    if (label_offset >= label_bound) {
        return adjacent_vertex_ids + adjacent_vertex_labels[label_offset - 1];
    }

    if (adjacent_vertex_labels[label_offset] == label) {
        if (label_offset == 0)
            return adjacent_vertex_ids;
        else
            return adjacent_vertex_ids + adjacent_vertex_labels[label_offset - 1];
    } else {
        return adjacent_vertex_ids + adjacent_vertex_labels[label_offset + 1];
    }
}

UINT* EdgeFs::OutgoingEdgeIterator(const UINT vertex_id) {
    // 1) Does vertex id start at 0? -> Yes -> +1 is correct because the offsets
    // stored in filesystem are for ending offsets. The start index of vertex i
    // is the end index of vertex i-1 since the offset is non-closed.
    // 2) FileSystem offset values already account for the fact that each
    //    (label,count) pair is two UINTs packs as binary
    if (vertex_id == 0) {
        // Special case from the rest
        return this->adjacent_vertex_labels;
    }
    UINT offset = *(this->data + vertex_id);
    return this->adjacent_vertex_labels + offset;
}

UINT* EdgeFs::IncomingEdgeIterator(const UINT vertex_id) {
    // 1) Does vertex id start at 0? -> Yes -> +1 is correct because the offsets
    // stored in filesystem are for ending offsets. The start index of vertex i
    // is the end index of vertex i-1 since the offset is non-closed.
    // 2) FileSystem offset values already account for the fact that each
    //    (label,count) pair is two UINTs packs as binary
    if (vertex_id == 0) {
        // Special case from the rest
        return this->rev_adjacent_vertex_labels;
    }
    UINT offset = *(this->reverse_data + vertex_id);
    return this->rev_adjacent_vertex_labels + offset;
}

UINT EdgeFs::GetOutDegree(const UINT vertex_id, const LABEL label) {
    int freq;
    int count = GetOutDegree(vertex_id);
    UINT* label_it = OutgoingLabelIterator(vertex_id);

    while (count != 0) {
        if ((vertex_id == 0 && label == 0) ||
            label_it == adjacent_vertex_labels) 
            freq = *(label_it + 1);
        else 
            freq = *(label_it + 1) - *(label_it - 1);
        if (label == *(label_it)) {
            count = freq;
            break;
        }
        count -= freq;
    }
    return count;
}

UINT EdgeFs::GetInDegree(const UINT vertex_id, const LABEL label) {
    int freq;
    int count = GetInDegree(vertex_id);
    UINT* label_it = IncomingLabelIterator(vertex_id);

    while (count > 0) {
        if ((vertex_id == 0 && label == 0) ||
            label_it == rev_adjacent_vertex_labels)
            freq = *(label_it + 1);
        else 
            freq = *(label_it + 1) - *(label_it - 1);
        if (label == *(label_it)) {
            count = freq;
            break;
        }
        count -= freq;
    }
    if (count < 0)
        count = 0;
    return count;
}

UINT EdgeFs::GetOutDegree(const UINT vertex_id) {
    int count = 0;
    UINT* start = this->OutgoingLabelIterator(vertex_id);
    UINT* end = this->OutgoingLabelIterator(vertex_id + 1);

    if (end == adjacent_vertex_labels) {
        return 0;
    }

    count = *(end - 1);
    if (start != adjacent_vertex_labels)
        count -= *(start - 1);

    if (count < 0) 
        count = 0;
    return count;
}

UINT EdgeFs::GetInDegree(const UINT vertex_id) {
    int count = 0;
    UINT* start = this->IncomingLabelIterator(vertex_id);
    UINT* end = this->IncomingLabelIterator(vertex_id + 1);

    if (end == rev_adjacent_vertex_labels) {
        return 0;
    }

    count = *(end - 1);
    if (start != rev_adjacent_vertex_labels)
        count -= *(start - 1);

    if (count < 0)
        count = 0;
    return count;
}

LABEL VertexFs::GetLabel(const UINT vertex) {
    UINT idx = 0;

    // Check first bucket
    if(vertex < label_end_offset[0]) {
        return 0;
    }

    // Iterate over all labels
    for(int i = 1; i <= data[1]; i += 1) {
        if(label_end_offset[i] > vertex && label_end_offset[i - 1] <= vertex) {
            return i;
        }
    }
    return 99;
}

UINT* EdgeFs::OutgoingLabelIterator(const UINT vertex_id) {
    if (vertex_id == 0) {
        return adjacent_vertex_labels;
    }

    // End of (vertex_id-1) is start of vertex_id
    return adjacent_vertex_labels + data[vertex_id];
}

UINT* EdgeFs::IncomingLabelIterator(const UINT vertex_id) {
    if (vertex_id == 0) {
        return rev_adjacent_vertex_labels;
    }

    // End of (vertex_id-1) is start of vertex_id
    return rev_adjacent_vertex_labels + reverse_data[vertex_id];
}

vector<VERTEX>* EdgeFs::GetOutdegreeVertices(VERTEX src, LABEL l) {
    vector<VERTEX>* acc = new vector<VERTEX>;
    UINT* out1 = OutgoingEdgeIterator(src, l);
    UINT* out2 = OutgoingEdgeIteratorEnd(src, l);
    while (out1 != out2) {
        acc->push_back( *(out1) );
        out1++;
    }
    return acc;
}

vector<VERTEX>* EdgeFs::GetIndegreeVertices(VERTEX src, LABEL l) {
    vector<VERTEX>* acc = new vector<VERTEX>;
    UINT* it = IncomingEdgeIterator(src, l);
    if (it != NULL) {
        for (int count = 0; count < GetInDegree(src, l); count += 1) {
            acc->push_back( *(it) );
            it += 1;
        }
    }
    return acc;
}

bool EdgeFs::HasEdge(const VERTEX from, const VERTEX to) {
    LABEL dst_label = vfs->GetLabel(to);
    UINT* begin = OutgoingEdgeIterator(from,dst_label);
    UINT* end = OutgoingEdgeIteratorEnd(from,dst_label);
    for(; begin != end; begin++) {
        if (*(begin) == to) {
            return true;
        }
    }
    return false;
}

// Diagnostic
#include <assert.h>
int main () {
    VertexFs vfs("vertex_file");
    EdgeFs efs("edge_file", "reverse_edge_file", &vfs);

    cout << "NumVertices" << vfs.NumVertices() << endl;
    cout << "Num Labels" << vfs.NumLabels() << endl;

    for (int i = 0; i < vfs.NumLabels(); i += 1)
        cout << "Num Vertices Label " << i << ":" << vfs.NumVertices(i) << endl;

    for(int label_idx = 0; label_idx < vfs.NumLabels(); label_idx += 1) {
        cout << "Label Index :" << label_idx << " : ";
        UINT* vertex_ptr = vfs.GetVertexIterator(label_idx);
        for(int vertex_idx = 0; vertex_idx < vfs.NumVertices(label_idx); vertex_idx += 1) {
            cout << ' ' << vertex_ptr[vertex_idx];
        }
        cout << endl;
    }

    vector<VERTEX>* v;
    for (int vertex = 0; vertex < vfs.NumVertices(); vertex += 1) {
        cout << "Vertex:" << vertex << endl;
        cout << " In:" << efs.GetInDegree(vertex) << endl;
        cout << " Out:" << efs.GetOutDegree(vertex) << endl;

        for (int label = 0; label < 3; label += 1) {
            cout << "\tLabel" << label << ": " ;
            cout << "\tIn: " << efs.GetInDegree(vertex, label);
            cout << "\tOut: " << efs.GetOutDegree(vertex, label);
            v = efs.GetIndegreeVertices(vertex,label);
            cout << " In Vertices: ";
            for (int i = 0; i < v->size(); i += 1)
                cout << "-" << v->at(i);
            delete v;
            v = efs.GetOutdegreeVertices(vertex,label);
            cout << " Out Vertices: ";
            for (int i = 0; i < v->size(); i += 1)
                cout << "-" << v->at(i);
            delete v;
            UINT* begin = efs.OutgoingEdgeIterator(vertex,label);
            UINT* end = efs.OutgoingEdgeIteratorEnd(vertex,label);

            cout << " Out Vertices: ";
            while (begin != end) {
                cout <<  *(begin) << "-";
                //assert(efs.HasEdge(vertex, *(begin)));
                begin += 1;
            }
            cout << endl;
        }
    }
}
#endif
