//main.cpp
//Michael Huang
//Dec - 3 - 2021

/*
ROBOT:
-Sensor
-Current position
*/

/*
-Empty space: 0
-Walls: 1
-Other robots: 2
-Battery: 3
*/

//Libraries
#include <iostream>
#include <limits>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <ctime>


// Define
// #define CLEAR system("clear");
#define CLEAR system("clear");
#define IM_WAIT implicit_wait();

// Function Prototype
void implicit_wait();
void UnitTest();            // UnitTest
void program_greeting();
void sort_fitness(int[], int[], int);
int randint(int, int);      //Creating the map
void graph_it(float[], int);

template<typename T>
void AssertEquals(T, T, std::string);   // Creating assertions

//Classes
class DateClass{
    int day;
    int month;
    int year;
    int cur_day;
    int cur_month;
    int cur_year;
public:
    DateClass(int d, int m, int y){
        // Store input date
        day = d;
        month = m;
        year = y;

        // Create and store current date
        time_t now = time(0);

        std::tm*ltm = std::localtime(&now);

        cur_day = ltm->tm_mday;
        cur_month = ltm->tm_mon;
        cur_year = 1900 + ltm->tm_year;
    }

    //Made just for the component test
    int get_input_day(){
        return day;
    }

    int get_input_month(){
        return month;
    }

    int get_input_year(){
        return year;
    }

    void get_input_date(){
        std::cout<<"Date of file creatation: ";
        std::cout<<month<<" - "<<day<<" - "<<year<<std::endl;
    }

    void get_current_sys_time(){
        std::cout<<"Current System Date: ";
        std::cout<<1 + cur_month<<" - "<<cur_day<<" - "<<cur_year<<std::endl;  // Year after 1900
    }

    // Static methods is bounded by the class but not part of the object.
    static void component_test(){
        // Testing maunal input date
        DateClass test_date(1, 2, 3); 
        AssertEquals(1, test_date.get_input_day(), "Get input Day: ");
        AssertEquals(2, test_date.get_input_month(), "Get input Month: ");
        AssertEquals(3, test_date.get_input_year(), "Get input year: ");

        // Testing system date
        time_t now = time(0);

        std::tm*ltm = std::localtime(&now);

        int test_day = ltm->tm_mday;
        int test_month = ltm->tm_mon;
        int test_year = 1900 + ltm->tm_year;
        AssertEquals(ltm->tm_mday, test_day, "Get Current Day: ");
        AssertEquals(ltm->tm_mon, test_month, "Get Current Month: ");
        AssertEquals(1900 + ltm->tm_year, test_year, "Get Cureent Year: ");
    }
};


//Generating map
class Map{
    int** grid;
    int x;
    int y;
public:
    Map(int x, int y){
        // The two is suppose to generate walls.
        this->x = x + 2;
        this->y = y + 2;

        //Generating Row
        grid = new int* [this->x];
        for(int i = 0; i < this->x; i++){
            grid[i] = new int[this->y];
            for(int j = 0; j < this->y; j++){
                grid[i][j] = 0;
            }
        }

        // Generating Walls
        // Top and bottom walls
        for(int i = 0; i < this->y; i++){
            grid[0][i] = 1;
            grid[this->x - 1][i] = 1;   // Counts from 0 first.
        }

        // Left and right.
        for(int i = 0; i < this-> x; i++){
            grid[i][0] = 1;
            grid[i][this->y - 1] = 1;
        }
    }

    // Clearing map
    void clear_map(){
        for(int i = 1; i < x - 1; i++){
            for(int j = 1; j < y - 1; j++){
                grid[i][j] = 0;
            }
        }
    }

    void generate_food(int value){
        for(int i = 0; i < value; i++){
            int x_coor = randint(1, x - 1);    // Account for the walls
            int y_coor = randint(1, y - 1);
            if(grid[x_coor][y_coor] != 0){  // No double dipping!
                // Reset counter to try again.
                i--;
            }
            else{
                grid[x_coor][y_coor] = 3;
            }
        }
    }

    int get_grid(int x_value, int y_value){
        return grid[x_value][y_value];
    }

