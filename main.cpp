#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <cassert>
#include <cstring>
#include <sched.h>
#include <sys/time.h>
// #include <sys/sysinfo.h>
//#include <sys/resource.h>
#include <float.h>
using namespace std;

#define ERR_CANNOT_OPEN -1
#define ERR_READ -2 
#define ERR_DEG_MATRIX -3   // РІС‹СЂРѕР¶РґРµРЅРЅР°СЏ РјР°С‚СЂРёС†Р°
#define EPS 5e-15

// #define LOG(...) std::cout<< #__VA_ARGS__<< " = " <<(__VA_ARGS__)<< "\n"
#define LOG(...)
#define LN       std::cout << "\n";





struct boundData{
	int firstPrime=0, lastPrime=0 , temp_count=0, prevTempMax = 0;
    int begPrime = 0, endPrime = 0;	
	
};

struct Args{
   
	double thr_time;
	boundData *bound;
    int p,n;
    int thr_num;
    int error = 0;
};



static pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t barrier;
static int count=0;
static int answer=0;


void *f(void * args);
double get_time();
double get_full_time();
bool isPrime(int k);
int nextPrime(int num);
void makeLastStep(int begin, int n , int prevPrime , int countn);





void *f(void * args){
    Args& data= *((Args *) args);

	int n=data.n, thr_num=data.thr_num, p=data.p;
	 bool flag;
	int len = 10000, step = 0 , prev_prime = 0, cur_prime=0 , temp_max=0 , temp_count = 0 , prev_temp_max = 0;
	while(count < n){
		LOG("+++++++++++++++++++++++++++++++++++");
		int beg = p*step*len + thr_num*len;
		int end = p*step*len + thr_num*len + len;
	
	
		for(int i = beg; i < end;i++){
			if(isPrime(i)){
				LOG(i);
				data.bound->temp_count++;
				if(prev_prime==0){
					if(thr_num==0){
						data.bound->begPrime = i;
					}
					prev_prime=i;
					data.bound->firstPrime=i;
				}else{
					cur_prime = i;
					if(cur_prime - prev_prime > temp_max){
						temp_max = cur_prime - prev_prime;
					}
					
					prev_prime = cur_prime;
				
				}
			}
		}
		if(cur_prime==0){
		data.bound->lastPrime=data.bound->firstPrime;
		}else{
			data.bound->lastPrime=cur_prime;
		}
		
		LOG(data.bound->begPrime);
		LOG(data.bound->lastPrime);
		LOG(data.bound->temp_count);
		LOG(temp_max);
		
		pthread_barrier_wait(&barrier);
		
	    bool flag = 0;
		if(thr_num ==0){
			for(int i = 0; i < p-1;i++){
				count+=data.bound[i].temp_count;
				
				if(count>n){
					LOG("VOT tut");
					makeLastStep(p*step*len + i*len, n , (i==0? data.bound[p-1].endPrime:data.bound[i -1].lastPrime) , count - data.bound[i].temp_count);
					flag=1;
					break;
				}
				if( data.bound[i+ 1].firstPrime - data.bound[i].lastPrime > answer){
					answer = data.bound[i+ 1].firstPrime - data.bound[i].lastPrime;
					
				}
				
			}
			LOG("POSLE");
			if(!flag){
				LOG("innnn");
				count+=data.bound[p-1].temp_count;
				if(count>n){
					makeLastStep(p*step*len + (p-1)*len, n , (p -1 ==  0? data.bound[p-1].endPrime:data.bound[p-2].lastPrime) , count - data.bound[p-1].temp_count);
				}
				
				//data.bound->endPrime=data.bound[p-1].lastPrime;
				if (p - 1 == 0) {
					LOG("FLEX");
					if (data.bound[p-1].endPrime != 0) {
						answer = std::max(answer, data.bound->firstPrime - data.bound[p-1].endPrime);
					}
				} else {
					LOG("KEKS");
					if (data.bound[p-2].lastPrime) {
						answer = std::max(answer,  data.bound->firstPrime - data.bound[p-2].lastPrime);
					}
				}
			}
		}
		
		LOG("4444444");
		pthread_barrier_wait(&barrier);
		if(count>n) {
			break;
		}
		step++;
		data.bound->prevTempMax = temp_max;
		LOG(data.bound->prevTempMax);
		prev_prime=0;
		cur_prime=0;
		data.bound->temp_count=0;
		if(thr_num == p-1){
			data.bound->endPrime = data.bound->lastPrime;
		}
	}
	LOG("visli");
	
	pthread_barrier_wait(&barrier);

	pthread_mutex_lock(&total_mutex);
        if(data.bound->prevTempMax > answer){
			answer= data.bound->prevTempMax;
        }


    pthread_mutex_unlock(&total_mutex);
	pthread_barrier_wait(&barrier);

    return 0;
}



