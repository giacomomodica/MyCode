class MessageQueue {
public:

	bool empty() const {
		lock_guard<mutex> lock(the_mutex);
		return the_queue.empty();
	}

	void push(const string& value) {
		unique_lock<mutex> lock(the_mutex);
		the_queue.push(value);
		lock.unlock();
		the_condition_variable.notify_one();
	}

	bool try_pop(string &popped_value) {
		lock_guard<mutex> lock(the_mutex);
		if (the_queue.empty()) {
			return false;
		}
		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	void wait_and_pop(string &popped_value) {
		unique_lock<mutex> lock(the_mutex);
		while (the_queue.empty()) {
			the_condition_variable.wait(lock);
		}
		popped_value = the_queue.front();
		the_queue.pop();
	}

private:
	queue<string> the_queue;
	mutable mutex the_mutex;
	condition_variable the_condition_variable;
};