    void set_grid(int x_value, int y_value, int set){
        grid[x_value][y_value] = set;
    }

    // Just to see if the map is working
    void spec_tech(){
        for(int i = 0; i < x; i++){
            for(int j = 0; j < y; j++){
                std::cout<<grid[i][j]<<" ";
            }
            std::cout<<std::endl;
        }
    }

    void display_map(){
      for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
          if(grid[i][j] == 0){
            std::cout<<"  ";
          }
          else if(grid[i][j] == 1){
            std::cout<<"*"<<" ";
          }
          else if(grid[i][j] == 2){
            std::cout<<"R"<<" ";
          }
          else{
            std::cout<<"o"<<" ";
          }
        }
        std::cout<<std::endl;
      }
    }

    //Manual Release
    ~Map(){
        std::cout<<"Deleting Map"<<std::endl;
        for(int i = 0; i < x; i++){
            // Deleting the columns
            delete []grid[i];
        }
        // Deleting the rows
        delete []grid;
    }
};


class Robot{
    int*** robot;
    int iterations;
    int row;
    int column;
public:
    // Generate 16 genes
    Robot(int iterations, int row, int column, int variables){
        this->iterations = iterations;
        this->row = row;
        this->column = column;

        // Generating the populations of Robots
        robot = new int** [iterations];
        // Creating the # of robot
        for(int i = 0; i < iterations; i++){
            robot[i] = new int* [row];
            // Creating the depth of each robot per the 16 in each robots
            for(int j = 0; j < row; j++){
                robot[i][j] = new int [column];
                // Generating the genes
                for(int g = 0; g < column; g++){
                    robot[i][j][g] = randint(0, variables);
                }
            }
        }
    }

    int get_robot(int pop, int x, int y){
        return robot[pop][x][y];
    }

    //Creating Child Robots
    void meisos(int num_robo_one, int num_robo_two, int stem_robo, int trait){
        int mid_way = row / 2;  // Let the bottom half copy the top half of the parent robot.
        if(trait == 1){
            // Top half.
            // Genes
            for(int i = 0; i < mid_way; i++){
                // alleles
                for(int j = 0; j < column; j++){
                    robot[stem_robo][i][j] = robot[num_robo_one][i][j]; // Top copy Top
                }
            }
            // Bottom half.
            for(int i = 0; i < mid_way; i++){
                for(int j = 0; j < column; j++){
                    robot[stem_robo][mid_way + i][j] = robot[num_robo_two][i][j];   // Bottom copy Bottom
                }
            }
        }
        else if(trait == 2){
            for(int i = 0; i < mid_way; i++){
                for(int j = 0; j < column; j++){
                    robot[stem_robo][i][j] = robot[num_robo_one][mid_way + i][j];   // Top copy Bottom
                }
            }
            for(int i = 0; i < mid_way; i++){
                for(int j = 0; j < column; j++){
                    robot[stem_robo][mid_way + i][j] = robot[num_robo_two][mid_way + i][j]; // Bottom copy Bottom
                }
            }

        }
    }

    // Just a spec, to see if everything was working right.
    void spec_tech(){
        for(int x = 0; x < iterations; x++){
            std::cout<<std::endl<<"Robot: "<<x<<std::endl;
            for(int y = 0; y < row; y++){
                for(int z = 0; z < column; z++){
                    std::cout<<robot[x][y][z]<<" ";
                }
                std::cout<<std::endl;
            }
        }
    }

    // Manual Release
    ~Robot(){
        std::cout<<"Killing all Remaining Robots!"<<std::endl;
        for(int i = 0; i < iterations; i++){
            // Deleting the columns in the row(s)
            for(int j = 0; j < row; j++){
                delete []robot[i][j];
            }
            // Deleting the rows
            delete []robot[i];
        }
        // Deleting the populations
        delete []robot;
    }
};


