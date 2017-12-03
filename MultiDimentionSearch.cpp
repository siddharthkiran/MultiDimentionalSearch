#include<iostream>
#include<map>
#include<vector>
#include<unordered_map>
#include<set>
#include<unordered_set>
#include<algorithm>
#include<string>

using namespace std;
class IDPPair{
	long id;
	int price;
	public:
	IDPPair(long i,int p){
		id=i;
		price=p;
	}
	
	long getID(){
		return id;
	}
	int getPrice(){
		return price;
	}
	
};
class mycomparator{
	public:
	bool operator()(pair<long,int>p1, pair<long,int> p2){
		return p2.second <p1.second;
	}
};
class MDS{
	
	public:
		unordered_map<long,vector<long>> ID_Desc;
		unordered_map<long,vector<long>> Desc_ID;
		unordered_map<long, float> Sup_Reput;		
		unordered_map<long, multimap<int,long>> ID_SUPandPRICE;
		unordered_map<long, map<long,int> > ID_Sup;
		//unordered_map<long,int> Sup_Price;
		map<long, float> S_R;
//		map<long, float> ID_Rep;
//		unordered_map<long, set<pair<int,long>>> Supp_Price;
		bool add(long id, long description[],int size);
		int add(long supplierID, IDPPair idprice[], int size);
		
		vector<long> findSuppliers(long ID);
		vector<long> findSuppliers(long ID, float minReputation);
		vector<long> findItem(long desc[], int size);
		vector<long> findItem(long n, int minPrice, int maxPrice, int minReputation);
		bool add(long supplierID, float reputation);
		
		int invoice(long arr[], int size, int minReputation);
		vector<long> purge(int maxreputation);
//		int add(long supplierId, set<pair<int,long>>);
		
};


vector<long> MDS::purge(int maxReputation){
	vector<long> result;
	for(auto itr = ID_Sup.begin(); itr!= ID_Sup.end();){
		long id = itr->first;
		
		bool flag =true;
		
		for(auto mapitr= itr->second.begin(); mapitr!= itr->second.end(); mapitr++){
			
			if(mapitr->second >= maxReputation){
				flag =false;
				break;
			}
		}
		
		if(flag){
			result.push_back(id);
			itr = ID_Sup.erase(itr);
		}
		else
			++itr;
	}
	
	for(auto itr = result.begin();itr!=result.end();itr++){
		ID_SUPandPRICE.erase(*itr);
	}
	return result;
}


int MDS::invoice(long arr[],int size, int minReputation){
	
	int sum=0;
	for(int i=0;i<size;i++){
		long id = arr[i];
		
		auto itr= ID_SUPandPRICE.find(id);
		
		if(itr!= ID_SUPandPRICE.end()){
			
			multimap<int,long> Price_ID = ID_SUPandPRICE[id];
			
			for(auto mapitr = Price_ID.begin();mapitr!=Price_ID.end();++mapitr){
				auto repitr = Sup_Reput.find(mapitr->second);
				if(repitr!=Sup_Reput.end()){
					if(repitr->second>= minReputation ){
						sum = sum + mapitr->first;
						break;
					}
				}
				else{
					//reject the supplier that doent have reputation
					
				}
			}
		}
		else{
			//id not present so dont calculate it in invoice
		}
	}
	return sum;
}


int MDS::add(long supplierID, IDPPair idprice[],int size){
	
	int count=0;
	for(int i=0;i<size;i++){
		
		long id = idprice[i].getID();
		auto itr = ID_Sup.find(id);
	
	//if item ID is not present - simply insert supplier and price in map and increment count
	if(itr==ID_Sup.end()){
		count++;
		map<long,int > sp;
		sp.insert(pair<long,int> (supplierID, idprice[i].getPrice()));
		ID_Sup.insert(pair<long, map<long,int> > (id,sp));
		//Sup_Price.insert(pair<long,int> (supplierID, idprice[i].getPrice()));
		
		//insert into ID_SUPandPRICE;
		
		multimap<int,long> mm;
		mm.insert( pair<int,long> (idprice[i].getPrice(), supplierID));
		ID_SUPandPRICE.insert(pair<long, multimap<int,long> > (id, mm));
		
	}
	else{							//if item is present
			map<long,int> sp = itr->second;
			auto mapitr = sp.find(supplierID);
			multimap<int,long> mp = ID_SUPandPRICE[id];
			
			//if supplier not present add supplier and increment count
			
			if(mapitr == sp.end()){
				sp.insert(pair<long,int> (supplierID,idprice[i].getPrice()));      //inset id_sup
				
				itr->second =sp;
				//insert into data structure ID_supandPrice as well
				mp.insert(pair<int, long> (idprice[i].getPrice(), supplierID));
				//ID_SUPandPRICE.insert(pair<long,multimap<int,long> > (id,mp ));
				ID_SUPandPRICE[id] = mp;
				count++;
			}
			else{				//if suppler is present
			
				int oldPrice= mapitr->second;
				sp[supplierID]= idprice[i].getPrice();
				//sp.insert(pair<long,int> (supplierID,idprice[i].getPrice()));      //insert id_sup
				
				itr->second=sp;
			
				//update into data structure ID_supandPrice as well
				typedef multimap<int,long>:: iterator iterator;
				pair<iterator,iterator> iterpair = mp.equal_range(oldPrice);
				
				for(auto it = iterpair.first; it!= iterpair.second;++it){
					if(it->second == supplierID){
						mp.erase(it);
						break;
					}
				}
				mp.insert( pair<int, long>(idprice[i].getPrice(), supplierID));
					ID_SUPandPRICE[id] = mp;
			}
			
	}}
	
	return count;
} 


