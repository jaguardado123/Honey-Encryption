#include <bitset>
#include <bits/stdc++.h>
#include <queue>
#include <unordered_map>
#include <string>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "base64encode.h"

using namespace std;

class DTE {
private:

	//Node class for Hoffman code tree.
	class Node {
	public:
		Node *left;
		Node *right;
		char letter;
		int freq;
		bool isletter;
	
		Node(int f) {
			left = right = NULL;
			isletter = false;
			freq = f;
		}
		Node(char l, int f) {
			left = right = NULL;
			isletter = true;
			letter = l;
			freq = f;
		}
	};

	Node *root;
	unordered_map<char, string> to_binary; //Keeps track of which binary string belongs to which character, ex: 'a' -> 10010000 .
	string message;

	void createTree(string msg) {

		// string lower = "abcdefghijklmnopqrstuvwxyz"; //26
		// string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //26
		// string nums = "0123456789"; //10
		// string symb = "()-+={}[]<>*~@#$%^&:'/;.,?!"; //28
		string alphabet = "";
		for (int i=0; i<256; i++) {
			alphabet += msg[i%msg.length()];
		}

		//Will create a binary tree with 256 leaf nodes at the bottom most layer.
		//Lower case letters are more frequently used, and symbols are the least frequently used.
		//string alphabet = " " + lower + upper + lower + nums + lower + symb + lower + nums + lower + upper + lower;

		//Creates Hoffman code tree.
		//Tree is of height 8, or log(256).
		//It utilizes characters more than once in order to keep property |M| = |S|.
		//Original Hoffman scheme compreses binary string, however we want our Seed space to be equal to our Message space.
		//This way also ensures each character gets assigned an 8 bit binary string.
		int index = 255;
		queue <Node*> nodeQ;
		while(index > 0) {

			//Create three set of nodes.
			//Both children have letters and frequencies.
			Node *child_1 = new Node(alphabet[index], 1);
			index--;
			Node *child_2 = new Node(alphabet[index], 1);
			index--;
			//Parent node holds the sum of 2 letter's frequencies.
			Node *parent = new Node(child_1->freq + child_2->freq);
			parent->right = child_1;
			parent->left = child_2;
			//Push parent node into queue. Parent will now be considered a child node.
			//256 leaf nodes will be stored in queue by the end.
			nodeQ.push(parent);
		}

		//Each two queue'd nodes are assigned a parent node.
		//Runs in O(log n) time. 
		while (!nodeQ.empty()) {

			Node *child_1 = nodeQ.front();
			nodeQ.pop();
			Node *child_2 = nodeQ.front();
			nodeQ.pop();

			//If only two nodes remain, attach to root.
			if (nodeQ.empty()) {
				root = new Node(child_1->freq + child_2->freq);
				root->right = child_1;
				root->left = child_2;
			}
			else {
				Node *parent = new Node(child_1->freq + child_2->freq);
				parent->right = child_1;
				parent->left = child_2;

				nodeQ.push(parent);
			}
		}
	}

	//Traverse throught tree all the way to leaf nodes.
	//Assigns each character a binary string according to the path traversed to get to it.
	//About how this works: https://www.youtube.com/watch?v=apcCVfXfcqE
	void traverse(Node *p, string binary) {

		if (p == NULL)
			return;
		else if (p->isletter) {
			to_binary[p->letter] = binary;
			return;
		}
		else {
			traverse(p->left, binary + "0");
			traverse(p->right, binary + "1");
		}

	}

	//Traverses throgh tree and returns corresponding 8 bit binary string for each char in Message.
	string s_to_binary(string msg) {

		traverse(root, "");
		string binaryS = "";
		string temp;
		//cout << "DTE" << endl;
		for(int i=0; i<msg.length(); i++) {
			temp = to_binary[msg[i]];
			//cout << msg[i] << " : " << temp << endl;
			binaryS += temp;
		}

		//Adds additional 0's to return 64 bit binary string.
		//cout << binaryS.length() << endl;
		while(binaryS.length()%64 != 0) {
			binaryS += "00000000";
		}
		cout << binaryS.length() << endl;

		return binaryS;
	}

	//Follows tree path to find corresponding char value.
	//If next bit is zero go left, else go right.
	//Every input binary string has 8 bits, so it is guaranteed to return a char.
	char find_binary(Node *p, string binary) {

		if (p == NULL || binary.length() == 0)
			return ' ';
		else if (p->isletter)
			return p->letter;
		else if (binary[0] == '0')
			return find_binary(p->left, binary.substr(1, binary.length() - 1));
		else
			return find_binary(p->right, binary.substr(1, binary.length() - 1));
	}

