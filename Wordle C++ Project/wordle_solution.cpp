//Calvin Hirschler 3/1/2022
//Wordle program

#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <iterator>
using namespace std;
using namespace chrono;

//test
void test();
//files
vector<string> getWordList(string); //opens file of guess and answer
void createFile(); //creates new output file
//play wordle
void addTuple(tuple<string, float>); //sorts a tuple into vector of tuples
bool findWord(string); //searches for guess in guess list
void compareGuess(string, string); //compares guess with answer. returns guess with colors
string getGuess(); //takes input, checks to see if valid guess
void playWordle(); //main executer in playing wordle
//computer time
vector<string> eliminateWords(string, string); //uses 3 strings (g/y/b) to eliminate possible answers
string compareGuessToAnswer(string, string); //gets green, yellow, and banned letters
void getTotal(string, string); //compares a guess and answer, then adds to the total
void computeFirstGuess(); //main executer in computing wordle
string getBestGuess(string, string);
void solveWordle(string);
void computeAllWordles();

//declaring variables
vector<string> answers, guesses, newAnswers;
vector<tuple<string, float>> weights;
vector<tuple<string, string>> colorList;
vector<int> totals;

int total, minimumTotal, flag;
float averageGuesses;

string guess, mainColors, bestGuess;

//main
int main() {
	srand(time(0)); //setting seed to random
	//get answers and guesses using txt files
	answers = getWordList("answers.txt");
	guesses = getWordList("guesses.txt");
	//play wordle
	cout << "To play wordle, type play\n" << "To test, type test\n" << "To feel get optimal first guess, type guess\n" << "To feel inferior, type loser\n";
	string play;
	cin >> play;
	while (play != "play" && play != "loser" && play != "test" && play != "guess")
		cin >> play;
	if (play == "play")
		playWordle();
	else if (play == "guess")
		computeFirstGuess();
	else if (play == "loser")
		//solveWordle(answers[800]);
		computeAllWordles();
	//	solveWordle(answers[177]);
	else if (play == "test")
		test();
}

//test
void test(){
	string test, tempGuess, tempAnswer, temp;
	cout << "Would you like to test a guess(1), a guess AND answer(2), or letters(3)?\n";
	cin >> test;
	while (test != "1" && test != "2" && test != "3")
		cin >> test;
//guess
	if (test == "1") {
		cout << "Enter your guess: ";
		cin >> temp;
	}
//guess AND answer
	if (test == "2") {
		cout << "Enter your guess: ";
		cin >> tempGuess;
		cout << "\nEnter your answer: ";
		cin >> tempAnswer;
	}
//letters
	if (test == "3") {
		cout << "Enter your guess: ";
		cin >> tempGuess;
		cout << "\nEnter your answer: ";
		cin >> tempAnswer;
		cout << endl;

		string colors = compareGuessToAnswer(tempGuess, tempAnswer);
		cout << colors << endl;
	}
}

//opens the file and grabs the words
vector<string> getWordList(string fname){
	ifstream stream;
	stream.open(fname.c_str());

	vector<string> v;
	string line;

	while (getline(stream, line)) {
		v.push_back(line); //add word to vector
	}
	stream.close();
	return v;
}

//creates file for word superiority
void createFile(){
	ofstream stream;
	stream.open("firstGuessWeight2.txt");
	stream << "Num\tGuess\tAverage remaining answers\n";
	for (int i = 0; i < weights.size(); i++) {
		stream << i + 1 << "\t" << get<0>(weights[i]) << ":\t" << fixed << setprecision(3) << get<1>(weights[i]) << endl;
	}
	stream.close();
}

//takes a guess and searches for it in the guess list
bool findWord(string guess){
	for (int i = 0; i < guesses.size(); i++){
		if (guess.compare(guesses[i]) == 0)
			return true;
	}
	return false;
}

