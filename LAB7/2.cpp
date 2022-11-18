#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b)	for(int i = a; i < (b); ++i)
#define each(a, b)	for(auto& a : b)


#define eb emplace_back
#define vt vector
#define vi vector<int>

// I heard they don't treat you right.

bool isSafeState(int n, int m, vi Avail, vt<vi> &Max, vt<vi> &Aloc){
	queue<int> q;
	rep(i, 0, n) q.emplace(i);

	bool b[n] = {0};
	while(1){
		int x = 0, y = 0;
		rep(i, 0, n){
			if(b[i]) continue;
			++x;
			bool ok = 1;
			rep(j, 0, m){
				if(Avail[j] < Max[i][j] - Aloc[i][j]) ok = 0;
			}
			if(ok){
				++y;
				b[i] = 1;
				rep(j, 0, m) Avail[j] += Aloc[i][j];
			}
		}
		if(x == 0) return 1;
		if(y == 0) return 0;
	}
}

void solve(){
	printf("Enter # of processes & resources.\n");
	int n, m; 		// # of processes, resource type
	scanf("%d%d", &n, &m);

	// Available matrix
	printf("Enter Available Matrix.\n");
	vi Avail(m);
	each(x, Avail) scanf("%d", &x);

	// Max & Allocation
	printf("Enter Max Matrix.\n");
	vt<vi> Max(n, vi(m)), Aloc = Max;
	each(x, Max) each(y, x) scanf("%d", &y);

	printf("Enter Allocation Matrix.\n");
	each(x, Aloc) each(y, x) scanf("%d", &y);


	if(isSafeState(n, m, Avail, Max, Aloc)){
		printf("It is a safe state.\n");
	}else{
		printf("It is in unsafe state\n");
	}

	//Request Process
	printf("Process # of the request & resource needed.\n");
	int pn; 		// prpocess number
	scanf("%d", &pn);
	vi Rn(m);		// Resource needed
	each(x, Rn) scanf("%d", &x);

	bool ok = 1;
	rep(i, 0, m) if(Aloc[pn][i] + Rn[i] > Max[pn][i]) ok = 0;

	if(!ok){
		printf("Error! Process need more resources than demand.\n");
		return;
	}

	rep(i, 0, m) if(Avail[i] < Rn[i]) ok = 0;

	if(ok){
		rep(i, 0, m){
			Avail[i] -= Rn[i];
			Aloc[pn][i] += Rn[i];
		}
	}


	if(ok == 0 || !isSafeState(n, m, Avail, Max, Aloc)){
		printf("Process can't complete immediately. But it can run after some processes.\n");
	}else{
		printf("Process can complete immediately.\n");
	}

}


int main(){
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	solve();
}