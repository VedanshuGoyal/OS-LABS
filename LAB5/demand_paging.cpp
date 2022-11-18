/*---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---*/

#include <bits/stdc++.h>
using namespace std;

#define rep(i, a, b)	for(int i = a; i < (b); ++i)
#define each(a, b)		for(auto& a : b)

#define sz(x)       (int)(x).size()
#define all(a)      (a).begin(),(a).end()

#define eb emplace_back
#define f first
#define s second
#define vi vector<int>
#define rsz resize

mt19937 rng((uint32_t)chrono::steady_clock::now().time_since_epoch().count());
using str = string;

void __print(int x) {cout << x;}
void __print(long x) {cout << x;}
void __print(long long x) {cout << x;}
void __print(unsigned x) {cout << x;}
void __print(unsigned long x) {cout << x;}
void __print(unsigned long long x) {cout << x;}
void __print(float x) {cout << x;}
void __print(double x) {cout << x;}
void __print(long double x) {cout << x;}
void __print(char x) {cout << '\'' << x << '\'';}
void __print(const char *x) {cout << '\"' << x << '\"';}
void __print(const string &x) {cout << '\"' << x << '\"';}
void __print(bool x) {cout << (x ? "true" : "false");}
 
template<typename T, typename V>
void __print(const pair<T, V> &x) {cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}';}
template<typename T>
void __print(const T &x) {int f = 0; cout << '{'; for (auto &i: x) cout << (f++ ? ", " : ""), __print(i); cout << "}";}
void _print() {cout << "]\n";}
template <typename T, typename... V>
void _print(T t, V... v) {__print(t); if (sizeof...(v)) cout << ", "; _print(v...);}
#define dbg(x...) cout <<" [" << #x << "] = ["; _print(x);

// Samne Hai Sanam Aur kya Chahiye
/*---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---XXX---*/
// CODE BEGINS

#define TLB_MISS -3
#define PAGE_FAULT -5

struct Te{// TLB entries
	int key; // VIRTUAL ADDRESS
	int PA; // PHYSICAL ADDRESS

	Te (int VA, int PA) : key(VA), PA(PA) {}
};

struct Frame{
	int key;

	Frame(int key) : key(key) {}
	Frame() : key(0) {}
};

template<class T> struct LRU{
	unsigned int nf; // # of frames

	LRU(int nf) : nf(nf) {}

	list<T> dq;
	unordered_map<int, typename list<T>::iterator> m;
	typename unordered_map<int, typename list<T>::iterator>::iterator it;

	void init(){
		dq.clear();
		m.clear();
	}

	T pop(){
		T x = dq.back();
		dq.pop_back();
		m.erase(x.key);
		return x;
	}

	void insert(T x, bool ok = 0){ 
		// ok = 1 means already found the iterator
		if(ok == 0) it = m.find(x.key);

		if(it == m.end()){
			if(sz(dq) == nf) pop();
			dq.emplace_front(x);
			m[x.key] = dq.begin();
		}
		else{
			dq.erase(it->s);
			dq.emplace_front(x);
			it->s = dq.begin();
		}

	}

};

struct TLB : LRU<Te>{

	TLB(int SZ) : LRU(SZ) {}
	TLB() : LRU(0) {}

	int find(int VA){
		it = m.find(VA);
		if(it == m.end()) return TLB_MISS;

		insert(*(it->s), 1);
		return (it->s = dq.begin())->PA;
	}

} buf;

struct FrameList : LRU<Frame>{
	list<Frame> FreeFrames;
	vector<int*> virAdd;
	// int** virAdd; // Reverse Mapping -> Virtual Address
	Frame x;

	void FLinit(){
		init();
		FreeFrames.clear();
		rep(i, 0, nf) FreeFrames.eb(i);
	}

	FrameList(int nf = 0) : LRU(nf), virAdd(nf) {
		FLinit();
	}

	Frame RetFreeFrame(int *VA){
		if(FreeFrames.empty()){
			x = pop();
			*virAdd[x.key] |= 1<<31; // setting invalid bit
		}else{
			x = FreeFrames.front();
			FreeFrames.pop_front();
		}

		virAdd[x.key] = VA;
		insert(x);
		return x;
	}

} Frames;


struct PageTable{
	int SZ;
	vector<int> pg;
	// 31th bit is on -> invalid bit else valid bit 

	PageTable(int SZ) : SZ(SZ), pg(SZ){
		rep(i, 0, SZ) pg[i] = -1;
	}

	int find(int VA){

		if(pg[VA] < 0){
			// valid bit is on
			return PAGE_FAULT;
		}

		Frames.insert({pg[VA]}); //updating LRU
		return pg[VA];
	}

	int insert(int VA){
		Frame x = Frames.RetFreeFrame(&pg[VA]);
		return pg[VA] = x.key;
	}

};


int k; // # of process
int m; // max virtual address space
int f; // physical address space
int s; // (TLB SIZE)


void init(int &M, vi &ref){
	buf.init(); Frames.FLinit();
	M = 1 + rng()%m;

	// generate refence string
	ref.rsz(2*M + rng()%(8*M));
	each(x, ref) x = rng()%M;
}


void solve(int pid){
	int M; // current proces virtual space;
	vi ref; // reference string (page # list)

	init(M, ref);
	PageTable pt(M);

	dbg(M);
	dbg(ref);

	int pf = 0, PA;
	each(x, ref){
		printf("\n");
		printf("Process %d: for page reference %d, ", pid, x);
		if((PA = buf.find(x)) != TLB_MISS){
			printf("TLB hit with frame no. %d", PA);
			continue;
		}
		printf("TLB miss -> ");
		
		if((PA = pt.find(x)) != PAGE_FAULT){
			// dbg("s");
			buf.insert({x, PA});
			printf("page table valid with frame no. %d", PA);
			continue;
		}

		printf("page fault");
		++pf;
		PA = pt.insert(x);
		buf.insert({x, PA});
		printf(" -> get frame no. %d", PA);
	}

	printf("\n\nThe end of process Execution.\nTotal number of PAGE FAULTS => %d\n\n\n", pf);
}

int gen_rand_prcs(){
	static vi v;
	if(v.empty()){
		rep(i, 1, k+1) v.eb(i);

		shuffle(all(v), rng);
	}

	int z = v.back();
	v.pop_back();
	return z;
}


int main(int argc, char* argv[]){
	if(argc != 5){
		fprintf(stderr, 
			"usage %s <# of process> <virtual address space> <physical adress space> <size of TLB>\n", argv[0]);
		exit(1);
	}

	k = atoi(argv[1]);
	m = atoi(argv[2]);
	f = atoi(argv[3]);
	s = atoi(argv[4]);

	buf = TLB(s);
	Frames = FrameList(f);

	rep(_, 0, k) solve(gen_rand_prcs());
}