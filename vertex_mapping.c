#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define MAX_V 512
#define MAX_E MAX_V * 8

int v_map[MAX_V][2];
int edge[MAX_E][2];

// 顶点结构体
typedef struct {
    int id;
    int inDegree;
    int map_id;
} Vertex;

Vertex* v_list[MAX_V];
int num_v, num_e;
bool used[MAX_V];
bool visited[MAX_V];

int compareVertices(const void* a, const void* b) {
    Vertex** vertexA = (Vertex**)a;
    Vertex** vertexB = (Vertex**)b;
    return (*vertexB)->inDegree - (*vertexA)->inDegree;  // 降序排列
}

int compareId(const void* a, const void* b) {
    Vertex** pa = (Vertex**)a;
    Vertex** pb = (Vertex**)b;
    int idA = (*pa)->id;
    int idB = (*pb)->id;
    return idA - idB;
}

void deg_sort() {
    for (int i = 0; i < MAX_V; i++) {
        v_list[i] = (Vertex*)malloc(sizeof(Vertex));
        v_list[i]->id = -1;
        v_list[i]->inDegree = 0;
    }

    for(int i = 0; i < num_e; i++) {
        int dest = edge[i][1];
        bool found = false;
        for(int j = 0; j < num_v; j++) {
            if(v_list[j]->id == dest) {
                v_list[j]->inDegree++;
                found = true;
                break;
            }
        }
        if (!found) {
            v_list[num_v]->id = dest;
            v_list[num_v]->inDegree = 1;
            num_v++;
            visited[dest] = true;
        }
    }

    qsort(v_list, num_v, sizeof(Vertex*), compareVertices);
    for (int i = 0; i < num_v; i++) {
        v_list[i]->map_id = i;
        // printf("%d %d\n", v_list[i]->id, v_list[i]->inDegree);
    }
    for (int i = 0; i < num_e; i++) {
        if (!visited[edge[i][0]]) {
            v_list[num_v]->id = edge[i][0];
            v_list[num_v]->map_id = num_v;
            num_v++;
        }
        if (!visited[edge[i][1]]) {
            v_list[num_v]->id = edge[i][1];
            v_list[num_v]->map_id = num_v;
            num_v++;
        }
    }
    // qsort(v_list, num_v, sizeof(Vertex*), compareId);
    return;
}

void hub_sort() {
    for (int i = 0; i < MAX_V; i++) {
        v_list[i] = (Vertex*)malloc(sizeof(Vertex));
        v_list[i]->id = -1;
        v_list[i]->inDegree = 0;
    }

    for(int i = 0; i < num_e; i++) {
        int dest = edge[i][1];
        bool found = false;
        for(int j = 0; j < num_v; j++) {
            if(v_list[j]->id == dest) {
                v_list[j]->inDegree++;
                found = true;
                break;
            }
        }
        if (!found) {
            v_list[num_v]->id = dest;
            v_list[num_v]->inDegree = 1;
            num_v++;
            visited[dest] = true;
        }
    }

    qsort(v_list, num_v, sizeof(Vertex*), compareVertices);
    // 计算平均入度
    int avg_in_degree = 0;
    for (int i = 0; i < num_v; i++) {
        avg_in_degree += v_list[i]->inDegree;
    }
    avg_in_degree /= num_v;
    int new_num_v = 0;
    for (int i = 0; i < num_v; i++) {
        if (v_list[i]->inDegree < avg_in_degree)
            break;
        v_list[i]->map_id = i;
        used[i] = true;
        new_num_v++;
        // printf("%d %d\n", v_list[i]->id, v_list[i]->inDegree);
    }
    int cur_id = 0;
    for (int i = new_num_v; i < num_v; i++) {
        if (v_list[i]->id >= new_num_v) {
            v_list[i]->map_id = v_list[i]->id;
            used[v_list[i]->id] = true;
        }
    }
    for (int i = new_num_v; i < num_v; i++) {
        // 需要重新分配编号
        if (v_list[i]->id < new_num_v) {
            while (used[cur_id]) {
                cur_id++;
            }
            v_list[i]->map_id = cur_id;
            used[cur_id] = true;
            cur_id++;
        }
    }

    for (int i = 0; i < num_e; i++) {
        if (!visited[edge[i][0]]) {
            if (!used[edge[i][0]]) {
                v_list[num_v]->id = edge[i][0];
                v_list[num_v]->map_id = edge[i][0];
                used[v_list[num_v]->id] = true;
                num_v++;
                visited[edge[i][0]] = true;
            }
        } 
        if (!visited[edge[i][1]]) {
            if (!used[edge[i][1]]) {
                v_list[num_v]->id = edge[i][1];
                v_list[num_v]->map_id = edge[i][1];
                used[v_list[num_v]->id] = true;
                num_v++;
                visited[edge[i][1]] = true;
            }
        }
    }

    for (int i = 0; i < num_e; i++) {
        if (!visited[edge[i][0]]) {
            if (used[edge[i][0]]) {
                while (used[cur_id])
                    cur_id++;
                v_list[num_v]->id = edge[i][0];
                v_list[num_v]->map_id = cur_id;
                used[cur_id] = true;
                num_v++;
                cur_id++;
                visited[edge[i][0]] = true;
            }
        } 
        if (!visited[edge[i][1]]) {
            if (used[edge[i][1]]) {
                while (used[cur_id])
                    cur_id++;
                v_list[num_v]->id = edge[i][1];
                v_list[num_v]->map_id = cur_id;
                used[cur_id] = true;
                num_v++;
                cur_id++;
                visited[edge[i][1]] = true;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    char* csr_fn = "data/graph-128-clean.txt";
    FILE* csr_fp = fopen(csr_fn, "r");
    if(!csr_fp){
        fprintf(stderr, "%s: %s\n", csr_fn, strerror(errno));
        exit(1);
    }

    char buf[1000];
    int v_from, v_to, v_prev = 0;
    while(fgets(buf, 1000, csr_fp)){
        sscanf(buf, "%u %u", &v_from, &v_to);
        edge[num_e][0] = v_from;
        edge[num_e][1] = v_to;
        num_e++;
    }
    fclose(csr_fp);

    if (argv[1][0] == '0') {
        deg_sort();
    } else {
        hub_sort();
    }

    FILE* out_fp = fopen("graph.map", "w");
    if(!out_fp){
        fprintf(stderr, "%s: %s\n", "graph.map", strerror(errno));
        exit(1);
    }


    for(int i = 0; i < num_v; i++) {
        Vertex* v = v_list[i];
        fprintf(out_fp, "%u %u\n", v->id, v->map_id);
    }
    fclose(out_fp);

    // for(int i = 0; i < num_v; i++) {
    //     Vertex* v = v_list[i];
    //     printf("%u %u\n", v->id, v->map_id);
    
    // }

    return 0;  
}