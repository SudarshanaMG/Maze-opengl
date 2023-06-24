#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAX_ROWS 30
#define MAX_COLS 30
typedef struct
{
    bool left;
    bool right;
    bool top;
    bool bottom;
    bool visited;
} Cell;

typedef struct
{
    int rows;
    int cols;
    Cell cells[MAX_ROWS][MAX_COLS];
    int startRow;
    int startCol;
    int endRow;
    int endCol;
} MazeGrid;

int window_width = 1000;
int window_height = 1000;
int cell_size = 30;

MazeGrid maze;
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void initialize_maze()
{
    for (int row = 0; row < maze.rows; row++)
    {
        for (int col = 0; col < maze.cols; col++)
        {
            maze.cells[row][col].left = true;
            maze.cells[row][col].right = true;
            maze.cells[row][col].top = true;
            maze.cells[row][col].bottom = true;
            maze.cells[row][col].visited = false;
        }
    }
}
void generate_maze(int row, int col)
{
    maze.cells[row][col].visited = true;

    while (true)
    {
        // Create an array to store the unvisited neighbors
        int unvisitedNeighbors[4] = {-1, -1, -1, -1};
        int numUnvisited = 0;

        // Check if each neighboring cell is within the maze and unvisited
        if (row > 0 && !maze.cells[row - 1][col].visited)
        {
            unvisitedNeighbors[numUnvisited] = 0; // Top neighbor
            numUnvisited++;
        }
        if (row < maze.rows - 1 && !maze.cells[row + 1][col].visited)
        {
            unvisitedNeighbors[numUnvisited] = 1; // Bottom neighbor
            numUnvisited++;
        }
        if (col > 0 && !maze.cells[row][col - 1].visited)
        {
            unvisitedNeighbors[numUnvisited] = 2; // Left neighbor
            numUnvisited++;
        }
        if (col < maze.cols - 1 && !maze.cells[row][col + 1].visited)
        {
            unvisitedNeighbors[numUnvisited] = 3; // Right neighbor
            numUnvisited++;
        }

        // If there are unvisited neighbors
        if (numUnvisited > 0)
        {
            // Choose a random unvisited neighbor
            int randomIndex = rand() % numUnvisited;
            int randomNeighbor = unvisitedNeighbors[randomIndex];

            // Remove the wall between the current cell and the chosen neighbor
            switch (randomNeighbor)
            {
            case 0: // Top neighbor
                maze.cells[row][col].top = false;
                maze.cells[row - 1][col].bottom = false;
                generate_maze(row - 1, col);
                break;
            case 1: // Bottom neighbor
                maze.cells[row][col].bottom = false;
                maze.cells[row + 1][col].top = false;
                generate_maze(row + 1, col);
                break;
            case 2: // Left neighbor
                maze.cells[row][col].left = false;
                maze.cells[row][col - 1].right = false;
                generate_maze(row, col - 1);
                break;
            case 3: // Right neighbor
                maze.cells[row][col].right = false;
                maze.cells[row][col + 1].left = false;
                generate_maze(row, col + 1);
                break;
            }
        }
        else
        {
            // No unvisited neighbors, backtrack to the previous cell
            return;
        }
    }
}

void draw_maze()
{
    // Calculate the offset to center the maze grid
    int offsetX = (window_width - (maze.cols * cell_size)) / 2;
    int offsetY = (window_height - (maze.rows * cell_size)) / 2;

    for (int row = 0; row < maze.rows; row++)
    {
        for (int col = 0; col < maze.cols; col++)
        {
            int x = col * cell_size + offsetX;
            int y = row * cell_size + offsetY;

            if (maze.cells[row][col].left)
            {
                glLineWidth(5.0);
                glBegin(GL_LINES);
                glVertex2i(x, y);
                glVertex2i(x, y + cell_size);
                glEnd();
            }
            if (maze.cells[row][col].right)
            {
                glLineWidth(5.0);
                glBegin(GL_LINES);
                glVertex2i(x + cell_size, y);
                glVertex2i(x + cell_size, y + cell_size);
                glEnd();
            }
            if (maze.cells[row][col].top)
            {
                glLineWidth(5.0);
                glBegin(GL_LINES);
                glVertex2i(x, y);
                glVertex2i(x + cell_size, y);
                glEnd();
            }
            if (maze.cells[row][col].bottom)
            {
                glLineWidth(5.0);
                glBegin(GL_LINES);
                glVertex2i(x, y + cell_size);
                glVertex2i(x + cell_size, y + cell_size);
                glEnd();
            }
        }
    }

    // Draw the starting point
    int startX = maze.startCol * cell_size + offsetX;
    int startY = maze.startRow * cell_size + offsetY;
    glColor3f(0.0, 1.0, 0.0); // Green color
    glRecti(startX, startY, startX + cell_size, startY + cell_size);

    // Draw the ending point
    int endX = maze.endCol * cell_size + offsetX;
    int endY = maze.endRow * cell_size + offsetY;
    glColor3f(1.0, 0.0, 0.0); // Red color
    glRecti(endX, endY, endX + cell_size, endY + cell_size);
    glutSwapBuffers();
}
void display_maze()
{
    glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, window_width, window_height, 0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(0.7, 0.5, 0.5);
        draw_maze();
    glutSwapBuffers();
}
int main(int argc, char **argv)
{
    srand(time(NULL)); // Initialize random seed
    /* printf("Enter the number of rows (1-%d): ", MAX_ROWS);
    scanf("%d", &maze.rows);
    printf("Enter the number of columns (1-%d): ", MAX_COLS);
    scanf("%d", &maze.cols); */
    maze.rows = 20;
    maze.cols = 20;
    if (maze.rows <= 0 || maze.rows > MAX_ROWS || maze.cols <= 0 || maze.cols > MAX_COLS)
    {
        printf("Invalid maze dimensions.\n");
        return 1;
    }

    maze.startRow = 0;
    maze.startCol = 0;
    maze.endRow = maze.rows - 1;
    maze.endCol = maze.cols - 1;

    initialize_maze();
    generate_maze(maze.startRow, maze.startCol);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Maze Generator");

    glutDisplayFunc(display_maze);
    glGetError();
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
