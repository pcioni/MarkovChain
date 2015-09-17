
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cassert>
#include "MersenneTwister.h"

typedef std::map<std::string, int>	word_count_map;
typedef std::map<std::string, std::map<std::string, int> >  full_map;
typedef std::map<std::string, std::map<std::string, std::map<std:: string, int> > > deep_map;



// Custom helper function that reads the input stream looking for
// double quotes (a special case delimiter needed below), and white
// space.  Contiguous blocks of alphabetic characters are lowercased &
// packed into the word.
bool ReadNextWord(std::istream &istr, std::string &word) {
  char c;
  word.clear();
  while (istr) {
    // just "peek" at the next character in the stream
    c = istr.peek();
    if (isspace(c)) {
      // skip whitespace before a word starts
      istr.get(c);
      if (word != "") {
	// break words at whitespace
	return true;
      }
    } else if (c == '"') {
      // double quotes are a delimiter and a special "word"
      if (word == "") {
	istr.get(c);
	word.push_back(c);
      }
      return true;
    } else if (isalpha(c)) {
      // this a an alphabetic word character
      istr.get(c);
      word.push_back(tolower(c));
    } else {
      // ignore this character (probably punctuation)
      istr.get(c);
    }
  }
  return false;
}


// Custom helper function that reads the input stream looking a
// sequence of words inside a pair of double quotes.  The words are
// separated by white space, but the double quotes might not have
// space between them and the neighboring word.  Punctuation is
// ignored and words are lowercased.
std::vector<std::string> ReadQuotedWords(std::istream &istr) {
  // returns a vector of strings of the different words
  std::vector<std::string> answer;
  std::string word;
  bool open_quote = false;
  while (ReadNextWord(istr,word)) {
    if (word == "\"") {
      if (open_quote == false) { open_quote=true; }
      else { break; }
    } else {
      // add each word to the vector
      answer.push_back(word);
    }
  }
  return answer;
}



// Loads the sample text from the file, storing it in the map data
// structure. Window specifies the width of the context (>= 2) of the
// sequencing stored in the map. .
void LoadSampleText(full_map &data, deep_map &deep_data, const std::string &filename, const int &window, 
					  const std::string &parse_method, std::string &last_word, std::string& last_word2) {
	// open the file stream
	std::ifstream istr(filename.c_str());
	if (!istr) { 
		std::cerr << "ERROR cannot open file: " << filename << std::endl; 
		exit(1);
	} 
	// verify the window parameter is appropriate
	if (window < 2) 
		std::cerr << "ERROR window size must be >= 2:" << window << std::endl;
	// verify that the parse method is appropriate
	bool ignore_punctuation = false;
	if (parse_method == "ignore_punctuation")
		ignore_punctuation = true;
	else {
		std::cerr << "ERROR unknown parse method: " << parse_method << std::endl;
		exit(1);
	}

	std::string word;
	std::vector<std::string> words;
	while (ReadNextWord(istr,word)) {
		// skip the quotation marks (not used for this part)
		if (word == "\"") continue;
		//throw all our words into a vector for ease of access. Though we could
		//theoretically make our data map inside of the ISTR, this method makes 
		//processing the words much easier for a paltry memory use increase.
		words.push_back(word);
	}
	
	if (window == 2) {
		for (int i = 0; i < words.size() - 1; ++i) { //stop one word early, since the last word has no following words
			++(data[words[i]][words[i+1]]);
		}
		last_word = words[words.size() - 1];
		last_word2 = words[words.size() - 2];
	}
	else if (window == 3) {
		for (int i = 0; i < words.size() - 2; ++i) {	//likewise, stop two words early since we're two words deep
			++(deep_data[words[i]][words[i+1]][words[i+2]]);
		}
		last_word = words[words.size() - 1];
		last_word2 = words[words.size() - 2];
	}
	
	//print out our parameters
	std::cout << "Loaded " << filename << " with window = " << window << " and parse method = " << parse_method
	<< std::endl << std::endl;

}

