#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <cstring>
#include <ios>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;
FILE *dat;int gylis = 2;bool did;int gyliai[10] = {0};
vector<unsigned char> instr[100];int kins = 0;
map<int,vector<int>> jumps;
unordered_map<string,int> vars;int lw = 256;
void add(int a){//byte code to add
	char m[1];m[0] = a;
	fwrite(m, 1, 1, dat);
}
void strips(string &line,char a){//strip all chars from string
	line.erase(remove(line.begin(), line.end(), a), line.end());
}
int sum_distance(int from){
	int sum = 0;
	for(int i = from;i<kins-1;i++){
		sum+=instr[i].size();
	}cout<<"jumped "<<sum<<" bytes"<<endl;
	return sum;
}
void didint_gyli(){gylis++;
	gyliai[gylis] = kins;
}
void mazint_gyli(){gylis--;
	if(gyliai[gylis+1] != 0){
		instr[kins].push_back(0xeb);instr[kins].push_back(255-sum_distance(gyliai[gylis+1]));
		kins++;
	}
	for(auto i:jumps[gylis]){
		instr[i][1] = sum_distance(i)+((!(gylis-1))*3);
	}jumps.erase(gylis);
	gyliai[gylis+1] = 0;
}
void generatecode(){
	for(int i = 0;i<kins;i++){
		for(auto y:instr[i]){
			add(y);
		}
	}
}
void write_in_hex(int num){
	instr[kins].push_back(num & 0xff);
	instr[kins].push_back((num >> 8) & 0xff);
	instr[kins].push_back((num >> 16) & 0xff);
	instr[kins].push_back((num >> 24) & 0xff);
}
int varloc(string line){
	auto var = vars.find(line);
	if (var != vars.end()) {
    	return var->second;
  	} else {lw -= 4;
  		vars[line] = lw;
  		instr[kins].push_back(199);instr[kins].push_back(69);instr[kins].push_back(lw);write_in_hex(0);kins++;
    	return lw;
  	}
}
int ifvar(string line){
	return isalpha(line[line.length()-1]);
}
int endofvar(string line){
	int lasti = 0;
	for(int i = 0;isalpha(line[i]);i++){
		lasti = i;
	}
	return lasti;
}
int startofnum(string line){
	int lasti = 0;
	for(int i = 0;(line[i]>='0'&&line[i]<='9')==false;i++){
		lasti = i;
	}
	return lasti+1;
}
int main(){
	char basefile[] = "base.forucod";
	char sourcecode[] = "script.ucod";
	char compiledprogram[] = "program.exe";
	ifstream  src(basefile, ios::binary);ofstream  dst(compiledprogram,   ios::binary);dst << src.rdbuf();dst.close();src.close();dat = fopen(compiledprogram, "ab+");ifstream file(sourcecode);string line;
    
	while (getline(file, line)){stringstream ss(line);
    	while (getline(ss, line,';')){did = false;strips(line,'\t');
    		if(line.starts_with("/")){continue;}//comment
        	if(line.ends_with(":")){didint_gyli();did = true;
        		if(line.starts_with("for ")){//for loop
        			int num = stoi(line.substr(4,line.length()-1))-1;
        			int varloca = varloc("i");
        			instr[kins].push_back(131);instr[kins].push_back(125);instr[kins].push_back(varloca);instr[kins].push_back(num);
        			kins++;
        			instr[kins].push_back(0x7f);
        			instr[kins].push_back(0);
					jumps[gylis-1].push_back(kins);
					kins++;
					instr[kins].push_back(131);instr[kins].push_back(69);instr[kins].push_back(varloca);instr[kins].push_back(1);
					kins++;
				}
				else{//if statement
					int varendas = endofvar(line.substr(3,line.length()))+1;
					int varloca = varloc(line.substr(3,varendas));
					instr[kins].push_back(131);instr[kins].push_back(125);instr[kins].push_back(varloca);
					instr[kins].push_back(stoi(line.substr(startofnum(line.substr(varendas,line.length()-1))+varendas,line.length()-2)));
					kins++;
					if(line[varendas+3]=='>'){instr[kins].push_back(126);}
					if(line[varendas+3]=='<'){instr[kins].push_back(0x7f);}
					if(line[varendas+3]=='='){instr[kins].push_back(0x75);}
					instr[kins].push_back(0);
					jumps[gylis-1].push_back(kins);
					kins++;
				}
			}
			if(line.starts_with("ret")){did = true;
				if(line=="ret"){
					instr[kins].push_back(235);instr[kins].push_back(0);
					jumps[0].push_back(kins);
					kins++;
				}
				else if(ifvar(line.substr(4,line.length()))){
					int varloca = varloc(line.substr(4,line.length()));
					instr[kins].push_back(139);instr[kins].push_back(69);//mov eax, ...
					instr[kins].push_back(varloca);
					kins++;
					instr[kins].push_back(235);instr[kins].push_back(0);
				jumps[1].push_back(kins);
				kins++;
				}
				else{
					instr[kins].push_back(184);//mov eax, ...
					int num = stoi(line.substr(4,line.length()));
					write_in_hex(num);
					kins++;
					instr[kins].push_back(235);instr[kins].push_back(0);
				jumps[1].push_back(kins);
				kins++;
				}
			}
			if(did == false){//do some calculations
				if(line.find("++")!=string::npos){
					int varloca = varloc(line.substr(0,line.find("++")));
					instr[kins].push_back(131);instr[kins].push_back(69);instr[kins].push_back(varloca);instr[kins].push_back(1);
					kins++;
				}
			}
			if(line.ends_with(".")){mazint_gyli();
				
			}
    	}
    }
	mazint_gyli();//for returns with value
	mazint_gyli();//for returns without value
	if(gylis != 0){cout<<"Error: bad depth "<<gylis<<"\nfile not compiled";return 0;}
    generatecode();
    add(72);add(49);add(192);add(93);add(195);file.close();fclose(dat);//xor rax,rax (if jump to end go here) pop rbx  ret
}
