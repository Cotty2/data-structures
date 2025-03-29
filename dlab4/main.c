#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 2001

char map[SIZE][SIZE];
int visited[SIZE][SIZE];
int distance[SIZE][SIZE];
int prev_x[SIZE][SIZE], prev_y[SIZE][SIZE];

int Qx[SIZE*SIZE], Qy[SIZE*SIZE];
int front = 0, rear = 0;

int M, N;

int canUp(int x, int y) {
    if (y == 0) return 0;
    switch(map[x][y]) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return (map[x][y-1] & 8) == 0;
        default: return 0;
    }
}

int canDown(int x, int y) {
    if (y == N-1) return 0;
    switch(map[x][y]) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return (map[x][y] & 8) == 0;
        default: return 0;
    }
}

int canLeft(int x, int y) {
    if (x == 0) return 0;
    switch(map[x][y]) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return (map[x-1][y] & 4) == 0;
        default: return 0;
    }
}

int canRight(int x, int y) {
    if (x == M-1) return 0;
    switch(map[x][y]) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return (map[x][y] & 4) == 0;
        default: return 0;
    }
}

void bfs(int start_x, int start_y) {
    front = rear = 0;
    Qx[rear] = start_x;
    Qy[rear] = start_y;
    rear++;
    
    visited[start_x][start_y] = 1;
    distance[start_x][start_y] = 0;
    
    while (front < rear) {
        int x = Qx[front];
        int y = Qy[front];
        front++;
        
        if (canUp(x, y)) {
            int nx = x;
            int ny = y - 1;
            if (!visited[nx][ny]) {
                visited[nx][ny] = 1;
                distance[nx][ny] = distance[x][y] + 1;
                prev_x[nx][ny] = x;
                prev_y[nx][ny] = y;
                Qx[rear] = nx;
                Qy[rear] = ny;
                rear++;
            }
        }
        
        if (canDown(x, y)) {
            int nx = x;
            int ny = y + 1;
            if (!visited[nx][ny]) {
                visited[nx][ny] = 1;
                distance[nx][ny] = distance[x][y] + 1;
                prev_x[nx][ny] = x;
                prev_y[nx][ny] = y;
                Qx[rear] = nx;
                Qy[rear] = ny;
                rear++;
            }
        }
        
        if (canLeft(x, y)) {
            int nx = x - 1;
            int ny = y;
            if (!visited[nx][ny]) {
                visited[nx][ny] = 1;
                distance[nx][ny] = distance[x][y] + 1;
                prev_x[nx][ny] = x;
                prev_y[nx][ny] = y;
                Qx[rear] = nx;
                Qy[rear] = ny;
                rear++;
            }
        }
        
        if (canRight(x, y)) {
            int nx = x + 1;
            int ny = y;
            if (!visited[nx][ny]) {
                visited[nx][ny] = 1;
                distance[nx][ny] = distance[x][y] + 1;
                prev_x[nx][ny] = x;
                prev_y[nx][ny] = y;
                Qx[rear] = nx;
                Qy[rear] = ny;
                rear++;
            }
        }
    }
}

void mark_path(int end_x, int end_y) {
    int x = end_x;
    int y = end_y;
    
    while (x != 0 || y != 0) {
        map[x][y] = '*';
        int new_x = prev_x[x][y];
        int new_y = prev_y[x][y];
        x = new_x;
        y = new_y;
    }
    map[0][0] = '*';
}

int main() {
    FILE *input = fopen("7x9.txt", "r");
    if (!input) {
        perror("Cannot open input file");
        return 1;
    }
    
    fscanf(input, "%d %d", &M, &N);
    for (int i = 0; i < M; i++) {
        fscanf(input, "%s", map[i]);
    }
    fclose(input);
    
    memset(visited, 0, sizeof(visited));
    memset(distance, -1, sizeof(distance));
    
    bfs(0, 0);
    
    if (distance[M-1][N-1] == -1) {
        printf("No path found!\n");
    } else {
        printf("%d\n", distance[M-1][N-1]);
        
        mark_path(M-1, N-1);
        
        FILE *output = fopen("output.txt", "w");
        for (int i = 0; i < M; i++) {
            fprintf(output, "%s\n", map[i]);
        }
        fclose(output);
    }
    
    return 0;
}