//takes a guess and answer and compares them. Returns the guess with colors
void compareGuess(string guess, string answer){
	string colors = compareGuessToAnswer(guess, answer);
	for (int i = 0; i < 5; i++){
		if (colors.at(i) == 'g')
			cout << "\033[32m" << guess.at(i);
		else if (colors.at(i) == 'y')
			cout << "\033[0;33m" << guess.at(i);
		else
			cout << "\033[0m" << guess.at(i);
	}
	cout << "\033[0m";
		
}

//takes input, checks to see if it is a valid guess
string getGuess(){
	cin >> guess;
	if (guess.length() != 5 || !findWord(guess)) {
		cout << "Invalid word..." << endl;
		return "";
	}
	return guess;
}

//main stuff that executes functions above
void playWordle() {
	//gets random word as answer
	int random = rand() % answers.size();
	string answer = answers[random];
	cout << "Guess the 5 letter word" << endl;

	//plays a round of wordle 6 times
	for (int i = 0; i < 6; i++){
		while (guess == "")
			guess = getGuess();
		cout << i + 1 << ": ";
		compareGuess(guess, answer);
		if (answer.compare(guess) == 0) {
			cout << "YOU WIN" << endl;
			break;
		}
		guess = "";
	}
	//end of 6 turns
	cout << "The answer is: " << answer << endl;
}

//COMPUTER TIME

//returns a string with information about the colors eg: "gybbb"
string compareGuessToAnswer(string guess, string answer){
	string colors = "00000";
	string temp = answer;
	for (int i = 0; i < 5; i++) { //only green letters
		if (guess.at(i) == answer.at(i)) {
			colors.replace(i, 1, "g");
			temp.replace(i, 1, "0"); //erase letter in answer where it is green
		}
	}
	for (int i = 0; i < 5; i++) {
		if (guess.at(i) == answer.at(i))
			continue;
		if (temp.find(guess.at(i)) != -1) {
			colors.replace(i, 1, "y");
			temp.replace(temp.find(guess.at(i)), 1, "0"); //erase letter
		}
		else
			colors.replace(i, 1, "b");
	}
	return colors;
}

//adds the color string to a vector of strings... uses binary search
void addColorToList(string guess, string answer){
	string colors = compareGuessToAnswer(guess, answer);

	tuple <string, string> tup; //create tuple with guess and colors
	tup = make_tuple(guess, colors);
	
	int high = colorList.size() - 1;
	int low = 0;
	int mid = 0;
	
	if (colorList.size() == 0) {//first element
		colorList.push_back(tup);
		return;		
	}
	while(colors.compare(get<1>(colorList[mid])) != 0){ //while string has not been found
		if (high < low)
			break;
		mid = low + (high - low) / 2;
		if (colors.compare(get<1>(colorList[mid])) > 0) 
			low = mid + 1;
		if (colors.compare(get<1>(colorList[mid])) < 0) 
			high = mid - 1;
		if (colors.compare(get<1>(colorList[mid])) == 0) {
			colorList.insert(colorList.begin() + mid, tup);
			return;
		}
	}
	if (colors.compare(get<1>(colorList[mid])) > 0) 
		mid++;
	colorList.insert(colorList.begin() + mid, tup);
}

//gives a total
void getTotal(){
	int count = 0;
	string color = get<1>(colorList[0]);
	string temp;
	for (int i = 0; i < colorList.size(); i++) {
		temp = get<1>(colorList[i]);
		if (color.compare(temp) != 0) { //if colors are different
			if (minimumTotal != 0 && total > minimumTotal) {
				colorList.clear();
				return;
			}
			total += count * count;
			count = 0;
			color = get<1>(colorList[i]);
		}
		count++;
	}
	total += count * count; //for last color
	minimumTotal = total;

	colorList.clear();
}

