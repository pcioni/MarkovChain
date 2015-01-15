HOMEWORK 7: WORD FREQUENCY MAPS


NAME:  Philip Cioni


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

cplusplus.com std::map documentation, Daniel Hayes

Remember: Your implementation for this assignment must be done on your
own, as described in "Academic Integrity for Homework" handout.



ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  9



ANALYSIS OF PERFORMANCE OF YOUR ALGORITHM:
(order notation & concise paragraph, < 200 words)

n = total number of words in the sample text file
m = number of unique words in the file
w = width of the sequencing window
p = average number of words observed to follow a particular word

memory: 

window-2: O(mlogn)
We only loop through the total number of words and find our words.

window-3: O(m(logn^2))
again, we only loop through the total number of words and find our words.

window-n: O(m(logn^(w-1))
The size of the window won't matter


What is the order notation for performance (running time) of each of
the commands?

load: O(m)
Load works very quickly, only calling a ++ operator once per word in
the data file.

print:
window-2: O(p^2)
window-3: O(p^2) for >1 word and O(m^2*p)
Both run incredibly well, with the exception of 1-word prints for a 
window of size 3, which isn't slow, but more cumbersome and complicated
than the longer phrase printing.

generate: 
h = num of words to generate
window 2: O(h*m)
window 3: O(h*m*p)
window n: O(h*m*p^(w-1))
Loop through get_next_word (O(m) for each new word we want (h).
Almost instantaneous, even for larger inputs.

EXTRA CREDIT
Parsing & using punctuation data, implementation supports *any*
window size, new test cases (describe & summarize performance, but
don't include large datasets with submission).

I feel like a 3-word-deep (size 4 window) is the best size for this
type of operation. After the third word, three-word pairs will get so
unique and rare that any sensible sentence will be a rare occurrence.
Four is the average size of common phrases such as "go to the x" or 
"came from a x", meaning sensible statements are still possible and
common.
-------------------------------------------------------------------
There was also a funny statement that generated --
specifically, shakespeare.txt -- generate "from" 23 most_common --
any number above 23 will throw the sentence into an endless loop
of the same phrase over and over again. 

"from the king and queen and her two sons and brothers haught and proud 
and ambitious humour makes him to the king and queen and her two sons and 
brothers haught and proud and ambitious humour makes him to the king and queen..."

making this Markov chain and endless list of a king, queen, and her two 
haught, proud, and ambitious brothers and sons (and another why a window size > 4 is superfluous!)


MISC. COMMENTS TO GRADER:  
(optional, please be concise!)


