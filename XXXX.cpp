// -----------------------------------------------------------------
// HOMEWORK 7 WORD FREQUENCY MAPS
//
// You may use all of, some of, or none of the provided code below.
// You may edit it as you like (provided you follow the homework
// instructions).
// -----------------------------------------------------------------
 
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "MersenneTwister.h"
 
 
 
//MAP TYPEDEFS for multilevel maps
typedef std::map<std::string, int> SI_MAP;
typedef std::map<std::string, SI_MAP>  MY_MAP2;
typedef std::map<std::string, MY_MAP2> MY_MAP3;
 
 
 
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
// structure Window specifies the width of the context (>= 2) of the
// sequencing stored in the map.  parse_method is a placeholder for
// optional extra credit extensions that use punctuation.
void LoadSampleText(MY_MAP2 &data, MY_MAP3 &data3, const std::string &filename, int window, const std::string &parse_method, std::string &last_word, std::string &last_word2) {
  // open the file stream
  std::ifstream istr(filename.c_str());
  if (!istr) {
    std::cerr << "ERROR cannot open file: " << filename << std::endl;
    exit(1);
  }
  // verify the window parameter is appropriate
  if (window < 2) {
    std::cerr << "ERROR window size must be >= 2:" << window << std::endl;
  }
  // verify that the parse method is appropriate
  bool ignore_punctuation = false;
  if (parse_method == "ignore_punctuation") {
    ignore_punctuation = true;
  } else {
    std::cerr << "ERROR unknown parse method: " << parse_method << std::endl;
    exit(1);
  }
  //list of words to add to
  std::string word;
  std::vector<std::string> words;
  while (ReadNextWord(istr,word)) {
    // skip the quotation marks (not used for this part)
    if (word == "\"") continue;
    words.push_back(word);
  }
  if (window==2){ //load for window=2
    for (int i=0;i<words.size()-1;++i){
       data.insert(make_pair(words[i],SI_MAP()));
       ++(data[words[i]][words[i+1]]);
    }
  }
  else if (window==3){ //load for window=3
    for (int i=0;i<words.size()-2;++i){
      data3.insert(make_pair(words[i],MY_MAP2()));
      data3[words[i]].insert(make_pair(words[i+1],SI_MAP()));
       ++(data3[words[i]][words[i+1]][words[i+2]]);
    }
  }
  //find last works (a check to make sure we count words at the end of the file)
  last_word = words[words.size()-1];
  last_word2 = words[words.size()-2];
  std::cout<<"Loaded "<<filename<<" with window = " << window << " and parse method = " << parse_method
  <<std::endl<<std::endl;
}
//function to find the next word based on window=2
std::string find_next_word(MY_MAP2 &data, std::string word, bool random_flag){
  MY_MAP2::iterator itr;
  SI_MAP::iterator itr2;
  for (itr = data.begin();itr!=data.end();++itr){
    //find our words spot in master map
    if (word==itr->first){
      //find the max value word, not random
      if (!random_flag){
        int max = 0;
        for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
          if (itr2->second > max)
            max = itr2->second;
        }
        //return word that is the max
        for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
          if (itr2->second == max)
            return itr2->first;
        }
      }
      //if we're looking for random...
      else {
        //create a list of possibilities to choose random from
        std::vector<std::string> possibilities;
        //random object
        MTRand mtr;
        for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
          for (int i=0;i<itr2->second;++i)
            possibilities.push_back(itr2->first);
        }
        int rand_pos = mtr.randInt(possibilities.size()-1);
        return possibilities[rand_pos];
      }
    }
  }
  //this shouldn't ever be used... but just in case
  return "nothing-found";
}
//function to find next word based on window=3
std::string find_next_word3(MY_MAP3 &data, std::string word1, std::string word2, bool random_flag){
  //three iterators for iterating through different stages of the data map
  MY_MAP3::iterator itr3; //deep_ data
  MY_MAP2::iterator itr;  //itr_word
  SI_MAP::iterator itr2;  //word_count_map
  for (itr3=data.begin();itr3!=data.end();++itr3){
    //find first word
    if (word1==itr3->first){
      //check to which iteration of the 3rd window find commmand we're at (starting has word2 = " ", after that, it's set)
      if (word2!=" "){
        for (itr=itr3->second.begin();itr!=itr3->second.end();++itr){
          //find second word
          if (word2==itr->first){
            //find the max value word
            if (!random_flag){
              int max = 0;
              for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
                if (itr2->second > max)
                  max = itr2->second;
              }
              //return word that is the max
              for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
                if (itr2->second == max)
                  return itr2->first;
              }
            }
            //random!
            else {
              //create a list of possibilities to choose random from
              std::vector<std::string> possibilities;
              //random object
              MTRand mtr;
              for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
                for (int i=0;i<itr2->second;++i)
                  possibilities.push_back(itr2->first);
              }
              int rand_pos = mtr.randInt(possibilities.size()-1);
              return possibilities[rand_pos];
            }
          }
        }
      }
      else {
        if (!random_flag){
          //find the most common 2nd word, because we're only generating from one word
          int max = 0;
          int size;
          //find the most keys in any map
          for (itr = itr3->second.begin();itr!=itr3->second.end();++itr){
            size = 0;
            for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
              ++size;
            }
            if (size > max)
              max = size;
          }
          //loop through and find the word that has the most keys in its map
          for (itr = itr3->second.begin();itr!=itr3->second.end();++itr){
            size = 0;
            for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
              ++size;
            }
            if (size==max){
              return itr->first;
            }
          }
        }
        //if random
        else {
          //create a list of possibilities to choose random from
          std::vector<std::string> possibilities;
          //random object
          MTRand mtr;
          for (itr=itr3->second.begin();itr!=itr3->second.end();++itr){
            for (itr2=itr->second.begin();itr2!=itr->second.end();++itr2){
              possibilities.push_back(itr->first);
            }
          }
          int rand_pos = mtr.randInt(possibilities.size()-1);
          return possibilities[rand_pos];
        }
      }
    }
  }
}
 
