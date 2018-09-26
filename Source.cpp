/*main.cpp*/

//
// Netflix movie review analysis in modern C++
//
// Michael Lederer
// U. of Illinois, Chicago
// CS 341: Fall 2018
// Project 02
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <iomanip>


using namespace std;

class Movie
{
private:
	int id;
	string name;
	int year;
	double numStars;
	int numReviews;
	double avgStar;

	int stars[5] = { 0,0,0,0,0 };

public:

	Movie(int movieID, string movieName, int pubYear)
		: id(movieID), name(movieName), year(pubYear)
	{
		id, year, numStars, numReviews, avgStar = 0;
	}

	//move constructor
	Movie(Movie&& other)
		: id(move(other.id)),
		name(move(other.name)),
		year(move(other.year)) 
	{
		id, year, numStars, numReviews, avgStar = 0;
	}

	int getID() { return id; }
	string getName() { return name; }
	int getYear() { return year; }
	double getAvgstar() { return avgStar; }
	int getNumreviews() { return numReviews; }
	void incrementNumReviews() { ++numReviews; }
	void updateNumStars(double reviewStars) { numStars += reviewStars; }
	void updateStarArray(int star) {stars[star - 1]++;}
	int getStars(int star) { return stars[star - 1]; }
	void updateAvgstar() 
	{ 
		if (numReviews == 0)
		{
			avgStar = 0;
			return;
		}
		avgStar = numStars / numReviews;
	}

};

class Review
{
private:
	int revID;
	int movID;
	int userID;
	int rating;
	string date;

public:

	Review(int reviewID, int movieID, int uID, int Rating, string revDate)
		: revID(reviewID), movID(movieID), userID(uID), rating(Rating), date(revDate)
	{
		revID, movID, userID, rating = 0;
	}

	//move constructor
	Review(Review&& other)
		: revID(move(other.revID)),
		movID(move(other.movID)),
		userID(move(other.userID)),
		rating(move(other.rating)),
		date(move(other.date))
	{
		revID, movID, userID, rating = 0;
	}

	int getReviewId() { return revID; }
	int getMovieID() { return movID; }
	int getUserID() { return userID; }
	int getRating() { return rating; }
	string getDate() { return date; }
};

void movieFileRead(ifstream& file, map<int, Movie>& movieMap, int& movieCount)
{
	string inputString[3];
	string line;
	getline(file, line);

	while (!file.eof())
	{
		++movieCount;
		getline(file, line);
		stringstream ss(line);

		if (line.compare("") == 0) { break; }

		for (auto &i : inputString)
			getline(ss, i, ',');

		movieMap.emplace(stoi(inputString[0]), Movie(stoi(inputString[0]), inputString[1], stoi(inputString[2])));
		
	}
}

void reviewFileRead(ifstream& file, map<int, Review>& reviewMap, int& reviewCount, map<int, Movie>& movieMap)
{
	string inputString[5];
	string line;
	getline(file, line);

	while (!file.eof())
	{
		++reviewCount;
		getline(file, line);
		stringstream ss(line);

		if (line.compare("") == 0) { break; }

		for (auto &i : inputString)
			getline(ss, i, ',');

		movieMap.at(stoi(inputString[1])).incrementNumReviews();
		movieMap.at(stoi(inputString[1])).updateNumStars(stod(inputString[3]));
		movieMap.at(stoi(inputString[1])).updateAvgstar();
		movieMap.at(stoi(inputString[1])).updateStarArray(stoi(inputString[3]));

		reviewMap.emplace(stoi(inputString[0]), Review(stoi(inputString[0]), stoi(inputString[1]), stoi(inputString[2]), stoi(inputString[3]), inputString[4]));

	}
}

void movieMapSort(map<int, Movie>& movieMap)
{
	multimap<double, Movie*> multiMap;

	for (auto& i : movieMap) 
	{ 
		multiMap.insert(pair<double, Movie*>(i.second.getAvgstar(), &i.second));
	}

	vector<Movie*> movieVec;
	int count = 0;

	for (auto i = --multiMap.end() ; count != 10; --i, ++count)
	{
		movieVec.push_back(i->second);
	}

	int counter = 0;
	for (auto &i : movieVec)
	{
		cout << ++counter << "." << "\t" << i->getID() << "\t" << i->getNumreviews() << "\t" << i->getAvgstar() << "\t"
			<< "'" << i->getName() << "'" << endl;
	}
}

