#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cstdlib>
#include <string>

using namespace std;

struct Event{
	int id;
	vector<int> featureNum;
	vector<double> featureValue;
	bool flag;
};//�¼����ݽṹ���ں��ĸ�������idΪ�¼������ţ�featureNumΪ�������������飬featureValueΪ������ֵ���飬�����±���ͬ��λ�ô洢ͳһ������flagΪ���¼��Ƿ��ǻ����¼����Ƿ�Ϊ��ǩ�¼���

map<int, multimap<string, Event> > patientMap;//�Ӳ��˱�ŵ��¼����е�ӳ���
map<int, int> featureMap;//����ר�ż�¼ÿ������labevent���¼���ž�����normal��0����abnormal��1������delta��2�������ݽṹ����labevent���¼���ŵ���0,1,2����ӳ���
map<int, int> matchMap;
int tempId[4000];
int labelCount[4000];
int eventCount[4000];
int spanCount[4000];
struct Lab{
	int id;
	int num;
} event[4000];

bool cmp(const Lab & a, const Lab & b){
	return a.num > b.num;
}

string toString(int c){
	string s = "";
	while (c > 0){
		s = (char)((c % 10) + '0') + s;
		c = c / 10;
	}
	while (s.size() < 2) s = '0' + s;
	return s;
}//������ת��Ϊ�ַ���

string getTime(string time){
	int pos = time.find(" ", 0);
	int pos2 = time.find(":", 0);
	int c = atoi((time.substr(pos + 1, pos2 - pos - 1)).c_str());
	if (c > 12) c -= 12;
	else c = 0;
	string h = toString(c);
	if (c < 10) h = "0" + h;
	return time.substr(0, pos) + " " + h + ":" + time.substr(pos2 + 1, time.size() - pos2 - 1);
}//�ڽ��ַ�����ʽ��ʱ����ǰ��12Сʱ���������¼�����12�����Ƶ�0�㣬�����ַ�����ʽ����ǰ�ƺ��ʱ��

void getFeature(string s, Event & e){
	if (s == "") return;
	s = s + ' ';
	while (s.find(" ", 0) != string::npos){
		int pos = s.find(" ", 0);
		int p = s.find(":", 0);
		string sa = s.substr(0, p);
		string sb = s.substr(p + 1, pos - p - 1);
		s.erase(0, pos + 1);
		int num = atoi(sa.c_str());
		double value = atof(sb.c_str());
		e.featureNum.push_back(num);
		e.featureValue.push_back(value);	
	}
}//��������е�feature�ֶβ���������ź���ֵ�ֱ𴢴���featureNum��featureValue��

void getExt(string st, bool flag){
	ifstream f0(st.c_str());
	string s;
	while (getline(f0, s)){
		Event e;
		e.flag = flag;
		s = s.substr(0, s.size() - 1);
		s = s + '\t';
		int pos = s.find("\t", 0);
		string tmp = s.substr(0, pos);
		int num = atoi(tmp.c_str());
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		tmp = s.substr(0, pos);
		int patient = atoi(tmp.c_str());
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		string feature = s.substr(0, pos);
		getFeature(feature, e);
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		string date = s.substr(0, pos);
		s.erase(0, pos + 1);
		if (patientMap.find(patient) == patientMap.end()){
			multimap<string, Event> eventMap;
			patientMap.insert(make_pair(patient, eventMap));
		}
		e.id = num;
		patientMap[patient].insert(make_pair(date, e));
	}
}//���tsv�ļ��е��ֶΣ���ȡ���¼��ı�ţ������洢���¼�����ӳ����У�֮���ԣ����ˣ��¼����У��Ķ�Ԫ����ʽ���벡�˵��¼����е�ӳ�����

