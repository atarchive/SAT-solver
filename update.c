#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<algorithm>
#include<unordered_map>
#define MAX_NODE 1000
#define MAX_GATE 1000

using namespace std;

struct NodeS {
	vector<int>gate;
};

struct GateS {
	vector<int>nodes;
};

int N, G = 0;
vector<int>assignment;
vector<int>poly,note;
vector<pair<int, int>>counter;
unordered_map<int, int>match;

bool cmp(pair<int, int>a, pair<int, int>b) { return (a.second > b.second); }

bool readfile(string fileName, vector<NodeS>& nodes, vector<GateS>& gates)
{
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "读取文件错误！" << endl;
		return false;
	}
	string line;
	for (int i = 0; i < 7;i++)getline(file, line);
	string lines = "";
	while (getline(file, line))lines += line + " ";
	stringstream ss(lines);
	string tmp;
	int cnt = 0,idx=0;
	while (ss >> tmp) {
		if (tmp == "p" || tmp == "cnf")continue;
		if (tmp == "%")break;
		cnt++;
		switch (cnt)
		{
		case 1: 
			N = stoi(tmp);
			nodes.resize(N+1);
			poly.assign(N + 1, 0);
			note.assign(N + 1, 0);
			counter.assign(N + 1, { 0 ,0});
			break;
		case 2:
			G = stoi(tmp);
			gates.resize(G);
			break;
		default:
			int num =stoi(tmp);
			if (!num) {
				idx++;
				continue;
			}
			else {
				if (!poly[abs(num)]) {
					if (note[abs(num)] == 0)note[abs(num)] = num;
					else if (note[abs(num)] != num)poly[abs(num)] = 1;
				}
				counter[abs(num)].first = abs(num);
				counter[abs(num)].second++;
				nodes[abs(num)].gate.push_back(idx);
				gates[idx].nodes.push_back(num);
			}
			break; 
		}
	}
	return true;
}

bool deduction(int num,const vector<NodeS>&nodes,vector<GateS>&gates) {
	//首先需要进行赋值并且判断矛盾
	vector<int>exact_num;
	for (int idx : nodes[abs(num)].gate) {
		if (gates[idx].nodes.size() == 0)continue;
		GateS& gate = gates[idx];
		auto it = std::find(gate.nodes.begin(), gate.nodes.end(), -num);
		if (it != gate.nodes.end()) {
			gate.nodes.erase(it);
			if (gate.nodes.size() == 0)return false;
			else if (gate.nodes.size() == 1)for (int i : gate.nodes) {
				int tmp = (i > 0) ? 1 : 0;
				if (assignment[abs(i)] == -1)exact_num.push_back(i);
				if (assignment[abs(i)] != -1 && assignment[abs(i)] != tmp) return false;
				else assignment[abs(i)] = tmp;
				gate.nodes.clear();
				
			}
		}
		else  gate.nodes.clear(); 
		//cout << num << " " << idx << " " << gate.nodes.size()<< endl;
		//for (int i = 1; i < assignment.size(); i++)cout << assignment[i] << " ";
		//cout << endl;
		/*
		if (gate.nodes.size() == 1 && (gate.nodes.find(num) == gate.nodes.end())) return false; 
		else if(gate.num_unassign>0){
			if (gate.nodes.find(num) != gate.nodes.end())gate.num_unassign == 0;
			else {
				gate.num_unassign--;
				if (gate.num_unassign == 1) for (auto i : gate.nodes) if (i.second == 1) { 
					int tmp = (i.first > 0) ? 1 : 0;
					if (assignment[abs(i.first)] != -1 && assignment[abs(i.first)] != tmp) return false; 
					else assignment[abs(i.first)] = tmp;
					i.second = 0;
					gate.num_unassign=0;
					if(assignment[abs(i.first)] == -1)exact_num.push_back(i.first);
					for (int x : nodes[abs(i.first)].gate)if (gates[x].nodes.find(i.first) != gates[x].nodes.end())gates[x].num_unassign = 0;
				}

				//for (int i = 1; i < assignment.size(); i++)cout << assignment[i] << " ";
				//cout << endl;
			}
		}	*/
	}
	//for (int i = 1; i < assignment.size(); i++)cout << assignment[i] << " ";
	//cout << endl;
	for (int exact_n : exact_num)if (!deduction(exact_n, nodes, gates)) return false; 
	return true;
}

bool assign(int i,const vector<NodeS>&nodes,vector<GateS>&gates) {
	//cout << match[i] << endl;
	if (match[i] == N+1)return true;
	else {
		assignment[match[i]] = 0;
		vector<int>tmp_assign = assignment;
		vector<NodeS>tmp_n = nodes;
		vector<GateS>tmp_g = gates;
		if (deduction(-match[i],nodes,gates)) {
			for (int j = i + 1; j <= N; j++) {
				if (assignment[match[j]] == -1) {
					if (assign(j, nodes, gates))return true;
					else break;
				}
				else continue;
			}
			int flag = 0;
			for (int idx = 0; idx < G; idx++)if (gates[idx].nodes.size() != 0) { flag = 1; break; }
			if (!flag)return true;
		}
		assignment = tmp_assign;
		assignment[match[i]] = 1;
		gates = tmp_g;
		//cout << i << "!" << endl;
		if (deduction(match[i],nodes,gates)) {
			for (int j = i + 1; j <= N; j++) {
				if (assignment[match[j]] == -1) {
					if (assign(j, nodes, gates))return true;
					else return false; 
				}
				else continue;
			}
			for (int idx = 0; idx < G; idx++)if (gates[match[i]].nodes.size() != 0)  return false; 
			return true;
		}
		return false;
	}
}
int main() {
	vector<NodeS>nodes;
	vector<GateS>gates;
	for (int i = 1; i <= 1000; i++) {
		//cout << i << endl;
		string patialName = "E:\\cnfs\\uf20-";
		string rest = "0" + std::to_string(i) + ".cnf";
		readfile(patialName + rest, nodes, gates);
		sort(counter.begin(), counter.end(),cmp);
		for (int idx = 1; idx <= counter.size(); idx++) match[idx] = counter[idx - 1].first; 
		assignment.assign(N + 1, -1);
		for (int idx = 1; idx <= N; idx++)if (!poly[idx]) {
			assignment[idx] = (note[idx] > 0) ? 1 : 0;
			int num = note[idx];
			for (int x : nodes[abs(num)].gate) {
				GateS& g = gates[x];
				if (g.nodes.size() == 0)continue;
				if (std::find(g.nodes.begin(), g.nodes.end(), num) != g.nodes.end()) {
					g.nodes.clear();
				}
				else g.nodes.erase(std::find(g.nodes.begin(), g.nodes.end(), -num));
			}
		}
		for (int idx = 1; idx <= N; idx++)if (assignment[match[idx]] == -1) { assign(idx, nodes, gates); break; }
		ofstream os("E:\\answers\\answer"+std::to_string(i)+".txt");
		for (int i = 1; i <= N; i++)os << assignment[i] << " ";
		assignment.clear();
		nodes.clear();
		gates.clear();
	}
	return 0;
}
