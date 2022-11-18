#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

#define rep(i, a, b)	for(int i = a; i < (b); ++i)

mt19937 rng((uint32_t)chrono::steady_clock::now().time_since_epoch().count());

using str = string;

struct to_print{
	pthread_mutex_t monitor;
	to_print() {
		monitor = PTHREAD_MUTEX_INITIALIZER;
	}

	void add(str x){
		pthread_mutex_lock(&monitor);
		cout << x << endl;
		pthread_mutex_unlock(&monitor);
	}

} ct;

class Barbershop{
private:
	pthread_mutex_t monitor;
	int n, customers;
	sem_t customer, workdone;
	vector<sem_t> q; //queue
	int cp, fp;

public:
	int tmp;
	Barbershop(int n) :  n(n), customers(0), q(n), cp(0), fp(0) {
		monitor = PTHREAD_MUTEX_INITIALIZER;
		sem_init(&customer, 0, 0);
		sem_init(&workdone, 0, 0);
		rep(i, 0, n) sem_init(&q[i], 0, 0);
	}

	void do_haircut(){
		sleep(1);
	}

	void* barber_thread(){
		int y;
		while(1){
			ct.add("Barber waiting...");
			sem_wait(&customer);

			pthread_mutex_lock(&monitor);

			y = fp;
			fp = (fp + 1)%n;
			--customers;
			ct.add("Barber gets client...");
			
			pthread_mutex_unlock(&monitor);

			sem_post(&q[y]);
			do_haircut();
			ct.add("Barber work finishes...");
			sem_post(&workdone);

		}
		return NULL;
	}

	void* customer_thread(int x){
		str xx = to_string(x);
		ct.add(xx + " Enters shop");
		pthread_mutex_lock(&monitor);

		if(customers == n){
			pthread_mutex_unlock(&monitor);
			ct.add(xx + " not get free chair");
			return NULL;
		}
		customers++;
		int y = cp;
		cp = (cp + 1)%n;
		
		pthread_mutex_unlock(&monitor);

		sem_post(&customer);
		sem_wait(&q[y]);
		ct.add(xx + " Sit on main chair ");
		sem_wait(&workdone);
		ct.add(xx + " Complete haircut");
		
		ct.add(xx + " Exits shop");
		return NULL;
	}

	static void* barber_launch(void* object){
		Barbershop* obj = reinterpret_cast<Barbershop*>(object);
		return obj->barber_thread();
	}

	static void* customer_launch(void* object){
		Barbershop* obj = reinterpret_cast<Barbershop*>(object);
		return obj->customer_thread(obj->tmp);
	}
};


void solve(int n, int m){
	Barbershop b(n);
	pthread_t barber;
	pthread_create(&barber, NULL, &Barbershop::barber_launch, &b);

	pthread_t customer[m];

	rep(i, 0, m){
		b.tmp = i;
		pthread_create(&customer[i], NULL, &Barbershop::customer_launch, &b);
		usleep(100000*(1 + rng()%5));
	}

	rep(i, 0, m){
		pthread_join(customer[i], NULL);
	}
}



int main(){
	int n = 4; // # of waiting chairs
	int m = 20; // # of customers
	solve(n, m);
}