void getApart(string st){
	ifstream f0(st.c_str());
	string s;
	while (getline(f0, s)){
		s = s.substr(0, s.size() - 1);
		s = s.substr(1, s.size() - 2);
		s = s + ',';
		int pos = s.find(",", 0);
		int pos2 = s.find(":", 0);
		string tmp = s.substr(pos2 + 2, pos - pos2 - 2);
		int num = atoi(tmp.c_str());
		s.erase(0, pos + 2);
		pos = s.find(",", 0);
		s.erase(0, pos + 2);
		pos = s.find(",", 0);
		string feature = s.substr(0, pos);
		s.erase(0, pos + 2);
		if (featureMap.find(num) == featureMap.end()) continue;
		if (s.find("labevents", 0) == string::npos) continue;
		if (feature.find("abnormal", 0) != string::npos){
			featureMap[num] = 1;
		} else if (feature.find("delta", 0) != string::npos){
			featureMap[num] = 2;
		}else{
			featureMap[num] = 0;
		}
	}
}//��ȡfeature_info.tsv�����ݣ����䴢����featureMap��

void match(){
	ifstream f0("feature_info.tsv");
	string s;
	while (getline(f0, s)){
		s = s.substr(0, s.size() - 1);
		s = s.substr(1, s.size() - 2);
		s = s + ',';
		int pos = s.find(",", 0);
		int pos2 = s.find(":", 0);
		string tmp = s.substr(pos2 + 2, pos - pos2 - 2);
		int num = atoi(tmp.c_str());
		s.erase(0, pos + 2);
		pos = s.find(",", 0);
		s.erase(0, pos + 2);
		pos = s.find(",", 0);
		string feature = s.substr(0, pos);
		s.erase(0, pos + 2);
		if (s.find("labevents", 0) == string::npos) continue;
		pos = s.find(".", 0);
		pos2 = s.find(",", 0);
		pos2 -= 1;
		pos += 1;
		int countId = atoi((s.substr(pos, pos2 - pos)).c_str());
		//cout<<countId<<endl;
		if (matchMap.find(countId) == matchMap.end()){
			matchMap.insert(make_pair(countId, num));
		}
		tempId[num] = matchMap[countId];
	}
}//ԭ��tsv������ÿ��labevent��normal abnromalʱ������һ����Ӧ�ı�ţ����ｫ���������ƥ������

int filter(){
	ifstream f0("labevents.tsv");
	string s;
	int sum = 4000;
	match();
	while (getline(f0, s)){
		s = s.substr(0, s.size() - 1);
		s = s + '\t';
		int pos = s.find("\t", 0);
		string tmp = s.substr(0, pos);
		int num = atoi(tmp.c_str());
		num = tempId[num];
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		tmp = s.substr(0, pos);
		int patient = atoi(tmp.c_str());
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		string feature = s.substr(0, pos);
		s.erase(0, pos + 1);
		pos = s.find("\t", 0);
		string date = s.substr(0, pos);
		s.erase(0, pos + 1);
		event[num].id = num;
		event[num].num++;
	}
	sort(event, event + sum, cmp);
	for (int i = 0; i < 20; i++){
		featureMap.insert(make_pair(event[i].id, 0));
	}
	for (int i = 0; i < 4000; i++){
		int id = i;
		if (tempId[id] == 0) continue;
		if (featureMap.find(tempId[id]) == featureMap.end()) continue;
		if (tempId[id] == id) continue;
		featureMap.insert(make_pair(id, 0));
	}
}//ɸѡlabevent�г���Ƶ����ߵ�20����Ϊlabel

