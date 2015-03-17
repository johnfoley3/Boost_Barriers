#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>

boost::mutex io;

// atomic add
void incrementNumSafely(int id, int& current) {

	boost::lock_guard<boost::mutex> lock(io);
	std::cout << id << " has the mutex to increment." << std::endl;
	current++;
}

// counts and then uses a lock to print the number this thread has
void thread_fun(int id, boost::barrier& cur_barrier, int& current) {

    incrementNumSafely(id, current);
    cur_barrier.wait();

    // Leveraging lock_guards simplicity wtih explicit scoping
    {
        boost::lock_guard<boost::mutex> lock(io);
	std::cout << current << std::endl;
    }

    cur_barrier.wait();

    incrementNumSafely(id, current);
    cur_barrier.wait();

    boost::lock_guard<boost::mutex> lock(io);
    std::cout << current << std::endl;
}

// creates two barriers and three threads, then calls thread_fun()
int main() {

    boost::barrier bar(3);
    boost::barrier bar_two(3);

    int current = 0;
    int id = 0;

    boost::thread thr1(boost::bind(&thread_fun, ++id, boost::ref(bar), boost::ref(current)));
    boost::thread thr2(boost::bind(&thread_fun, ++id, boost::ref(bar), boost::ref(current)));
    boost::thread thr3(boost::bind(&thread_fun, ++id, boost::ref(bar), boost::ref(current)));

    thr1.join();
    thr2.join();
    thr3.join();
}