int main(){
    //Start the PRNG setting seed
    srand(time(NULL));

    CLEAR;
    // UnitTest();
    // IM_WAIT;
    // CLEAR;
    program_greeting();
    IM_WAIT;
    CLEAR;

    // Varaibles
    int const population = 200;
    int const genes = 26;
    int const alleles = 5;
    int x, y;
    x = 20;
    y = 20;
    // int user_map_input;
    // // Dynamic map size
    // std::cout<<"Please enter the size of the map. Nothing lower than 20. It would be a square matrix to keep things looking nice.
    // // Just to enter the do-while loop first 
    // bool toggle = false;
    // do{

    //     std::cin>>user_map_input;

    //     toggle = std::cin.fail();
    //     if(toggle){
    //         std::cin.clear();
    //         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    //         std::cout<<"This is an invalid input, please try again: ";
    //     }
    // }while(toggle);

    Map map(x, y);
    Robot r(population, genes, alleles, 4);

    // r.spec_tech(100, 16, 5);

    // Generate the batteries
    int x_batteries = (x * y) * .4; // Only cover 40% percent of the map(always).
    map.generate_food(x_batteries);
    // Robot Maze Begins
    int robot_number = 0;
    // Keep track of the matches
    int up = 0;
    int right = 0;
    int down = 0;
    int left = 0;
    // LeaderBoard
    int leaderboard[200] = {0};
    int schindlers_list[200] = {0};
    // Generations
    int iterations = 0;
    float generation_list[200] = {0};
    while(iterations < population){
        // Variables
        float generation_avg = 0;
        bool toggle = false;
        int x_position = 0;
        int y_position = 0;
        int neo_positionx = 0;
        int neo_positiony = 0;
        int sensors[4] = {0};  // Giving the robot eyes!!!!
        int compass = 4;    // Can only look North, East, South, West

        for(robot_number = 0; robot_number < population; robot_number++){
            // Set robot
            do{
                x_position = randint(1, x + 1);
                y_position = randint(1, y + 1);
                if(map.get_grid(x_position, y_position) != 0){
                    toggle = true;
                }
                else{
                    map.set_grid(x_position, y_position, 2);
                    toggle = false;
                }
            }while(toggle);
            // Making a move
            int move = 0;
            int total_move = 0;
            for(int i = 0; i < 5; i++){
                // Forming the eyes
                // Looking North
                sensors[0] = map.get_grid(x_position - 1, y_position);
                // Looking East
                sensors[1] = map.get_grid(x_position, y_position + 1);
                // Looking South
                sensors[2] = map.get_grid(x_position + 1, y_position);
                // Looking West
                sensors[3] = map.get_grid(x_position, y_position - 1);

                bool rand = true;
                for(int j = 0; j < genes; j++){
                    // Gathering all values to see if it matches.
                    for(int g = 0; g < 4; g++){
                        if(sensors[g] == r.get_robot(robot_number, j, g)){
                            move++;
                        }
                    }
                    // If not all matach return it to zero.
                    if(move != 4){
                        move = 0;
                    }
                    // Checking if we have a match
                    if(move == 4){
                        // Move Up
                        if(r.get_robot(robot_number, j, 4) == 0){   // The 4 is there since we are counting starting zero.
                            neo_positionx = x_position - 1;
                            neo_positiony = y_position;
                            // Checking for batteries
                            if(map.get_grid(neo_positionx, neo_positiony) == 3){
                                i = 0; // Ate a battery
                                map.set_grid(x_position, y_position, 0);    // Covering the track
                                map.set_grid(neo_positionx, neo_positiony, 2);    // Occupying new space.
                                // Saving new spot
                                x_position = neo_positionx;
                                y_position = neo_positiony;

                                total_move += 5;
                            }
                            else if(map.get_grid(neo_positionx, neo_positiony) == 0){
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);
                                // Saving new spot
                                x_position = neo_positionx;
                                y_position = neo_positiony;
                            }
                            // up++;
                        }
                        // Move Right
                        else if(r.get_robot(robot_number, j, 4) == 1){
                            neo_positionx = x_position;
                            neo_positiony = y_position + 1;
                            if(map.get_grid(neo_positionx, neo_positiony) == 3){
                                i = 0;
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;

                                total_move += 5;
                            }
                            else if(map.get_grid(neo_positionx, neo_positiony) == 0){
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;
                            }
                            // right++;
                        }
                        // Move Down
                        else if(r.get_robot(robot_number, j, 4) == 2){
                            neo_positionx = x_position + 1;
                            neo_positiony = y_position;
                            if(map.get_grid(neo_positionx, neo_positiony) == 3){
                                i = 0;
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;

                                total_move += 5;
                            }
                            else if(map.get_grid(neo_positionx, neo_positiony) == 0){
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;
                            }
                            // down++;
                        }
                        //Move Left
                        else if(r.get_robot(robot_number, j, 4) == 3){
                            neo_positionx = x_position;
                            neo_positiony = y_position - 1;
                            if(map.get_grid(neo_positionx, neo_positiony) == 3){
                                i = 0;
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;

                                total_move += 5;
                            }
                            else if(map.get_grid(neo_positionx, neo_positiony) == 0){
                                map.set_grid(x_position, y_position, 0);
                                map.set_grid(neo_positionx, neo_positiony, 2);

                                x_position = neo_positionx;
                                y_position = neo_positiony;
                            }
                            // left++;

                            //Flagging rand
                            rand = false;
                        }
                        move = 0;
                        // total_move += 5;             
                    }
                }
                // Randondl move if unable to find a match
                if(rand){
                    // Move randomly
                    int randomly = randint(0, 4);
                    if(randomly == 0){
                        neo_positionx = x_position - 1;
                        neo_positiony = y_position;
                    }
                    else if(randomly == 1){
                        neo_positionx = x_position;
                        neo_positiony = y_position + 1;
                    }
                    else if(randomly == 2){
                        neo_positionx = x_position + 1;
                        neo_positiony = y_position;
                    }
                    else{
                        neo_positionx = x_position;
                        neo_positiony = y_position - 1;
                    }

                    if(map.get_grid(neo_positionx, neo_positiony) == 3){
                        i = 0;
                        map.set_grid(x_position, y_position, 0);
                        map.set_grid(neo_positionx, neo_positiony, 2);

                        x_position = neo_positionx;
                        y_position = neo_positiony;

                        total_move += 5;
                    }
                    else if(map.get_grid(neo_positionx, neo_positiony) == 0){
                        map.set_grid(x_position, y_position, 0);
                        map.set_grid(neo_positionx, neo_positiony, 2);

                        x_position = neo_positionx;
                        y_position = neo_positiony;
                    }
                    // total_move += 5;
                }
                // CLEAR;
                // // std::cout<<"Gen: "<<iterations + 1<<std::endl;
                // std::cout<<"Up: "<<up<<std::endl;
                // std::cout<<"Right: "<<right<<std::endl;
                // std::cout<<"Down: "<<down<<std::endl;
                // std::cout<<"Left: "<<left<<std::endl;
                // std::cout<<"Total Matches in this Gen: "<< total_move <<std::endl;
                // std::cout<<"Current Robot: "<<robot_number + 1<<std::endl;
                // map.display_map();
            }
            map.clear_map();    // Get rid of the robot and old batteries before letting the next one in.
            map.generate_food(x_batteries); // Regenerating the food for the next robot.
            leaderboard[robot_number] = total_move;
            total_move = 0;
        }
        for(int i = 0; i < population; i++){
            // // TODO
            // std::cout<<"Robot: "<<i<<" "<<leaderboard[i]<<std::endl;
            generation_avg += leaderboard[i];
        }
        generation_avg = generation_avg / population;
        // std::cout<<"Generation average: "<<generation_avg<<std::endl;
        sort_fitness(leaderboard, schindlers_list, population);  // Killing the bottom 50%

        // TODO
        // for(int i = 0; i < population; i++){
        //     std::cout<<"Schindler's List: "<<schindlers_list[i]<<std::endl;
        //     std::cout<<"Leaderboard: "<<i<<" "<< leaderboard[i]<<std::endl;
        // }

        // Creating new next generations
        // Need to find the top two parent that has not breed yet.
        // Find any blank or < 50% survivors and convert them to childrens.
        // Finding parents
        for(int i = 0; i < (population / 2); i += 2){
            int children_one = 0;
            int children_two = 0;
            for(int j = 0; j < population; j++){
                if(leaderboard[j] >= 0){
                    children_one = j;
                    leaderboard[j] = -200; // No double dipping.
                    break;
                }
            }
            for(int j = 0; j < population; j++){
                if(leaderboard[j] >= 0){
                    children_two = j;
                    leaderboard[j] = -200;
                    break;
                }
            }
            // std::cout<<"Skip: "<<children_one<<" "<<children_two<<std::endl;
            r.meisos(schindlers_list[i], schindlers_list[i + 1], children_one, 1);
            r.meisos(schindlers_list[i], schindlers_list[i + 1], children_two, 2);
        }
        // r.spec_tech();
        // std::cout<<iterations<<std::endl;
        generation_list[iterations] = generation_avg;
        iterations++;
        // std::cout<<generation_avg<<std::endl;
        // IM_WAIT;
    }
    for(int i = 0; i < population; i++){
        std::cout<<"Gen: "<<i<<" Averagae: "<<generation_list[i]<<std::endl;
    }
    float largest = 0;
    float smallest = 100;
    float average = 0;
    for(int i = 0; i < population; i++){
        if(generation_list[i] > largest){
            largest = generation_list[i];
        }

        if(generation_list[i] < smallest){
            smallest = generation_list[i];
        }

        average += generation_list[i];
    }
    std::cout<<std::endl<<"------------------"<<std::endl;
    std::cout<<"Largest: "<<largest<<std::endl;
    std::cout<<"Smallest: "<<smallest<<std::endl;
    std::cout<<"Average: "<<average / population<<std::endl;
    std::cout<<"------------------"<<std::endl<<std::endl;

    std::cout<<"General Graph: "<<std::endl;
    graph_it(generation_list, population);
    return 0;
}


