#include <iostream>
#include <fstream>
#include<algorithm>
#include <sstream>
#include <thread>
#include <vector>




bool customCompare(const std::string& a, const std::string& b) {
	// Define your custom order here
	const std::string customAlphabet = "etaonrishdlfcmugypwbvkjxzq";

	// Find the minimum size between a and b
	size_t minSize = std::min(a.size(), b.size());

	// Compare characters based on the custom order
	for (size_t i = 0; i < minSize; i++) {
		if (customAlphabet.find(a[i]) < customAlphabet.find(b[i])) {
			return true;
		}
		else if (customAlphabet.find(a[i]) > customAlphabet.find(b[i])) {
			return false;
		}
		// If characters are equal, continue to the next character
	}

	// If one string is a prefix of the other, the shorter one comes first
	return a.size() < b.size();
}














bool isUnique(const std::string& word) {
	for (int i = 1; i < 5; i++) {
		if (word.substr(0, i).find(word[i]) != std::string::npos) {
			return false;
		}
	}
	return true;
};



int wordToInt(const std::string& letterOrder, const std::string& word) {
	int result = 0;
	for (int i = 0; i < word.length(); i++) {
		result |= 1 << letterOrder.find(word[i]);
	}
	return result;
}




std::vector<std::string> removeAnagrams(const std::vector<std::string>& words, const std::vector<int>& wordsINT) {
	std::vector<std::string> resultWords;
	for (int i = 1; i < words.size(); i++) {
		if (wordsINT[i - 1] != wordsINT[i]) {
			resultWords.emplace_back(words[i - 1]);
		}
	}
	if (wordsINT[words.size() - 2] != wordsINT[words.size() - 1]) {
		resultWords.emplace_back(words[words.size() - 1]);
	}
	return resultWords;
}


std::vector<std::string> loadWords(const std::string& filePath, const std::string& letterOrder) {
	std::ifstream file(filePath);

	
	std::vector<int> wordsVectorINT;
	std::vector<std::string> wordsVectorSTR;
	std::string line;

	std::cout << "loading words...\n";
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string word;

		while (iss >> word) {


			if (isUnique(word)) {

				int wordINT = wordToInt(letterOrder, word);

				auto it = std::lower_bound(wordsVectorINT.begin(), wordsVectorINT.end(), wordINT);
				int index = std::distance(wordsVectorINT.begin(), it);

				wordsVectorINT.insert(it, wordINT);
				wordsVectorSTR.insert(wordsVectorSTR.begin() + index, word);
			}

		}
	}
	file.close();

	wordsVectorSTR = removeAnagrams(wordsVectorSTR, wordsVectorINT);
	

	return wordsVectorSTR;
}





std::vector<short> computeSteps(const std::vector<int>& wordsINT) {
	std::vector<short> steps;
	steps.resize(wordsINT.size());
	for (int i = 0; i < wordsINT.size(); i++) {
		int A = wordsINT[i];
		short j;
		for (j = i + 1; j < wordsINT.size(); j++) {
			int B = wordsINT[j];
			if ((A & B) == 0) break;
		}
		steps[i] = j;
	}
	return steps;
}



std::vector<std::vector<short>> computeSkips2d(const std::vector<int>& wordsINT) {
	std::vector<std::vector<short>> skip;
	skip.resize(wordsINT.size());
	for (int i = 0; i < wordsINT.size(); i++) {
		skip[i].resize(wordsINT.size() + 1);
		skip[i][wordsINT.size()] = (short)wordsINT.size();
		int A = wordsINT[i];
		for (int j = wordsINT.size() - 1; j >= i; j--) {
			int B = wordsINT[j];
			skip[i][j] = ((A & B) == 0) ? (short)j : skip[i][j + 1];
		}
	}
	return skip;
}


std::vector<int> wordsToInts(const std::vector<std::string>& words, const std::string& letterOrder) {
	std::vector<int> wordsINT;
	wordsINT.resize(words.size());
	for (int i = 0; i < words.size(); i++) {
		wordsINT[i] = wordToInt(letterOrder, words[i]);
	}
	return wordsINT;
}

void mainSearch(const std::vector<int>& wordsINT, const std::vector<std::string>& wordsSTR, const std::vector<short>& initSkip, const std::vector<std::vector<short>>& skip, const short& totalThreads, const short& threadNum) {
	const int wordsSize = wordsINT.size();
	int comparitor = (threadNum + 1) * (wordsSize / totalThreads);
	if (threadNum == totalThreads - 1) comparitor = wordsSize;
	for (int first = threadNum*(wordsSize/totalThreads); first < comparitor; first++) {
		int A = wordsINT[first];
		for (int second = initSkip[first]; second < wordsSize; second++,
			second = skip[first][second]) {
			int B = wordsINT[second];
			if ((A & B) != 0) continue;
			int AB = A | B;
			for (int third = initSkip[second]; third < wordsSize; third++,
				third = skip[first][third],
				third = skip[second][third]) {
				int C = wordsINT[third];
				if ((AB & C) != 0) continue;
				int ABC = AB | C;
				for (int fourth = initSkip[third]; fourth < wordsSize; fourth++,
					fourth = skip[first][fourth],
					fourth = skip[second][fourth],
					fourth = skip[third][fourth]) {
					int D = wordsINT[fourth];
					if ((ABC & D) != 0) continue;
					int ABCD = ABC | D;
					for (int fifth = initSkip[fourth]; fifth < wordsSize; fifth++,
						fifth = skip[first][fifth],
						fifth = skip[second][fifth],
						fifth = skip[third][fifth],
						fifth = skip[fourth][fifth]) {
						int E = wordsINT[fifth];
						if ((ABCD & E) != 0) continue;
						printf("FOUND: %s %s %s %s %s\n", wordsSTR[first].c_str(), wordsSTR[second].c_str(), wordsSTR[third].c_str(), wordsSTR[fourth].c_str(), wordsSTR[fifth].c_str());
					}
				}
			}
		}
	}
}

void wordleThing() {
	const std::string letterOrder = "etaonrishdlfcmugypwbvkjxzq";


	
	std::vector<std::string> wordsSTR = loadWords("words.txt", letterOrder);
	std::sort(wordsSTR.begin(), wordsSTR.end(), customCompare);
	
	const std::vector<int> wordsINT = wordsToInts(wordsSTR, letterOrder);

	const std::vector<std::vector<short>> skip = computeSkips2d(wordsINT);
	const std::vector<short> initSkip = computeSteps(wordsINT);


	int totalThreads = 10;
	std::vector<std::thread> threads;
	for (int threadNum = 0; threadNum < totalThreads; threadNum++) {
		threads.emplace_back([&, threadNum]() {
			mainSearch(wordsINT, wordsSTR, initSkip, skip, totalThreads, threadNum);
			});
	}
	for (auto& thread : threads) {
		thread.join();
	}
}











int main() {
	wordleThing();
	return 0;
}