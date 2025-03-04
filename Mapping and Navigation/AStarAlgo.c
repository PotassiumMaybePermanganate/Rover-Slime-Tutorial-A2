#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
/* and not not_eq */
#include <iso646.h>
/* add -lm to command line to compile with this header */
#include <math.h>
// #include "mappingPart3.c"
// #include "../PWM and PID/PWMPID.c"

#define map_size_rows 9
#define map_size_cols 11

void printPath(int p_len, int **pathCoord);
int follow_The_Path(int **path, int total_Cost);
void turnLeft();
void turnRight();
void uTurn();
void moveForward();

char map[map_size_rows][map_size_cols] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

/* description of graph node */
struct stop {
    double col, row;
    /* array of indexes of routes from this stop to neighbours in array of all routes */
    int * n;
    int n_len;
    double f, g, h;
    int from;
};

int ind[map_size_rows][map_size_cols] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

/* description of route between two nodes */
struct route {
    /* route has only one direction! */
    int x; /* index of stop in array of all stops of src of this route */
    int y; /* intex of stop in array of all stops od dst of this route */
    double d;
};

int main() {
    int i, j, k, l, b, found;
    int p_len = 0;
    int * path = NULL;
    int c_len = 0;
    int * closed = NULL;
    int o_len = 1;
    int * open = (int*)calloc(o_len, sizeof(int));
    double min, tempg;
    int s;
    int e;
    int current;
    int s_len = 0;
    struct stop * stops = NULL;
    int r_len = 0;
    struct route * routes = NULL;
    int **pathCoord;
    for (i = 1; i < map_size_rows - 1; i++) {
        for (j = 1; j < map_size_cols - 1; j++) {
            if (!map[i][j]) {
                ++s_len;
                stops = (struct stop *)realloc(stops, s_len * sizeof(struct stop));
                int t = s_len - 1;
                stops[t].col = j;
                stops[t].row = i;
                stops[t].from = -1;
                stops[t].g = DBL_MAX;
                stops[t].n_len = 0;
                stops[t].n = NULL;
                ind[i][j] = t;
            }
        }
    }

    for (i = 0; i < map_size_rows; i++) {
        for (j = 0; j < map_size_cols; j++) {
            printf("%2d ", ind[i][j]);
        }
        printf("\n");
    }

    /* index of start stop */
    s = 32;
    /* index of finish stop */
    e = 17;

    //Getting Heuristic Cost of all walkable nodes.
    for (i = 0; i < s_len; i++) {
        stops[i].h = sqrt(pow(stops[e].row - stops[i].row, 2) + pow(stops[e].col - stops[i].col, 2));
    }


    // Neighbours available added to route list
    for (i = 1; i < map_size_rows - 1; i++) {
        for (j = 1; j < map_size_cols - 1; j++) {
            if (ind[i][j] >= 0) {
                for (k = i - 1; k <= i + 1; k++) {
                    for (l = j - 1; l <= j + 1; l++) {
                        if (((k == i) and (l == j)) or ((k != i) and (l != j))) { // disable diagonal pathing
                            continue;
                        }
                   
                        if (ind[k][l] >= 0) {
                            ++r_len;
                            routes = (struct route *)realloc(routes, r_len * sizeof(struct route));
                            int t = r_len - 1;
                            routes[t].x = ind[i][j];
                            routes[t].y = ind[k][l];
                            routes[t].d = sqrt(pow(stops[routes[t].y].row - stops[routes[t].x].row, 2) + pow(stops[routes[t].y].col - stops[routes[t].x].col, 2));
                            ++stops[routes[t].x].n_len;
                            stops[routes[t].x].n = (int*)realloc(stops[routes[t].x].n, stops[routes[t].x].n_len * sizeof(int));
                            stops[routes[t].x].n[stops[routes[t].x].n_len - 1] = t;
                        }
                    }
                }
            }
        }
    }

    open[0] = s;
    stops[s].g = 0;
    stops[s].f = stops[s].g + stops[s].h;
    found = 0;

    while (o_len and not found) {
        min = DBL_MAX;

        for (i = 0; i < o_len; i++) {
            if (stops[open[i]].f < min) {
                current = open[i];
                min = stops[open[i]].f;
            }
        }

        if (current == e) {
            found = 1;

            ++p_len;
            path = (int*)realloc(path, p_len * sizeof(int));
            path[p_len - 1] = current;
            while (stops[current].from >= 0) {
                current = stops[current].from;
                ++p_len;
                path = (int*)realloc(path, p_len * sizeof(int));
                path[p_len - 1] = current;
            }
        }

        for (i = 0; i < o_len; i++) {
            if (open[i] == current) {
                if (i not_eq (o_len - 1)) {
                    for (j = i; j < (o_len - 1); j++) {
                        open[j] = open[j + 1];
                    }
                }
                --o_len;
                open = (int*)realloc(open, o_len * sizeof(int));
                break;
            }
        }

        ++c_len;
        closed = (int*)realloc(closed, c_len * sizeof(int));
        closed[c_len - 1] = current;

        for (i = 0; i < stops[current].n_len; i++) {
            b = 0;

            for (j = 0; j < c_len; j++) {
                if (routes[stops[current].n[i]].y == closed[j]) {
                    b = 1;
                }
            }

            if (b) {
                continue;
            }

            tempg = stops[current].g + routes[stops[current].n[i]].d;

            b = 1;

            if (o_len > 0) {
                for (j = 0; j < o_len; j++) {
                    if (routes[stops[current].n[i]].y == open[j]) {
                        b = 0;
                    }
                }
            }

            if (b or (tempg < stops[routes[stops[current].n[i]].y].g)) {
                stops[routes[stops[current].n[i]].y].from = current;
                stops[routes[stops[current].n[i]].y].g = tempg;
                stops[routes[stops[current].n[i]].y].f = stops[routes[stops[current].n[i]].y].g + stops[routes[stops[current].n[i]].y].h;

                if (b) {
                    ++o_len;
                    open = (int*)realloc(open, o_len * sizeof(int));
                    open[o_len - 1] = routes[stops[current].n[i]].y;
                }
            }
        }
    }

    for (i = 0; i < map_size_rows; i++) {
        for (j = 0; j < map_size_cols; j++) {
            if (map[i][j]) {
                putchar(0xdb);
            } else {
                b = 0;
                for (k = 0; k < p_len; k++) {
                    if (ind[i][j] == path[k]) {
                        ++b;
                    }
                }
                if (b) {
                    putchar('x');
                } else {
                    putchar('.');
                }
            }
        }
        putchar('\n');
    }
    
    
    //Memory Allocation for 2D path array
    pathCoord = malloc(sizeof(int*) * p_len);
            if(pathCoord == NULL){
                exit(1);
            }
    
    for (i = 0; i < p_len; ++i) {
        pathCoord[i] = malloc(sizeof(int) * 2);
        if(pathCoord[i] == NULL){
            exit(1);
        }
    }

    if (not found) {
        puts("IMPOSSIBLE");
    } else {
        printf("path cost is %d:\n", p_len);
        for (j = 0, i = p_len - 1; j<p_len,i >= 0; j++, i--) {
            printf("(%1.0f, %1.0f)\n", stops[path[i]].col, stops[path[i]].row);
            pathCoord[j][0] = stops[path[i]].col;
            pathCoord[j][1] = stops[path[i]].row;
        }
    }



    printPath(p_len, pathCoord);

    printf("\n[*]Following is the car movement to the end point: \n\n");
    follow_The_Path(pathCoord, p_len);




    for (i = 0; i < s_len; ++i) {
        free(stops[i].n);
        free(pathCoord[i]);
    }
    free(stops);
    free(routes);
    free(path);
    free(open);
    free(closed);
    free(pathCoord);

    return 0;
}
//int pathCoord[15][2];
int follow_The_Path(int **path, int total_Cost){
    int i;
    for(i = 0; i < total_Cost -1 ; i++){ // total_cost - 1 because last node don't have to move anymore.
        if(i == 0){
            moveForward();
        }else if((path[i][1] == path[i-1][1] and path[i][1] < path[i+1][1] and path[i][0] < path[i-1][0]) or // Horizontal Left <-
        (path[i][1] == path[i-1][1] and path[i][1] > path[i+1][1] and path[i][0] > path[i-1][0]) or  // Horizontal Left ->
        (path[i][0] == path[i-1][0] and path[i][0] > path[i+1][0] and path[i][1] < path[i-1][1]) or // Vertical Left ^ 
        (path[i][0] == path[i-1][0] and path[i][0] < path[i+1][0] and path[i][1] > path[i-1][1])) { // Vertical Left _ 

            turnLeft();
            moveForward();

        }else if((path[i][1] == path[i-1][1] and path[i][1]>path[i+1][1] and path[i][0] < path[i-1][0]) or  // Horizontal Right <-
        (path[i][1] == path[i-1][1] and path[i][1] < path[i+1][1] and path[i][0] > path[i-1][0]) or // Horizontal Right ->
        (path[i][0] == path[i-1][0] and path[i][0] < path[i+1][0] and path[i][1] < path[i-1][1]) or // Vertial Right ^
        (path[i][0] == path[i-1][0] and path[i][0] > path[i+1][0] and path[i][1] > path[i-1][1])) { // Vertial Right _

            turnRight();
            moveForward();

        }else{
            moveForward();
        }
    }
}




void turnLeft(){
    // notchTurnLeft();
    printf("Turning Left 90 Degrees!\n");
}

void turnRight(){  
    notchTurnRight();
    printf("Turning right 90 Degrees!\n");
}

void uTurn(){
    turnLeft();
    turnLeft();
}

void moveForward(){
    //Car moves forward by MOVE_DISTANCE [13.5cm]
    // moveForward(8, 13);
    printf("Moving Forward!\n");
}

void printPath(int p_len, int **pathCoord){
        printf("\n**********\n");
    for (int i = 0; i < p_len; i++) {
            printf("(%d, %d)\n", pathCoord[i][0], pathCoord[i][1] );
        }
}