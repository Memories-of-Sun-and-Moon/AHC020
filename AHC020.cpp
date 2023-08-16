# include "bits/stdc++.h"
using namespace std;
using ll = long long;
# define all(qpqpq)           (qpqpq).begin(),(qpqpq).end()
# define UNIQUE(wpwpw)        sort(all((wpwpw)));(wpwpw).erase(unique(all((wpwpw))),(wpwpw).end())
# define rep(i,upupu)         for(int i = 0, i##_len = (upupu);(i) < (i##_len);(i)++)
# define len(x)               ((int)(x).size())
template<class T> bool chmin(T& a,T b) { if(a > b){a = b; return true;} return false; }
template<class T> bool chmax(T& a,T b) { if(a < b){a = b; return true;} return false; }
template<class T>constexpr T INF() { return ::std::numeric_limits<T>::max(); }
template<class T>constexpr T HINF() { return INF<T>() / 2; }

#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#ifdef LOCAL
#  include "_debug_print.hpp"
#  define debug(...) debug_print::multi_print(#__VA_ARGS__, __VA_ARGS__)
#else
#  define debug(...) (static_cast<void>(0))
#endif

const double TIME_LIMIT = 2000.0;

class xor_shift_128 {
public:
    typedef uint32_t result_type;
    xor_shift_128(uint32_t seed = 42) {
        set_seed(seed);
    }
    void set_seed(uint32_t seed) {
        a = seed = 1812433253u * (seed ^ (seed >> 30));
        b = seed = 1812433253u * (seed ^ (seed >> 30)) + 1;
        c = seed = 1812433253u * (seed ^ (seed >> 30)) + 2;
        d = seed = 1812433253u * (seed ^ (seed >> 30)) + 3;
    }
    uint32_t operator() () {
        uint32_t t = (a ^ (a << 11));
        a = b; b = c; c = d;
        return d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
    }
    static constexpr uint32_t max() { return numeric_limits<result_type>::max(); }
    static constexpr uint32_t min() { return numeric_limits<result_type>::min(); }
private:
    uint32_t a, b, c, d;
};
 
xor_shift_128 gen;
 
//[_first, _second] の（たぶん）一様分布
#define RND(_first, _second)    uniform_int_distribution<int>(_first, _second)(gen)
#define RND_float(_first, _second) uniform_real_distribution<float>(_first, _second)(gen);

struct measure_time{
    using timer_point = chrono::high_resolution_clock::time_point;
    using timer_duration = chrono::high_resolution_clock::duration;
private:
    timer_point clock_start;
    timer_duration time_length;
    bool is_stopping;
public:

    measure_time() : time_length(std::chrono::high_resolution_clock::duration()), is_stopping(false) {}

    inline void start(){
        clock_start = chrono::high_resolution_clock::now();
    }

    inline void stop(){
        assert(is_stopping == false);
        timer_point lap = chrono::high_resolution_clock::now();

        timer_duration d = lap - clock_start;

        time_length += d;
        is_stopping = true;
    }

    inline void resume(){
        assert(is_stopping == true);
        clock_start = chrono::high_resolution_clock::now();
        is_stopping = false;
    }

    inline int elapsed(){
        assert(is_stopping == false);
        timer_point lap = chrono::high_resolution_clock::now();

        timer_duration d = lap - clock_start;

        time_length += d;

        clock_start = chrono::high_resolution_clock::now();

        return chrono::duration_cast<chrono::milliseconds>(time_length).count();
    }
}timer;

struct graph {
    int cost;
    int to;
    int id;
};

int get_D(int &x, int &y, int &u, int &v) {
    int d = (x - u) * (x - u) + (y - v) * (y - v);
    double r = round(sqrt(d));
    return int(r);
}

const int N = 100;
int M, K;
vector<int> X, Y;
vector<vector<graph>> G;
vector<int> A, B;

inline void input(){
    int _n;
    cin >> _n >> M >> K;
    X.resize(N);
    Y.resize(N);
    G.resize(N);
    A.resize(K);
    B.resize(K);
    rep(i, N)cin >> X[i] >> Y[i];
    int u, v, w;
    rep(i, M){
        cin >> u >> v >> w;
        u--, v--;
        G[u].push_back({w, v, i});
        G[v].push_back({w, u, i});
    }
    rep(i, K){
        cin >> A[i] >> B[i];
    }
    return;
}