int main()
{

	cout << "** Netflix Movie Review Analysis **" << endl;
	cout << endl;

	// input the filenames to process:
	string moviesFN, reviewsFN;

	cin >> moviesFN;
	cin >> reviewsFN;

	auto moviebeginTime = std::chrono::steady_clock::now();

	ifstream movieFile, reviewFile;
	movieFile.open(moviesFN);
	reviewFile.open(reviewsFN);

	int movieCount = 0;
	int reviewCount = 0;

	map<int, Review> reviewMap;
	map<int, Movie> movieMap;



	movieFileRead(movieFile, movieMap, movieCount);
	auto  movieendTime = std::chrono::steady_clock::now();
	auto moviediff = movieendTime - moviebeginTime;

	auto reviewbeginTime = std::chrono::steady_clock::now();

	reviewFileRead(reviewFile, reviewMap, reviewCount, movieMap);

	auto reviewendTime = std::chrono::steady_clock::now();
	auto reviewdiff = reviewendTime - reviewbeginTime;

	cout << setprecision(5) << endl;
	cout << fixed << endl;
	
	cout << ">> Reading movies... " << movieCount << "  [  time:  " << std::chrono::duration<double, std::milli>(moviediff).count() << "  ms  ]" << endl;
	cout << ">> Reading reviews... " << reviewCount << "  [  time:  " << std::chrono::duration<double, std::milli>(reviewdiff).count() << "  ms  ]" << endl;
	cout << ">> Top-10 Movies <<" << endl << endl;
	cout << "Rank" << "\t" << "ID" << "\t" << "Reviews" 
		 << "\t" << "Avg" << "\t" << "Name" << endl;

	movieMapSort(movieMap);

	cout << ">> Movie and Review Information << " << endl;

	while (1)
	{
		cout << "Please enter a movie ID (< 100,000), a review ID (>= 100,000), or 0 to stop> ";

		int input;
		cin >> input;

		//If its a movie
		if (input == 0)
		{
			cout << endl << endl << "** DONE! **" << endl;
			break;
		}
		if (input < 100000)
		{
			if (input < 0)
			{
				cout << "**invalid id..." << endl << endl;
				continue;
			}
			auto it = movieMap.find(input);

			if (it == movieMap.end())
				cout << "movie not found..." << endl << endl;
			else 
			{
				cout << "Movie:" << "\t" << "'" <<
					movieMap.at(input).getName() << "'" << endl;
				cout << "Year:" << "\t" << movieMap.at(input).getYear() << endl;
				cout << "Avg rating:" << "\t" << movieMap.at(input).getAvgstar() << endl;
				cout << "Num reviews:" << "\t" << movieMap.at(input).getYear() << endl;
				cout << "  1 star: " << "\t" << movieMap.at(input).getStars(1) << endl;
				cout << "  2 star: " << "\t" << movieMap.at(input).getStars(2) << endl;
				cout << "  3 star: " << "\t" << movieMap.at(input).getStars(3) << endl;
				cout << "  4 star: " << "\t" << movieMap.at(input).getStars(4) << endl;
				cout << "  5 star: " << "\t" << movieMap.at(input).getStars(5) << endl;
			}
		}
		//If its a review
		else if (input >= 100000)
		{
			auto it = reviewMap.find(input);
			if (it == reviewMap.end())
				cout << "review not found..." << endl << endl;
			else
			{
				cout << "Movie: " << "\t" << movieMap.at(reviewMap.at(input).getMovieID()).getID() << " ("
					 << movieMap.at(reviewMap.at(input).getMovieID()).getName() << ")" << endl;
				cout << "Num stars: " << "\t" << reviewMap.at(input).getRating() << endl;
				cout << "User id: " << "\t" << reviewMap.at(input).getUserID() << endl;
				cout << "Num stars: " << "\t" << reviewMap.at(input).getDate() << endl;
			}
		}
	}
}