std::string get_next_word( full_map &data,  std::string &word, bool random_flag){

	full_map::iterator itr_word = data.find(word);
	word_count_map::iterator itr_count;
	word_count_map::iterator max_location; //keep track of where our value is
	
	if (itr_word == data.end() ) //map.find() returns an itr to .end() if the value is not found
		std::cout << " ERROR: Invalid word for \"next word\" call" << std::endl;
		
	else if (random_flag == true) {
		//collect a vector of the possible words, with each word added once for each time its appearance value. 
		//this makes it so each word has a chance to be chosen proportional to its appearance count.
		std::vector<std::string> words;
		MTRand mtr;	
		for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count){ 
			for (int i=0; i < itr_count->second; ++i)
				words.push_back(itr_count->first);
		}
		int rand = mtr.randInt(words.size()-1);
		return words[rand];
	}

	else {
		int max = 0;					//find the max occurrence
		for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count) {
			if (itr_count->second > max) {
				max = itr_count->second;
				max_location = itr_count;
			}
		}
		return max_location->first; //return the max word
	}
}
	
std::string get_next_word_deep( deep_map &data,  std::string &word1,  std::string &word2, const bool &random_flag) {

	deep_map::iterator deep_data = data.find(word1);
	full_map::iterator itr_word = deep_data->second.find(word2);
	word_count_map::iterator itr_count;

	//dont run this if we don't need a 2nd word
	if (word2 != " ") {
		word_count_map::iterator max_location;
		if (!random_flag) {
			int max = 0;
			for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count) {
				if (itr_count->second > max) {
					max = itr_count->second;
					max_location = itr_count;
				}
			}
			return max_location->first;
		}
		//this part runs relatively the same way as get_next_word
		else if (random_flag == true) {
			std::vector<std::string> words;
			MTRand mtr;
			for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count) {
				for (int i=0 ; i < itr_count->second; ++i)
					words.push_back(itr_count->first);
			}
			int rand_pos = mtr.randInt(words.size()-1);
			return words[rand_pos];
		}
	}
	else {
		full_map::iterator max_location;
		if (!random_flag) {
			//find the 2nd most common word since our phrase.size() == 1
			int max = 0;
			//find the max keys in any map from data
			for (itr_word = deep_data->second.begin(); itr_word != deep_data->second.end(); ++itr_word) {
				if (itr_word->second.size() > max) {
					max = itr_word->second.size();
					max_location = itr_word;
				}
			}
			return max_location->first; 
		}
		else {
			//create a list of words to choose random from, same as get_next_word
			std::vector<std::string> words;
			MTRand mtr;
			for (itr_word = deep_data->second.begin(); itr_word != deep_data->second.end(); ++itr_word) {
				for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
					words.push_back(itr_word->first);
			}
			int rand_pos = mtr.randInt(words.size()-1);
			return words[rand_pos];
		}
	}
}

