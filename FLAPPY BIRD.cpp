#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <iomanip>
#include<ctype.h>
#include<mmsystem.h>
#pragma comment (lib,"Winmm.lib")
using namespace std;

const int playerARRsize = 20;

//global variables
int positionofthepipe[2];
int positionofthegap[2];
int pipecontrol[2];
char bird[2][6] = { '/','-','-','o','\\',' ',
                    '|',' ',' ','_',' ','>' };
int birdpos = 6;
int score = 0;

const int screenwidth = 90;
const int screenheight = 26;
const int playscreenwidth = 70;//separtes main game and score area
//const int scoreareawidth = 20; //90-70=20
int gapsize = 11;
//const int pipedif = 45;
float birdvelocity = 0.0;
const float gravity = 0.5;
const int birdwidth = 6; // Number of characters of birds width  use in drawing and deleting bird after setting its position
const int birdheight = 2; // Number of characters of birds height use in drawing and deleting bird after setting its position
bool ispaused = false;      //default
int sleepindex = 100;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);//a link to console window that allows you to control how text is displayed
COORD cursorposition;//it stores current coordinates of x and y

struct player //type for player with their name and the score 
{
    int score; //variable that is going to be updated
    string name;
};

player arrAllPlayers[playerARRsize]; //global array of all players


void jumpsound()
{
    PlaySound(TEXT("jump.wav"), NULL, SND_FILENAME | SND_ASYNC);//sound play when bird jump and SND_ASYNC IT MEANS THAT SOUND PLAY WITHOUT STOPPING GAME.
}