vector<long> MDS::findItem(long n, int minPrice, int maxPrice, int minReputation){
	
	auto itrIdSupPrice = ID_SUPandPRICE.find(n);
	if(itrIdSupPrice== ID_SUPandPRICE.end())
	{
		return vector<long>();
	}
	multimap<int,long> temp = itrIdSupPrice->second;
	auto itr1 =  temp.lower_bound(minPrice);
	cout<<"minprice "<<itr1->first;
	auto itr2 = temp.upper_bound(maxPrice);
	
	cout<<"maxprice "<< itr2->first;
	vector<long> res;
	for(auto itr = itr1;itr!=itr2; ++itr){
		
		unordered_map<long, float>::iterator itr3 = Sup_Reput.find(itr->second);
		if(itr3!=Sup_Reput.end() && itr3->second >=minReputation) {
			res.push_back(itr->second);
		}
		else{
			//do nothin if reputation is not present or less than minimum reputation
		}
	}
	return res;		
}



 /* given an array of Longs, return an array of items whose
      description contains one or more elements of the array, sorted
      by the number of elements of the array that are in the item's
      description (non-increasing order).
    */

vector<long> MDS::findItem(long desc[], int size){
	
	unordered_map<long, int> count;
	
	for(int i=0;i<size;i++){
		auto itr = Desc_ID.find(desc[i]);
		if(itr!= Desc_ID.end()){
			vector<long> v = itr->second;
			
			for(long l : v){
				auto itr2 = count.find(l);
				if(itr2!=count.end()){
					count[l]=count[l]+1;
				}
				else{
					count.insert(pair<long,int> (l,1));
				}
			}
		}
		
	}
//	for(auto itr3 = count.begin();itr3!=count.end();++itr3){
//		cout<<itr3->first<<" "<<itr3->second<<endl;
//	}
	
	//convert the map into ordered multiset as there can be duplicate count values
	
	multiset<pair<long, int> , mycomparator> countset(count.begin(),count.end());
	
	
//	for(auto itr= countset.begin();itr!=countset.end();++itr){
//		cout<<itr->first<<" "<<itr->second<<endl;
//	}

	//return vector of sorted ids
	vector <long> res;
	for(auto itr= countset.begin();itr!=countset.end();++itr){
		res.push_back(itr->first);
	}
	return res;
}



bool MDS::add(long supplierID, float reputation){
	bool result= false;
	
	auto itr = Sup_Reput.find(supplierID);
	
	if(itr!=Sup_Reput.end()){
		Sup_Reput[supplierID]=reputation;
	}
	else{
		Sup_Reput.insert(pair<long, float> (supplierID, reputation));
		result = true;
	}
	
	
	return result;
}


 /* given an id, return an array of suppliers who sell that item,
      ordered by the price at which they sell the item (non-decreasing order).
    */

vector<long> MDS::findSuppliers(long ID){
	vector <long> res;
	
	auto itr = ID_SUPandPRICE.find(ID);
	
	if(itr!= ID_SUPandPRICE.end())
		{
			multimap<int,long> temp= itr->second;
			
			for(auto mapitr= temp.begin(); mapitr!=temp.end(); ++mapitr ){
				res.push_back(mapitr->second);
			}
		}
	return res;
}



vector<long> MDS::findSuppliers(long ID, float minReputation){
	vector <long> res;
	
	auto itr = ID_SUPandPRICE.find(ID);
	
	if(itr!= ID_SUPandPRICE.end())
		{
			multimap<int,long> temp= itr->second;
			
			for(auto mapitr= temp.begin(); mapitr!=temp.end(); ++mapitr ){
				
				auto itr = Sup_Reput.find(mapitr->second);
				if(itr!=Sup_Reput.end() && Sup_Reput[mapitr->second]>minReputation)
					res.push_back(mapitr->second);
				else{
					//rejecting values that dont have reputation...
				}
			}
		}
	return res;
}



