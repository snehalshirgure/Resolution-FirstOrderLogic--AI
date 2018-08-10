#include<iostream>
#include<string>
#include<list>
#include<stack>
#include<map>
#include<fstream>
#include<algorithm>

using namespace std;

list<string> queries;
list<string> sentences;
list<string> predicates;
map<string,list<string> > kbmap;

//function to check whether variable or constant
int isVar(string s){
if((int)(s[0])>=65 && (int)(s[0])<=90)
    return 0;
else
    return 1;
}    

//function to seperate the predicates with variable arguments
void seperatePredicates(){

string s;

list<string>::iterator itr;
for(itr=sentences.begin();itr!=sentences.end();++itr){

s=*itr;
size_t start = 0;
size_t end = s.find("(");

while(end<s.size()){ 
    predicates.push_back(s.substr(start,end-start));
    start= s.find("|",start+1,1) + 1 ;
    end = s.find("(",end+1,1);
}//endwhile

}//endfor

predicates.sort();
predicates.unique();
 
}
  
  
// extra ------- function 
list<string> seperateSentences(string s){

list<string> lsp;
s+="|";
size_t start=0;
size_t end= s.find("|");
 
while(start<s.size()&&end<s.size()){
lsp.push_back(s.substr(start,end-start));
//cout<<s.substr(start,end-start)<<endl;
start = end+1;
end= s.find("|",end+1,1);
}    
return lsp;
}

// extra ------- function 
list<string> seperateVariables(string s){
list<string> lsp;
s+=",";
size_t start=0;
size_t end= s.find(",");

while(start<s.size()&&end<s.size()){
lsp.push_back(s.substr(start,end-start));
//cout<<s.substr(start,end-start)<<endl;
start = end+1;
end= s.find(",",end+1,1);
}    
return lsp;
}

//function to handle multiple unifications
string front(list<string> s, string sfront){
string r = "";
for( list<string>::iterator itr = s.begin(); itr!=s.end(); ++itr){
if(*itr == sfront){
  itr = s.erase(itr);
  }
}
 r+= sfront+"|";
for(list<string>::iterator itr = s.begin() ; itr!=s.end(); ++itr){
string ss = *itr;
r += ss + "|";	
}
return r.substr(0,r.size()-1);
}

//function to map individual predicates to sentences containing them q-->q
void buildmap(){
list<string>::iterator itr;
list<string>::iterator s_itr;
 
for(itr=predicates.begin();itr!=predicates.end();++itr){
    string p = *itr;
//cout<<*itr<<endl;

 list<string> l;
 int flag ;
    for(s_itr = sentences.begin(); s_itr!= sentences.end(); ++s_itr)
    { 
     string s = *s_itr;
     
     list<string> slist = seperateSentences(s);
     
   for(list<string>::iterator sitr = slist.begin(); sitr!= slist.end(); ++sitr){ 
     
    string ss = *sitr;
    ss =  ss.substr(0,ss.find("("));
     
     if(ss==p){
      int index = ss.find(p);
      flag=1;
      //remove inconsistency
      if( p[0]!='~' && ss[index-1]=='~')
       flag=0; 
      
      if(flag){  
      string new_s = front(slist,*sitr);
      l.push_back(new_s);
      kbmap[p] = l;
       }   
      }//endif    
      
      }//endfor subsentences
     
     } //endfor sentences
 }// endof for loop for predicates
}

    
//change predicate 
string change(string q){
string qq;
if(q.find("~")==0)
qq = q.substr(1,q.size()-1);  
else
qq="~" + q.substr(0,q.size());
return qq;
}  

// update after unifying variables to constants
string update(string p, string v , string c){
size_t p1 = p.find("("); 
string pred = p.substr(0,p1);
size_t p2 = p.find(")");
string pvar = p.substr(p1+1,p2-p1 - 1);
list<string> pvariablelist =  seperateVariables(pvar);
string pvarlist ="";
for(list<string>::iterator pitr = pvariablelist.begin(); pitr!= pvariablelist.end(); ++pitr)
{
    string &s(*pitr);
    if(s == v){
    s=c;
    }   
    pvarlist +=  s + ",";       
}
pvarlist = pvarlist.substr(0,pvarlist.size()-1);    

  p = pred + "(" + pvarlist +")";
 return p;
}

map<string,string> updatemap(map<string,string> m, string v, string c){

for(map<string,string>::iterator itr= m.begin(); itr!=m.end() ; ++itr) {
if(itr->second==v)
 {
 itr->second = c; 
 } 

}    
return m;
}    