class Broadcasting {
private:
    vector<ll> P;
    vector<bool> is_in_set;
    set<int> using_power;
    vector<vector<int>> good_P_distance;
    vector<vector<set<int>>> current_disance_set;
    vector<int> current_P_idx;

    vector<set<int>> covered_by; // 人あたりのカバーしている放送局のid
    vector<set<int>> cover_to; // 放送局あたりのカバーしている人のid

    ll P_cost, W_cost;
    ll score;
    ll get_W_cost_naivety();
public:
    Broadcasting();

    ll get_score() { return score = P_cost + W_cost; };
    ll get_P(int id) { return P[id]; };
    int get_current_idx(int id) {return current_P_idx[id]; };
    int get_idx_size(int id) {return len(good_P_distance[id]); };
    void change_P_one(int id, int power);
    void change_P_idx(int id, int new_idx);
    void output();
};

Broadcasting::Broadcasting(){
    P = vector<ll>(N, 5000);
    P_cost = 0;
    W_cost = 0;
    score = INF<ll>();
    rep(i, N)P_cost += P[i] * P[i];
    covered_by.resize(K);
    cover_to.resize(N);
    rep(i, K){
        rep(j, N){
            if(get_D(A[i], B[i], X[j], Y[j]) <= P[j]){
                covered_by[i].insert(j);
                cover_to[j].insert(i);
            }
        }
    }
    is_in_set.resize(M);
    rep(i, N)using_power.insert(i);
    good_P_distance.resize(N);
    current_disance_set.resize(N);
    current_P_idx.resize(N);
    rep(i, N){
        good_P_distance[i].push_back(0);
        rep(j, K){
            good_P_distance[i].push_back(get_D(X[i], Y[i], A[j], B[j]));
        }
        good_P_distance[i].push_back(5000);
        sort(all(good_P_distance[i]));
        UNIQUE(good_P_distance[i]);
        rep(j, K){
            int d = get_D(X[i], Y[i], A[j], B[j]);
            current_disance_set[i][lower_bound(all(good_P_distance[i]), d) - good_P_distance[i].begin()].insert(j);
        }
        current_P_idx[i] = len(good_P_distance[i]) - 1;
    }
    W_cost = get_W_cost_naivety();
}

vector<ll> steiner_dist;
priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
ll Broadcasting::get_W_cost_naivety(){
    ll ret = 0;
    if(steiner_dist.empty())steiner_dist.resize(N);
    rep(i, M)is_in_set[i] = false;
    set<int> st;
    st.insert(0);
    while(st != using_power){
        rep(i, N)steiner_dist[i] = INF<ll>();
        for(auto e : st){
            steiner_dist[e] = 0;
            pq.push({0, e});
        }
        while(not pq.empty()){
            auto [dist, v] = pq.top();
            pq.pop();

            if(steiner_dist[v] < dist)continue;

            for(auto nxt : G[v]){
                if(chmin(steiner_dist[nxt.to], steiner_dist[v] + nxt.cost)){
                    pq.push({steiner_dist[nxt.to], nxt.to});
                }
            }
        }
        ll minl = INF<ll>(), mini = -1;
        rep(i, N){
            if(st.count(i))continue;
            if(not using_power.count(i))continue;
            if(chmin(minl, steiner_dist[i])){
                mini = i;
            }
        }
        st.insert(mini);
        ret += minl;

        ll d = minl;
        ll cur = mini;
        while(d > 0){
            for(auto nxt : G[cur]){
                if(d - nxt.cost == steiner_dist[nxt.to]){
                    is_in_set[nxt.id] = true;
                    d -= nxt.cost;
                    cur = nxt.to;
                    break;
                }
            }
        }
    }
    return ret;
}

void Broadcasting::change_P_one(int id, int power){
    P_cost -= P[id] * P[id];
    rep(i, K){
        covered_by[i].erase(id);
    }
    cover_to[id].clear();
    P[id] = power;
    P_cost += P[id] * P[id];
    rep(i, K){
        if(get_D(X[id], Y[id], A[i], B[i]) <= P[id]){
            covered_by[i].insert(id);
            cover_to[id].insert(i);
        }
    }
    if(P[id] == 0){
        using_power.erase(id);
        W_cost = get_W_cost_naivety();
    }else{
        using_power.insert(id);
        W_cost = get_W_cost_naivety();
    }
}