int main(){
	getExt("chartevent_1.tsv", false);
	getExt("chartevent_2.tsv", false);
	getExt("chartevent_3.tsv", false);
	getExt("chartevent_4.tsv", false);
	getExt("chartevent_5.tsv", false);
	getExt("chartevent_6.tsv", false);
	getExt("chartevent_8.tsv", false);
	getExt("chartevent_9.tsv", false);
	getExt("chartevent_10.tsv", false);
	getExt("chartevent_11.tsv", false);
	getExt("chartevent_12.tsv", false);
	getExt("chartevent_13.tsv", false);
	getExt("chartevent_14.tsv", false);
	getExt("admissions.admittime.tsv", false);
	getExt("admissions.deathtime.tsv", false);
	getExt("admissions.dischtime.tsv", false);
	getExt("datetimeevents.tsv", false);
	getExt("icustays.tsv", false);
	getExt("inputevents_cv.tsv", false);
	getExt("inputevents_mv.tsv", false);
	getExt("labevents.tsv", true);
	getExt("outputevents.tsv", false);
	getExt("procedureevents.tsv", false);
	//���Ͼ�Ϊ�����������¼���Ⲣ������patientMap���γ�ҽ���¼�����
	filter();
	getApart("feature_info.tsv");//��ȡfeatureMap
	ofstream f0("dataSeq.txt");
	map<int, multimap<string, Event> >::iterator i;
	int n = 0;
	for (i = patientMap.begin(); i != patientMap.end(); i++){
		int num = i->first;
		if (patientMap[num].size() < 20) continue;
		n++;
	}
	f0<<n<<endl;
	int maxLength = 0;
	int maxLabel = 0;
	int mmpcount = 0;
	int mmpcnt = 0;
	for (i = patientMap.begin(); i != patientMap.end(); i++){//��������patientMap�����ҽ���¼�����
		int num = i->first;
		int countLabel = 0;
		int sum = 0;
		multimap<string, Event>::iterator lastLabel = patientMap[num].begin();
		if (patientMap[num].size() < 20) continue;
		multimap<string, Event>::iterator j;
		//multimap<string, Event>::iterator start = patientMap[num].begin();
		for (j = patientMap[num].begin(); j != patientMap[num].end(); j++){
			Event e = j->second;
			sum++;
			if (sum > 5000) break;
			if (featureMap.find(e.id) != featureMap.end()){
				countLabel++;
				//spanCount[getMinus(j->first, lastLabel->first)]++;
				lastLabel = j;
			}
		}
		sum = 0;
		int Length = patientMap[num].size();
		if (Length > 5000) mmpcount++;
		maxLength = max(maxLength, Length);
		maxLabel = max(maxLabel, countLabel);
		labelCount[countLabel / 100]++;
		eventCount[Length / 100]++;
		Length = min(5000, Length);
		f0<<num<<'\t'<<Length<<'\t'<<countLabel<<endl;
		cout<<num<<endl;
		for (j = patientMap[num].begin(); j != patientMap[num].end(); j++){//�����¼����У�������5000��ضϣ������ո�ʽ���
			string time = j->first;
			Event e = j->second;
			int label = 0;
			if (featureMap.find(e.id) != featureMap.end()){
				label = 1;
				if (sum > 5000) mmpcnt++;
			}
			sum++;
			if (sum > 5000) break;
			//if (sum == 1000) start++;
			//else sum++;
			//if (!(j->second).flag) continue;
			//if (sum < 100) continue;
			//multimap<string, Event>::iterator l;
			//string sttime = getTime(j->first);
			/*int eventsum = 0;
			for (l = start; l != j; l++){
				if (l->first <= sttime){
					eventsum++;
				}else{
					break;
				}
			}
			if (eventsum < 100) continue;
			f0<<num<<'\t'<<eventsum<<endl;*/
			//for (l = start; l != j, eventsum > 0; l++, eventsum--){
				//string time = l->first;
				//Event e = l->second;
			f0<<label<<'\t'<<e.id<<'\t'<<time<<endl;
			if (label == 1) f0<<featureMap[e.id]<<endl;
			f0<<e.featureNum.size()<<endl;
			for (int k = 0; k < e.featureNum.size(); k++){
				f0<<e.featureNum[k]<<'\t'<<e.featureValue[k]<<endl;
			}
			//}
			//f0<<(j->second).id<<' '<<(j->second).featureValue[0]<<' '<<featureMap[(j->second).id]<<endl;
		}
		//f0<<"----------------------------"<<sum<<"---------------------------"<<endl;
	}
	f0<<maxLength<<' '<<maxLabel<<endl;
	/*cout<<mmpcount<<' '<<mmpcnt<<endl;
	ofstream f1("labelDistr.txt"), f2("eventDistr.txt"), f3("spanDistr.txt");
	for (int i = 0; i < 400; i++){
		f1<<i * 100<<' '<<(i + 1) * 100<<' '<<labelCount[i]<<' '<<double(labelCount[i]) / n<<endl;
		f2<<i * 100<<' '<<(i + 1) * 100<<' '<<eventCount[i]<<' '<<double(eventCount[i]) / n<<endl;
	}
	for (int i = 0; i < 400; i++){
		f3<<i * 3<<' '<<(i + 1) * 3<<' '<<spanCount[i]<<endl;
	}*/
}
