#include "head.h"
int cnt=0;
int getOption() {
	string temp = "";
	bool isDigit;
	int size;
	cin >> temp;
	if (temp == "")
		return -1;
	isDigit = true;
	size = temp.size();
	for (int i = 0; i < size; i++)
		if ((temp.at(i)>'9') || (temp.at(i)<'0'))
			isDigit = false;
	if (!isDigit)
		return -1;
	return atoi(temp.c_str());
}
HuffTree<int> * getHuffTree(int* freq, int count) {
	HuffTree<int> **treeArray = new HuffTree<int>*[count];
	for (int i = 0, num = 0; i < 270; i++) {
		if (freq[i] != 0) {
			HuffTree<int>*temp_tree;
			temp_tree = new HuffTree<int>(i, freq[i]);
			*(treeArray + num) = temp_tree;
			num++;
		}
	}
	return buildHuff(treeArray, count);
}
void traverse(HuffNode<int> *root, string code, KVpair<int, string> **form) {
	if (root->isLeaf()) {
		LeafNode<int> * leafNode = dynamic_cast<LeafNode<int> *>(root);
		form[cnt] = new KVpair<int, string>(leafNode->val(), code);
		cnt++;
		return;
	}
	else {
		InitNode<int>* initNode = dynamic_cast<InitNode<int> *>(root);
		traverse(initNode->left(), code + '0', form);
		traverse(initNode->right(), code + '1', form);
	}
}
void compress() {
	string sourceFileName;
	string codeFileName;

	//获取操作文件名
	cout << "please enter a source file name: ";
	cin >> sourceFileName;
	cout << "please enter a code file name: ";
	cin >> codeFileName;

	//读入数据
	ifstream inFile(sourceFileName);
	if (!inFile.is_open()) {
		cout << "can't open file" << endl;
	}
	else {
		//创建频率数组
		int count;
		char curr;
		KVpair<int, string> **p;

		inFile.clear();
		inFile.seekg(ios::beg);

		int *freq = new int[270];
		for (int i = 0; i < 270; i++)
			freq[i] = 0;

		while (!inFile.eof())
		{
			inFile.get(curr);
			//消除最后一行的影响
			if (inFile.eof())
				break;
			//char是有符号的，数组下标是unsigned 所以要换成unsigned char
			freq[(unsigned char)curr]++;
		}
		freq[256] = 1;


		//测试频率数组
		/*for (int i = 0; i < 270; i++)
			if (freq[i] != 0) {
			cout << i << "  ";
			cout << freq[i] << endl;
			}*/

		//创建Huffman树
		count = 0;
		for (int i = 0; i < 270; i++) {
			if (freq[i] != 0) {
				count++;
			}
		}

		//HuffTree<int> **treeArray = new HuffTree<int>*[count];
		//for (int i = 0, count = 0; i < 270; i++) {
		//	if (freq[i] != 0) {
		//		HuffTree<int>*temp_tree;
		//		temp_tree = new HuffTree<int>(i, freq[i]);
		//		*(treeArray + count) = temp_tree;
		//		count++;
		//	}
		//}
		
		HuffTree<int> *tree = getHuffTree(freq, count);

		//创建键值对
		p = new KVpair<int, string>*[count];
		string code = "";
		traverse(tree->root(), code, p);

		//测试键值对
			//for (int i = 0; i < count; i++) {
			//cout << p[i]->key() << ": ";
			//cout << p[i]->value() << endl;
			//}

		//写入数据
		ofstream outFile(codeFileName);
		if (!outFile.is_open()) {
			cout << "can't open file" << endl;
		}
		else {
			outFile << count;
			//写入频率数组
			unsigned char c;
			for (int i = 0; i < 270; i++) {
				if (freq[i] != 0 && i != 256) {
					c = freq[i];
					outFile << c;
				}
			}
			for (int i = 0; i < 270; i++) {
				if (freq[i] != 0 && i != 256) {
					c = i;
					outFile << c;
				}
			}
			outFile << " ";

			//写入编译后数据
			unsigned char tmp_code, out_code;
			string code, out_string;
			int length;
			code.clear();
			//读完文件
			inFile.clear();
			inFile.seekg(ios::beg);
			while (!inFile.eof()) {
				inFile.get(curr);
				//消除最后一行的影响
				if (inFile.eof())
					break;
				//找到对应的键值对
				for (int j = 0; j < count; j++)
					if (curr == p[j]->key()) {
						code += p[j]->value();
						length = code.size();
						if (length > 10) {
							int m, n;
							out_string.clear();
							for (m = 0; m + 7 < length; m += 8) {
								out_code = 0;
								for (n = 0; n < 8; n++) {
									if (code[m + n] == '0')
										tmp_code = 0;
									else
										tmp_code = 1;
									out_code += tmp_code << (7 - n);
								}
								out_string += out_code;
							}
							outFile << out_string;
							code = code.substr(m, length - m);
						}
						break;
					}
			}

			//插入结尾标识符
			for (int i = 0; i < count; i++) {
				if (p[i]->key() == 256) {
					code += p[i]->value();
					break;
				}
			}

			//处理剩下编码
			length = code.size();
			out_code = 0;
			for (int i = 0; i < length; i++) {
				if (code[i] == '0')
					tmp_code = 0;
				else
					tmp_code = 1;
				out_code += tmp_code << (7 - (i % 8));
				if ((i + 1) % 8 == 0 || i == length - 1) {
					outFile << out_code;
					out_code = 0;
				}
			}
		}
		outFile.close();
	}
	inFile.close();

	cout << "have finished" << endl;
}
void decompress() {
	string codeFileName;
	string targetFileName;

	cout << "please enter a code file name: ";
	cin >> codeFileName;
	cout << "please enter a target file name: ";
	cin >> targetFileName;

	//读入数据
	ifstream inFile(codeFileName);
	if (!inFile.is_open()) {
		cout << "can't open file" << endl;
	}
	else {
		int keyNum;
		int *freq;
		int *tmpFreq;
		char curr;
		
		//读取频率数组
		keyNum = 0;
		inFile >> keyNum;
		freq = new int[270];
		tmpFreq = new int[keyNum-1];

		for (int i = 0; i < 270; i++)
			freq[i] = 0;

		for (int i = 0; i < keyNum-1; i++) {
			inFile.get(curr);
			tmpFreq[i] = curr;
		}
		for (int i = 0; i < keyNum-1; i++) {
			inFile.get(curr);
			freq[curr] = tmpFreq[i];
		}
		freq[256] = 1;
		
		//测试频率数组
		/*for (int i = 0; i < 270; i++)
			if (freq[i] != 0) {
				cout << i << "  ";
				cout << freq[i] << endl;
			}*/

		HuffTree<int> *tree = getHuffTree(freq, keyNum);
		HuffNode<int> *root = tree->root();
		/*for (int i = 0; i < keyNum; i++) {
			char tmpChar;
			char* tmpString;
			int tmpInt;
			int tmpStringSize;

			inFile >> tmpInt;
			tmpChar = tmpInt;
			inFile.seekg(1, ios::cur);

			tmpStringSize = 0;
			curr = ' ';
			while (curr != '.') {
				tmpStringSize++;
				inFile.read((char*)&curr, sizeof(char));
			}
			tmpString = new char[tmpStringSize];
			inFile.seekg(-tmpStringSize, ios::cur);
			inFile.read(tmpString, tmpStringSize - 1);
			*(tmpString + tmpStringSize) = '\0';
			inFile.seekg(1, ios::cur);

			int codeLength;
			string code(tmpString);
			codeLength = code.size() - 1;
			node = root;
			for (int j = 0; j < codeLength; j++) {
				if (code[j] == '0')
					tmp_node = dynamic_cast<InitNode<int>*>(node)->left();
				else
					tmp_node = dynamic_cast<InitNode<int>*>(node)->right();

				if (tmp_node == NULL) {
					if (j == codeLength - 1)
						new_node = new LeafNode<int>(tmpInt, 0);
					else
						new_node = new InitNode<int>(NULL, NULL);

					if (code[j] == '0')
						dynamic_cast<InitNode<int>*>(node)->setLeft(new_node);
					else
						dynamic_cast<InitNode<int>*>(node)->setRight(new_node);

					tmp_node = new_node;
				}

				node = tmp_node;
			}
		}*/

		//测试Huffman树
		//HuffNode<int> *miao, *miao2, *miao3;
		//miao = dynamic_cast<InitNode<int>*>(root)->right();
		//miao2 = dynamic_cast<InitNode<int>*>(miao)->left();
		//miao3 = dynamic_cast<InitNode<int>*>(miao2)->left();
		//if (dynamic_cast<InitNode<int>*>(miao3)->right()->isLeaf()) {
		//cout << "haha" << endl;
		//}
		//if (dynamic_cast<InitNode<int>*>(miao3)->left()->isLeaf()) {
		//cout << "haha" << endl;
		//}

		//写入数据
		ofstream outFile(targetFileName);
		if (!outFile.is_open()) {
			cout << "can't open file" << endl;
		}
		else {
			int id;
			bool eof;
			string out_string;
			HuffNode<int> *node;
			unsigned char u_curr, flag;

			eof = false;
			node = root;
			inFile.seekg(1, ios::cur);
			//读完文件
			while (!inFile.eof()) {
				inFile.get(curr);
				u_curr = (unsigned char)curr;
				flag = 0x80;
				for (int i = 0; i < 8; i++) {
					if (u_curr&flag)
						node = dynamic_cast<InitNode<int>*>(node)->right();
					else
						node = dynamic_cast<InitNode<int>*>(node)->left();
					if (node->isLeaf()) {
						id = dynamic_cast<LeafNode<int>*>(node)->val();
						if (id == 256) {
							eof = true;
							break;
						}
						if (id == 10) {
							out_string += "\n";
							node = root;
						}
						else {
							out_string += id;
							node = root;
						}
					}
					flag = flag >> 1;
				}
				//cout << out_string << endl;
				if (eof) break;
				if (out_string.length() > 10) {
					outFile << out_string;
					out_string.clear();
				}
			}

			//处理剩余编码
			if (!out_string.empty())
				outFile << out_string;
		}
		outFile.close();
	}
	inFile.close();

	cout << "have finished" << endl;
}
int main() {
	int option = -1;
	while (1) {
		try {
			cout << "Welcome!" << endl;
			cout << "0.exit" << endl;
			cout << "1.compress" << endl;
			cout << "2.decompress" << endl;
			option = getOption();
			switch (option) {
			case 1:
				compress();
				break;
			case 2:
				decompress();
				break;
			case 0:
				exit(-1);
			default:
				throw "valid value of option";
			}
			option = -1;
		}
		catch (char *str) {
			cout << str << endl;
		}
		catch (exception e) {
			cout << e.what() << endl;
		}
		system("pause");
	}
	return 0;

}