void Broadcasting::change_P_idx(int id, int new_idx){
    if(current_P_idx[id] == new_idx)return;
    P_cost -= P[id] * P[id];
    for(int i = current_P_idx[id] + 1;i <= new_idx;i++){
        for(auto people : current_disance_set[id][i]){
            covered_by[people].insert(id);
            cover_to[id].insert(people);
        }
    }
    for(int i = new_idx + 1;i <= current_P_idx[id];i++){
        for(auto people : current_disance_set[id][i]){
            covered_by[people].erase(id);
            cover_to[id].erase(people);
        }
    }
    P[id] = good_P_distance[id][new_idx];
    P_cost += good_P_distance[id][new_idx] * good_P_distance[id][new_idx];
    if(P[id] == 0){
        using_power.erase(id);
        W_cost = get_W_cost_naivety();
    }else if(current_P_idx[id] == 0){
        using_power.insert(id);
        W_cost = get_W_cost_naivety();
    }
}

void Broadcasting::output(){
    rep(i, N)cout << P[i] << " \n"[i == N - 1];
    rep(i, M)cout << (is_in_set[i] ? 1 : 0) << " \n"[i == M - 1];
}

class Neighborhood{
protected:
    Broadcasting *broadcasting;
public:
    virtual void exec() = 0;
    virtual void roll_back() = 0;
    virtual ll score() = 0;
};

// P の値を減らす
class SinglePExchange : Neighborhood{
private:
    SinglePExchange(Broadcasting *_broadcasting){ broadcasting = _broadcasting; };
    int id;
    int old_P;
public:
    static Neighborhood *neighborhood;
    static Neighborhood *get_object(Broadcasting *Broadcasting){
        if(neighborhood == nullptr){
            neighborhood = new SinglePExchange(Broadcasting);
        }
        return neighborhood;
    }
    void exec();
    void roll_back();
    ll score() {return broadcasting->get_score(); };
};

Neighborhood *SinglePExchange::neighborhood = nullptr;

void SinglePExchange::exec(){
    
    id = RND(0, N - 1);
    old_P = broadcasting->get_P(id);
    broadcasting->change_P_one(id, max(0, old_P - RND(0, 1000)));
}

void SinglePExchange::roll_back(){
    broadcasting->change_P_one(id, old_P);
}


// P の値を人の集合が変化するまで減らす
class SinglePExchangeByIdx : Neighborhood{
private:
    SinglePExchangeByIdx(Broadcasting *_broadcasting){ broadcasting = _broadcasting; };
    int id;
    int old_idx;
public:
    static Neighborhood *neighborhood;
    static Neighborhood *get_object(Broadcasting *Broadcasting){
        if(neighborhood == nullptr){
            neighborhood = new SinglePExchangeByIdx(Broadcasting);
        }
        return neighborhood;
    }
    void exec();
    void roll_back();
    ll score() {return broadcasting->get_score(); };
};

Neighborhood *SinglePExchangeByIdx::neighborhood = nullptr;

void SinglePExchangeByIdx::exec(){
    
    id = RND(0, N - 1);
    old_idx = broadcasting->get_current_idx(id);
    broadcasting->change_P_idx(id, RND(0, broadcasting->get_idx_size(id) - 1));
}

void SinglePExchangeByIdx::roll_back(){
    broadcasting->change_P_idx(id, old_idx);
}
Neighborhood *select_neighborhood(Broadcasting &broadcasting){
    return SinglePExchangeByIdx::get_object(&broadcasting);
}

int main(){
    input();
    timer.start();
    Broadcasting broadcasting = Broadcasting();
    int iteration = 0;
    ll best_score = broadcasting.get_score();

    while(true){
        iteration++;
        if(timer.elapsed() > TIME_LIMIT * 0.98){
            break;
        }

        Neighborhood *neighbor = select_neighborhood(broadcasting);
        neighbor->exec();
        ll new_score = neighbor->score();

        if(chmin(best_score, new_score)){
            debug(timer.elapsed(), new_score);
        }else{
            neighbor->roll_back();
        }
    }
    broadcasting.output();
}