//return the end string after unifying
string concat(list<string>s1, list<string> s2){
string result = ""; 
for(list<string>::iterator itr1 = s1.begin() ; itr1!= s1.end() ; ++itr1)
for(list<string>::iterator itr2 = s2.begin() ; itr2!= s2.end() ; ++itr2)
  {
      string &p( *itr1 );
      string &s (*itr2 );
  if( p == change(s) ){
  p = "";
  s = "";
  }
      
  }  
  
for(list<string>::iterator itr1 = s1.begin() ; itr1!= s1.end() ; ++itr1)
for(list<string>::iterator itr2 = s2.begin() ; itr2!= s2.end() ; ++itr2)
  {
      string &p( *itr1 );
      string &s (*itr2 );
  if( p == (s) ){
  p = "";
  }
      
  } 
      
for(list<string>::iterator itr1 = s1.begin() ; itr1!= s1.end() ; ++itr1)
{
   string &p( *itr1 );
    if(p!="") 
    result+= p + "|";   
}    

for(list<string>::iterator itr2 = s2.begin() ; itr2!= s2.end() ; ++itr2)
{
   string &s( *itr2 );
    if(s!="") 
    result+= s + "|";   
} 
result = result.substr(0,result.size()-1);   
return result ;
}    
   
string concatSentences(list<string>s1){
string result = ""; 
for(list<string>::iterator itr1 = s1.begin() ; itr1!= s1.end() ; ++itr1)
  {
     string &s (*itr1 );
     result+=s+"|";  
  }  
result = result.substr(0,result.size()-1);   
return result;
}
  
  
//function to unify a sentence s on predicate p -- PASS P as same in S!!
string unification(string predicate,string sentence){

//end of resolution
if(sentence==change(predicate))
 return "";

list<string> predicatelist = seperateSentences(predicate);
list<string> sentencelist = seperateSentences(sentence);
 
map<string,string> unify1;
map<string,string> unify2;

//unify resolvable terms
for(list<string>::iterator pitr = predicatelist.begin(); pitr!=predicatelist.end() ;++pitr){

string p = *pitr;

size_t p1 = p.find("("); 
string pred = p.substr(0,p1);
size_t p2 = p.find(")");
string pvar = p.substr(p1+1,p2-p1 - 1);
list<string> pvariablelist =  seperateVariables(pvar);
 
for(list<string>::iterator sitr = sentencelist.begin(); sitr!=sentencelist.end(); ++sitr){

string s = *sitr;
size_t s1 = s.find("(");
string sent = s.substr(0,s1);
size_t s2 = s.find(")");
 
if(  pred == change(sent) ){    
string svar = s.substr(s1+1,s2-s1-1);
list<string> svariablelist =  seperateVariables(svar);

int n = svariablelist.size();

 int flag = 0;
 
for(int i=0;i<n; i++){
string left;
string right;

left = pvariablelist.front();
pvariablelist.pop_front();
right = svariablelist.front();
svariablelist.pop_front();

//unify x with John if no x is unified
if(isVar(left)&&!isVar(right)){
    
if( unify1.find(left)== unify1.end() || unify1[left]==right || isVar(unify1[left]) ){
unify1[left]=right;
unify2 = updatemap(unify2,left,right);
}
else
flag=1;
}

//unify John with y if no y is unified
if(!isVar(left)&&isVar(right)){ 
if( unify2.find(right) == unify2.end() || unify2[right]==left || isVar(unify2[right]) ){
unify2[right]=left;
unify1 = updatemap(unify1,right,left);
}
else
flag=2;
}

//both constant 
if(!isVar(left)&&!isVar(right)){
if(left!=right)
flag=3;
}

//both var
if(isVar(left)&&isVar(right)){
    // x and y not present in either map
if((unify1.find(left)==unify1.end()||unify1[left]==right) && (unify2.find(right)==unify2.end()||unify2[right]==left) )
{ 
    unify1[left]=right;
    unify2[right]= left;
}
 
 // x not in map but y is mapped
else if( (unify1.find(left)==unify1.end()||unify1[left]==right) && unify2.find(right)!=unify2.end() ){
 
 unify1[left] = unify2[right];
 unify2 = updatemap(unify2,left,unify2[right]);
}
 // x  in map but y is not mapped
else if( unify1.find(left)!=unify1.end() && (unify2.find(right)==unify2.end()||unify2[right]==left)){
 unify2[right] = unify1[left];
 unify1 = updatemap(unify1,right,unify1[left]);
}
 else
  flag = 4;
}
}   //end of variable loop

 if(flag)
return sentence;
 else
  break;

}   //end of if
 
}//end of for sentence loop  

}//end of for predicate loop

for(list<string>::iterator itr = predicatelist.begin() ; itr!=predicatelist.end() ; ++itr){
string &p (*itr); 
for(map<string,string>::iterator itr=unify1.begin();itr!=unify1.end();++itr){
 string v = itr->first;
 string c = itr->second;
  if(!isVar(c))
   p = update(p,v,c);
  }   
}  

for(list<string>::iterator itr = sentencelist.begin() ; itr!=sentencelist.end() ; ++itr){
string &s (*itr); 
for(map<string,string>::iterator itr=unify2.begin();itr!=unify2.end();++itr){
 string v = itr->first;
 string c = itr->second;  
   s = update(s,v,c);
  }   
}
 //remove resolving terms
for(list<string>::iterator itr1 = predicatelist.begin() ; itr1!= predicatelist.end() ; ++itr1)
for(list<string>::iterator itr2 = sentencelist.begin() ; itr2!= sentencelist.end() ; ++itr2)
  {
      string &p( *itr1 );
      string &s (*itr2 );
  if( p == change(s) ){
  itr1 = predicatelist.erase(itr1);
  itr2 = sentencelist.erase(itr2);
   }
} 
 
//unify similar terms
for(list<string>::iterator pitr = predicatelist.begin(); pitr!=predicatelist.end() ;++pitr){

string p = *pitr;

size_t p1 = p.find("("); 
string pred = p.substr(0,p1);
size_t p2 = p.find(")");
string pvar = p.substr(p1+1,p2-p1 - 1);
list<string> pvariablelist =  seperateVariables(pvar);

for(list<string>::iterator sitr = sentencelist.begin(); sitr!=sentencelist.end(); ++sitr){

string s = *sitr;
size_t s1 = s.find("(");
string sent = s.substr(0,s1);
size_t s2 = s.find(")");

if(  pred == sent ){  
string svar = s.substr(s1+1,s2-s1-1);
list<string> svariablelist =  seperateVariables(svar);
int flag=0;
int n = svariablelist.size();

for(int i=0;i<n; i++){
string left;
string right;

left = pvariablelist.front();
pvariablelist.pop_front();
right = svariablelist.front();
svariablelist.pop_front();

//unify x with John if no x is unified
if(isVar(left)&&!isVar(right)){
    
if( unify1.find(left)== unify1.end() || unify1[left]==right || isVar(unify1[left]) ){
unify1[left]=right;
unify2 = updatemap(unify2,left,right);
}
else
flag=1;
}

//unify John with y if no y is unified
if(!isVar(left)&&isVar(right)){   
if( unify2.find(right) == unify2.end() || unify2[right]==left  || isVar(unify2[right]) ){
unify2[right]=left;
unify1 = updatemap(unify1,right,left);
}
else
flag=2;
}

//both constant 
if(!isVar(left)&&!isVar(right)){
if(left!=right)
flag=3;
}

//both var
if(isVar(left)&&isVar(right)){
    // x and y not present in either map
if((unify1.find(left)==unify1.end()||unify1[left]==right) && (unify2.find(right)==unify2.end()||unify2[right]==left) )
{ 
    unify1[left]=right;
    unify2[right]= left;
}
 
 // x not in map but y is mapped
else if( (unify1.find(left)==unify1.end()||unify1[left]==right) && unify2.find(right)!=unify2.end() ){
 
 unify1[left] = unify2[right];
 unify2 = updatemap(unify2,left,unify2[right]);
}
 // x  in map but y is not mapped
else if( unify1.find(left)!=unify1.end() && (unify2.find(right)==unify2.end()||unify2[right]==left)){
 unify2[right] = unify1[left];
 unify1 = updatemap(unify1,right,unify1[left]);
}
//x y both mapped to same const
else if( unify1.find(left)!=unify1.end() && unify2.find(right)!=unify2.end() && unify1[left]==unify2[right]);

else
flag = 4; 
}
}   //end of variable loop

 if(flag)
return sentence;
 else
  break;
 
}//end of if

}//end of for sentence loop
 
}   //end of for predicate loop


for(list<string>::iterator itr = predicatelist.begin() ; itr!=predicatelist.end() ; ++itr){
string &p (*itr); 
for(map<string,string>::iterator itr=unify1.begin();itr!=unify1.end();++itr){
 string v = itr->first;
 string c = itr->second;
 if(!isVar(c))
   p = update(p,v,c);
  }   
}  

for(list<string>::iterator itr = sentencelist.begin() ; itr!=sentencelist.end() ; ++itr){
string &s (*itr); 
for(map<string,string>::iterator itr=unify2.begin();itr!=unify2.end();++itr){
 string v = itr->first;
 string c = itr->second;
   s = update(s,v,c);
  }   
} 

//return single unified string
if(predicatelist.size() >0 && sentencelist.size()>0)
return concat(predicatelist,sentencelist);
else if(sentencelist.size()>0)
  return concatSentences(sentencelist);
else
  return concatSentences(predicatelist);
 
}    

