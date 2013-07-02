/* 
 *  Copyright (c) 2010 Yasuo Tabei
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
*/

#include "fmIndex.hpp"
#include "sais.hxx"

using namespace std;

/*
  void exportCF(std::vector<uint32_t> cf){
	cout<<"I'm about to export: cf.txt (count table)"<<endl;
	ofstream myfile;
	myfile.open ("cf.txt");
	vector<uint32_t>::iterator it;
	  //cout << "Suffix array contains:"<<endl;
	for ( it=cf.begin() ; it < cf.end(); it++ ){
	    //cout << " " << *it;      
	    myfile << *it<<endl;
	}
	 myfile.close();
  }

  void exportMap(std::map<unsigned char, uint8_t> mapping){
  	cout<<"I'm about to export: symbol_id_map.txt"<<endl;
	ofstream myfile;
	myfile.open ("symbol_id_map.txt");
	map<uint8_t,unsigned char >::iterator itm1;
	int i=0;
	  for ( itm1=mapping.begin() ; itm1 != mapping.end(); itm1++,i++ ){
	   // cout << i<<"=>" << (*itm1).second<<"\t"<<(*itm1).first  << endl;
	    myfile << i<<"=>" << (*itm1).second<<"\t"<<(*itm1).first  << endl;
	}
	 myfile.close();
  }

  void exportOCC(std::map<unsigned char, uint8_t> mapping,  vector<uint8_t> s,wat_array::WatArray wa){
       	cout<<"I'm about to occurance table: occ.txt"<<endl;
	ofstream myfile;
	myfile.open ("occ.txt");
	map<uint8_t,unsigned char >::iterator itm1;
	int i=0;
        for(int k=1;k<=s.size(); k++){
	  for ( itm1=mapping.begin() ; itm1 != mapping.end(); itm1++,i++ ){
            uint8_t c = (*itm1).first ;
	    //cout << k<<"=>" << (*itm1).second<<"\t"<< c  << endl;
	   // myfile << i<<"=>" << (*itm1).second<<"\t"<<(*itm1).first  << endl;
	    myfile<< wa.Rank(c,k)<<"\t";
	    //cout << wa.Rank(c,k)<<"\t";
	 }
	 // cout<<endl;
	  myfile<<endl;
       } 
	 myfile.close();

  }

  void exportSampledSuffixArray(vector<uint32_t> ssa){
	cout<<"I'm about to export: sampled_suffix_array.txt "<<endl;
	ofstream myfile;
	myfile.open ("sampled_suffix_array.txt");
	vector<uint32_t>::iterator it;
	  //cout << "Suffix array contains:"<<endl;
	for ( it=ssa.begin() ; it < ssa.end(); it++ ){
	    //cout << " " << *it;      
	    myfile << *it<<endl;
	}
	 myfile.close();
  }

  void exportSuffixArray(vector<uint32_t> sa){
	cout<<"I'm about to export: suffix_array.txt"<<endl;
	ofstream myfile;
	myfile.open ("suffix_array.txt");
	vector<uint32_t>::iterator it;
	  //cout << "Suffix array contains:"<<endl;
	for ( it=sa.begin() ; it < sa.end(); it++ ){
	    //cout << " " << *it;      
	    myfile << *it<<endl;
	}
	 myfile.close();  
  }

  void exportBWT(vector<uint64_t> bwt){
	cout<<"I'm about to export: bwt.txt"<<endl;
	ofstream myfile;
	myfile.open ("bwt.txt");
	vector<uint64_t>::iterator it;
	  //cout << "Suffix array contains:"<<endl;
	for ( it=bwt.begin() ; it < bwt.end(); it++ ){
	    //cout << " " << *it;      
	    myfile << *it<<endl;
	}
	 myfile.close();
  }

*/

int fmIndex::read(const char *fname, vector<uint8_t> &s) {
  ifstream ifs(fname);
  string line;
  
  mapping['$'] = 0;
  rmapping[0]  = '$';
  alphaSize = 1;
  while (getline(ifs, line)) {
    for (size_t i = 0; i < line.size(); i++) {
      if (mapping.find(line[i]) == mapping.end()) {
        mapping[line[i]]    = alphaSize;
        rmapping[alphaSize] = line[i];
        if (++alphaSize == 0)
              cerr << "warning: the variety of characters exceeds 255." << endl;
      }
      s.push_back(mapping[line[i]]);
    }
  }
  s.push_back(mapping['$']);

  return 0;
}