void implicit_wait(){
    char something;
    std::cout<<"Please entering something to continue: ";
    std::cin>>something;

    // Cleaning up
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


void UnitTest(){
    std::cout<<"UNIT TEST"<<std::endl;
    std::cout<<"========="<<std::endl;
    DateClass::component_test();
}


void program_greeting(){
    DateClass now(6, 12, 2021);

    std::cout<<"Program Greeting"<<std::endl;
    std::cout<<"================"<<std::endl;
    std::cout<<"Name: Michael Huang"<<std::endl;
    std::cout<<"I do not have the power."<<std::endl;
    std::cout<<"Due Date: 28 - November - 2021"<<std::endl;
    now.get_current_sys_time();
    now.get_input_date();
}


void sort_fitness(int *board, int* list, int size){
    // Finding the Greatest values.
    int largest = 0;
    int index = 0;

    int i = 0;
   while(i < (size / 2)){
        for(int j = 0; j < size; j++){  // Finding the largets values in the entire array.
            if(board[j] > largest){
                largest = board[j];
                index = j;  // Know where to delete, Part of no double dipping.
            }
        }
        list[i] = index;    // Save the spot.
        board[index] *= -1;   // No double dipping.

        // Reseting counters
        largest = 0;
        index = 0;

        i++;
    }
}


int randint(int min, int max){
    int value = rand() % max + min;
    return value;
}


// Using template would reduce time and space complexity
template<typename T>
void AssertEquals(T sample_one, T sample_two, std::string output_text){
    std::cout<<output_text;
    if(sample_one == sample_two){
        std::cout<<"Passed"<<std::endl;
    }
    else{
        std::cout<<"Failed"<<std::endl;     
    }
}


void graph_it(float* graph_data, int size){
    for(int i = 0; i < 50; i++){
        std::cout<<"-";
    }
    std::cout<<std::endl;

    for(int i = 0; i < size; i++){
        if(i <= 9){
            std::cout<<i<<"  |";
        }
        else if(i > 9 && i < 100){
            std::cout<<i<<" |";
        }
        else{
            std::cout<<i<<"|";
        }

        for(int j = 0; j < (int)graph_data[i] - 1; j++){
            std::cout<<" ";
        }
        std::cout<<"*"<<std::endl;
    }
}