//find key to resolve query q with ~q or vice versa
string findKey(string s){
string key;
size_t q = s.find("(");
if(s.find("~")==0)
key = s.substr(1,q-1);  
else
key="~" + s.substr(0,q);
return key;   
}    

  // to check if query is repeating
int isPresent(string s, list<string> r){
for( list<string>::iterator itr=r.begin();itr!=r.end();++itr){  
 if(*itr ==s)
  return 1;
}    
return 0;
}    

int resolution(string query){

stack<string> strStack;
list<string> repeat;
string key = findKey(query);
repeat.push_back(query);

 list<string> li = kbmap[key];

  for( list<string>::iterator li_itr=li.begin();li_itr!=li.end();++li_itr){  
  string s=*li_itr;
  string toPush = unification(query,s);

  if(toPush != s && !isPresent(toPush,repeat)){
     strStack.push(toPush); 
   } 
 }  

while(1){
if(strStack.empty())
 return 0;
 
key = strStack.top();
 strStack.pop();
 repeat.push_back(key);

if(key=="")
 return 1;
 
if(key!=query){
    
 list<string> keylist = seperateSentences(key);
 
 for(list<string>::iterator itr = keylist.begin(); itr!=keylist.end(); ++itr) {   
  
  string k = *itr;
  list<string> li = kbmap[findKey(k)];
  list<string>::iterator li_itr;
  for(li_itr=li.begin();li_itr!=li.end();++li_itr){
  string s= *li_itr;  
  string toPush = unification(key,s);
 
  if(toPush != s && !isPresent(toPush,repeat) ){
   //cout<<key<<" unified with : "<<s<<" gives---"<<toPush<<endl;
  strStack.push(toPush);
   }  
   }//end of sentence list
  }//end of keylist
   
 } //end of if  

}//end of while    
}  

