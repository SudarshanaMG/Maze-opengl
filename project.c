#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAX_ROWS 30
#define MAX_COLS 30
int df = 10, count,tempx,tempy;
int textX = 700;
int textY = 100;
char t[2];
clock_t start, end;
void winscreen();
void menu();
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
    int playerRow;
    int playerCol;
} MazeGrid;

MazeGrid maze;

void output(int x, int y, char *string)
{
    glRasterPos2i(x, y);
    for (int i = 0; i < strlen(string); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}

int window_width = 1000;
int window_height = 1000;
int cell_size = 30;

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
    maze.playerRow = maze.startRow;
    maze.playerCol = maze.startCol;
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
            return; // No unvisited neighbors, backtrack to the previous cell
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
    // player block
    int playerX = maze.playerCol * cell_size + offsetX;
    int playerY = maze.playerRow * cell_size + offsetY;
    glColor3f(0.0, 0.0, 1.0); // Blue color
    glRecti(playerX, playerY, playerX + cell_size, playerY + cell_size);

    // starting point
    int startX = maze.startCol * cell_size + offsetX;
    int startY = maze.startRow * cell_size + offsetY;
    glColor3f(0.0, 1.0, 0.0); // Green color
    glRecti(startX, startY, startX + cell_size, startY + cell_size);

    // ending point
    int endX = maze.endCol * cell_size + offsetX;
    int endY = maze.endRow * cell_size + offsetY;
    glColor3f(1.0, 0.0, 0.0); // Red color
    glRecti(endX, endY, endX + cell_size, endY + cell_size);

    if (maze.playerRow == maze.endRow && maze.playerCol == maze.endCol)
    {
        df = 6;
    }
    glutSwapBuffers();
}

void controlkeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP: // Up arrow key
        if (!maze.cells[maze.playerRow][maze.playerCol].top)
            maze.playerRow--;
        break;
    case GLUT_KEY_DOWN: // Down arrow key
        if (!maze.cells[maze.playerRow][maze.playerCol].bottom)
            maze.playerRow++;
        break;
    case GLUT_KEY_LEFT: // Left arrow key
        if (!maze.cells[maze.playerRow][maze.playerCol].left)
            maze.playerCol--;
        break;
    case GLUT_KEY_RIGHT: // Right arrow key
        if (!maze.cells[maze.playerRow][maze.playerCol].right)
            maze.playerCol++;
        break;
    case 27:
        df = 0;
        break;
    }
    glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{

    if (df == 10 && key == 13)
        df = 0;
    else if ((df == 0 || df == 5 || df == 6) && key == '1')
    {
        df = 4;
    }
    else if (df == 4 && key == '1')
    {
        df = 1;
        maze.rows = 10;
        maze.cols = 10;
        maze.endRow = maze.rows - 1;
        maze.endCol = maze.cols - 1;
        initialize_maze();
        generate_maze(maze.startRow, maze.startCol); // for generation of new mazes
        maze.playerRow = maze.startRow;
        maze.playerCol = maze.startCol; // for restarting of maze
        start = clock();
        controlkeys((int)key, x, y);
        glutPostRedisplay();
    }
    else if (df == 4 && key == '2')
    {
        df = 1;
        maze.rows = 15;
        maze.cols = 15;
        maze.endRow = maze.rows - 1;
        maze.endCol = maze.cols - 1;
        initialize_maze();
        generate_maze(maze.startRow, maze.startCol); // for generation of new mazes
        maze.playerRow = maze.startRow;
        maze.playerCol = maze.startCol; // for restarting of maze
        start = clock();
        controlkeys((int)key, x, y);
        glutPostRedisplay();
    }
    else if (df == 4 && key == '3')
    {
        df = 1;
        maze.rows = 20;
        maze.cols = 20;
        maze.endRow = maze.rows - 1;
        maze.endCol = maze.cols - 1;
        initialize_maze();
        generate_maze(maze.startRow, maze.startCol); // for generation of new mazes
        maze.playerRow = maze.startRow;
        maze.playerCol = maze.startCol; // for restarting of maze
        start = clock();
        controlkeys((int)key, x, y);
        glutPostRedisplay();
    }
    else if (df == 4 && key == '4')
    {
        df = 1;
        maze.rows=tempx;
        maze.cols=tempy;
        maze.endRow = maze.rows - 1;
        maze.endCol = maze.cols - 1;
        initialize_maze();
        generate_maze(maze.startRow, maze.startCol); // for generation of new mazes
        maze.playerRow = maze.startRow;
        maze.playerCol = maze.startCol; // for restarting of maze
        start = clock();
        controlkeys((int)key, x, y);
        glutPostRedisplay();
    }
    else if (df == 0 && key == '2')
        df = 2;
    else if (df == 0 && key == '3')
        exit(0);
    else if (key == 27)
    {
        df = 0;
        key == 0;
    }
    if ((key == '0' || key == '1') && (df == 5 || df == 1))
    {
        df = 1;
        controlkeys((int)key, x, y);
        glutPostRedisplay();
    }
    glutPostRedisplay();
}
void winscreen()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(0.0, 1.0, 0.0);
    output(textX + 100, textY, "CONGRATS!!!");
    glColor3f(0.0, 1.0, 1.0);
    output(textX - 100, textY + 100, "YOU HAVE SUCCEEDED IN FINDING OUT THE PATH");
    output(textX + 30, textY + 200, "YOU TOOK ");
    sprintf(t, "%d", count);
    output(textX + 180, textY + 200, t);
    output(textX + 230, textY + 200, "SECONDS");
    glColor3f(1.0, 0.0, 1.0);
    output(textX - 20, textY + 300, "* PRESS ESC TO GO TO MAIN MENU");
    output(textX - 20, textY + 400, "* PRESS 1 TO RESTART THE GAME");
    glFlush();
}
void menu()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(1.0, 1.0, 0.0);
    output(textX + 100, textY + 100, "MENU");
    output(textX, textY + 120, "--------------------");
    glColor3f(0.0, 1.0, 1.0);
    output(textX + 50, textY + 200, "1.EASY MODE");
    output(textX + 50, textY + 300, "2.MEDIUM MODE");
    output(textX + 50, textY + 400, "3.DIFFICULT MODE");
    output(textX + 50, textY + 500, "4.MANUAL MODE");
    output(textX + 50, textY + 600, "5.AUTO PATH FINDING");
    glColor3f(1.0, 0.0, 1.0);
    output(textX - 50, textY + 700, "* PRESS ESC TO GO TO MAIN MENU");
    glFlush();
}
void instructions()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 0.0);
    output(textX + 100, textY + 50, "INSTRUCTIONS:");
    glBegin(GL_LINES);
    glVertex2f(textX + 80.0, textY + 70.0);
    glVertex2f(textX + 550.0, textY + 70.0);
    glEnd();
    glColor3f(0, 1, 0);
    output(textX - 200, textY + 200, "* TO MOVE THE POINT USE ARROW KEYS");
    output(textX - 200, textY + 300, "* FIND THE WAY TO MOVE INTO THE MAZE AND GET OUT");
    output(textX - 200, textY + 400, "* GREEN COLOURED BLOCK INDICATE THE POINT FROM WHERE YOU HAVE TO START");
    output(textX - 200, textY + 500, "* RED COLOURED BLOCK INDICATE THE POINT WHERE YOU HAVE TO REACH");
    output(textX - 200, textY + 600, "* YOU WILL HAVE TO HURRY AS YOU HAVE ONLY 60 SECONDS TIME");
    output(textX - 200, textY + 700, "* PRESS ESC TO GO TO MAIN MENU");
    glFlush();
}
void timeover()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0, 0);
    output(textX + 200, textY, "TIMEOVER");
    glColor3f(0, 1, 1);
    output(textX + 100, textY + 100, "YOU HAVE LOST THE GAME");
    output(textX + 100, textY + 200, "BETTER LUCK NEXT TIME");
    glColor3f(1, 1, 0);
    output(textX + 50, textY + 300, "* PRESS ESC TO GO TO MAIN MENU");
    output(textX + 50, textY + 400, "* PRESS 1 TO RESTART THE GAME");
    glFlush();
}
void startscreen()
{
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, window_width, window_height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0, 1.0, 0.0);
    output(textX, textY + 150, "WELCOME TO MAZE GENERATION AND SOLVING");
    glColor3f(0.0, 1.0, 1.0);
    output(textX + 150, textY + 300, "1.NEW GAME");
    glColor3f(0.0, 1.0, 1.0);
    output(textX + 150, textY + 400, "2.INSTRUCTIONS");
    glColor3f(0.0, 1.0, 1.0);
    output(textX + 150, textY + 500, "3.QUIT");
}
void frontscreen()
{
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, window_width, window_height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    char collegeName[] = "JSS ACADEMY OF TECHNICAL EDUCATION";
    char department[] = "DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING";
    char projectName[] = "A MINI PROJECT ON :";
    char proName[] = "MAZE GENERATION USING DFS";
    char doneBy[] = "By :";
    char byName[] = "Aneesh Khatawkar (1JS17CS350) and Sudarshana MG (1JS20CS167)";
    char underGuide[] = "Under The Guidence of :";
    char lecturerName[] = " Dr. Sharana B.G - Associate Professor - Department of CSE";
    char next[] = "Press ENTER to go to next screen";

    glColor3f(1.0, 0.0, 0.0);
    output(textX, textY, collegeName);
    glColor3f(0.0, 1.0, 0.0);
    output(textX - 100, textY + 100, department);

    glColor3f(0.0, 0.0, 1.0);
    output(textX + 100, textY + 200, projectName);

    glColor3f(1.0, 1.0, 0.0);
    output(textX + 40, textY + 250, proName);

    glColor3f(1.0, 0.0, 1.0);
    output(textX + 100, textY + 400, underGuide);

    glColor3f(0.0, 1.0, 1.0);
    output(textX - 70, textY + 450, lecturerName);

    glColor3f(1.0, 0.0, 1.0);
    output(textX + 200, textY + 600, doneBy);

    glColor3f(0.0, 1.0, 0.0);
    output(textX - 100, textY + 650, byName);

    glColor3f(1.0, 1.0, 1.0);
    output(textX + 300, textY + 850, next);
}
void idle()
{
    if (df == 1)
    {
        end = clock();
        count = (end - start) / CLOCKS_PER_SEC;
        if (count == 60)
        {
            df = 4;
        }
        else if ((count < 60) && (maze.playerRow == maze.endRow && maze.playerCol == maze.endCol))
        {
            df = 5;
            winscreen();
        }
    }
    glutPostRedisplay();
}
void display_maze()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (df == 10)
        frontscreen();
    else if (df == 0)
        startscreen();
    else if (df == 1)
    {
        glViewport(0, 0, window_width, window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, window_width, window_height, 0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(0.0, 0.0, 1.0);
        output(textX + 50, textY, "TIME REMAINING : ");
        output(textX + 50, textY + 50, "HURRY UP!!!!");
        glColor3f(1, 0, 0);
        output(textX + 250, textY + 50, "Time is running out");
        sprintf(t, "%d", 60 - count);
        output(textX + 300, textY, t);
        glColor3f(0.8, 0.5, 0.6);
        draw_maze();
    }
    else if (df == 2)
        instructions();
    else if (df == 3)
    {
        exit(1);
    }
    else if (df == 4)
    {
        menu();
    }
    else if (df == 5)
    {
        timeover();
    }
    else if (df == 6)
    {
        winscreen();
    }
    glutSwapBuffers();
}
int main(int argc, char **argv)
{
    // Initialize random seed
    printf("Enter the number of rows for maze(1-%d): ", MAX_ROWS);
    scanf("%d", &maze.rows);
    printf("Enter the number of columns for maze(1-%d): ", MAX_COLS);
    scanf("%d", &maze.cols);
    if (maze.rows <= 0 || maze.rows > MAX_ROWS || maze.cols <= 0 || maze.cols > MAX_COLS)
    {
        printf("Invalid maze dimensions.\n");
        return 1;
    }
    tempx=maze.rows;
    tempy=maze.cols;
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
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(controlkeys);

    glutMainLoop();
    return 0;
}