int main () {
 
  // ASSIGNMENT: THE MAIN DATA STRUCTURE
  MY_MAP2 data;
  MY_MAP3 data3;
 
  // Parse each command
  std::string command; 
  //special last word to make sure we count number of times correctly
  std::string last_word;
  std::string last_word2;
  int window; 
  while (std::cin >> command) {
 
    // load the sample text file
    if (command == "load") {
      std::string filename;
       
      std::string parse_method;
      std::cin >> filename >> window >> parse_method;     
      //load text in
      LoadSampleText(data, data3, filename, window, parse_method, last_word, last_word2);
 
    }
    // print the portion of the map structure with the choices for the
    // next word given a particular sequence.
    else if (command == "print") {
      std::vector<std::string> sentence = ReadQuotedWords(std::cin);
      //print sentence label for
      for (int i=0;i<sentence.size();++i){
        std::cout<<sentence[i];
        if (i!=sentence.size()-1){
          std::cout<<" ";
        }
      }
      //print the header of the current word we're looking at. ex: could(3)
      std::cout<<" (";
      //some iterators to iterate through our data object
      MY_MAP3::iterator itr3;
      MY_MAP2::iterator itr;
      SI_MAP::iterator itr2;
      if (window==2){
        //if our window is 2, loop through and find total count of the printed word
        for (itr = data.begin();itr!=data.end();++itr){
          if (sentence[0] == itr->first){
              //sum all of the values in right hand
              int sum = 0;
              for (itr2 = itr->second.begin(); itr2!=itr->second.end();++itr2){
                sum+=itr2->second;
              }
              if (sentence[0]==last_word){
                ++sum;
              }
              std::cout<<sum;
          }
        }
      }
      else if (window==3){
        //if our window is 3, loop through all windows and find total count of the printed word
        for (itr3 = data3.begin();itr3!=data3.end();++itr3){
          if (sentence[0] == itr3->first){
            if (sentence.size()>1){
              for (itr = itr3->second.begin(); itr!=itr3->second.end();++itr){
                if (sentence[1] == itr->first){
                  int sum = 0;
                  for (itr2 = itr->second.begin(); itr2!=itr->second.end();++itr2){
                    sum+=itr2->second;
                  }
                  if (sentence[0] == last_word2 && sentence[1]==last_word){
                    ++sum;
                  }
                  std::cout<<sum;
                }
              }
            }
            else {
              //if we're just looking at printing 1 word with window3
              int sum = 0;
              for (itr = itr3->second.begin(); itr!=itr3->second.end();++itr){
                for (itr2 = itr->second.begin(); itr2!=itr->second.end();++itr2){
                  sum+=itr2->second;
                }
              }
              if (sentence[0]==last_word2){
                  ++sum;
              }
              std::cout<<sum;
            }
          }
        }
      }
      std::cout<<")";
      std::cout<<std::endl;
 
 
      //print for window = 2
      if (window==2){
        for (itr = data.begin();itr!=data.end();++itr){
          if (sentence[0] == itr->first){
            for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
              std::cout<<sentence[0]<<" "<<itr2->first<<" "<<"("<<itr2->second<<")"<<std::endl;
            }
          }
        }
      }
      //print for window = 3
      else if (window==3){
        for (itr3 = data3.begin();itr3!=data3.end();++itr3){
          if (sentence[0] == itr3->first){
            if (sentence.size()>1){
              for (itr = itr3->second.begin(); itr!=itr3->second.end();++itr){
                if (sentence[1] == itr->first){
                  for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
                    std::cout<<sentence[0]<<" "<<itr->first<<" "<<itr2->first<<" "<<"("<<itr2->second<<")"<<std::endl;
                  }
                }
              }
            }
            //if we're just looking for print from one word
            else {
              for (itr = itr3->second.begin(); itr!=itr3->second.end();++itr){
                int tot = 0;
                for (itr2 = itr->second.begin();itr2!=itr->second.end();++itr2){
                  tot+=itr2->second;
                }
                std::cout<<sentence[0]<<" "<<itr->first<<" "<<"("<<tot<<")"<<std::endl;
              }
            }
          }
        }
      }
      std::cout<<std::endl;
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
      if (selection_method == "random") {
        random_flag = true;
      } else {
         assert (selection_method == "most_common");
         random_flag = false;
      }
      //sentence to output to
      std::string generated_sentence;
       
        if (window==2){
          std::string next_word = sentence[0];
          for (int i=0;i<length+1;++i){
            generated_sentence+=next_word;
            next_word = find_next_word(data, next_word, random_flag);
            if (i!=length)
              generated_sentence+=" ";
          }
          std::cout<<generated_sentence<<std::endl<<std::endl;
        }
        if (window==3){
          std::string next_word = sentence[0];
          if (sentence.size()>1){
            std::string next_word2 = sentence[1];
            for (int i=0;i<length+1;++i){
              generated_sentence+=next_word;
              generated_sentence+=next_word2;
              //find next based on two words
              std::string temp = find_next_word3(data3, next_word, next_word2, random_flag);
              //add and reset next and next2
              generated_sentence+=temp;
              next_word = next_word2;
              next_word2 = temp;
              //tack on a space if we need it between letters
              if (i!=length)
                generated_sentence+=" ";
            }
          }
          else {
            //word after the current one we're looking at
            std::string next_word2 = " ";
            //temporary to store while we find next on next and next2
            std::string next_temp;
            for (int i=0;i<length+1;++i){
              if (next_word2!=" "){
                next_temp = next_word2;
              }
              next_word2 = find_next_word3(data3, next_word, next_word2, random_flag);
              if (next_temp!=""){
                next_word = next_temp;
              }
              generated_sentence+=next_word;
              if (i!=length)
                generated_sentence+=" ";
            }
          }
          std::cout<<generated_sentence<<std::endl<<std::endl;
        }
 
    } else if (command == "quit") {
      break;
    } else {
      std::cout << "WARNING: Unknown command: " << command << std::endl;
    }
  }
}