	//Iterates through string by 8 bits, gets char value from Hoffman tree.
	string b_to_string(string binary) {

		string msg = "";
		int end = 7;
		string temp;
		//cout << "DTE" << endl;
		for(int i=0; i<binary.length(); i+=8) {
			temp = find_binary(root, binary.substr(i, end));
			//cout << temp << endl;
			msg += temp;

			end += 8;
		}
		return msg;
	}

public:

	//Constructor, sets root to null.
	DTE() {
		root = NULL;
	}

	//Creates Hoffman Code binary tree.
	//Returns encoded english text in binary form.
	string encode(string msg) {

		message = msg;
		if (root == NULL)
			createTree(msg);

		return s_to_binary(msg);;
	}

	//Creates Hoffman Code binary tree.
	//Returns decoded binary string into english text.
	string decode(string binary) {

		if (root == NULL)
			createTree(message);

		return b_to_string(binary);
	}
};


class Honey {
private:

	DTE dte;

	//Converts message m from string to 64-bit binary string.
	string s_to_binary(string m) {

		string binary = "";
		string temp;
		//cout << "Honey: " << endl;
		for(int i=0; i<m.length(); i++) {
			//Converts every char in m into an 8 bit string.
			temp = bitset<8>(m[i]).to_string();
			//cout << m[i] << " : " << temp << endl;
			binary += temp;
		}

		//Adds additional 0's to return 64 bit binary string.
		while(binary.length()%64 != 0) {
			binary += "00000000";
		}

		return binary;
	}

	//Converts binary string to hexadecimal.
	string b_to_hexadecimal(string b) {

		stringstream res;
		int end = 3;
		for(int i=0; i<=b.length(); i+=4) {
			//converts the first 4 bits into hexadecimal.
			bitset<4> set(b.substr(i, end));
			res << hex << set.to_ulong();
			end += 4;
		}
		//return "f";
		return res.str();
	}

	//Converts hexadecimal back to binary.
	string h_to_binary(string h) {

		//unordered_map<char, string> char_to_binary;
		unordered_map<char, string> to_binary;
		to_binary['0'] = "0000";
		to_binary['1'] = "0001";
		to_binary['2'] = "0010";
		to_binary['3'] = "0011";
		to_binary['4'] = "0100";
		to_binary['5'] = "0101";
		to_binary['6'] = "0110";
		to_binary['7'] = "0111";
		to_binary['8'] = "1000";
		to_binary['9'] = "1001";
		to_binary['a'] = "1010";
		to_binary['b'] = "1011";
		to_binary['c'] = "1100";
		to_binary['d'] = "1101";
		to_binary['e'] = "1110";
		to_binary['f'] = "1111";

		string binaryS = "";
		for(int i=0; i<h.length(); i++) {
			binaryS += to_binary[h[i]];
		}

		return binaryS;
	}

	//xor's to binary strings.
	string xor_binary(string m, string k) {

		string xor_b = "";
		int size;
		if (m.length() > k.length())
			size = m.length();
		else
			size = k.length();

		//cout << "XOR" << endl;
		//int j = 1;
		for (int i=0; i<size; i++) {
			if (m[i%m.length()] != k[i%k.length()])
				xor_b += "1";
			else
				xor_b += "0";
			//if (j%4 == 0)
			//	cout << xor_b << endl;
			//j++;
		}
		return xor_b;
	}

	//Converts binary back to string.
	string b_to_string(string b) {

		string str = "";
		int end = 8;
		char temp;
		for (int i=0; i<b.length(); i+=8) {
			temp = static_cast<unsigned char>(bitset<8>(b.substr(i, end)).to_ulong());
			end+=8;
			str += temp;
		}

		return str;
	}

public:

	//Encrypts password by xor'ing its binary string with the key word's binary string,
	// then transforms to hexadecimal.
	string encrypt(string message, string key) {

		string binary = s_to_binary(message);//dte.encode(" " + message);
		string binary_key = s_to_binary(key);

		string binary_xor = xor_binary(binary, binary_key);
		string cipher = b_to_hexadecimal(binary_xor);

		return cipher;
	}

	//Decrypts hexadecimal cipher by xo'ring its binary string with they key word's binary string,
	// then transforms from binary to alphanumeric string.
	//If the output string is invalid, then generate a new password.
	string decrypt(string cipher, string key) {

		string binary = h_to_binary(cipher);
		string binary_key = s_to_binary(key);

		string binary_xor = xor_binary(binary, binary_key);
		
		string msg = b_to_string(binary_xor);//dte.decode(binary_xor);

		return msg;
	}

};


int main(int argc, char * argv[]) {


	string message = "";
	string key = argv[1];

	for(int i=2; i<argc; i++) {
		message = message + " " + argv[i];
	}

	Honey honey;
	cout << base64_encode(honey.encrypt(message, key), true) << endl;

	return 0;
}