int fmIndex::readQstring(const char *fname, vector<vector<uint8_t> > &qs) {
  ifstream ifs(fname);
  string line;
  while (getline(ifs, line)) {
    if (line.size() == 0)
      continue;
    qs.resize(qs.size() + 1);
    vector<uint8_t> &tmp = qs[qs.size() - 1];
    for (size_t i = 0; i < line.size(); i++) {
      if (mapping.find(line[i]) == mapping.end()) {
	mapping[line[i]] = alphaSize;
	rmapping[alphaSize]  = line[i];
	if (++alphaSize == 0)
	  cerr << "warning: the variety of characters exceeds 255." << endl;
      }
      tmp.push_back(mapping[line[i]]);
    }
  }
  
  return 0;
}

int fmIndex::buildQstring(std::string line, vector<vector<uint8_t> > &qs){
    qs.resize(1);
    vector<uint8_t> &tmp = qs[qs.size() - 1];
    for (size_t i = 0; i < line.size(); i++) {
      if (mapping.find(line[i]) == mapping.end()) {
	mapping[line[i]] = alphaSize;
	rmapping[alphaSize]  = line[i];
	if (++alphaSize == 0)
	  cerr << "warning: the variety of characters exceeds 255." << endl;
      }
      tmp.push_back(mapping[line[i]]);
    }
   return 0;
}

int fmIndex::buildSA(const vector<uint8_t> &s, vector<uint32_t> &sa) {
  sa.resize(s.size());
  if (saisxx(s.begin(), sa.begin(), (int)s.size(), 0x100) == -1) {
    cerr << "suffix array construction error" << endl;
    return 1;
  }

  return 0;
}

int fmIndex::buildBWT(const vector<uint8_t> &s, const vector<uint32_t> &sa, vector<uint64_t> &bwt) {
  size_t seqLen = s.size();
  bwt.resize(seqLen);
  for (size_t i = 0; i < seqLen; i++) {
    if (sa[i] > 0) bwt[i] = s[sa[i] - 1];
    else           bwt[i] = s[seqLen - 1];
  }
  
  return 0;
}

int fmIndex::buildSampledSA(const vector<uint8_t> &s, const vector<uint32_t> &sa) {
  size_t seqLen = s.size();
  size_t divSeq = seqLen*((float)percent/100.f);
  divSeq = seqLen/divSeq;
  
  for (size_t i = 0; i < seqLen; i++) {
    if (i%divSeq == 0)
      sampledSA.push_back(sa[i]);
  }
  return 0;
}

void fmIndex::calculateStatistics(const vector<uint8_t> &s) {
  size_t seqLen = s.size();
  vector<uint32_t> tmpCf(UINT8_MAX + 1);
  for (size_t i = 0; i < seqLen; i++) {
    tmpCf[s[i]]++;
  }
  cf.resize(UINT8_MAX + 1);
  cf[0] = 0;
  for (size_t i = 1; i <= UINT8_MAX; i++) {
    cf[i]     = tmpCf[i-1];
    tmpCf[i] += tmpCf[i-1];
  }
}

size_t fmIndex::locate(uint64_t i) {
  size_t bsize = wa.length();
  size_t divSeq = bsize*((float)percent/100.f);
  divSeq = bsize/divSeq;

  size_t j = i;
  size_t t = 0;
  while (j % divSeq != 0) {
    j = cf[wa.Lookup(j)] + wa.Rank(wa.Lookup(j), j+1) - 1;
    t++;
  }
  if (sampledSA[j/divSeq] + t >= bsize) {
    return (size_t)sampledSA[j/divSeq] + t - bsize;
  }
  return (size_t)sampledSA[j/divSeq] + t;
}

int fmIndex::save(ostream& os)  {
  os.write((const char*)(&percent), sizeof(percent));
  os.write((const char*)(&alphaSize), sizeof(alphaSize));
  size_t cfSize = cf.size();
  os.write((const char*)(&cfSize), sizeof(cfSize));
  size_t sampledSASize = sampledSA.size();
  os.write((const char*)(&sampledSASize), sizeof(sampledSASize));
  os.write((const char*)(&cf[0]), sizeof(cf[0])*cf.size());
  os.write((const char*)(&sampledSA[0]), sizeof(sampledSA[0])*sampledSA.size());

  uint32_t count = 0;
  vector<unsigned char> key;
  vector<uint8_t> val;
  for (std::map<unsigned char, uint8_t>::iterator it = mapping.begin(); it != mapping.end(); it++) {
    key.push_back(it->first);
    val.push_back(it->second);
    count++;
  }
  os.write((const char*)(&count), sizeof(uint32_t));
  os.write((const char*)(&key[0]), sizeof(key[0])*count);
  os.write((const char*)(&val[0]), sizeof(val[0])*count);

  wa.Save(os);
  
  return 0;
}