bool MDS:: add(long id, long description[],int size){
	
	bool res = false;
	unordered_map<long,vector<long>> :: iterator itr;


	vector<long> v;
	for(int i=0;i<size;i++)
	{
		v.push_back(description[i]);
	}
		
		
	itr = ID_Desc.find(id);
	
	if(itr == ID_Desc.end()){
		ID_Desc.insert(pair<long,vector<long>> (id,v));
		res = true;
	}
	else{
		vector<long> *pre = &itr->second;
		(*pre).insert((*pre).end(),v.begin(),v.end());
	}
	
	//add to descID map as well

	vector<long> :: iterator itrvec;
	for(itrvec = v.begin();itrvec!=v.end();++itrvec){

		itr = Desc_ID.find(*itrvec);
		if(itr == Desc_ID.end()){
		//	cout<<"1"<<endl;
			Desc_ID.insert(pair<long,vector<long>> (*itrvec, vector<long>{id}));
		}	
		else
		{
		 	vector<long> *pre = &itr->second;
			(*pre).insert((*pre).end(),id);
		}
	}
	return res;
} 

void printMap(unordered_map<long,vector<long> > m ){
	unordered_map<long,vector<long>>:: iterator itr;
	
	for(itr= m.begin();itr != m.end(); ++itr){
		cout<< itr->first<<" --";
		for(long l: itr->second){
			cout<<l<<" ,";
		}
		cout<<endl;
	}
}
void printVector(vector<long> v){
	cout<<endl;
	cout<<"vector of ->";
	for(long l: v){
		cout<<l<<" ";
	}
	cout<<endl;
}

void printID_Supplier_Price(unordered_map<long, map<long,int> > m){
	
	for(auto it = m.begin(); it!= m.end(); ++it){
		cout<<"Item is: "<<it->first<<endl;
		
		auto mapit = it->second.begin();
		for(; mapit!= it->second.end();++mapit){
			cout<<"supplier: "<<mapit->first<<"  price: "<<mapit->second<<endl;
		}
	}
}

void printID_Price_Supplier(unordered_map<long, multimap<int,long> > m){
	
		for(auto it = m.begin(); it!= m.end(); ++it){
		cout<<"Item is: "<<it->first<<endl;
		
		auto mapit = it->second.begin();
		for(; mapit!= it->second.end();++mapit){
			cout<<"price: "<<mapit->first<<"  supplier: "<<mapit->second<<endl;
		}
	}
}

int main(){
	
	MDS obj;
	
	// data for supplier ,Item and price pair
	
	
	IDPPair i1(121,20);
	IDPPair i2(122,30);
	IDPPair i3(123,5);
	IDPPair i4(124,6);
	
	IDPPair arr[] ={i1,i2,i3,i4};
	
	int cnt =obj.add(10, arr,4);
	 cout<<"count is:"<<cnt<<endl;
	 
	IDPPair i5(121,19);
	IDPPair i6(125,33);
	IDPPair i7(123,6);
	IDPPair i8(129,40); 
	 
	IDPPair arr2[]= {i5,i6,i7,i8};
	 
	cnt = obj.add(20, arr2, 4);
	 cout<<"count is:"<<cnt<<endl;
	 
	IDPPair i9(122,25);
	IDPPair i10(128,32);
	IDPPair i11(129,38);
	
	IDPPair arr3[]= {i9,i10,i11};
	
	cnt = obj.add(30,arr3,3); 
	 
	 
	IDPPair i12(122,27);
	IDPPair i13(129,41);
	
	IDPPair arr4[]= {i12,i13};
	cnt = obj.add(10,arr4,2);
	
	cout<<" cnt "<<cnt<<endl;
	 printID_Supplier_Price(obj.ID_Sup);
	 
	 printID_Price_Supplier(obj.ID_SUPandPRICE);
	 
	//	IDPPair i1(121,20);
	//	IDPPair i1(121,20);				
	
	//add supplier id and reputation
	obj.add(30, 1);
	obj.add(20, 3);
	obj.add(10, 2);
	
	vector<long> res=obj.findItem(129, 38,41,1);
	printVector(res);
	cout<<"here-------"<<endl;
	
	
	
	
	///////////////////////////
	
	// to generate invoice
	
	long arritems[]={129,124,122};
	
	int total =obj.invoice(arritems,3,2);
	
	cout<<"total for array of items is:"<<total<<endl;
	
	
	vector<long> v2= obj.purge(2);
	printVector(v2);
	
	printID_Supplier_Price(obj.ID_Sup);
	 
	printID_Price_Supplier(obj.ID_SUPandPRICE);
	 
	
	//////////////////////////
	long a[9]={14,56,366,1681,4995,45,77,66,11};
	long b[3]={4,5,6};
	long c[4]={88,22,11,99};
	int size = sizeof a / sizeof* a;

	obj.add(5,a,size);
	obj.add(1,b,3);
	obj.add(2,c,4);
	obj.add(1,c,4);
	
	long desc[]={11,22};
//	printMap(obj.ID_Desc);
//	cout<<"here"<<endl;
	printMap(obj.Desc_ID);
	cout<<"now here"<<endl;
	
	vector<long> v = obj.findItem(desc,2);
	printVector(v);
	return 0;
}