//adding tuples to a big vector... uses binary sort
void addTuple(tuple<string, float> tup){
	float tupValue = get<1>(tup);
	int high = weights.size() - 1;
	int low = 0;
	int mid = 0;
	
	if (weights.size() == 0) {//first element
		weights.push_back(tup);
		return;		
	}
	while(tupValue != get<1>(weights[mid])){ //while value has not been found
		if (high < low)
			break;
		mid = low + (high - low) / 2;
		if (get<1>(weights[mid]) < tupValue) 
			low = mid + 1;
		if (get<1>(weights[mid]) > tupValue)
			high = mid - 1;
		if (get<1>(weights[mid]) == tupValue) {
			weights.insert(weights.begin() + mid, tup);
			return;
		}
	}
	if (tupValue > get<1>(weights[mid]))
		mid++;
	weights.insert(weights.begin() + mid, tup);
}

void computeFirstGuess() {
	auto t1 = high_resolution_clock::now(); //for timing
	int random = rand() % answers.size();
	tuple <string, float> tup;

	weights.clear();
	minimumTotal = 0;

	//main boy right here
	for (string guess : guesses) { //for every guess
//	for (int i = 0; i < 100; i++) {
//		guess = guesses[i];
		for (string answer : answers) { //for every answer
			addColorToList(guess, answer);
		}
		getTotal();
		tup = make_tuple(guess, (float)total / answers.size());
		addTuple(tup);
		total = 0;
	}
	createFile();

	//used for timing
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);
	cout << "done in " << (double)ms_int.count() / 1000 << "sec\n";
}

string getBestGuess(string guess, string trueAnswer){
	vector<string> remainingAnswers;
	string mainColors = compareGuessToAnswer(guess, trueAnswer);
	//checks for matching colors to trueAnswer	
	for (string answer : answers) { //for every answer
		if (mainColors == compareGuessToAnswer(guess, answer))
			remainingAnswers.push_back(answer); //add word to new list if the colors match the answer
	}
	//slims down the answers<string> vector
	answers = remainingAnswers;
	cout << "REMAINING WORDS: " << remainingAnswers.size() << "\t";
	//checks size of vector to do certain logic
	if (answers.size() == 1) {
		cout << endl;
		return remainingAnswers[0];
	}
	if (answers.size() == 2) {
		cout << endl;
	//	answers.erase(answers.begin());
		return remainingAnswers[0];
	}
	//gets the best guess if not a special case
	computeFirstGuess();
	//searches for remaining answers
	for (int i = 0; i < answers.size(); i++){
		auto it = find_if(weights.begin(), weights.end(), [i](const tuple<string, float>& e) {return get<0>(e) == answers[i];});
		if (get<1>(*it) == get<1>(weights[0])) {
			if (compareGuessToAnswer(get<0>(*it), trueAnswer) != "ggggg") {
				compareGuess(get<0>(*it), trueAnswer); cout << "\t"; //writes colored word
			}
			return get<0>(*it);
		}
	}
	if (compareGuessToAnswer(get<0>(weights[0]), trueAnswer) != "ggggg") {
		compareGuess(get<0>(weights[0]), trueAnswer); cout << "\t"; //writes colored word
	}
	return get<0>(weights[0]);
}

void solveWordle(string answer) {
	int count = 2;
	compareGuess("roate", answer); //writes colored word
	cout << "\t"; //used for time
	bestGuess = getBestGuess("roate", answer);
	
	while (compareGuessToAnswer(bestGuess, answer) != "ggggg") {
		bestGuess = getBestGuess(bestGuess, answer);
		count++;
	}
	compareGuess(bestGuess, answer); //writes colored word
	cout << "\033[32m" << "\tSolved in " << count << " steps" << endl;
	cout << "\033[0m" << "Average solve rate: ";
	averageGuesses += count;
}

void computeAllWordles() {
	for (int i = 0; i < answers.size(); i++) {
		solveWordle(answers[i]);
		cout << averageGuesses / (i+1) << endl;
		answers = getWordList("answers.txt");
	}
}