int fmIndex::load(istream& is) {
  is.read((char*)(&percent), sizeof(percent));
  is.read((char*)(&alphaSize), sizeof(alphaSize));
  size_t cfSize;
  is.read((char*)(&cfSize), sizeof(cfSize));
  size_t sampledSASize;
  is.read((char*)(&sampledSASize), sizeof(sampledSASize));
  cf.resize(cfSize);
  is.read((char*)(&cf[0]), sizeof(cf[0])*cfSize);
  sampledSA.resize(sampledSASize);
  is.read((char*)(&sampledSA[0]), sizeof(sampledSA[0])*sampledSASize);

  uint32_t              count;
  vector<unsigned char> key;
  vector<uint8_t>       val;
  is.read((char*)(&count), sizeof(count));
  key.resize(count);
  is.read((char*)(&key[0]), sizeof(key[0])*count);
  val.resize(count);
  is.read((char*)(&val[0]), sizeof(val[0])*count);
  for (uint32_t i = 0; i < count; i++) {
    mapping[key[i]]  = val[i];
    rmapping[val[i]] = key[i];
  }
  wa.Load(is);

  return 0;
}

int fmIndex::buildWaveletTree(const vector<uint64_t> &bwt) {
  wa.Init(bwt);
  
  return 0;
}

int fmIndex::buildFmIndex(const char *fname, int _percent) {
  percent = _percent;

  cerr << "start reading the input-file" << endl;
  vector<uint8_t> s;
  read(fname, s);

  cerr << "alphabet size:" << (int)alphaSize << endl;

  double sTime = clock();
  vector<uint32_t> sa;
  cerr << "build SA" << endl;
  buildSA(s, sa);

  cerr << "calculate statistics" << endl;
  calculateStatistics(s);

  vector<uint64_t> bwt;
  cerr << "build BWT" << endl;
  buildBWT(s, sa, bwt);

  cerr << "build WaveletTree" << endl;
  buildWaveletTree(bwt);

  cerr << "build sampledSA" << endl;
  buildSampledSA(s, sa);
  
  double eTime = clock();
  
  fprintf(stderr, "cpu time:%f\n", (eTime - sTime)/CLOCKS_PER_SEC);

/*
  vector<uint8_t>::iterator it0;
  cout << "Given string s contains:"<<endl;
  for ( it0=s.begin() ; it0 < s.end(); it0++ ){
      //cout << " " << *it0;     
      printf("%d ",*it0); 
   }
  cout << endl;

  exportMap(rmapping);
  exportSuffixArray(sa);
  exportSampledSuffixArray(sampledSA);
  exportBWT(bwt);
  exportCF(cf);
  exportOCC(rmapping,s,wa);
*/
  
  return 0;
}

int fmIndex::buildAll(const char *fname, int _percent, vector<uint8_t> &s, vector<uint32_t> &sa, vector<uint64_t> &bwt) {
    percent = _percent;
    
//    cerr << "start reading the input-file" << endl;
    //vector<uint8_t> s;
    read(fname, s);

//    cerr << "alphabet size:" << (int)alphaSize << endl;
    
//    double sTime = clock();
    //vector<uint32_t> sa;
//    cerr << "build SA" << endl;
    buildSA(s, sa);
    
//    cerr << "calculate statistics" << endl;
    calculateStatistics(s);
    
    //vector<uint64_t> bwt;
//    cerr << "build BWT" << endl;
    buildBWT(s, sa, bwt);
    
//    cerr << "build WaveletTree" << endl;
    buildWaveletTree(bwt);
    
    //cerr << "build sampledSA" << endl;
    buildSampledSA(s, sa);
    
//    double eTime = clock();
    
//    fprintf(stderr, "cpu time:%f\n", (eTime - sTime)/CLOCKS_PER_SEC);
    
//    cerr<<endl<<"Building: ref, suffix array, bwt"<<endl;
    /*
     vector<uint8_t>::iterator it0;
     cout << "Given string s contains:"<<endl;
     for ( it0=s.begin() ; it0 < s.end(); it0++ ){
     //cout << " " << *it0;
     printf("%d ",*it0);
     }
     cout << endl;
     */
    
    //exportMap(rmapping);
    //exportSuffixArray(sa);
    //exportSampledSuffixArray(sampledSA);
    //exportBWT(bwt);
    //exportCF(cf);
    //exportOCC(rmapping,s,wa);
    
    return 0;
}

