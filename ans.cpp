#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

constexpr int N = 1000;

struct Point {
    int x; int y;

    int diff(const Point& rhs) { return abs(x - rhs.x) + abs(y - rhs.y); }
};

struct Order {
    int id;
    Point from;
    Point to;
    int diff;
    bool operator<( const Order& right ) const {
        return diff < right.diff;
    }
};

typedef double Score;
typedef unordered_set<int> Took;

struct State {
    Took took;
    vector<Point> routes;
    Score score;

    static State getInit() {
        State initState;
        initState.routes.push_back({400, 400});
        initState.routes.push_back({400, 400});
        initState.score = 0;
        return initState;
    }

    void addBestPlace(Order order) {
        int choices = routes.size() - 1;
        // 始点の挿入位置による増加タイム
        vector<Score> incScoreFrom(choices);
        for (int i = 0; i < choices; ++i) {
            double beforeScore = routes[i].diff(routes[i + 1]);
            double afterScore = routes[i].diff(order.from) + order.from.diff(routes[i + 1]);

            incScoreFrom[i] = afterScore - beforeScore;
        }

        Score leastScore = 1000000;
        int leastFromPos = -1;
        int leastToPos = -1;

        int j = 0;
        int leastScoreIncreaseFrom = 0; // jの更新をあとでやる

        for (int i = 0; i < choices; ++i) {
            Score beforeScore = routes[i].diff(routes[i + 1]);
            Score afterScore = routes[i].diff(order.to) + order.to.diff(routes[i + 1]);

            Score incScore = afterScore - beforeScore;

            // しゃくとりによる最善をチェック
            Score tempIncScore = incScore + incScoreFrom[j];
            if (leastFromPos == -1 || tempIncScore < leastScore) {
                leastScore = tempIncScore;
                leastFromPos = j;
                leastToPos = i;
            }

            // 同じ位置に挿入する場合
            Score scoreWithSamePlace =
              routes[i].diff(order.from)
              + order.from.diff(order.to)
              + order.to.diff(routes[i + 1]
              - routes[i].diff(routes[i + 1]);

            if (leastFromPos == -1 || scoreWithSamePlace < leastScore) {
                leastScore = scoreWithSamePlace;
                leastFromPos = i;
                leastToPos = i;
            }

            // しゃくとりのjを更新
            if (incScoreFrom[i] < incScoreFrom[j]) {
                j = i;
            }
        }

        vector<Point> newRoutes;
        newRoutes.reserve(routes.size() + 2);
        for (int i = 0; i < routes.size(); ++i) {
            newRoutes.push_back(routes[i]);

            if (i == leastFromPos) newRoutes.push_back(order.from);
            if (i == leastToPos) newRoutes.push_back(order.to);
        }
        took.insert(order.id);
        routes = newRoutes;
        // cout << "insert "<< order.id + 1 << " into from" << leastFromPos << " to " << leastToPos << " score diff " << leastScore << endl;
        // cout << "score "<< score << " " << score + leastScore << endl;
        score += leastScore;
    }

    static void generateAndInsertNextChoices(State currentState, vector<Order>& orders, priority_queue<State> & nextQueue, int width) {
        int worstScore = -1;
        for (auto& order : orders) {
            if (currentState.took.count(order.id) > 0) continue;
            State nextState = currentState;

            nextState.addBestPlace(order);

            if (worstScore == -1 || nextQueue.size() < width || nextState.score < worstScore) {
                nextQueue.push(nextState);
                if (worstScore == -1) {
                    worstScore = nextState.score;
                } else if (worstScore < nextState.score) {
                    worstScore = nextState.score;
                }
            }
        }
    }

    void printScore() { cout << "score: " << score << endl; }
    void print() {
        cout << took.size();
        for (auto& e : took) { cout << " " << e + 1; }
        cout << endl;

        cout << routes.size();
        for (auto& s : routes) { cout << " " << s.x << " " << s.y; }
        cout << endl;
    }

    bool operator<(const State& rhs) const { return score > rhs.score; } // rev
    // bool operator<(const State& rhs) const { return score < rhs.score; }
};

static State EMPTY_STATE = { Took(), vector<Point>(), -1 };

int main() {

    Point center = {400, 400};
    vector<Order> orders(N);
    for (int i = 0; i < N; ++i) {
        orders[i].id = i;
        cin >> orders[i].from.x >> orders[i].from.y >> orders[i].to.x >> orders[i].to.y;
        orders[i].diff = center.diff(orders[i].from) + center.diff(orders[i].to);
    }

    // sort(orders.begin(), orders.end());
    
    // vector<Order> orders2; orders2.reserve(N);
    // for (int i = 0; i < 500; ++i) {
    //     orders2.push_back(orders[i]);
    // }

    // vector<Order> orders2; orders2.reserve(N);
    // for (int i = 0; i < N; ++i) {
    //     if (orders[i].diff < LIMIT) orders2.push_back(orders[i]);
    // }
    // cout << orders2.size() << " " << orders2[0].diff << endl;

    priority_queue<State> queue;
    queue.push(State::getInit());

    int stock = 0;
    while (stock < 50) {
        priority_queue<State> nextQueue;
        vector<Took> taboo;
        // priority_queue<State> nextQueue;
        constexpr int BEAM_WIDTH = 6;

        State bestState = queue.top();
        bestState.print();
        int loop = 0;
        while(loop < BEAM_WIDTH) {
            if (queue.size() == 0) break;

            bool shouldSkip = false;

            State::generateAndInsertNextChoices(queue.top(), orders, nextQueue, BEAM_WIDTH);
            queue.pop();
            ++loop;
        }

        queue = nextQueue;
        stock++;
    }

    State bestState = queue.top();
    // bestState.printScore();
    bestState.print();

    return 0;
}