//function to remove spaces
string removespaces(string s){
string r = "";
size_t start = 0;
size_t end = s.find(" ");
 
while(end < s.size()){
   r+= s.substr(start,end-start); 
 start=end+1;
 end= s.find(" ",end+1,1);
 }
 
 return r;
}
//standardizing function
string standardize(string sentence,int count){
string r = "";
list<string> sent_list = seperateSentences(sentence);
 
for(list<string>::iterator itr = sent_list.begin(); itr!=sent_list.end(); ++itr){
 
string s = *itr;
size_t s1 = s.find("(");
string sent = s.substr(0,s1);
size_t s2 = s.find(")");
string svar = s.substr(s1+1,s2-s1-1);
list<string> svariablelist =  seperateVariables(svar);
  
 string varlist=""; 
 for(list<string>::iterator vitr = svariablelist.begin(); vitr!=svariablelist.end(); ++vitr){
 string &v(*vitr);
  if(isVar(v)){
   v= v + to_string(count);
  }
  varlist+= v + ",";
 }
  varlist= varlist.substr(0,varlist.size()-1); 
  r+= sent + "(" + varlist + ")" + "|";
 }
 r = r.substr(0,r.size()-1);
 return r;
}

int main(){

int count = 0 ;
ifstream infile("input.txt");
string line;
getline(infile,line);
int no_queries = stoi(line);
string query;
if (infile.is_open()){
    while ( no_queries-- ){
     getline (infile,query);
     queries.push_back(query); 
    }

getline(infile,line);
int no_sentences = stoi(line);
string sentence;
    while ( no_sentences-- ){
     getline (infile,sentence);
     sentence = removespaces(sentence+" ");      
     count++;
     sentence = standardize(sentence,count);
     sentences.push_back(sentence); 
    }
}
     infile.close();

 
ofstream outfile("output.txt");
 
list<string>::iterator i;
 for(i=queries.begin();i!=queries.end();++i){ 

  sentences.push_back(change(*i));
  seperatePredicates();
  buildmap();
  
 int resolved = resolution(change(*i));
 if(resolved){
 outfile<<"TRUE"<<endl;
 }
 else{
 outfile<<"FALSE"<<endl;
 }  
  sentences.remove(change(*i));
  
 }
  
 outfile.close();
    return 0;
}    