int main () {

	full_map data;
	deep_map deep_data;
	// Parse each command
	std::string command;  
	int window;  
	std::string last_word, last_word2;
	while (std::cin >> command) {

		// load the sample text file
		if (command == "load") {
			std::string filename;
			std::string parse_method;
			std::cin >> filename >> window >> parse_method;     
			LoadSampleText(data, deep_data, filename, window, parse_method, last_word, last_word2);
		} 

		// print the portion of the map structure with the choices
		// for the next word given a particular sequence.
		else if (command == "print") {
			std::vector<std::string> sentence = ReadQuotedWords(std::cin);
			for (int i = 0; i < sentence.size(); ++i) 
				std::cout << sentence[i] << ' ';

			if (window == 2) {
				full_map::iterator itr_word = data.find(sentence[0]);
				word_count_map::iterator itr_count;
				//find the total appearance of the word
				int sum = 0;
				for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
					sum += itr_count->second;
				if (sentence[0] == last_word)	//the last word doesnt get counted, so add it manually
					++sum;
				std::cout << '(' << sum << ')' << std::endl;
				//print our sentence
				for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
					std::cout << sentence[0] << " " << itr_count->first << " " << "(" << itr_count->second << ")" << std::endl;
					std::cout << std::endl;
			} 
			
			else if (window == 3) {
				word_count_map::iterator itr_count;
				deep_map::iterator deep_data_itr = deep_data.find(sentence[0]);
				int sum = 0;
				//if window == 3, loop through the windows and find the total count of the phrase/word
				if (sentence.size() > 1) {
					full_map::iterator itr_word = deep_data_itr->second.find(sentence[1]);
					for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
						sum += itr_count->second;
					if (sentence[0] == last_word2 && sentence[1] == last_word)
						++sum;
					std::cout << '(' << sum << ')' << std::endl;
					//if we have a >1 word phrase, printing requires some iterator finagling. 
               for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
						std::cout << sentence[0] << " " << itr_word->first << " "<< itr_count->first << 
															" " << "(" << itr_count->second << ")" << std::endl;
					std::cout << std::endl;
				}
				
				else {
					full_map::iterator itr_word;
					int tot;
					//if were only printing one word, we don't need to dig far into our deep_data
					for (itr_word = deep_data_itr->second.begin(); itr_word != deep_data_itr->second.end(); ++itr_word) {
						for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
							sum+=itr_count->second;
					}
					if (sentence[0]==last_word2)
						++sum;
					std::cout << '(' << sum << ')' << std::endl;;
					//find the total occurrences of our word
					for (itr_word = deep_data_itr->second.begin(); itr_word != deep_data_itr->second.end(); ++itr_word) {
						tot = 0;
						for (itr_count = itr_word->second.begin(); itr_count != itr_word->second.end(); ++itr_count)
							tot+=itr_count->second;
						std::cout << sentence[0] << " " << itr_word->first << " " << "(" << tot << ")" << std::endl;
               }
					std::cout << std::endl;
				}
			}
      }

		// generate the specified number of words 
		else if (command == "generate") {
			std::vector<std::string> sentence = ReadQuotedWords(std::cin);
			// how many additional words to generate
			int length;
			std::cin >> length;
			std::string selection_method;
			std::cin >> selection_method;
			bool random_flag;
			if (selection_method == "random") 
				random_flag = true;
			else {
				assert (selection_method == "most_common");
				random_flag = false;
			}

			std::string new_sentence;
			if (window == 2) {
				std::string next_word = sentence[0];
				//Easy printing. For each word, find the next most_common word and spit it out.
				//Rinse and repeat for the desired length.
				for (int i = 0; i <= length; ++i) {
					new_sentence += next_word + ' ';
					next_word = get_next_word(data, next_word, random_flag);
					if (i == length) //clean up the final space.
						new_sentence = new_sentence.substr(0, new_sentence.size()-1);
				}
				std::cout << new_sentence << std::endl << std::endl;
			}

			else if (window == 3) {
				std::string next_word = sentence[0];
				if (sentence.size() > 1) {
					std::string next_word2 = sentence[1];
					new_sentence += next_word + " " + next_word2 + " ";
					for (int i=0; i < length; ++i){
						//find the next word
						std::string temp = get_next_word_deep(deep_data, next_word, next_word2, random_flag);
						//add and reset next and next2
						new_sentence += temp + " ";
						next_word = next_word2;
						next_word2 = temp;
						//delete the last space of the sentence, for perfection's sake
						if (i == length - 1) 
							new_sentence = new_sentence.substr(0, new_sentence.size()-1);
					}	
				}
				else {
					//word our current word
					std::string next_word2 = " ";
					//store our current location
					std::string next_temp;
					for (int i=0; i < length+1; ++i) {
						//check to make sure we aren't on the first iteration
						if (next_word2 != " ")
							next_temp = next_word2;
						next_word2 = get_next_word_deep(deep_data, next_word, next_word2, random_flag);
						if (next_temp.size() > 0)
							next_word = next_temp;
						new_sentence += next_word + " ";
						if (i == length) 
							new_sentence = new_sentence.substr(0, new_sentence.size()-1);
					}
				}
				std::cout << new_sentence << std::endl << std::endl;
        }
      }

		else if (command == "quit") 
			break;
		else 
			std::cout << "WARNING: Unknown command: " << command << std::endl;
	}
}