void fmIndex::search(const std::vector<uint8_t> &qs, std::pair<uint64_t, uint64_t> &res) {
  size_t sp = 1, ep = wa.length();
  uint8_t c;
  int i = qs.size() - 1;

  while ((sp < ep) && (i >= 0)) {
    c  = qs[i]; i--;
    sp = cf[c] + wa.Rank(c, sp);
    ep = cf[c] + wa.Rank(c, ep);
  }
  if (sp < ep) 
    res = make_pair(sp, ep-1);
  else 
    res = make_pair((size_t)-1, (size_t)-1);
}

void fmIndex::searchHamming(const std::vector<uint8_t> &qs, int dist, vector<pair<uint64_t, uint64_t> > &res) {
  res.clear();
  searchHamming(qs, qs.size() - 1, 1, wa.length(), 0, dist, res);
}

void fmIndex::searchHamming(const vector<uint8_t> &qs, int i, size_t sp, size_t ep, int d, int dist, vector<pair<uint64_t, uint64_t> > &res) {
  if (sp >= ep) {
    return;
  }
  if (dist < d) {
    return;
  }
  if (i < 0) {
    res.push_back(make_pair(sp, ep-1));
    return;
  }
  
  uint8_t s = qs[i];
  int     newd;
  uint8_t m;
  for (map<unsigned char, uint8_t>::iterator it = mapping.begin(); it != mapping.end(); it++) {
    m = it->second;
    if (m == 0)
      continue;
    newd = d;
    if (m != s)
      newd = d + 1;
    searchHamming(qs, i-1, cf[m] + wa.Rank(m, sp), cf[m] + wa.Rank(m, ep), newd, dist, res);
  }
}

inline int min(int a, int b, int c) {
  if (a <= b) {
    if (a <= c) 
      return a;
    else if (b <= c)
      return b;
    else
      return c;
  }
  else {
    if (b <= c)
      return b;
    else if (a <= c)
      return a;
    else
      return c;
  }
}

wat_array::WatArray  fmIndex::getWa(){
return wa;
}

std::vector<uint32_t> fmIndex::getCF(){
 return cf;
}


void fmIndex::searchEdit(const vector<uint8_t> &qs, int dist, vector<pair<uint64_t, uint64_t> > &res) {
  res.clear();

  cols.clear();
  cols.resize(qs.size() + 1);
  for (size_t i = 0; i < cols.size(); i++)
    cols[i].resize(qs.size() + 1);
  
  for (size_t i = 0; i < cols.size(); i++) {
    cols[0][i] = i;
    cols[i][0] = i;
  }
  searchEdit(qs, 1, 1, wa.length(), dist, res);
}

void fmIndex::searchEdit(const vector<uint8_t> &qs, int count, int sp, int ep, int dist, vector<pair<uint64_t, uint64_t> > &res) {
  if (sp >= ep)
    return;

  if (count > (int)qs.size()) {
    if (cols[qs.size()][qs.size()] <= dist)
      res.push_back(make_pair(sp, ep-1));
    return;
  }
  
  int minval, diag;
  uint8_t m;
  for (map<unsigned char, uint8_t>::iterator it = mapping.begin(); it != mapping.end(); it++) {
    m = it->second;
    if (it->second == 0)
      continue;
    cols[count][0] = count-1;
    minval         = INT_MAX;
    for (size_t k = 1; k <= qs.size(); k++) {
      if (qs[qs.size() - k] == it->second)
	diag = 0;
      else
	diag = 1;
      cols[count][k] = min(cols[count-1][k]+1, cols[count][k-1]+1, cols[count-1][k-1]+diag);
      if (cols[count][k] < minval) minval = cols[count][k];
    }
    if (minval <= dist)
      searchEdit(qs, count + 1, cf[m] + wa.Rank(m, sp), cf[m] + wa.Rank(m, ep), dist, res);
  }
}