void gameoversound() //sound play when the bird lose
{
    PlaySound(TEXT("gameover.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void coordinates(int x, int y)//moving anything on console screen in x and y dircetions 
{
    cursorposition.X = x;
    cursorposition.Y = y;
    SetConsoleCursorPosition(console, cursorposition);//this is essential for update specific part without redrawing window
}

void removecursor(bool visible, DWORD size)//function indicate whether the cursor should be visible or hidden
{
    CONSOLE_CURSOR_INFO cursor;//structure defined in windows (cursor)is variable to hold setting of cursor
    cursor.bVisible = visible;//boolean value to determine ,true(show)false(hide)
    cursor.dwSize = 1;//typical size
    SetConsoleCursorInfo(console, &cursor);//this function is provided by windowsAPI to change without calling it
}

void makescreen()
{
    for (int i = 0; i < screenwidth; i++)
    {
        coordinates(i, 0);//top border
        cout << "<";
        coordinates(i, screenheight);//bottom border
        cout << "<";
    }

    for (int i = 0; i < screenheight; i++)
    {
        coordinates(0, i);//left border
        cout << "*";
        coordinates(screenwidth, i);//right border
        cout << "*";
    }

    for (int i = 0; i < screenheight; i++)
    {
        coordinates(playscreenwidth, i);
        cout << "*";//to draw scoring boundries
    }
}

void gapandpipe(int index)//Function to generate the pipe and determine the gap's position
{
    positionofthegap[index] = 3 + rand() % 14;//Using the rand function to have random positions for the gap 
}

void createpipe(int index)//Function to draw the pipe 
{
    if (pipecontrol[index] == 1)//Checking whether to draw the pipe or not 
    {
        for (int i = 0; i < positionofthegap[index]; i++)//drawing the upper part of the pipe 
        {
            coordinates(playscreenwidth - positionofthepipe[index], i + 1);
            cout << "*";//going to a specific coordinates and draw the pipe with stars
        }
        for (int i = positionofthegap[index] + gapsize; i < screenheight - 1; i++)//drawing the lower part of the pipe 
        {
            coordinates(playscreenwidth - positionofthepipe[index], i + 1);
            cout << "*";
        }
    }
}

void deletepipe(int index)//Function to erase the pipe
{
    if (pipecontrol[index] == 1)//checking whether to erase the pipe or not 
    {
        for (int i = 0; i < positionofthegap[index]; i++)//Erasing the upper part of the pipe 
        {
            coordinates(playscreenwidth - positionofthepipe[index], i + 1);
            cout << "   ";//overwriting the stars with spaces to erase the pipe  
        }
        for (int i = positionofthegap[index] + gapsize; i < screenheight - 1; i++)//Erasing the lower part of the pipe
        {
            coordinates(playscreenwidth - positionofthepipe[index], i + 1);
            cout << "   ";
        }
    }
}

void createbird()
{
    for (int i = 0; i < birdheight; i++)
    { //i is rows ,j coolumns, 2d array to print the bird
        for (int j = 0; j < birdwidth; j++)
        {
            coordinates(j + 2, i + birdpos); //bird is printed through the x axis first then y axis
            cout << bird[i][j];
        }
    }
}
//bird only jumps up and down so only the y axis changes according to the birdposition, birdposition is the y coordinate
void deletebird()
// bird has 2 options either to fall with gravity or to jump again, in both options,position is changing so i have to delete the bird always after drawing it
// and draw it in the new position
{
    for (int i = 0; i < birdheight; i++)
    {
        for (int j = 0; j < birdwidth; j++)
        {
            coordinates(j + 2, i + birdpos);
            cout << " ";
        }
    }
}

int collision()
{
    if (positionofthepipe[0] >= 61)
    {
        if (birdpos < positionofthegap[0] || birdpos > positionofthegap[0] + gapsize) //if it hits the pipes from above the gap or under
        {
            return 1; //there is collison
        }
    }
    if (birdpos <= 1 || birdpos >= screenheight - 2)   // if it hits the border 
    {
        return 1;   //there is collison
    }
    return 0;
}
void bedifficult()
{
    system("cls");
    coordinates(25, 4);
    cout << "Choose mode to return to menu and play";
    coordinates(25, 6);
    cout << "1.Easy" << endl;;
    coordinates(25, 8);
    cout << "2.medium" << endl;;
    coordinates(25, 10);
    cout << "3.Hard\n";

    char mode = _getche();//geting a number from the user

    //decreasing gap size and increasing console movement
    if (mode == '1')
    {
        gapsize = 11;
        sleepindex = 100;
    }
    if (mode == '2')
    {
        gapsize = 7;
        sleepindex = 80;
    }
    if (mode == '3')
    {
        gapsize = 5;
        sleepindex = 60;
    }

}
void gameover()
{
    system("cls");
    coordinates(25, 6);
    cout << "\t\t--------------------------";
    coordinates(25, 7);
    cout << "\t\t-------- Game Over -------";
    coordinates(25, 8);
    cout << "\t\t--------------------------";
    coordinates(25, 10);
    cout << "\t\t your score is: " << score;
    coordinates(25, 12);
    cout << "\t\tPress any key to go back to menu.\n";
    _getch();
}

void updatescore()
{
    coordinates(playscreenwidth + 7, 5); cout << "Score: " << score << endl;
}


void game(int indexofplayer)
{
    birdpos = 6;
    score = 0;
    pipecontrol[0] = 1;//Giving the green light to the first pipe
    pipecontrol[1] = 0;//Giving the red light to the second pipe
    positionofthepipe[0] = positionofthepipe[1] = 4;

    system("cls");
    makescreen();
    gapandpipe(0);
    updatescore();

    coordinates(playscreenwidth + 5, 2); cout << "FLAPPY BIRD";
    coordinates(playscreenwidth + 6, 4); cout << "----------";
    coordinates(playscreenwidth + 6, 6); cout << "----------";
    coordinates(playscreenwidth + 7, 12); cout << "Control ";
    coordinates(playscreenwidth + 7, 13); cout << "-------- ";
    coordinates(playscreenwidth + 2, 14); cout << " Spacebar = jump";
    coordinates(playscreenwidth + 2, 18); cout << "Press[P] to pause";



    coordinates(10, 5); cout << "Press any key to start";
    _getch();
    coordinates(10, 5); cout << "                      ";

    while (1)
    {
        if (_kbhit())//checks the console for keyboard input and returns a nonzero value if a key has been pressed. 
        {
            char ch = _getch(); //when space is hit from keyboard, bird position and velocity changes.bird rises up
            if (ch == 32)
            {
                if (!ispaused)
                    birdpos = birdpos - 3;
                birdvelocity = -0.5;
                 jumpsound();
            }


            if (ch == 'p' || ch == 'P')
            {
                ispaused = !ispaused;  //to swap from pause to resume or from resume to pause


                if (ispaused)
                {
                    coordinates(20, 10);
                    cout << "Game Paused...,Press p to Resume";
                }
                else
                {
                    coordinates(20, 10);
                    cout << "                                    "; //to erase the game paused message

                }
            }
        }
        if (!ispaused)
        {
            birdvelocity = birdvelocity + gravity;  //no key is pressed so bird falls, so its velocity and position changes
            birdpos = birdpos + birdvelocity;

            createbird();
            createpipe(0);
            createpipe(1);

            if (collision() == 1)
            {
                  gameoversound(); 
                gameover();
                return;
            }

            Sleep(sleepindex);
            deletebird();
            deletepipe(0);
            deletepipe(1);

            if (pipecontrol[0] == 1)//Checking whether to move the pipe or not
                positionofthepipe[0] += 2;//Moving the pipe 2 units

            if (pipecontrol[1] == 1)
                positionofthepipe[1] += 2;

            if (positionofthepipe[0] >= 40 && positionofthepipe[0] < 42)//Checking the position of the first pipe to generate and draw the second pipe
            {
                pipecontrol[1] = 1;//Giving the green light to the second pipe
                positionofthepipe[1] = 4;
                gapandpipe(1);//Generate the second pipe
            }

            if (positionofthepipe[0] > 68) //the pipe walked through the whole screen
            {
                score++;
                updatescore();
                pipecontrol[1] = 0;
                positionofthepipe[0] = positionofthepipe[1];
                positionofthegap[0] = positionofthegap[1];
                arrAllPlayers[indexofplayer].score = score;//to update the current player playing score in their struct array
            }
        }
        else
        {
            Sleep(100);  //delay to slow down the loop when it's paused 
        }
    }
}

void showranklist()
{
    system("cls");

    for (int i = 0; i < playerARRsize - 1; i++)//bubble sorting the array of players
    {
        for (int j = 0; j < playerARRsize - 1; j++)
        {
            if (arrAllPlayers[j].score < arrAllPlayers[j + 1].score)
            {
                // Swaping players highest score to lowest
                player temp = arrAllPlayers[j];
                arrAllPlayers[j] = arrAllPlayers[j + 1];
                arrAllPlayers[j + 1] = temp;
            }
        }
    }

    coordinates(25, 5);
    cout << setw(5) << "Rank no." << setw(20) << "Player Name" << setw(10) << "Score" << endl;
    coordinates(25, 6);
    cout << "--------------------------------------------" << endl;

    for (int i = 0; i < playerARRsize; i++)
    {
        if (!arrAllPlayers[i].name.empty())//check if this index have player name or not
        {
            coordinates(25, i + 8); // Add a new y-coordinate on each loop
            cout << setw(5) << i + 1;
            cout << setw(20) << arrAllPlayers[i].name;
            cout << setw(10) << arrAllPlayers[i].score << endl;
        }
    }

    coordinates(27, 25);
    cout << "Press any key to go back to menu.";
    _getch();
    return;
}


bool tolowercasecompare(const string str1, const string str2)
{
    // checking different lengths string
    if (str1.length() != str2.length())
    {
        return false;
    }

    for (int i = 0; i < str1.length(); i++)
    {
        if (tolower(str1[i]) != tolower(str2[i]))//converting both strings to lower case to compare in accurate way
        {
            return false; // Characters don't match 
        }
    }

    return true; // All characters match
}


int main()
{
    removecursor(0, 0); //hide the cursor of the console
    srand((unsigned)time(NULL)); //to gentare random number everytime in gap of pipe
    int mainindex = 0;
    do
    {
        system("cls");
        coordinates(25, 5);
        cout << " ------  |      ------    -----  |   |  -----   ------    -----    -----    \n";
        coordinates(25, 6);
        cout << " |       |     |      |  |     |  | |  |      |    |     |      | |     |    \n";
        coordinates(25, 7);
        cout << " ----    |      ------    -----    |   |-----|     |     |------  |      |   \n";
        coordinates(25, 8);
        cout << " |       |     |      |  |         |   |      |    |     |    |   |     |    \n";
        coordinates(25, 9);
        cout << " |       ----- |      |  |         |    -----    ------  |      |  -----    \n";
        coordinates(25, 12);
        cout << "press [1] to start the game\n";
        coordinates(25, 13);
        cout << "press [2] to show ranking list\n";
        coordinates(25, 14);
        cout << "press [3] to choose game mode\n";
        coordinates(25, 15);
        cout << "press [4] exit\n";

        coordinates(25, 16);
        char input = _getche();//geting a number from the user

        if (input == '1')
        {
            string currentName;
            coordinates(25, 18);
            cout << "Please,Enter your name then press [enter] to start game";
            coordinates(25, 19);
            cin >> currentName;

            int indexofplayer; // index of the player in the array
            bool Found = FALSE; //check player repeated 

            // search player
            for (int Index = 0; Index < playerARRsize; Index++)
            {
                player currentPlayer = arrAllPlayers[Index];

                //seeing if name entered is a player who played before or not
                if (tolowercasecompare(currentName, currentPlayer.name))
                {
                    indexofplayer = Index;
                    Found = TRUE;
                }
            }

            // if a player that hasn't played before
            //creating an player variable of their name
            if (!Found)
            {
                arrAllPlayers[mainindex].name = currentName;
                indexofplayer = mainindex;
                mainindex++;
            }

            game(indexofplayer);
        }
        else if (input == '2')//shows the name of players that played
            //with their score sorted from highest to lowest
        {
            showranklist();
        }
        else if (input == '3')//choosing the mode of the game
        {
            bedifficult();
        }
        else if (input == '4')
        {
            exit(0);
        }

    } while (1);//game keeps playing unless user pressed 4


    return 0;
}