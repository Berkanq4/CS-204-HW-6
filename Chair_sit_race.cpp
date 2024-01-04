#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include <random>
#include <sstream>
#include "IntQueueHW6.H"
#include <mutex>
#include <vector>

using namespace std;
mutex my_mutex;
time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
struct tm *ptm = new struct tm;  //initialise the neccessary things.
int num_players;
vector <int> losers; // make it global because we will access it in everywhere.


bool is_in(int id){ //check if related id exists in losers list.
    for (int i = 0; i < losers.size(); i++){
        if (losers[i] == id){
            return true;
        }
    }
    return false;
}


void id_getter(int order, IntQueueHW6 & object1, int & ideal_sum, int oyuncu_sayisi, vector <int> & remaining_players){ //function to print the random id to the object1.
    this_thread::sleep_until(chrono::system_clock::from_time_t(mktime(ptm)));
    my_mutex.lock(); //lock the function here so that only one thread will be able to access in.
    int id = order;
    static int loser_id;
    static int sum = 0; // make it static so that in each time it is called. it will remember the previous value.
    ostringstream print;



    if (object1.isFull() == false){ // if it is not full and the id does not appear in losers list.
        if (is_in(id) == false){
            sum += id; // add that id to the sum.
            object1.enqueue(id); // put that id to the linked list.
            if (remaining_players.size() != oyuncu_sayisi-1){ // while the winners of the round are not equal to the numberofplayers -1 , let the thread get inside the if statement.
                remaining_players.push_back(id); //push it to the list we create so that we will print the remainings.
                print << "PLayer " << id << " captured a chair at " << put_time(ptm, "%X") << '.' <<endl;
                cout << print.str();
            }

        }
    }

    if (object1.isFull() == true){
        loser_id = ideal_sum - sum; // ideal sum means the expected sum with all numbers added. and sum means the numbers we manually add. Therefore their substraction should give the loser id.
        cout << "Player " << loser_id << " couldn't capture a chair." << endl;
        losers.push_back(loser_id); //  list all losers in losers list.
        cout << "Remaining players are as follows: ";
        for (const auto winners: remaining_players){ //print winners.
            cout << winners << " ";
        }
        cout << endl;
        remaining_players.clear(); //remove all the elements of remaining players since the order will change.
        cout << endl;
        ideal_sum = ideal_sum - loser_id; // alter the ideal sum because the next round ideal sum will be different.
        sum = 0;
        //ideal_sum_ptr = &ideal_sum;
        object1.clear(); // clear the linked list as the players have stand up the chairs.
    }
    my_mutex.unlock(); // now we can unlock the mutex.
}

int main_sum_finder(int num_players){
    return (num_players*(num_players+1))/2; // it will return the ideal sum.
}


int main() {
    std::cout << "Welcome to Musical Chairs game!\n"
                 "Enter the number of players in the game:" << std::endl;
    cin >> num_players;
    cout << "Game Start!" << endl << endl;
    localtime_s(ptm, &tt);
    cout << "Time is now " << put_time(ptm,"%X") << endl;
    int game_times = num_players-1;
    int ideal_sum = main_sum_finder(num_players);
    vector <int> remaining_players;// intialise a vector to keep the players who passed the round.
    while (game_times != 0){ //build a while loop to proceed the game in each round.
        ptm->tm_sec += 2;
        thread threads[num_players]; //initialise a thread according to the number of players.
        IntQueueHW6 queue(num_players-1); // initialise a Intqueuehw6 object based on the number of chairs.

        IntQueueHW6 object1 = queue; // assign that object to another object which does not have a parameter that is a default constructor so that we can use it in thread parameter.

        for (int i = 1; i <= num_players; i++){
            threads[i - 1] = (thread(&id_getter, i, std::ref(object1), std::ref(ideal_sum), num_players, std::ref(remaining_players))); //start sending the threads
        }



        for (int i = 1 ; i <= num_players; i++){
            threads[i-1].join(); //start joinging the threads
        }

        if (game_times != 1){
            cout << "Time is now " << put_time(ptm, "%X") << endl; // if it is not the last round print the current time.
        }
        game_times -= 1; //decrease the game times so that while loop will not stuck in infinity.
        num_players = num_players - 1; // decrease the number of players by one in each round.

    }
    cout << "Game Over!" << endl;
    cout << "Winner is player " << remaining_players[0] << '!' << endl;
    return 0;
}
