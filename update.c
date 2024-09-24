#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<algorithm>
#include<unordered_map>

using namespace std;
//store literals
struct NodeS {
	vector<int>gate;
};
//store clauses
struct GateS {
	vector<int>nodes;
};

int N, G = 0;
vector<int>assignment;//a valid assignment
vector<int>poly,note;//used to determine whether there is a unit-poly literal
vector<pair<int, int>>counter;//used to determine the frequecy of each literal
unordered_map<int, int>match;//how reordered literals correspond to original literals

bool cmp(pair<int, int>a, pair<int, int>b) { return (a.second > b.second); }
//read cnf and store it in nodes and gates
bool readfile(string fileName, vector<NodeS>& nodes, vector<GateS>& gates)
{
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "Error:fail to read file!" << endl;
		return false;
	}
	string line;
	for (int i = 0; i < 1;i++)getline(file, line);//delete useless lines
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
				//if some literals' value vary from 0 to 1,set poly[i] to 1
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
//if a literal num is assigned,for a clause with num,we will delete all the literals in it;otherwise we will just delete num only,then if this is only a literal num',then its value is determined ,just deduction(num')
bool deduction(int num,const vector<NodeS>&nodes,vector<GateS>&gates) {
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
	}
	for (int exact_n : exact_num)if (!deduction(exact_n, nodes, gates)) return false; 
	return true;
}
//recursively assign each literal according to their frequncies.
bool assign(int i,const vector<NodeS>&nodes,vector<GateS>&gates) {
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
	string filename;
	cin >> filename;
	readfile(filename, nodes, gates);
	sort(counter.begin(), counter.end(),cmp);//reorder the literals according to their frequencies
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
	int answer = 0;
	for (int idx = 1; idx <= N; idx++)if (assignment[match[idx]] == -1) { answer=assign(idx, nodes, gates); break; }
	if (answer) {
		cout << "SAT!" << endl;
		cout << "The assignment is:";
		for (int i = 1; i <= N; i++)cout << assignment[i] << " ";
	}
	else cout << "UNSAT" << endl;
	assignment.clear();
	nodes.clear();
	gates.clear();
	return 0;
}