void makeLastStep(int begin, int n , int prevPrime , int countn){
	LOG(prevPrime);
	int cur_prime = 0, next_prime = 0 ;
	while(countn!=n){
		if(cur_prime == 0){
			cur_prime = nextPrime(begin);
			LOG(cur_prime);
			if(cur_prime - prevPrime > answer){
				answer = cur_prime - prevPrime;
			}
        countn++;			
		}else{
			
			next_prime = nextPrime(cur_prime);
			LOG(next_prime);
			if(next_prime  - cur_prime > answer){
				
				answer = next_prime - cur_prime;
			}
			
			cur_prime = next_prime;
			countn++;
		}
		
	}
	
	
}


int nextPrime(int num){
	if(num < 2) return 2;
	int i=1;
	int ans;
	while(true){
		if(isPrime(num+i)){ 
		ans = num + i;
		break;
		}
		i++;
	}
	
	return ans;
}

bool isPrime(int k){
	if(k==2) return true;
	if(k%2==0 || k < 2) return false;
	int end = sqrt(k);
	for(int i=3; i < end + 1 ;i+=2){
		if(k % i==0) return false;
	}
	
	return true;
}








// double get_time(){
        // struct rusage buf;
        // getrusage(RUSAGE_THREAD,&buf);
        // return (double)buf.ru_utime.tv_sec+(double)buf.ru_utime.tv_usec/1000000.;
        // return 1;
// }

// double get_full_time(){
        // struct timeval buf;
        // gettimeofday(&buf,0);
        // return (double)buf.tv_sec+(double)buf.tv_usec/1000000.;
// }



int main(int argc , char *argv[]){
    pthread_t *threads;
    Args *args;
    boundData *bound;
    if(argc < 3){
        printf ("Invalid input data");
        return 1;
    }

	int n = 0 ,p =0;
    
	if (((p = stoi(argv[1])) <= 0) || ((n  = stoi(argv[2])) <= 0) ) {
            printf("usage: %s p n \n ", argv[0]);
	    return -1;
	}

	

    args = new Args[p];
    bound = new boundData[p];

    for(int i=0;i<p;i++){
        args[i].p =p;
        args[i].n= n;
		args[i].thr_num=i;
		args[i].bound = bound + i;
    }

    
    threads = new pthread_t[p];
    pthread_barrier_init (&barrier, nullptr, p);


	double time=clock();

    for(int i = 0;i<p; i++){
        if(pthread_create(threads+ i, 0 , f , (void *) (args + i))){
            printf("Cant create thread %d\n",i);
            return 2;
        }
    }


    for(int i = 0; i < p;i++){
        if(pthread_join (threads[i],0)){
              printf("Cant wait thread %d\n",i);
        }
    }
	time=clock() - time;
	
	cout<<"TIME : "<<time/CLOCKS_PER_SEC<<endl;
	LN;
	LN;
    cout << "answer = " << (answer);



   pthread_barrier_destroy(&barrier);

  

    delete [] bound;
    delete [] threads;
    delete [] args;


}
